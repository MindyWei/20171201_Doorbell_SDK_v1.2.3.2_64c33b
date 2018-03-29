#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <pthread.h>	
#include "ite/ith.h"   // for 
#include "ite/itp.h"
#include "config.h"
#include "tslib-private.h"


/****************************************************************************
 * ENABLE_TOUCH_POSITION_MSG :: just print X,Y coordination & 
 * 								touch-down/touch-up
 * ENABLE_TOUCH_IIC_DBG_MSG  :: show the IIC command 
 * ENABLE_TOUCH_PANEL_DBG_MSG:: show send-queue recieve-queue, 
 *                              and the xy value of each INTr
 ****************************************************************************/
//#define ENABLE_TOUCH_POSITION_MSG
//#define ENABLE_TOUCH_PANEL_DBG_MSG
//#define ENABLE_TOUCH_IIC_DBG_MSG
//#define ENABLE_SEND_FAKE_SAMPLE

/***************************
 *
 **************************/
#define TP_GPIO_PIN	    CFG_GPIO_TOUCH_INT

#ifdef	CFG_GPIO_TOUCH_WAKE
#define TP_GPIO_WAKE_PIN	CFG_GPIO_TOUCH_WAKE
#endif 

#define TOUCH_DEVICE_ID       (0x90>>1)

#if (TP_GPIO_PIN<32)
#define TP_GPIO_MASK    (1<<TP_GPIO_PIN)
#else
#define TP_GPIO_MASK    (1<<(TP_GPIO_PIN-32))
#endif

//#define ENABLE_SCALE_XY
//#define ENABLE_SWITCH_XY
//#define ENABLE_SCALE_X
//#define ENABLE_SCALE_Y
//#define ENABLE_REVERSE_X
//#define ENABLE_REVERSE_Y

#define TOUCH_SAMPLE_RATE_100ms	(100)
#define TOUCH_SAMPLE_RATE_50ms	(50)
#define TOUCH_SAMPLE_RATE_66ms	(66)
#define TOUCH_SAMPLE_RATE_33ms	(33)
#define TOUCH_SAMPLE_RATE_16ms	(16)
#define TOUCH_SAMPLE_RATE_10ms	(10)
#define TOUCH_SAMPLE_RATE_1ms	(1)


#define	TOUCH_NO_CONTACT		(0)
#define	TOUCH_DOWN				(1)
#define	TOUCH_UP				(2)

/***************************
 *
 **************************/
struct ft5316_ts_event {
	signed short pressure;
	signed int x;
	signed int y;
	int millisecs;
	int flags;
};

/***************************
 *
 **************************/
#ifdef CFG_TOUCH_INTR
static char g_TouchDownIntr = false;
#endif

static char g_IsTpInitialized = false;
static short lastx=0;
static short lasty=0;
static short lastp=0;

static short g_MaxRawX = 1024;// 800;
static short g_MaxRawY = 600;// 480;

static short g_xResolution =800;// 800;
static short g_yResolution =480;// 480;

static unsigned int dur=0;
struct timeval startT, endT;
struct timeval T1, T2;

static int g_tchDevFd=0;
static pthread_mutex_t 	gTpMutex;
static short g_currIntr=0;
static char g_TouchStatus = TOUCH_NO_CONTACT;	//0:normal, 1:down, 2:up
static char g_1stSampHasSend = 0;



static int g_tpCntr = 0;
static unsigned char MAX_FAKE_NUM = 31;
static int gFakeTableX[] = {688,100,100,100,100,100,562,436,100,100,100,100,100,310,200,100};
static int gFakeTableY[] = {406,250,200,160,120, 70,406,406,250,200,160,120, 70,406,406,406};
//static int gFakeTableX[] = {688,100,100,100,100,100,562,436,100,100,100,100,100,310,200,100,100,100,100,688,562,436,310,200,100,688,562,436,310,200,100};
//static int gFakeTableY[] = {406,250,200,160,120, 70,406,406,250,200,160,120, 70,406,406,406,160,120, 70,406,406,406,406,406,406,406,406,406,406,406,406};
/* //it's for 
static int gFakeTableX[] = {688,562,436,310,200,100};
static int gFakeTableY[] = {406,406,406,406,406,406};
*/
/*##################################################################################
 *                        the private function implementation
 ###################################################################################*/
#ifdef CFG_TOUCH_INTR
void _tp_isr(void* data)
{	
	unsigned int regValue;
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//ithPrintf("$in\n");
	#endif
	
	regValue = ithGpioGet(TP_GPIO_PIN);
	if ( (regValue & TP_GPIO_MASK) )
	{
		g_TouchDownIntr = false;
	}
	else
	{
		g_TouchDownIntr = true;
	}

    ithGpioClearIntr(TP_GPIO_PIN);
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//ithPrintf("$out(%x,%x)\n",g_TouchDownIntr,regValue);
	#endif
}

static void _initTouchIntr(void)
{
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("TP init in\n");	
    #endif
    
    ithEnterCritical();
    
    ithGpioClearIntr(TP_GPIO_PIN);
    ithGpioRegisterIntrHandler(TP_GPIO_PIN, _tp_isr, NULL);
    ithGpioCtrlDisable(TP_GPIO_PIN, ITH_GPIO_INTR_LEVELTRIGGER);	//set as edge trigger
    ithGpioCtrlDisable(TP_GPIO_PIN, ITH_GPIO_INTR_BOTHEDGE);		//set as single edge
    ithGpioCtrlEnable(TP_GPIO_PIN, ITH_GPIO_INTR_TRIGGERFALLING);	//set as falling edge
    ithIntrEnableIrq(ITH_INTR_GPIO);
    ithGpioEnableIntr(TP_GPIO_PIN); 
        
    ithExitCritical();
    
    #ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("TP init out\n");	
    #endif
}
#endif

void _initTouchGpioPin(void)
{
	ithGpioSetMode(TP_GPIO_PIN, ITH_GPIO_MODE1);
	ithGpioSetIn(TP_GPIO_PIN);
	ithGpioCtrlEnable(TP_GPIO_PIN, ITH_GPIO_PULL_ENABLE);
	ithGpioCtrlEnable(TP_GPIO_PIN, ITH_GPIO_PULL_UP);
	ithGpioEnable(TP_GPIO_PIN);	     
}

/******************************************************************************
 * the read flow for reading the FT5316's register by using iic repead start
 ******************************************************************************/
static void _initSample(struct ts_sample *s, int nr)
{
	int i;
	struct ts_sample *samp=s;
	
	//samp=s;
	
	for(i = 0; i < nr; i++)
	{
		samp->x = 0;
		samp->y = 0;
		samp->pressure = 0;
		gettimeofday(&(samp->tv),NULL);		
		samp++;
	}
}

static void _ft5306_getPoint(struct ts_sample *samp, int nr)
{
	int i2cret;
	int real_nr=0;
	struct ts_sample *s=samp;
	ITPI2cInfo *ft5306_evt;
	unsigned char	I2cCmd[3];
	unsigned char buf[10]={0};
	
	ft5306_evt = alloca(sizeof(*ft5306_evt));
	
	_initSample(s, nr);	
	
	while(real_nr<nr) 
	{
	      //int tmpX1=0,tmpY1=0;
		//I2cCmd = 0x00;		//1100 0010
		I2cCmd[0]= 0x10;
		I2cCmd[1]= 0x00;
		ft5306_evt->slaveAddress   = TOUCH_DEVICE_ID;
		ft5306_evt->cmdBuffer      = &I2cCmd;
		ft5306_evt->cmdBufferSize  = 2;
		ft5306_evt->dataBuffer     = buf;
		ft5306_evt->dataBufferSize =9;		
		i2cret = read(g_tchDevFd, ft5306_evt, 1);
		if(i2cret<0)	break;
		/*tmpX1=buf[4];
		tmpX1<<=8;
		tmpX1+=buf[3];
		tmpY1=buf[6];
		tmpY1<<=8;
		tmpY1+=buf[5];
		printf("ydz buf=%x,%x,%x,%x,%x,%x,%x,%x,%x,X1=%d,Y1=%d\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7], buf[8],tmpX1,tmpY1);
		break;*/
		if(buf[8]>0)
		{
			if((g_TouchStatus==TOUCH_DOWN) && (buf[8]==4) )
			{
				samp->x = (int)0;
				samp->y = (int)0;
				samp->pressure = 0;		//?? how to get ft5306's presure?	
			}
			else
			{
				int tmpX=0,tmpY=0;	//g_MaxRawX		 g_xResolution
				tmpX=buf[4];
		             tmpX<<=8;
		             tmpX+=buf[3];
		             tmpY=buf[6];
		             tmpY<<=8;
		             tmpY+=buf[5];
				#ifdef	ENABLE_SCALE_XY
				unsigned int u32X=0, u32Y=0;
				//tmpX = (int)((((unsigned int)buf[3]<<8)&0x0F00) | ((unsigned int)buf[4]));
				//tmpY = (int)((((unsigned int)buf[5]<<8)&0x0F00) | ((unsigned int)buf[6]));
				u32X = (((unsigned int)tmpX)*((unsigned int)g_xResolution))/(unsigned int)g_MaxRawX;
				u32Y = (((unsigned int)tmpY)*((unsigned int)g_yResolution))/(unsigned int)g_MaxRawY;
				samp->x = (int)u32X;
				samp->y = (int)u32Y;
				#else
				samp->x =tmpX;
				samp->y =tmpY;
				#endif	
				samp->pressure = 1;			//?? how to get ft5306's presure?
			}

			#ifdef	ENABLE_REVERSE_X
			if(samp->pressure)
			{
				short tempX = (short)samp->x;
				if(g_xResolution>=tempX)	samp->x = (int)(g_xResolution - tempX);
				else						samp->x = 0;
			}
			#endif

			#ifdef	ENABLE_REVERSE_Y
			if(samp->pressure)
			{
				short tempY = (short)samp->y;
				if(g_yResolution>=tempY)	samp->y = (int)(g_yResolution - tempY);
				else						samp->y = 0;
			}
			#endif
			
			#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
        	printf("	RAW->--------------------> %d %d %d\n", samp->pressure, samp->x, samp->y);
			#endif
			
			gettimeofday(&samp->tv,NULL);				
		}
        else
        {
			#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
			printf("tch inv buf3=%x\r\n",buf[3]);
			#endif
			break;		
		}
		
		s++;
		real_nr++;
	}
}
static void* _tpProbeHandler(void* arg)
{
    unsigned int regValue;
    //unsigned char NeedToGetSample = 1;	//for sending a I2C command when each INTr happens.
    //unsigned int TchIntrCnt=0;			//for detecting the touch-UP event
    
    printf("_tpProbeHandler.start~~\n");
    
	while(1)
	{
		if(g_IsTpInitialized==true)
		{
		       static int upFingerTimes=0;
		      #ifdef CFG_TOUCH_INTR
			if ( g_TouchDownIntr )
			#else
			regValue = ithGpioGet(TP_GPIO_PIN);
			if ( !(regValue & TP_GPIO_MASK) )
			#endif
			{
			      struct ts_sample tmpSamp;
			      #ifdef CFG_TOUCH_INTR
				ithGpioDisableIntr(TP_GPIO_PIN);
				#endif
				
				//gettimeofday(&endT,NULL);
				//dur = (unsigned int)itpTimevalDiff(&startT, &endT);		
						
				//#ifndef CFG_TOUCH_INTR
				//if( !NeedToGetSample )	continue;	//to prevent over sampling
				//NeedToGetSample = 0;
				//#endif
								
				_ft5306_getPoint(&tmpSamp, 1);
				
				upFingerTimes=0;
				
				pthread_mutex_lock(&gTpMutex);
				g_currIntr = 1;
				lastp = 1;
				lastx = tmpSamp.x;
				lasty = tmpSamp.y;
				pthread_mutex_unlock(&gTpMutex);
			      //printf("	EnQue:p=%x,xy=%d,%d\n", tmpSamp.pressure, tmpSamp.x, tmpSamp.y);	
				#ifdef CFG_TOUCH_INTR
				g_TouchDownIntr = 0;				
				ithGpioEnableIntr(TP_GPIO_PIN); 
				#endif
				
				//#ifndef CFG_TOUCH_INTR
				usleep(10000);//2000
				//#endif
			}else{
			       if(g_currIntr>0)	upFingerTimes++;
				if(upFingerTimes>3){
					upFingerTimes=0;
					pthread_mutex_lock(&gTpMutex);	
					g_currIntr =1;
					lastp =0;
					lastx =0;
					lasty =0;
					pthread_mutex_unlock(&gTpMutex);

				};
				//printf("tp up\n");
			       usleep(10000);
			};	
		}
		else	//if(g_IsTpInitialized==true)
		{
			printf("WARNING:: TP has not initial, yet~~~\n");
			usleep(100000);
		}
	};
	return NULL;
}
/*
static void* _tpProbeHandler(void* arg)
{
    unsigned int regValue;
    unsigned char NeedToGetSample = 1;	//for sending a I2C command when each INTr happens.
    //unsigned int TchIntrCnt=0;			//for detecting the touch-UP event
    
    printf("_tpProbeHandler.start~~\n");
    
	while(1)
	{
		if(g_IsTpInitialized==true)
		{
			#ifdef CFG_TOUCH_INTR
			if ( g_TouchDownIntr )
			#else
			regValue = ithGpioGet(TP_GPIO_PIN);
			if ( !(regValue & TP_GPIO_MASK) )
			#endif
			{
				struct ts_sample tmpSamp;
				unsigned int NeedUpdateSample = 0;
				
				//TchIntrCnt=0;
				
				#ifdef CFG_TOUCH_INTR
				ithGpioDisableIntr(TP_GPIO_PIN);
				#endif
				
				gettimeofday(&endT,NULL);
				dur = (unsigned int)itpTimevalDiff(&startT, &endT);		
						
				#ifndef CFG_TOUCH_INTR
				if( !NeedToGetSample )	continue;	//to prevent over sampling
				NeedToGetSample = 0;
				#endif
								
				_ft5306_getPoint(&tmpSamp, 1);
				
				pthread_mutex_lock(&gTpMutex);
				
				#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
				printf("	lastp=%x, dur=%x, p=%x, s=%x\n",lastp, dur, tmpSamp.pressure, g_TouchStatus);
				#endif
				if(tmpSamp.pressure) gettimeofday(&T1,NULL);
				switch(g_TouchStatus)
				{
					case TOUCH_NO_CONTACT:
						if(tmpSamp.pressure)
						{
							g_TouchStatus = TOUCH_DOWN;
							g_currIntr = 1;
							NeedUpdateSample = 1;
							g_1stSampHasSend = 0;
						}
						break;
					
					case TOUCH_DOWN:
						if(!tmpSamp.pressure)
						{
							g_TouchStatus = TOUCH_UP;
						}				
						if(g_1stSampHasSend)	NeedUpdateSample = 1;
						break;
						
					case TOUCH_UP:
						if(!tmpSamp.pressure)
						{
							g_TouchStatus = TOUCH_NO_CONTACT;
							g_currIntr = 0;
						}
						else
						{
							g_TouchStatus = TOUCH_DOWN;
							g_currIntr = 1;
							NeedUpdateSample = 1;
						}
						break;
						
					default:
						printf("ERROR touch STATUS, need to check it!!\n");
						break;				
				}
				
				#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
				printf("	tpStatus=%x, NSQ=%x, cINT=%x, send=%x\n", g_TouchStatus, NeedUpdateSample, g_currIntr, g_1stSampHasSend);
				#endif
				
				pthread_mutex_unlock(&gTpMutex);
				
				if(NeedUpdateSample)
				{
					gettimeofday(&startT,NULL);
					
					pthread_mutex_lock(&gTpMutex);
					
					lastp = tmpSamp.pressure;
					lastx = tmpSamp.x;
					lasty = tmpSamp.y;
					
					#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
					printf("	EnQue:p=%x,xy=%d,%d\n", tmpSamp.pressure, tmpSamp.x, tmpSamp.y);
					#endif	
										
					pthread_mutex_unlock(&gTpMutex);
				}		
				
				#ifdef CFG_TOUCH_INTR
				g_TouchDownIntr = 0;				
				ithGpioEnableIntr(TP_GPIO_PIN); 
				#endif
				
				#ifndef CFG_TOUCH_INTR
				usleep(2000);
				#endif
			}			
			else	//if ( g_TouchDownIntr )
			{
				#ifndef CFG_TOUCH_INTR
				NeedToGetSample = 1;
				#endif
				
				if(g_TouchStatus == TOUCH_UP)
				{
					if(g_1stSampHasSend)
					{
						pthread_mutex_lock(&gTpMutex);
					
						g_TouchStatus = TOUCH_NO_CONTACT;
						g_currIntr = 0;
						lastp = 0;
						lastx = 0;
						lasty = 0;						

						pthread_mutex_unlock(&gTpMutex);
					}
					#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
					printf("INT=0, force to set status=0!!\n");
					#endif
				}

				if( (g_TouchStatus == TOUCH_DOWN) && ithGpioGet(TP_GPIO_PIN) )
				{
					//if(!TchIntrCnt)
					//{
					//	gettimeofday(&T1,NULL);
					//}
					
					gettimeofday(&T2,NULL);
					dur = (unsigned int)itpTimevalDiff(&T1, &T2);	
					
					if( dur>TOUCH_SAMPLE_RATE_66ms && g_1stSampHasSend )
					{
						pthread_mutex_lock(&gTpMutex);
						
						g_TouchStatus = TOUCH_UP;
						g_currIntr = 0;
						lastp = 0;
						lastx = 0;
						lasty = 0;						
						
						#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
						printf("INT=0, and dur>%dms, so force to set status=2!!\n",TOUCH_SAMPLE_RATE_16ms);
						#endif
						
						pthread_mutex_unlock(&gTpMutex);
						
						gettimeofday(&startT,NULL);
						

					}
					
					//TchIntrCnt++;
				}
				#ifndef CFG_TOUCH_INTR
				usleep(2000);
				#endif
			}			
		}
		else	//if(g_IsTpInitialized==true)
		{
			printf("WARNING:: TP has not initial, yet~~~\n");
			usleep(100000);
		}
	}
	return NULL;
}*/

/******************************************************************************
 * do initial flow of FT5316
 ******************************************************************************/
static int _doInitTouch(int dev)
{
	int ret;
    int res;
    pthread_t task;
    pthread_attr_t attr;
    	
	gettimeofday(&startT,NULL);	

    printf("Create touch pthread~~\n");    
	
	//create thread	
    pthread_attr_init(&attr);
    res = pthread_create(&task, &attr, _tpProbeHandler, NULL);
        
    if(res)
    {
    	printf( "[TouchPanel]%s() L#%ld: ERROR, create _tpProbeHandler() thread fail! res=%ld\n", res );
    	return -1;
    }
    
    //create touch mutex	
    res = pthread_mutex_init(&gTpMutex, NULL);
   	if(res)
   	{
   	    printf("[AudioLink]%s() L#%ld: ERROR, init touch mutex fail! res=%ld\r\n", __FUNCTION__, __LINE__, res);
   	    return -1;
   	}  
    
    _initTouchGpioPin();
	
	#ifdef CFG_TOUCH_INTR
	_initTouchIntr();
	#endif
	
	#ifdef	CFG_GPIO_TOUCH_WAKE
	if( (TP_GPIO_WAKE_PIN>0) && (TP_GPIO_WAKE_PIN<64) )
	{
		ithGpioSetMode(TP_GPIO_WAKE_PIN,ITH_GPIO_MODE1);
   		ithGpioSetOut(TP_GPIO_WAKE_PIN);
   		ithGpioSet(TP_GPIO_WAKE_PIN);    	
   		ithGpioEnable(TP_GPIO_WAKE_PIN);
   	}
	else
	{
		printf("NOT initial TOUCH_GPIO_WAKE_PIN\n");
	}
	#endif

	g_tchDevFd = dev;		
	g_IsTpInitialized = true;
	
	return 0;
}

static int _getTouchSample(struct ts_sample *samp, int nr)
{
	int ret=0;
	struct ts_sample *s=samp;

	pthread_mutex_lock(&gTpMutex);	
	
	if(g_currIntr)
	{
		s->pressure = lastp;
		s->x = lastx;
		s->y = lasty;
		gettimeofday(&s->tv,NULL);
		if(s->pressure)	g_1stSampHasSend = 1;
		if((lastp==0)&&(lastx==0)&&(lasty==0)) g_currIntr=0;
		
		ret++;
	}
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//printf("gfQ, r=%x, INT=%x, s=%x, pxy=(%d,%d,%d)\n", ret, g_currIntr, g_TouchStatus, s->pressure, s->x, s->y);
	#endif
	
	pthread_mutex_unlock(&gTpMutex);
	
	return ret;
}

#ifdef	ENABLE_SEND_FAKE_SAMPLE
int _getFakeSample(struct ts_sample *samp, int nr)
{
	_initSample(samp, nr);
	
	printf("tp_getXY::cnt=%x\n",g_tpCntr);
	
	if(g_tpCntr++>0x100)
	{
		if( !(g_tpCntr&0x07) )
		{
			unsigned int i;
			i = (g_tpCntr>>3)&0x1F;
			if(i<MAX_FAKE_NUM)
			{
				samp->pressure = 1;
				samp->x = gFakeTableX[i];
				samp->y = gFakeTableY[i];
				printf("sendXY.=%d,%d\n",samp->x,samp->y);	
			}
		}
	}

	return nr;
}
#endif
/*##################################################################################
 *                           private function above
 ###################################################################################*/










/*##################################################################################
 #                       public function implementation
 ###################################################################################*/
static int ft5316_read(struct tslib_module_info *inf, struct ts_sample *samp, int nr)
{
	struct tsdev *ts = inf->dev;
	unsigned int regValue;
	int ret;
	int total = 0;
	int tchdev = ts->fd;
	struct ts_sample *s=samp;
	
	#ifdef	ENABLE_SEND_FAKE_SAMPLE
	return _getFakeSample(samp,nr);
	#endif	
	
	if(g_IsTpInitialized==false)
	{
		printf("TP first init(INT is GPIO %d)\n",TP_GPIO_PIN);
		if(_doInitTouch(tchdev)!=0)	return -1;
	}
	
	_initSample(s, nr);
	
	//to receive touch sample 
	ret = _getTouchSample(samp, nr);
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//if(ret)	printf("	deQue-O:%x=(%d,%d,%d)r=%d\n", samp, samp->pressure, samp->x, samp->y, ret);
	#endif
	
	return nr;
}

static const struct tslib_ops ft5316_ops =
{
	ft5316_read,
};

TSAPI struct tslib_module_info *castor3_mod_init(struct tsdev *dev, const char *params)
{
	struct tslib_module_info *m;

	m = malloc(sizeof(struct tslib_module_info));
	if (m == NULL)
		return NULL;

	m->ops = &ft5316_ops;
	return m;
}

#ifndef TSLIB_STATIC_CASTOR3_MODULE
	TSLIB_MODULE_INIT(castor3_mod_init);
#endif
