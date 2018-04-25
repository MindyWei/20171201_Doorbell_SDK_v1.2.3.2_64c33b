/*
 you have to sure 3 things before porting TP driver
 1).INT is work normal
 2).I2C BUS can read data from TP chip
 3).Parse the X,Y coordination correctly

 These function are customized.
 You Have to modify these function with "(*)" mark.
 These functions(3&4) are almost without modification
 Function(5~7) will be modified deponding on chip's feature.
  0._tpInitSpec_vendor()           //set control config(*)
  1._tpReadPointBuffer_vendor()    //read point buffer(*)
  2._tpParseRawPxy_vendor()        //parse the touch point(*)
  3._tpIntActiveRule_vendor()      //touch-down RULE
  4._tpIntNotActiveRule_vendor()   //touch-up RULE
5._tpParseKey_vendor()           //depend on TP with key
6._tpDoPowerOnSeq_vendor();      //depend on TP with power-on sequence
7._tpDoInitProgram_vendor();         //depend on TP with initial programming
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <pthread.h>	
#include "ite/ith.h" 
#include "ite/itp.h"
#include "config.h"
#include "tslib-private.h"
#include "fat/udefs_f.h"
#include "fat/api_f.h"

//#include "raw_api.h"
/****************************************************************************
 * initial Kconfig setting
 ****************************************************************************/

#if	defined(CFG_TOUCH_I2C0) || defined(CFG_TOUCH_I2C1)
#define TP_INTERFACE_I2C   (0)
#endif

#if	defined(CFG_TOUCH_SPI) || defined(CFG_TOUCH_SPI0) || defined(CFG_TOUCH_SPI1)
#define TP_INTERFACE_SPI   (1)
#endif

#define TP_INT_PIN	    CFG_GPIO_TOUCH_INT
#define TP_GPIO_MASK    (1<<(TP_INT_PIN%32))

#ifdef	CFG_GPIO_TOUCH_WAKE
#if (CFG_GPIO_TOUCH_WAKE<128)
#define TP_GPIO_WAKE_PIN	CFG_GPIO_TOUCH_WAKE
#endif 
#endif 

#ifdef	CFG_GPIO_TOUCH_RESET
#if (CFG_GPIO_TOUCH_RESET<128)
#define TP_GPIO_RESET_PIN	CFG_GPIO_TOUCH_RESET
#else
#define TP_GPIO_RESET_PIN	(-1)
#endif 
#endif 

#ifdef	CFG_TOUCH_ADVANCE_CONFIG

#ifdef	CFG_TOUCH_SWAP_XY
#define	TP_SWAP_XY		(1)
#else
#define	TP_SWAP_XY		(0)
#endif

#ifdef	CFG_TOUCH_REVERSE_X
#define	TP_REVERSE_X	(1)
#else
#define	TP_REVERSE_X	(0)
#endif

#ifdef	CFG_TOUCH_REVERSE_Y
#define	TP_REVERSE_Y	(1)
#else
#define	TP_REVERSE_Y	(0)
#endif

#else

#define	TP_SWAP_XY		(0)
#define	TP_REVERSE_X	(0)
#define	TP_REVERSE_Y	(0)

#endif

#define	TOUCH_NO_CONTACT		(0)
#define	TOUCH_DOWN				(1)
#define	TOUCH_UP				(2)

#define	TP_ACTIVE_LOW           (0)
#define	TP_ACTIVE_HIGH          (1)

#define	TP_INT_LEVLE_TRIGGER    (1)
#define	TP_INT_EDGE_TRIGGER     (0)

#define	TP_INT_TYPE_KEEP_STATE  (0)
#define	TP_INT_TYPE_ZT2083      (0)
#define	TP_INT_TYPE_FT5XXX      (1)
#define	TP_INT_TYPE_IT7260      (2)

#define	TP_WITHOUT_KEY          (0)
#define	TP_HAS_TOUCH_KEY        (1)
#define	TP_GPIO_PIN_NO_DEF      (-1)

#ifdef CFG_TOUCH_INTR
#define	TP_ENABLE_INTERRUPT     (1)
#else
#define	TP_ENABLE_INTERRUPT     (0)
#endif

#define	MAX_FINGER_NUM	(1)
//#define	ENABLE_GT911_FW_UPGRADE
/****************************************************************************
 * touch cofig setting
 ****************************************************************************/
#define TP_IDLE_TIME                (20000)
#define TP_IDLE_TIME_NO_INITIAL     (100000)

/****************************************************************************
 * ENABLE_TOUCH_POSITION_MSG :: just print X,Y coordination & 
 * 								touch-down/touch-up
 * ENABLE_TOUCH_IIC_DBG_MSG  :: show the IIC command 
 * ENABLE_TOUCH_PANEL_DBG_MSG:: show send-queue recieve-queue, 
 *                              and the xy value of each INTr
 ****************************************************************************/
//#define ENABLE_TOUCH_POSITION_MSG
//#define ENABLE_TOUCH_RAW_POINT_MSG
//#define ENABLE_TOUCH_PANEL_DBG_MSG
//#define ENABLE_TOUCH_IIC_DBG_MSG
//#define ENABLE_SEND_FAKE_SAMPLE

/****************************************************************************
 * MACRO define of ICN8826
 ****************************************************************************/
#define TOUCH_DEVICE_ID1      (0x90>>1)
#define TOUCH_DEVICE_ID2      (0x60>>1)

#define TP_I2C_DEVICE_ID       TOUCH_DEVICE_ID1

#define TP_SAMPLE_RATE	(33)

#ifdef	CFG_LCD_ENABLE
#define	TP_SCREEN_WIDTH	    ithLcdGetWidth()
#define	TP_SCREEN_HEIGHT    ithLcdGetHeight()
#else
#define	TP_SCREEN_WIDTH	    800
#define	TP_SCREEN_HEIGHT	480
#endif

#define GTP_POLL_TIME         10
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MAX_LENGTH 186
#define FAIL                  0
#define SUCCESS               1


#define X2Y_Enable		 0x08
#define RESOLUTION_LOC   3
#define TRIGGER_LOC      8

#define GTP_MAX_HEIGHT   600
#define GTP_MAX_WIDTH    1024
#define GTP_INT_TRIGGER  1    //0:Rising 1:Falling
#define GTP_MAX_TOUCH         6
#define GTP_ESD_CHECK_CIRCLE  2000
#define GTP_ADDR_LENGTH       2


/****************************************
 *
 ***************************************/
typedef struct 
{
	char tpCurrINT;
	char tpStatus;
	char tpNeedToGetSample;
	char tpNeedUpdateSample;
	char tpFirstSampHasSend;
	char tpIntr4Probe;
	char tpIsInitFinished;
	int  tpDevFd;
	int  tpIntrCnt;
}tp_info_tag;//tp_gv_tag

typedef struct tp_spec_tag
{
    //TP H/W setting
    char tpIntPin;		    //INT signal GPIO pin number
    char tpIntActiveState;	//High=1, Low=0
    char tpIntTriggerType;  //interrupt trigger type. 0:edge trigger, 1:level trigger
    char tpWakeUpPin;		//Wake-Up pin GPIO pin number, -1: means NO Wake-Up pin.
    char tpResetPin;		//Reset pin GPIO pin number, -1: means NO reset pin.
    char tpIntrType;		//0:keep state when touch down(like ZT2083), 1:like FT5XXX type 2:like IT7260, 3:others....  
    char tpInterface; 		//0:I2C, 1:SPI, 2:other...
    char tpI2cDeviceId; 	//I2C device ID(slave address) if TP has I2C interface
    char tpHasTouchKey;		//0: NO touch key, 1:touch key type I, 2:touch key type II, ...
    char tpIntUseIsr;	    //0:polling INT siganl, 1:INT use interrupt, 
        
    //TP resolution
    int  tpMaxRawX;
    int  tpMaxRawY;
    int  tpScreenX;
    int  tpScreenY;
    
    //TP convert function
    char tpCvtSwapXY;		//0:Disable, 1:Enable
    char tpCvtReverseX;     //0:Disable, 1:Enable
    char tpCvtReverseY;     //0:Disable, 1:Enable 
    char tpCvtScaleX;		//0:Disable, 1:Enable
    char tpCvtScaleY;		//0:Disable, 1:Enable
    
    //TP sample specification
    char tpEnTchPressure;	//0:disable pressure info, 1:enable pressure info
    char tpSampleNum;		//0:NO scense, 1: single touch 2~10:multi-touch
    char tpSampleRate;		//UNIT: mill-second, range 8~16 ms(60~120 samples/per second)  
    
    //TP idle time
    int  tpIdleTime;		//sleep time for polling INT signal(even if interrupt mode).    
    int  tpIdleTimeB4Init;	//sleep time if TP not initial yet.       
    int  tpReadChipRegCnt;	//read register count for getting touch xy coordination
    
    //TP specific function
    char tpHasPowerOnSeq;	//0:NO power-on sequence, 1:TP has power-on sequence
    char tpNeedProgB4Init;	//0:TP IC works well without programe flow, 1:TP IC need program before operation.
    char tpNeedAutoTouchUp;
    char tpIntPullEnable;	//use internal pull up/down function
} TP_SPEC;

/***************************
 * global variable
 **************************/
static struct ts_sample g_sample[MAX_FINGER_NUM];

static char g_TouchDownIntr = false;
static char  g_IsTpInitialized = false;
static pthread_mutex_t 	gTpMutex;

static TP_SPEC     gTpSpec;
static tp_info_tag gTpInfo = { 0,TOUCH_NO_CONTACT,1,0,0,0,0,0,0};

static unsigned int dur=0;
struct timeval T1, T2;
static int g_tpCntr = 0;

//for the function "_tpFixIntHasNoResponseIssue()"
static int  g_IntrLowCnt = 0;
struct timeval tv1, tv2;

static int g_tchCoorX2Y=0;
/*##################################################################################
 *                         the protocol of private function
 ###################################################################################*/
#ifdef CFG_TOUCH_INTR
static void _tp_isr(void* data);
static void _initTouchIntr(void);
#endif

/* *************************************************************** */
static void _tpInitSpec_vendor(void);
static int  _tpReadPointBuffer_vendor(unsigned char *buf, int cnt);
static int  _tpParseRawPxy_vendor(struct ts_sample *s, unsigned char *buf);
static void _tpParseKey_vendor(struct ts_sample *s, unsigned char *buf);

static void _tpIntActiveRule_vendor(struct ts_sample *tpSmp);
static void _tpIntNotActiveRule_vendor(struct ts_sample *tpSmp);

static void _tpDoPowerOnSeq_vendor(void);
static int _tpDoInitProgram_vendor(void);

/* *************************************************************** */

/* *************************************************************** */
static int  _tpDoInitial(void);
static void _tpInitWakeUpPin(void);
static void _tpInitResetPin(void);
static void _tpInitIntPin(void);
static void _tpInitTouchGpioPin(void);

static void _initSample(struct ts_sample *s, int nr);
static char _tpGetIntr(void);
/* *************************************************************** */

/* *************************************************************** */
static int _tpGetRawPoint(struct ts_sample *samp, int nr);
static void _tpConvertRawPoint(struct ts_sample *samp, int nr);
static void _tpUpdateLastXY(struct ts_sample *smp);

static bool _tpNeedToGetSample(void);
static int _tpGetSample(struct ts_sample *samp, int nr);
static void _tpUpdate(struct ts_sample *tpSmp);
/* *************************************************************** */

static void* _tpProbeHandler(void* arg);
static int  _tpProbeSample(struct ts_sample *samp, int nr);

//*************************** PART2:TODO define **********************************
// STEP_1(REQUIRED): Define Configuration Information Group(s)
// Sensor_ID Map:
/* sensor_opt1 sensor_opt2 Sensor_ID
    GND           GND          0 
    VDDIO         GND          1 
    NC            GND          2 
    GND           NC/300K      3 
    VDDIO         NC/300K      4 
    NC            NC/300K      5 
*/

/*##################################################################################
 *                        the private function implementation
 ###################################################################################*/

/******************************************************************************
 * the read flow for reading the FT5316's register by using iic repead start
 ******************************************************************************/
static bool gtp_i2c_read(uint8_t* cmd, uint8_t *buf, uint32_t len)
{
	int i2cret;
	ITPI2cInfo *icn8826_evt;

	icn8826_evt = alloca(sizeof(*icn8826_evt));
	icn8826_evt->slaveAddress   = TP_I2C_DEVICE_ID;
	icn8826_evt->cmdBuffer      = cmd;
	icn8826_evt->cmdBufferSize  = 2;
	icn8826_evt->dataBuffer     = buf;
	icn8826_evt->dataBufferSize = len;	
	i2cret = read(gTpInfo.tpDevFd, icn8826_evt, 1);	
	return i2cret;
}

static bool gtp_i2c_read2(unsigned int cmdVal,int cmdLen, uint8_t *buf, uint32_t len)
{
	int i2cret;
	ITPI2cInfo *icn8826_evt;
    char cmd[4];
    cmd[3] = cmdVal &0xFF;
    cmd[2] = (cmdVal &0xFF00)>>8;
    cmd[1] = (cmdVal &0xFF0000)>>16;
    cmd[0] = (cmdVal &0xFF000000)>>24;
	icn8826_evt = alloca(sizeof(*icn8826_evt));
	icn8826_evt->slaveAddress   = TOUCH_DEVICE_ID2;
	icn8826_evt->cmdBuffer      = cmd+(4-cmdLen);
	icn8826_evt->cmdBufferSize  = cmdLen;
	icn8826_evt->dataBuffer     = buf;
	icn8826_evt->dataBufferSize = len;	
	i2cret = read(gTpInfo.tpDevFd, icn8826_evt, 1);	
    //printf("gtp_i2c_read2 :%x,%x,%x\n",icn8826_evt->cmdBuffer[0],icn8826_evt->cmdBuffer[1],icn8826_evt->cmdBuffer[2]);
	return i2cret;
}

static bool gtp_i2c_read3(unsigned int cmdVal,int cmdLen, uint8_t *buf, uint32_t len)
{
	int i2cret;
	ITPI2cInfo *icn8826_evt;
    char cmd[4];
    cmd[3] = cmdVal &0xFF;
    cmd[2] = (cmdVal &0xFF00)>>8;
    cmd[1] = (cmdVal &0xFF0000)>>16;
    cmd[0] = (cmdVal &0xFF000000)>>24;
	icn8826_evt = alloca(sizeof(*icn8826_evt));
	icn8826_evt->slaveAddress   = TOUCH_DEVICE_ID1;
	icn8826_evt->cmdBuffer      = cmd+(4-cmdLen);
	icn8826_evt->cmdBufferSize  = cmdLen;
	icn8826_evt->dataBuffer     = buf;
	icn8826_evt->dataBufferSize = len;	
	i2cret = read(gTpInfo.tpDevFd, icn8826_evt, 1);	
	return i2cret;
}


static bool gtp_i2c_write2(unsigned int cmdVal,int cmdLen, uint8_t *buf, uint32_t len)
{
	int i2cret;
	ITPI2cInfo *icn8826_evt;
    char cmd[4],*buffer;
    cmd[3] = cmdVal &0xFF;
    cmd[2] = (cmdVal &0xFF00)>>8;
    cmd[1] = (cmdVal &0xFF0000)>>16;
    cmd[0] = (cmdVal &0xFF000000)>>24;
	icn8826_evt = alloca(sizeof(*icn8826_evt));
	buffer = alloca(cmdLen+len);
	memcpy(buffer,cmd+(4-cmdLen),cmdLen);
	memcpy(buffer+cmdLen,buf,len);
	icn8826_evt->slaveAddress   = TOUCH_DEVICE_ID2;
	icn8826_evt->cmdBuffer      = buffer;
	icn8826_evt->cmdBufferSize  = cmdLen+len;
	icn8826_evt->dataBuffer     = buf;
	icn8826_evt->dataBufferSize = len;	
	i2cret = write(gTpInfo.tpDevFd, icn8826_evt, 1);	
    //printf("gtp_i2c_write2 :%x,%x,%x\n",icn8826_evt->cmdBuffer[0],icn8826_evt->cmdBuffer[1],icn8826_evt->cmdBuffer[2]);
	return i2cret;
}


static unsigned char gtp_rev8(unsigned char *SrcData, unsigned char length)
{
	unsigned int value = 0;
	unsigned char rev = 0;
	unsigned char i;
	
	for (i = 0; i < length; i++)
	{
		value += SrcData[i];
		//printf("i=%d, sum=%02x, v=%02x\n",i,value,SrcData[i]);
	}
	rev = (unsigned char)(value&0xFF);
	rev = (~rev) + 1;
	//printf("sum = %02x, rev=%02x\n",value,rev);
	return rev;
}

/*##################################################################################
 *                        the private function implementation
 ###################################################################################*/
static void _tpInitSpec_vendor(void)
{
    gTpSpec.tpIntPin          	= (char)TP_INT_PIN;           //from Kconfig setting
    gTpSpec.tpWakeUpPin         = (char)TP_GPIO_PIN_NO_DEF;   //from Kconfig setting
    gTpSpec.tpResetPin          = (char)TP_GPIO_RESET_PIN;   //from Kconfig setting
    gTpSpec.tpIntUseIsr         = (char)0;  //from Kconfig setting
    gTpSpec.tpIntActiveState    = (char)TP_ACTIVE_LOW;        //from Kconfig setting    
    gTpSpec.tpIntTriggerType    = (char)TP_INT_EDGE_TRIGGER; //from Kconfig setting   level/edge
    
    gTpSpec.tpInterface         = (char)TP_INTERFACE_I2C;	  //from Kconfig setting
        
    gTpSpec.tpMaxRawX           = (int)1024;//CFG_TOUCH_X_MAX_VALUE; //from Kconfig setting
    gTpSpec.tpMaxRawY           = (int)600;//CFG_TOUCH_Y_MAX_VALUE; //from Kconfig setting
    gTpSpec.tpScreenX           = (int)TP_SCREEN_WIDTH;       //from Kconfig setting
    gTpSpec.tpScreenY           = (int)TP_SCREEN_HEIGHT;      //from Kconfig setting
    
    gTpSpec.tpCvtSwapXY        = (char)TP_SWAP_XY;            //from Kconfig setting
    gTpSpec.tpCvtReverseX      = (char)TP_REVERSE_X;          //from Kconfig setting
    gTpSpec.tpCvtReverseY      = (char)TP_REVERSE_Y;          //from Kconfig setting
    gTpSpec.tpCvtScaleX        = (char)1;                    //from Kconfig setting
    gTpSpec.tpCvtScaleY        = (char)1;                    //from Kconfig setting
    
    gTpSpec.tpI2cDeviceId       = (char)TP_I2C_DEVICE_ID;	  //from this driver setting
    gTpSpec.tpEnTchPressure     = (char)0;                    //from this driver setting
    gTpSpec.tpSampleNum         = (char)1;                    //from this driver setting
    gTpSpec.tpSampleRate        = (char)TP_SAMPLE_RATE;       //from this driver setting
    gTpSpec.tpIntrType          = (char)TP_INT_TYPE_FT5XXX;	  //from this driver setting
    gTpSpec.tpHasTouchKey       = (char)TP_WITHOUT_KEY;       //from this driver setting                                                               
    gTpSpec.tpIdleTime          = (int)TP_IDLE_TIME;          //from this driver setting
    gTpSpec.tpIdleTimeB4Init    = (int)TP_IDLE_TIME_NO_INITIAL;//from this driver setting    
    gTpSpec.tpReadChipRegCnt    = (int)6;
    
    //special initial flow
    gTpSpec.tpHasPowerOnSeq     = (char)1;
    gTpSpec.tpNeedProgB4Init    = (char)0;//1;    
    gTpSpec.tpNeedAutoTouchUp	= (char)0; 
    gTpSpec.tpIntPullEnable     = (char)0;
    
/*
    printf("gTpSpec.tpIntPin         = %d\n",gTpSpec.tpIntPin);
    printf("gTpSpec.tpIntActiveState = %x\n",gTpSpec.tpIntActiveState);
    printf("gTpSpec.tpWakeUpPin      = %d\n",gTpSpec.tpWakeUpPin);
    printf("gTpSpec.tpResetPin       = %d\n",gTpSpec.tpResetPin);
    printf("gTpSpec.tpIntrType       = %x\n",gTpSpec.tpIntrType);
    printf("gTpSpec.tpInterface      = %x\n",gTpSpec.tpInterface);
    printf("gTpSpec.tpI2cDeviceId    = %x\n",gTpSpec.tpI2cDeviceId);
    printf("gTpSpec.tpHasTouchKey    = %x\n",gTpSpec.tpHasTouchKey);
    printf("gTpSpec.tpIntUseIsr      = %x\n",gTpSpec.tpIntUseIsr);
    printf("gTpSpec.tpMaxRawX        = %d\n",gTpSpec.tpMaxRawX);
    printf("gTpSpec.tpMaxRawY        = %d\n",gTpSpec.tpMaxRawY);
    printf("gTpSpec.tpScreenX        = %d\n",gTpSpec.tpScreenX);
    printf("gTpSpec.tpScreenY        = %d\n",gTpSpec.tpScreenY);
    printf("gTpSpec.tpCvtSwapXY     = %x\n",gTpSpec.tpCvtSwapXY);
    printf("gTpSpec.tpCvtReverseX   = %x\n",gTpSpec.tpCvtReverseX);
    printf("gTpSpec.tpCvtReverseY   = %x\n",gTpSpec.tpCvtReverseY);
    printf("gTpSpec.tpCvtScaleX     = %x\n",gTpSpec.tpCvtScaleX);
    printf("gTpSpec.tpCvtScaleY     = %x\n",gTpSpec.tpCvtScaleY);
    printf("gTpSpec.tpEnTchPressure  = %x\n",gTpSpec.tpEnTchPressure);
    printf("gTpSpec.tpSampleNum      = %x\n",gTpSpec.tpSampleNum);
    printf("gTpSpec.tpSampleRate     = %x\n",gTpSpec.tpSampleRate);
    printf("gTpSpec.tpIdleTime       = %d\n",gTpSpec.tpIdleTime);
    printf("gTpSpec.tpIdleTimeB4Init = %d\n",gTpSpec.tpIdleTimeB4Init);
    printf("gTpSpec.tpHasPowerOnSeq  = %x\n",gTpSpec.tpHasPowerOnSeq);
    printf("gTpSpec.tpNeedProgB4Init = %x\n",gTpSpec.tpNeedProgB4Init);
	printf("gTpSpec.tpNeedAutoTouchUp= %x\n",gTpSpec.tpNeedAutoTouchUp);
    */
    //initial global variable "gTpInfo"
/*    
    printf("gTpInfo.tpCurrINT              = %x\n",gTpInfo.tpCurrINT);
    printf("gTpInfo.tpStatus               = %x\n",gTpInfo.tpStatus);
    printf("gTpInfo.tpNeedToGetSample      = %x\n",gTpInfo.tpNeedToGetSample);
    printf("gTpInfo.tpNeedUpdateSample     = %x\n",gTpInfo.tpNeedUpdateSample);
    printf("gTpInfo.tpFirstSampHasSend     = %x\n",gTpInfo.tpFirstSampHasSend);
    printf("gTpInfo.tpFirstSampHasSend     = %x\n",gTpInfo.tpIsInitFinished);
    printf("gTpInfo.tpIntr4Probe           = %x\n",gTpInfo.tpIntr4Probe);
    printf("gTpInfo.tpDevFd                = %x\n",gTpInfo.tpDevFd);    
    printf("gTpInfo.tpIntrCnt              = %x\n",gTpInfo.tpIntrCnt);
*/
}    

static void _tpDoPowerOnSeq_vendor(void)
{
	//for Gt911 Linux  power on methods.	

    printf("DO POWER-ON sequence, reset pin:%d\n",gTpSpec.tpResetPin);	
    if(gTpSpec.tpResetPin == (char)-1) return;
    
	//1.set "Reset pin" & "INT pin" are output-low for 10ms	
	ithGpioSetMode( gTpSpec.tpResetPin, ITH_GPIO_MODE0);
	ithGpioClear(gTpSpec.tpResetPin);
	ithGpioSetOut(gTpSpec.tpResetPin);
	ithGpioEnable(gTpSpec.tpResetPin);	
	
#if 0
	ithGpioSetMode( gTpSpec.tpIntPin, ITH_GPIO_MODE0);
	ithGpioClear(gTpSpec.tpIntPin);
	ithGpioSetOut(gTpSpec.tpIntPin);
	ithGpioEnable(gTpSpec.tpIntPin);
#endif
	ithGpioSetMode( gTpSpec.tpIntPin, ITH_GPIO_MODE0);
	ithGpioClear(gTpSpec.tpIntPin);
	ithGpioSetIn(gTpSpec.tpIntPin);
	ithGpioEnable(gTpSpec.tpIntPin);
	
	usleep(500*1000);

    //2.set "Reset pin" output HIGH for 55ms
	ithGpioSet(gTpSpec.tpResetPin);
	//usleep(55*1000);

    //3.set "INT pin" output HIGH for 1ms
#if 0
	ithGpioSet(gTpSpec.tpIntPin);
	usleep(1000);
#endif

}


int icn8826_read_version(void)
{
    int version = 0;
    unsigned char version_ary[2]={0,0};
    int ret = gtp_i2c_read3(0x000c,2,version_ary, 1);
    ret = gtp_i2c_read3(0x000d,2,version_ary+1, 1);
    version = ((int)version_ary[0]<<8) | version_ary[1];
    printf("icn8826 read version:%x\n", version);
    return version;
}


static int is_icn8826_update(int binVer)
{
    char retry = 0;   
    int version_now = 0;
    usleep(100*1000);
    while(version_now == 0)
    {
        version_now = icn8826_read_version();
        retry++;
        if(retry >= 5)
        {
            break;
        }
    }
    printf("version_now :%x,BinVer:%x\n",version_now,binVer);
    if(version_now != binVer) return 1;
    return 0;
}

#define B_SIZE 128
#define FLASH_TOTAL_SIZE                0x00010000     
#define FLASH_PAGE_SIZE                 0x1000 
#define FLASH_AHB_BASE_ADDR             0x00100000 
#define FLASH_PATCH_PARA_BASE_ADDR      (FLASH_TOTAL_SIZE - FLASH_PAGE_SIZE)                     //  allocate 1 page for patch para, 0xff00
#define FLASH_CODE_INFO_BASE_ADDR       (FLASH_PATCH_PARA_BASE_ADDR - FLASH_PAGE_SIZE)           //  0xfe00,allocate 1 page for system para
#define FLASH_CRC_ADDR                  (FLASH_AHB_BASE_ADDR + FLASH_CODE_INFO_BASE_ADDR + 0x00) //  0xfe00
#define FLASH_CODE_LENGTH_ADDR          (FLASH_AHB_BASE_ADDR + FLASH_CODE_INFO_BASE_ADDR + 0x04) //  0xfe04


static unsigned int crc32table[256] = {
 0x00000000L, 0x04c11db7L, 0x09823b6eL, 0x0d4326d9L, 0x130476dcL, 0x17c56b6bL,    
 0x1a864db2L, 0x1e475005L, 0x2608edb8L, 0x22c9f00fL, 0x2f8ad6d6L, 0x2b4bcb61L,    
 0x350c9b64L, 0x31cd86d3L, 0x3c8ea00aL, 0x384fbdbdL, 0x4c11db70L, 0x48d0c6c7L,    
 0x4593e01eL, 0x4152fda9L, 0x5f15adacL, 0x5bd4b01bL, 0x569796c2L, 0x52568b75L,    
 0x6a1936c8L, 0x6ed82b7fL, 0x639b0da6L, 0x675a1011L, 0x791d4014L, 0x7ddc5da3L,    
 0x709f7b7aL, 0x745e66cdL, 0x9823b6e0L, 0x9ce2ab57L, 0x91a18d8eL, 0x95609039L,    
 0x8b27c03cL, 0x8fe6dd8bL, 0x82a5fb52L, 0x8664e6e5L, 0xbe2b5b58L, 0xbaea46efL,    
 0xb7a96036L, 0xb3687d81L, 0xad2f2d84L, 0xa9ee3033L, 0xa4ad16eaL, 0xa06c0b5dL,    
 0xd4326d90L, 0xd0f37027L, 0xddb056feL, 0xd9714b49L, 0xc7361b4cL, 0xc3f706fbL,    
 0xceb42022L, 0xca753d95L, 0xf23a8028L, 0xf6fb9d9fL, 0xfbb8bb46L, 0xff79a6f1L,    
 0xe13ef6f4L, 0xe5ffeb43L, 0xe8bccd9aL, 0xec7dd02dL, 0x34867077L, 0x30476dc0L,    
 0x3d044b19L, 0x39c556aeL, 0x278206abL, 0x23431b1cL, 0x2e003dc5L, 0x2ac12072L,    
 0x128e9dcfL, 0x164f8078L, 0x1b0ca6a1L, 0x1fcdbb16L, 0x018aeb13L, 0x054bf6a4L,    
 0x0808d07dL, 0x0cc9cdcaL, 0x7897ab07L, 0x7c56b6b0L, 0x71159069L, 0x75d48ddeL,    
 0x6b93dddbL, 0x6f52c06cL, 0x6211e6b5L, 0x66d0fb02L, 0x5e9f46bfL, 0x5a5e5b08L,    
 0x571d7dd1L, 0x53dc6066L, 0x4d9b3063L, 0x495a2dd4L, 0x44190b0dL, 0x40d816baL,    
 0xaca5c697L, 0xa864db20L, 0xa527fdf9L, 0xa1e6e04eL, 0xbfa1b04bL, 0xbb60adfcL,    
 0xb6238b25L, 0xb2e29692L, 0x8aad2b2fL, 0x8e6c3698L, 0x832f1041L, 0x87ee0df6L,    
 0x99a95df3L, 0x9d684044L, 0x902b669dL, 0x94ea7b2aL, 0xe0b41de7L, 0xe4750050L,    
 0xe9362689L, 0xedf73b3eL, 0xf3b06b3bL, 0xf771768cL, 0xfa325055L, 0xfef34de2L,    
 0xc6bcf05fL, 0xc27dede8L, 0xcf3ecb31L, 0xcbffd686L, 0xd5b88683L, 0xd1799b34L,    
 0xdc3abdedL, 0xd8fba05aL, 0x690ce0eeL, 0x6dcdfd59L, 0x608edb80L, 0x644fc637L,    
 0x7a089632L, 0x7ec98b85L, 0x738aad5cL, 0x774bb0ebL, 0x4f040d56L, 0x4bc510e1L,    
 0x46863638L, 0x42472b8fL, 0x5c007b8aL, 0x58c1663dL, 0x558240e4L, 0x51435d53L,    
 0x251d3b9eL, 0x21dc2629L, 0x2c9f00f0L, 0x285e1d47L, 0x36194d42L, 0x32d850f5L,    
 0x3f9b762cL, 0x3b5a6b9bL, 0x0315d626L, 0x07d4cb91L, 0x0a97ed48L, 0x0e56f0ffL,    
 0x1011a0faL, 0x14d0bd4dL, 0x19939b94L, 0x1d528623L, 0xf12f560eL, 0xf5ee4bb9L,    
 0xf8ad6d60L, 0xfc6c70d7L, 0xe22b20d2L, 0xe6ea3d65L, 0xeba91bbcL, 0xef68060bL,    
 0xd727bbb6L, 0xd3e6a601L, 0xdea580d8L, 0xda649d6fL, 0xc423cd6aL, 0xc0e2d0ddL,    
 0xcda1f604L, 0xc960ebb3L, 0xbd3e8d7eL, 0xb9ff90c9L, 0xb4bcb610L, 0xb07daba7L,    
 0xae3afba2L, 0xaafbe615L, 0xa7b8c0ccL, 0xa379dd7bL, 0x9b3660c6L, 0x9ff77d71L,    
 0x92b45ba8L, 0x9675461fL, 0x8832161aL, 0x8cf30badL, 0x81b02d74L, 0x857130c3L,    
 0x5d8a9099L, 0x594b8d2eL, 0x5408abf7L, 0x50c9b640L, 0x4e8ee645L, 0x4a4ffbf2L,    
 0x470cdd2bL, 0x43cdc09cL, 0x7b827d21L, 0x7f436096L, 0x7200464fL, 0x76c15bf8L,    
 0x68860bfdL, 0x6c47164aL, 0x61043093L, 0x65c52d24L, 0x119b4be9L, 0x155a565eL,    
 0x18197087L, 0x1cd86d30L, 0x029f3d35L, 0x065e2082L, 0x0b1d065bL, 0x0fdc1becL,    
 0x3793a651L, 0x3352bbe6L, 0x3e119d3fL, 0x3ad08088L, 0x2497d08dL, 0x2056cd3aL,    
 0x2d15ebe3L, 0x29d4f654L, 0xc5a92679L, 0xc1683bceL, 0xcc2b1d17L, 0xc8ea00a0L,    
 0xd6ad50a5L, 0xd26c4d12L, 0xdf2f6bcbL, 0xdbee767cL, 0xe3a1cbc1L, 0xe760d676L,    
 0xea23f0afL, 0xeee2ed18L, 0xf0a5bd1dL, 0xf464a0aaL, 0xf9278673L, 0xfde69bc4L,    
 0x89b8fd09L, 0x8d79e0beL, 0x803ac667L, 0x84fbdbd0L, 0x9abc8bd5L, 0x9e7d9662L,    
 0x933eb0bbL, 0x97ffad0cL, 0xafb010b1L, 0xab710d06L, 0xa6322bdfL, 0xa2f33668L,    
 0xbcb4666dL, 0xb8757bdaL, 0xb5365d03L, 0xb1f740b4L
};

int  icn85xx_crc_enable(char enable)
{
    unsigned char ucTemp;
    int ret = 1;
    if(enable==1)
    {
        ucTemp = 1;
    }
    else if(enable==0)
    {
        ucTemp = 0;
    } 
    if(gtp_i2c_write2(0x40028,3, &ucTemp,1))
    {
        ret = 0;
    }
    return ret;
}

unsigned int icn85xx_crc_calc(unsigned int crc_in, unsigned char *buf, int len, unsigned int *crc32table)  
{
    int i;
    unsigned int crc = crc_in;
    for(i = 0; i < len; i++)
    {
        crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *buf++) & 0xFF];   
    }
    return crc;
}

int icn85xx_check_progmod(void)
{
    int ret;
    unsigned char ucTemp = 0x0;
    ret = gtp_i2c_read2(0x040002,3, &ucTemp,1);
    printf("icn85xx_check_progmod: %x\n", ucTemp);
    if(ret == 0)
    {
        printf("icn85xx_check_progmod error, ret: %d\n", ret);
        return -1;
    }
    if(ucTemp == 0x85)
        return 0;
    else 
        return -1;

}

int icn85xx_goto_progmode(void)
{
    int ret = -1;
    int retry = 3;
    unsigned char ucTemp;  
    unsigned int length = 0;
    
    printf("icn85xx_goto_progmode\n"); 
    
    while(retry > 0)
    {       
        ucTemp = 0x5a;               
        length = 1;
        ret = gtp_i2c_write2(0xcc3355,3, &ucTemp,1);
        printf("after ret = icn85xx_prog_i2c_write_old(0xcc3355, &ucTemp,1);  %d\n",ret);
        ucTemp = 0;
        ret = gtp_i2c_read2(0xcc3355,3, &ucTemp,1);
        printf("0xcc3355 : %x\n", ucTemp);
        usleep(2000);             
        ucTemp = 01;           
        ret = gtp_i2c_write2(0x040400,3, &ucTemp,1);
        usleep(2000);
        ret = icn85xx_check_progmod();
        printf("after ret = icn85xx_check_progmod();\n");
        if(ret == 0)
        {
            return ret;
        }   
        retry--;
        usleep(2000);
    }
    printf("icn85xx_goto_progmode over\n");
    if(retry == 0)
        return -1;
 
    return 0;
}

int  icn85xx_fw_download(unsigned int offset, unsigned char * buffer, unsigned int size)
{
    int ret = gtp_i2c_write2(offset,3,buffer,size);
    return 0;   
}

int  icn85xx_crc_check(unsigned int crc, unsigned int len)
{
    int ret;
    unsigned int crc_len;
    unsigned int crc_result;    
    unsigned char ucTemp[4] = {0,0,0,0};

    gtp_i2c_read2(0x4002c,3,ucTemp,1);
    gtp_i2c_read2(0x4002d,3,ucTemp + 1,1);
    gtp_i2c_read2(0x4002e,3,ucTemp + 2,1);
    ret = gtp_i2c_read2(0x4002f,3,ucTemp + 3,1);
    printf("icn85xx_crc_check ret:%d\n",ret);
    crc_result = ((unsigned int)ucTemp[3]&0xFF)<<24 | ((unsigned int)ucTemp[2]&0xFF)<<16 | ((unsigned int)ucTemp[1]&0xFF) << 8 | ((unsigned int)ucTemp[0]&0xFF);

    gtp_i2c_read2(0x40034,3,ucTemp,1);
    gtp_i2c_read2(0x40035,3,ucTemp+1,1);
    crc_len = ((unsigned int)ucTemp[1]&0xFF) << 8 | ((unsigned int)ucTemp[0]&0xFF);
    //vs_printf("crc_len: %d\n", crc_len);
    printf("icn85xx_crc_check : %x,%x,%d,%d\n",crc_result,crc,crc_len,len);
    if((crc_result == crc) && (crc_len == len))
        return 0;
    else 
    {
        printf("crc_fw: %x", crc >> 16);
        printf(" %x\n", crc & 0xFFFF);
        printf("crc_result: %x", crc_result >> 16);
        printf(" %x\n", crc_result & 0xFFFF);
        printf("crc_len: %d,%d\n", crc_len,len);
        return -1;
    }
    
}

int  icn85xx_bootfrom_sram(void)
{
    int ret = -1;   
    unsigned char ucTemp = 0x03;                      
    if(gtp_i2c_write2(0x40400,3,&ucTemp,1))
    {
        ret = 0;
    }
    return ret;
}

#if 1

unsigned char FlashState(unsigned char State_Index)
{
    unsigned char ucTemp[4] = {0,0,0,0};

    ucTemp[2]=0x08;
    ucTemp[1]=0x10;
    ucTemp[0]=0x00;

    gtp_i2c_write2(0x4062c,3,ucTemp,3);
    if(State_Index==0)
    {
        ucTemp[0]=0x05;
    }
    else if(State_Index==1)
    {
        ucTemp[0]=0x35;
    }
    gtp_i2c_write2(0x40630,3,ucTemp,1);
    ucTemp[1]=0x00;
    ucTemp[0]=0x01;
    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=1;
    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while(ucTemp[0])
    {
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }
  
    gtp_i2c_read2(0x40648,3,ucTemp,1);
    return (unsigned char)(ucTemp[0]);
}

void FlashWriteEnable(void)
{
    unsigned char ucTemp[4] = {0,0,0,0};

    ucTemp[2]=0x00;
    ucTemp[1]=0x10;
    ucTemp[0]=0x00;
    gtp_i2c_write2(0x4062c,3,ucTemp,3);
    ucTemp[0]=0x06; 
    gtp_i2c_write2(0x40630,3,ucTemp,1);
    ucTemp[0]=0x00;
    ucTemp[1]=0x00;    
    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=1;
    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while(ucTemp[0])
    {    
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }
    
    ucTemp[0]=FlashState(0);
    while( (ucTemp[0]&0x02)!=0x02)
    {
        ucTemp[0]=FlashState(0);    
    }
}

void EarseFlash(unsigned char erase_index,unsigned int flash_addr)
{
    unsigned char ucTemp[4] = {0,0,0,0};
    FlashWriteEnable();    
    if(erase_index==0)                       //erase the chip
    {
        ucTemp[0]=0xc7;        
        gtp_i2c_write2(0x40630,3,ucTemp,1);
        ucTemp[2]=0x00;
        ucTemp[1]=0x10;
        ucTemp[0]=0x00;     
        gtp_i2c_write2(0x4062c,3,ucTemp,3);
    }
    else if(erase_index==1)       //erase 32k space of the flash
    {
        ucTemp[0]=0x52;       
        gtp_i2c_write2(0x40630,3,ucTemp,1);
        ucTemp[2]=0x00;
        ucTemp[1]=0x13;
        ucTemp[0]=0x00;        
        gtp_i2c_write2(0x4062c,3,ucTemp,3);
    }
    else if(erase_index==2)     //erase 64k space of the flash
    {
        ucTemp[0]=0xd8;
        gtp_i2c_write2(0x40630,3,ucTemp,1);
        ucTemp[2]=0x00;
        ucTemp[1]=0x13;
        ucTemp[0]=0x00;
        gtp_i2c_write2(0x4062c,3,ucTemp,3);
    } 
    else if(erase_index==3)
    {
        ucTemp[0]=0x20;         
        gtp_i2c_write2(0x40630,3,ucTemp,1);
        ucTemp[2]=0x00;
        ucTemp[1]=0x13;
        ucTemp[0]=0x00;       
        gtp_i2c_write2(0x4062c,3,ucTemp,3);
    }
    ucTemp[2]=(unsigned char)(flash_addr>>16);
    ucTemp[1]=(unsigned char)(flash_addr>>8);
    ucTemp[0]=(unsigned char)(flash_addr);
    gtp_i2c_write2(0x40634,3,ucTemp,3);
    ucTemp[1]=0x00;
    ucTemp[0]=0x00;
    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=1;
    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while(ucTemp[0])
    {
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }      
            
}


void ClearFlashState(void)
{
    unsigned char ucTemp[4] = {0,0,0,0};
    gtp_i2c_read2(0x40603,3,ucTemp,1);
    ucTemp[0]=(ucTemp[0]|0x20);
    gtp_i2c_write2(0x40603,3,ucTemp,1);
    FlashWriteEnable();   
    ////////////////////////////write comd to flash
    ucTemp[2]=0x00;
    ucTemp[1]=0x10;
    ucTemp[0]=0x10;
    gtp_i2c_write2(0x4062c,3,ucTemp,3);
    ucTemp[0]=0x01;
    gtp_i2c_write2(0x40630,3,ucTemp,1);
    ucTemp[0]=0x00;
    ucTemp[1]=0x00;
    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=0x00;    
    gtp_i2c_write2(0x40638,3,ucTemp,1);
    ucTemp[0]=1;    
    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while(ucTemp[0])
    {
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }
    while(FlashState(0)&0x01);

}

int  icn85xx_prog_buffer(unsigned int flash_addr,unsigned int sram_addr,unsigned int copy_length,unsigned char program_type)
{
    unsigned char ucTemp[4] = {0,0,0,0};
    unsigned char prog_state=0;
    unsigned int i=0;
    unsigned char program_commond=0;

    if(program_type == 0)
    {
        program_commond = 0x02;
    }
    else if(program_type == 1)
    {
        program_commond = 0xf2;
    }
    else
    {
        program_commond = 0x02;
    }
    
     
    for(i=0; i<copy_length; )
    {
        prog_state=(FlashState(0)&0x01);
        while(prog_state)
        {
            prog_state=(FlashState(0)&0x01);
        }
        FlashWriteEnable();           

        ucTemp[2]=0;
        ucTemp[1]=0x13;
        ucTemp[0]=0;
        gtp_i2c_write2(0x4062c,3,ucTemp,3);
        ucTemp[2]=(unsigned char)(flash_addr>>16);
        ucTemp[1]=(unsigned char)(flash_addr>>8);
        ucTemp[0]=(unsigned char)(flash_addr);
        gtp_i2c_write2(0x40634,3,ucTemp,3);
        ucTemp[2]=(unsigned char)(sram_addr>>16);
        ucTemp[1]=(unsigned char)(sram_addr>>8);
        ucTemp[0]=(unsigned char)(sram_addr);
        gtp_i2c_write2(0x4063c,3,ucTemp,3);
        if(i+256<=copy_length)
        {
            ucTemp[1]=0x01;
            ucTemp[0]=0x00;
        }
        else
        {
            ucTemp[1]=(unsigned char)((copy_length-i)>>8);
            ucTemp[0]=(unsigned char)(copy_length-i);
        }
        gtp_i2c_write2(0x40640,3,ucTemp,3);
        ucTemp[0]=program_commond;
        gtp_i2c_write2(0x40630,3,ucTemp,3);
        ucTemp[0]=0x01;
        gtp_i2c_write2(0x40644,3,ucTemp,3);
        flash_addr+=256;
        sram_addr+=256;
        i+=256;
        while(ucTemp[0])
        {
            gtp_i2c_read2(0x40644,3,ucTemp,3);
        }
    
    }     
  
    prog_state=(FlashState(0)&0x01);
    while(prog_state)
    {
        prog_state=(FlashState(0)&0x01);
    }
    return 0;   
}

int  icn85xx_erase_flash(void)
{
    ClearFlashState(); 
    while(FlashState(0)&0x01);
    FlashWriteEnable();
    EarseFlash(1,0);            
    while((FlashState(0)&0x01));  
    FlashWriteEnable();
    EarseFlash(3,0x8000);         //?which block   
    while((FlashState(0)&0x01)); 
    FlashWriteEnable();
    EarseFlash(3,0x9000);            
    while((FlashState(0)&0x01)); 
    FlashWriteEnable();
    EarseFlash(3,0xe000);            
    while((FlashState(0)&0x01));     
    return 0;
}

void  icn85xx_read_flash(unsigned int sram_address,unsigned int flash_address,unsigned long copy_length,unsigned char i2c_wire_num)
{
    unsigned char ucTemp[4] = {0,0,0,0};

    if(i2c_wire_num==1)
    {
        ucTemp[2]=0x18;
        ucTemp[1]=0x13;
        ucTemp[0]=0x00;
    }
    else if(i2c_wire_num==2)
    {
        ucTemp[2]=0x1a;
        ucTemp[1]=0x13;
        ucTemp[0]=0x01;
    }
    else if(i2c_wire_num==4)
    {
        ucTemp[2]=0x19;
        ucTemp[1]=0x13;
        ucTemp[0]=0x01;
    }
    else
    {
        ucTemp[2]=0x18;
        ucTemp[1]=0x13;
        ucTemp[0]=0x01;
    }
    gtp_i2c_write2(0x4062c,3,ucTemp,3);
    if(i2c_wire_num==1)
    {           
        ucTemp[0]=0x03;
    }
    else if(i2c_wire_num==2)
    {
        ucTemp[0]=0x3b;
    }
    else if(i2c_wire_num==4)
    {
        ucTemp[0]=0x6b;
    }
    else
    {
        ucTemp[0]=0x0b;
    }   
    gtp_i2c_write2(0x40630,3,ucTemp,1);
    ucTemp[2]=(unsigned char)(flash_address>>16);
    ucTemp[1]=(unsigned char)(flash_address>>8);
    ucTemp[0]=(unsigned char)(flash_address);
    gtp_i2c_write2(0x40634,3,ucTemp,3);
    ucTemp[2]=(unsigned char)(sram_address>>16);
    ucTemp[1]=(unsigned char)(sram_address>>8);
    ucTemp[0]=(unsigned char)(sram_address);
    gtp_i2c_write2(0x4063c,3,ucTemp,3);
    
    ucTemp[1]=(unsigned char)(copy_length>>8);
    ucTemp[0]=(unsigned char)(copy_length);
    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=0x01;

    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while(ucTemp[0])
    {
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }

}

#endif

int  icn85xx_prog_data(unsigned int flash_addr, unsigned int data)
{
    unsigned char ucTemp[4] = {0,0,0,0};
    
    ucTemp[3]=(unsigned char)(data>>24);
    ucTemp[2]=(unsigned char)(data>>16);
    ucTemp[1]=(unsigned char)(data>>8);
    ucTemp[0]=(unsigned char)(data);
    
    gtp_i2c_write2(0x7f00,3,ucTemp,4);
    icn85xx_prog_buffer(flash_addr , 0x7f00, 0x04,  0);
    return 0;
}


int  icn85xx_read_flashid(void)
{
    unsigned char ucTemp[4] = {0,0,0,0};
    int flashid=0;
    unsigned char i = 0;
    unsigned int while_count = 0;

    ucTemp[2]=0x08;
    ucTemp[1]=0x10;
    ucTemp[0]=0x00;
    gtp_i2c_write2(0x4062c,3,ucTemp,3);
    ucTemp[0]=0x9f;

    gtp_i2c_write2(0x40630,3,ucTemp,1);
    ucTemp[1] = 0x00;
    ucTemp[0] = 0x03;

    gtp_i2c_write2(0x40640,3,ucTemp,2);
    ucTemp[0]=1;
    gtp_i2c_write2(0x40644,3,ucTemp,1);
    while( ucTemp[0] && while_count < 100000 )
    {
        while_count ++;
        gtp_i2c_read2(0x40644,3,ucTemp,1);
    }

    for (i = 0; i < 4; i ++)
    {
        gtp_i2c_read2(0x40648+i,3,&ucTemp[i],1);
    }

    memcpy( (void*)&flashid, ucTemp, 4 );

    flashid=flashid&0x00ffffff;

    return flashid;
}



static int  icn85xx_fw_update(char *program,int programLen)
{
    int last_length;
    int j, num;
    unsigned char temp_buf[B_SIZE];
    unsigned int crc_fw;
    char* update_file_buff;
    if(icn85xx_goto_progmode() != 0)
    {
       printf("icn85xx_goto_progmode() != 0 error\n");
       return -1; 
    } 
    usleep(1000); 
    icn85xx_read_flashid();
    icn85xx_crc_enable(1);
    printf("icn85xx_fw_download start\n");
    crc_fw = 0;

    update_file_buff = program;
    num = programLen/B_SIZE;   

    for(j=0; j < num; j++)
    {

        //memset(temp_buf, 0, B_SIZE);
        memcpy(temp_buf,update_file_buff+j*B_SIZE, B_SIZE);
        crc_fw = icn85xx_crc_calc(crc_fw, temp_buf, B_SIZE, crc32table);
        if(icn85xx_fw_download(j*B_SIZE, temp_buf, B_SIZE) != 0)
        {
            printf("error j:%d\n",j);
            return -1;
        }    
    }
    last_length = programLen - B_SIZE*j;
    if(last_length > 0)
    {
        //memset(temp_buf, 0, last_length);
        memcpy(temp_buf,update_file_buff+j*B_SIZE, last_length);

        crc_fw = icn85xx_crc_calc(crc_fw, temp_buf, last_length, crc32table);
        if(icn85xx_fw_download(j*B_SIZE, temp_buf, last_length) != 0)
        {
            printf("error last length\n");
            return -1;
        }
    }    

    icn85xx_crc_enable(0);
    if(icn85xx_crc_check(crc_fw, programLen) != 0)
    {
        printf("down fw error, crc error\n");
        return -1;
    }
    else
    {
        printf("downoad fw ok, crc ok\n");
    }
    printf("icn85xx_fw_download ok\n"); 

    //…’–¥µΩflash
    printf("download to flash\n");
    icn85xx_erase_flash();
    FlashWriteEnable();
    icn85xx_prog_buffer( 0, 0, programLen,0);
    while((FlashState(0)&0x01));

    FlashWriteEnable();
    icn85xx_prog_data(FLASH_CRC_ADDR, crc_fw);
    icn85xx_prog_data(FLASH_CRC_ADDR+4, programLen);

    icn85xx_crc_enable(1);   
    icn85xx_read_flash( 0,  0, programLen,  2);    
    icn85xx_crc_enable(0);
    if(icn85xx_crc_check(crc_fw,programLen) != 0)
    {
        printf("read flash data error, crc error\n");
        return -1;
    }
    else
    {
        printf("read flash data ok, crc ok\n");
    }                  
    while((FlashState(0)&0x01));

    if(icn85xx_bootfrom_sram() != 0)	//code already in ram
    {
        printf("icn85xx_bootfrom_flash error\n");
        return -1;
    }

    printf("icn85xx upgrade ok\n");
    return 0;
}

#ifndef CONFIG_ICN8826_FW_PATH
#define CONFIG_ICN8826_FW_PATH "A:\\"
#endif
static int _tpDoInitProgram_vendor(void)
{
	int ret;
	char buffer[128];
    FN_FIND find;
    strcpy(buffer,CONFIG_ICN8826_FW_PATH);
    strcat(buffer,"TP_ICN8826_*.bin");
    ret = f_findfirst(buffer, &find);
    if (ret)
    {
        printf("f_findfirst:%d\n",ret);
        return -1;
    }
    strcpy(buffer,CONFIG_ICN8826_FW_PATH);
    strcat(buffer,find.filename);
    if(buffer[0])
    {
        char *ver = NULL;
        char fileName[64];
        strcpy(fileName,buffer);
        strtok(fileName,"_.");
        strtok(NULL,"_.");
        ver = strtok(NULL,"_.");
        printf("find :%s\nver :%s\n",buffer,ver);
        if(ver != NULL)
        {
            int verNum = strtoul(ver,NULL,16);
            if(is_icn8826_update(verNum))
            {
                FN_FILE *fp = f_open(buffer, "r");
                int fileLen; 
                char *binData; 
                f_seek(fp,0,SEEK_END);
                fileLen = f_tell(fp);
                f_seek(fp,0,SEEK_SET);
                binData = malloc(fileLen);
                f_read(binData,1 ,fileLen,fp);
                icn85xx_fw_update(binData,fileLen);
                free(binData);
                f_close(fp);
            
            }
        }
    }

	printf("CFG_RawX=%d, CFG_RawY=%d\n",gTpSpec.tpMaxRawX,gTpSpec.tpMaxRawY);

    return 0;
}


static bool _tpChkIntActive(void)
{
	unsigned int regValue = ithGpioGet(TP_INT_PIN);
	
    if(gTpSpec.tpIntActiveState)
    {
    	if ( regValue & TP_GPIO_MASK )	return true;
    	else	return false;	    
    }
    else
    {
    	if ( !(regValue & TP_GPIO_MASK) )	return true;
    	else	return false;	
    }
}

static int _tpReadPointBuffer_vendor(unsigned char *buf, int cnt)
{
    bool recv = true;
    int ret;
    int real_nr=0;
    uint8_t tmpBuf[1]={0};
    static int downFlag = 0;
    
    ret = gtp_i2c_read3(0x1008,2, tmpBuf, 1);
    if (ret < 0)
    {
        printf("I2C transfer error. errno:%d\n ", ret);
        recv = false;
        goto end;
    }
#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("	------tmpBuf=%02x-------\r\n",tmpBuf[0]);
#endif
    if((tmpBuf[0] == 0 || tmpBuf[0] == 4) && downFlag == 0)
    {
        recv = false;
        goto end;
    }
    if(tmpBuf[0] == 4 || tmpBuf[0] == 0)
    {
        buf[0]= 0;
        downFlag = 0;
    }
    else
    {
        buf[0]= 1;
        downFlag = 1;
    }
    ret = gtp_i2c_read3(0x1004,2, buf+1, 1);
    if (ret < 0)
    {
        printf("I2C transfer error. errno:%d\n ", ret);
        recv = false;
        goto end;
    }
    ret = gtp_i2c_read3(0x1003,2, buf+2, 1);
    if (ret < 0)
    {
        printf("I2C transfer error. errno:%d\n ", ret);
        recv = false;
        goto end;
    }
    ret = gtp_i2c_read3(0x1006,2, buf+3, 1);
    if (ret < 0)
    {
        printf("I2C transfer error. errno:%d\n ", ret);
        recv = false;
        goto end;
    }
    ret = gtp_i2c_read3(0x1005,2, buf+4, 1);
    if (ret < 0)
    {
        printf("I2C transfer error. errno:%d\n ", ret);
        recv = false;
        goto end;
    }
end:
    if(recv==true)  return 0;
	else            return -1;
}                               

static int _tpParseRawPxy_vendor(struct ts_sample *s, unsigned char *buf)
{
	//if(g_tchCoorX2Y)
		s->x = (short)(((unsigned int)buf[1]<<8)+buf[2]);
		s->y = (short)(((unsigned int)buf[3]<<8)+buf[4]);
    s->pressure = buf[0];		//it`s just Setting to one point to touch.		
    //printf("pressure : %d,x:%d,y:%d\n",s->pressure,s->x,s->y);
#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("	RAW->-----> %d %d %d\n", s->pressure, s->x, s->y);
#endif

    gettimeofday(&s->tv,NULL);
    return 0;
}

static void _tpParseKey_vendor(struct ts_sample *s, unsigned char *buf)
{
    //TODO: get key information and input to xy sample...? as a special xy?
    //maybe define a special area for key
    //(like touch is 800x480, for example, y>500 for key, x=0~100 for keyA, x=100~200 for keyB... )
    //SDL layer could parse this special defination xy into key event(but this layer is not ready yet).
}

static void _tpIntActiveRule_vendor(struct ts_sample *tpSmp)
{
    gTpInfo.tpIntrCnt = 0;
    gTpInfo.tpNeedUpdateSample = 0;
    
    if(!gTpSpec.tpIntUseIsr)
    {
        //for prevent from the issue that polling INT signal will get the same sample.
        if(!gTpInfo.tpNeedToGetSample)	return;
        else    gTpInfo.tpNeedToGetSample = 0;
    }
    
    //status rule for TOUCH_DOWN/TOUCH_UP/TOUCH_NO_CONTACT
	switch(gTpInfo.tpStatus)
	{
		case TOUCH_NO_CONTACT:
			if(tpSmp->pressure)
			{
				gTpInfo.tpStatus = TOUCH_DOWN;
				gTpInfo.tpIntr4Probe = 1;
				gTpInfo.tpNeedUpdateSample = 1;
				gTpInfo.tpFirstSampHasSend = 0;
			}
			break;
		
		case TOUCH_DOWN:
			if(!tpSmp->pressure)
			{
				gTpInfo.tpStatus = TOUCH_UP;
			}				
			if(gTpInfo.tpFirstSampHasSend)	gTpInfo.tpNeedUpdateSample = 1;
			break;
			
		case TOUCH_UP:
			if(!tpSmp->pressure)
			{
				gTpInfo.tpStatus = TOUCH_NO_CONTACT;
				gTpInfo.tpIntr4Probe = 0;
			}
			else
			{
				gTpInfo.tpStatus = TOUCH_DOWN;
				gTpInfo.tpIntr4Probe = 1;
				gTpInfo.tpNeedUpdateSample = 1;
			}
			break;
			
		default:
			printf("ERROR touch STATUS, need to check it!!\n");
			break;				
	}

	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	printf("	tpStatus=%x, NSQ=%x, cINT=%x, send=%x\n", gTpInfo.tpStatus, gTpInfo.tpNeedUpdateSample, gTpInfo.tpIntr4Probe, gTpInfo.tpFirstSampHasSend);
	#endif		
	
	//use this flag to judge if update the touch sample
	//1.have to update the first TOUCH_DOWN event
	//2.don't update the touch event if UI does not get the first event
	//3.real-time update the X,Y point after send the 1st event
	//4.must send the touch event if last status is touch-up, and INT active again in this time.
	//  to handle the quickly touch case.
	//5.others...
	if(gTpInfo.tpNeedUpdateSample)
	{
		_tpUpdateLastXY(tpSmp);
	}		
	
	if(gTpSpec.tpIntUseIsr)
	{
	    //clear INT flag and enable interrupt if use ISR to handle INT signal
	    g_TouchDownIntr = 0;
	    ithGpioEnableIntr(TP_INT_PIN); 
	}
}

static void _tpIntNotActiveRule_vendor(struct ts_sample *tpSmp)
{
    if(!gTpSpec.tpIntUseIsr)
    {
        //if INT not active, then set this flag to call _tpGetSample() if next INT active
	    gTpInfo.tpNeedToGetSample = 1;
	}
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	if( (gTpInfo.tpStatus != TOUCH_NO_CONTACT) )
		printf("	UpdateSmp0:INT=%x, ss=(%d,%d)\n",gTpInfo.tpCurrINT, gTpInfo.tpStatus, gTpInfo.tpFirstSampHasSend);
	#endif
	
	//In order to prevent from loss of the first touch event
	//Need To set "status=TOUCH_NO_CONTACT" if "last status=TOUCH_UP" + "first sample has send"
	if( (gTpInfo.tpStatus == TOUCH_UP) && (gTpInfo.tpFirstSampHasSend) )
	{
        _tpUpdateLastXY(NULL);
	    gTpInfo.tpStatus = TOUCH_NO_CONTACT;
	    gTpInfo.tpIntr4Probe = 0;

		#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
		printf("INT=0, force to set status=0!!\n");
		#endif
	}
	
	//For prevent from lossing the touch-up event
	//sometimes, S/W can not get TOUCH-UP event when INT is actived
	//So, this code will force to set touch-status as TOUCH_UP after INT is not actived for a specific time(16ms)
	if( gTpSpec.tpNeedAutoTouchUp && (gTpInfo.tpStatus == TOUCH_DOWN) && (_tpGetIntr()==false) )
	{
	    //printf("	UdSmp:s=%d, int=%x, ic=%d\n",gTpInfo.tpStatus,gTpInfo.tpCurrINT,gTpInfo.tpIntrCnt);
	    
		if(!gTpInfo.tpIntrCnt)	gettimeofday(&T1,NULL);
		gettimeofday(&T2,NULL);
		dur = (unsigned int)itpTimevalDiff(&T1, &T2);	

		if( gTpInfo.tpFirstSampHasSend && (gTpInfo.tpIntrCnt > 3) )
		{
			//when first smaple has send, or main-loop idle over 33 ms.
			//for fixing the FT5XXX's issue that sometimes it cannot get the TOUCH_UP EVENT
			//and need "gTpInfo.tpIntrCnt" > 3 times to prevent from main task idle issue
			if( (gTpSpec.tpIntrType == TP_INT_TYPE_ZT2083) || (dur > gTpSpec.tpSampleRate) )
			{
				//FORCE TOUCH_UP if TP_INT_TYPE_ZT2083 or dur > one-sample-rate-time
				gTpInfo.tpStatus = TOUCH_UP;
				gTpInfo.tpIntr4Probe = 0;
				_tpUpdateLastXY(NULL);					
				
				#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
				printf("INT=0, and dur>%dms, so force to set status=2!!\n",gTpSpec.tpSampleRate);
				#endif
				//printf("INT=0, and dur>%dms, force TOUCH_UP!!\n",gTpSpec.tpSampleRate);
			}
		}
		
		gTpInfo.tpIntrCnt++;
	}

	//to handle the INT actived, but g_TouchDownIntr doesn't become true.
	//need send a i2c read command to clear INT for IT7260.
	//If INT will keep active state until I2C send command to TP IC for clearing INT active state(like IT7260).
	//Then this workaround will be necessary for fixing the issue 
	//which TP's INT signal has NO response after suspend mode
    if(gTpSpec.tpIntrType == TP_INT_TYPE_IT7260)
    {
        //_tpFixIntHasNoResponseIssue();
    	if( gTpSpec.tpIntUseIsr && (_tpChkIntActive()==true) )
    	{
    	    if(!g_IntrLowCnt++)	gettimeofday(&tv1,NULL);
    	    gettimeofday(&tv2,NULL);
    	    dur = (unsigned int)itpTimevalDiff(&tv1, &tv2);	
    	    
    	    if(dur>33)
    	    {
     			unsigned char *buf = (unsigned char *)malloc(gTpSpec.tpReadChipRegCnt);
     			memset(buf, 0, gTpSpec.tpReadChipRegCnt);
     			if(_tpReadPointBuffer_vendor(buf, gTpSpec.tpReadChipRegCnt)<0)
     			{
     			    //don't care this error
     			}
     			g_IntrLowCnt = 0;
     			free(buf);
     			printf("read Sample while INT is active\n");
    		}
    	}
    	else
    	{
    	    g_IntrLowCnt = 0;
    	}
    }
}

/*##################################################################################
 *                middle APIs for handling raw x,y data
 ###################################################################################*/
#ifdef CFG_TOUCH_INTR
static void _tp_isr(void* data)
{	
	unsigned int regValue;
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//ithPrintf("$in\n");
	#endif

	g_TouchDownIntr = true;

    ithGpioClearIntr(TP_INT_PIN);
    if(gTpSpec.tpIntrType == TP_INT_TYPE_KEEP_STATE)
    {
        ithGpioDisableIntr(TP_INT_PIN); 
    }
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	//ithPrintf("$out(%x)\n",g_TouchDownIntr);
	#endif
}

static void _initTouchIntr(void)
{
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("TP init in\n");	
    #endif
    
    ithEnterCritical();
    
    ithGpioClearIntr(TP_INT_PIN);
    ithGpioRegisterIntrHandler(TP_INT_PIN, _tp_isr, NULL);
        
    if(gTpSpec.tpIntTriggerType==TP_INT_LEVLE_TRIGGER)     
        ithGpioCtrlEnable(TP_INT_PIN, ITH_GPIO_INTR_LEVELTRIGGER);
    else
        ithGpioCtrlDisable(TP_INT_PIN, ITH_GPIO_INTR_LEVELTRIGGER);
        
    if(gTpSpec.tpIntTriggerType==TP_INT_EDGE_TRIGGER)  //if edge trigger
        ithGpioCtrlDisable(TP_INT_PIN, ITH_GPIO_INTR_BOTHEDGE);		//set as single edge
//    else
//        ithGpioCtrlEnable(TP_INT_PIN, ITH_GPIO_INTR_BOTHEDGE);		//set as single edge
    
    if(gTpSpec.tpIntActiveState==TP_ACTIVE_HIGH)    
        ithGpioCtrlDisable(TP_INT_PIN, ITH_GPIO_INTR_TRIGGERFALLING);	//set as rising edge
    else
        ithGpioCtrlEnable(TP_INT_PIN, ITH_GPIO_INTR_TRIGGERFALLING);	//set as falling edge

    ithIntrEnableIrq(ITH_INTR_GPIO);
    ithGpioEnableIntr(TP_INT_PIN);
        
    ithExitCritical();
    
    #ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    printf("TP init out\n");	
    #endif
}
#endif

static void _tpInitWakeUpPin(void)
{
	if( (gTpSpec.tpWakeUpPin>0) && (gTpSpec.tpWakeUpPin<128) )
	{
		ithGpioSetMode(gTpSpec.tpWakeUpPin,ITH_GPIO_MODE0);
   		ithGpioSetOut(gTpSpec.tpWakeUpPin);
   		ithGpioSet(gTpSpec.tpWakeUpPin);    	
   		ithGpioEnable(gTpSpec.tpWakeUpPin);
   	}
	else
	{
		printf("NOT initial TOUCH_GPIO_WAKE_PIN\n");
	}
}

static void _tpInitResetPin(void)
{
	if( (gTpSpec.tpResetPin>0) && (gTpSpec.tpResetPin<128) )
	{
		ithGpioSetMode(gTpSpec.tpResetPin,ITH_GPIO_MODE0);
   		ithGpioSetOut(gTpSpec.tpResetPin);
   		ithGpioSet(gTpSpec.tpResetPin);    	
   		ithGpioEnable(gTpSpec.tpResetPin);
   	}
	else
	{
		printf("NOT initial TOUCH_RESET_PIN(%d)\n",gTpSpec.tpResetPin);
	}
}

static void _tpInitIntPin(void)
{
	ithGpioSetMode(TP_INT_PIN, ITH_GPIO_MODE0);
	ithGpioSetIn(TP_INT_PIN);

	if(gTpSpec.tpIntPullEnable)
	{
		if(gTpSpec.tpIntActiveState)    ithGpioCtrlDisable(TP_INT_PIN, ITH_GPIO_PULL_UP);
		else    ithGpioCtrlEnable(TP_INT_PIN, ITH_GPIO_PULL_UP);
			
		ithGpioCtrlEnable(TP_INT_PIN, ITH_GPIO_PULL_ENABLE);		
	}
    
	ithGpioEnable(TP_INT_PIN);	
}

static void _tpInitTouchGpioPin(void)
{
	_tpInitWakeUpPin();
	
	_tpInitResetPin();
	
	_tpInitIntPin();
	
	#ifdef CFG_TOUCH_INTR
	_initTouchIntr();
	#endif
}

static char _tpGetIntr(void)
{
    if(gTpSpec.tpIntUseIsr)
    {
        //printf("GetInt1:%x\n",g_TouchDownIntr);
        return g_TouchDownIntr;
    }
    else
    {
    	return _tpChkIntActive();
    }
}

static void _initSample(struct ts_sample *s, int nr)
{
	int i;
	struct ts_sample *samp=s;	
	
	for(i = 0; i < nr; i++)
	{
		samp->x = 0;
		samp->y = 0;
		samp->pressure = 0;
		gettimeofday(&(samp->tv),NULL);		
		samp++;
	}
}

static int _tpGetRawPoint(struct ts_sample *samp, int nr)
{
	int real_nr=0;
	struct ts_sample *s=samp;
	unsigned char *buf = (unsigned char *)malloc(gTpSpec.tpReadChipRegCnt);
	
	_initSample(s, nr);	
	memset(buf, 0, gTpSpec.tpReadChipRegCnt);
	
	while(real_nr++<nr) 
	{
		if(_tpReadPointBuffer_vendor(buf, gTpSpec.tpReadChipRegCnt)<0)	break;
		
		if(_tpParseRawPxy_vendor(s, buf)<0)	break;
		
		if(gTpSpec.tpHasTouchKey)	_tpParseKey_vendor(s, buf);

		s++;
	}
	free(buf);
	return 0;
}

static void _tpConvertRawPoint(struct ts_sample *samp, int nr)
{
	int real_nr=0;
	int tmpMaxRawX = gTpSpec.tpMaxRawX;
	int tmpMaxRawY = gTpSpec.tpMaxRawY;
	struct ts_sample *s=samp;
	
	#ifdef ENABLE_TOUCH_RAW_POINT_MSG
	printf("	CvtRawPnt:%x,%d,%d, [%x,%x,%x]\n",s->pressure,s->x,s->y, gTpSpec.tpCvtSwapXY,gTpSpec.tpCvtReverseX,gTpSpec.tpCvtReverseY);	
	#endif
	
    while(real_nr++<nr)
    {
        if(gTpSpec.tpCvtSwapXY)
        {
            int tmp = s->x;
            s->x = s->y;
            s->y = tmp;
            tmpMaxRawX = gTpSpec.tpMaxRawY;
            tmpMaxRawY = gTpSpec.tpMaxRawX;
        }

        if( (gTpSpec.tpHasTouchKey) && (s->y > tmpMaxRawX) )	
        {
            s++;
		    real_nr++;
            continue;//NEED TO VERIFY THIS CODE
        }
        
        if(gTpSpec.tpCvtReverseX)
        {
            if(s->x>=tmpMaxRawX)	s->x = 0;
            else					s->x = tmpMaxRawX - s->x;
        }

        if(gTpSpec.tpCvtReverseY)
        {
            if(s->y>=tmpMaxRawY)	s->y = 0;
            else					s->y = tmpMaxRawY - s->y;
        }

        if(gTpSpec.tpCvtScaleX)
        {
            s->x = (short)(((uint32_t)s->x*gTpSpec.tpScreenX)/tmpMaxRawX);
        }
        
        if(gTpSpec.tpCvtScaleY)
        {
            s->y = (short)(((uint32_t)s->y*gTpSpec.tpScreenY)/tmpMaxRawY);
        }
    			
/*
    	if( (s->x>=gTpSpec.tpScreenX) || (s->y>=gTpSpec.tpScreenY) || (s->x<0) || (s->y<0) )
    		printf("[TP warning] XY are abnormal, x=%d,%d y=%d,%d\n",s->x,gTpSpec.tpScreenX,s->y,gTpSpec.tpScreenY);
    			
    	if(s->x>=gTpSpec.tpScreenX)	s->x = gTpSpec.tpScreenX - 1;
    	if(s->y>=gTpSpec.tpScreenY)	s->y = gTpSpec.tpScreenY - 1;
    			
    	if(s->x<0)	s->x = 0;
    	if(s->y<0)	s->y = 0;
    				
    	//printf("modify x,y = %d, %d -##\n",s->x,s->y);		
*/ 
		
		#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
       	printf("	cvtPXY--> %d %d %d\n", s->pressure, s->x, s->y);
		#endif
        
		s++;
    }
}

static int  _tpGetSample(struct ts_sample *samp, int nr)
{
	int real_nr=0;
	struct ts_sample *s=samp;
	
	while(real_nr++<nr) 
	{
		if(_tpGetRawPoint(s, 1) < 0)
		{
		    return -1;
		}
		
		_tpConvertRawPoint(s, 1);
		
		s++;
	}
    return 0;
}

static void _tpUpdateLastXY(struct ts_sample *smp)
{
	pthread_mutex_lock(&gTpMutex);
	if(smp!=NULL)
	{
        memcpy((void *)&g_sample[0] ,(void *)smp, sizeof(struct ts_sample));
	}
	else
	{
        memset((void *)&g_sample[0] , 0, sizeof(struct ts_sample));
	}
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	if(smp!=NULL)	printf("	EnQue:p=%x,xy=%d,%d\n", smp->pressure, smp->x, smp->y);
	else            printf("	EnQue:p=%x,xy=%d,%d\n", 0, 0, 0);
	#endif							
	pthread_mutex_unlock(&gTpMutex);
}

/**
 * to judge if S/W needs to get touch point
 *
 * @return: true for YES, false for NO
 * 
 * [NOTE]:return true if need to get touch sample via i2c bus(ex: when TP's INT signal actived, or other special rules)
          return false if NO need to get sample(NO TP's INT actived, or other special rules)
          in usual case, return true for INT actived, false for INT not actived..
          special rule1: To prevent from getting the same sample, when use "pulling INT" without interrupt.
          special rule2: DO NOT wanna loss the first point when finger just touch down(or quickly touch)
 */
static bool _tpNeedToGetSample(void)
{
    if(g_IsTpInitialized!=true)	return false;

    if(_tpGetIntr()==true)
    {
        gTpInfo.tpCurrINT = 1;
        
    	if(gTpSpec.tpIntUseIsr)
    	{
    		if(gTpSpec.tpIntrType == TP_INT_TYPE_ZT2083)
    		{
		    	if(!g_IntrLowCnt++)	gettimeofday(&tv1,NULL);
		    	gettimeofday(&tv2,NULL);
		    	dur = (unsigned int)itpTimevalDiff(&tv1, &tv2);
		    	
		    	if(dur>16)
		    	{
		    		//printf("	NGS:%x,%d,%d\n", gTpInfo.tpNeedToGetSample, g_IntrLowCnt, dur);	
	    			//printf("	  -ztInt:%x,%x\n",regValue,regValue & TP_GPIO_MASK);
	    			g_IntrLowCnt = 0;
		    		gTpInfo.tpNeedToGetSample = 1;
		    		ithGpioEnableIntr(TP_INT_PIN); 
		    	}
		    	
	    		if ( gTpInfo.tpNeedToGetSample )	return true;
	    		else	return false;
    		}
    		else
    		{
	    		ithGpioDisableIntr(TP_INT_PIN);
    	        return true;
    		}
    	}
    	else
    	{
        	if(gTpSpec.tpIntrType == TP_INT_TYPE_ZT2083)
        	{
				//printf("IAR2:%x,%x,%d,%d\n",gTpInfo.tpNeedToGetSample,gTpInfo.tpFirstSampHasSend,g_IntrLowCnt,dur);
				if(gTpInfo.tpFirstSampHasSend)
				{
			    	if(!g_IntrLowCnt++)	gettimeofday(&tv1,NULL);
			    	gettimeofday(&tv2,NULL);
			    	dur = (unsigned int)itpTimevalDiff(&tv1, &tv2);	

			    	if(dur>16)
			    	{
			    		gTpInfo.tpNeedToGetSample = 1;
			    		g_IntrLowCnt = 0;
			    	}
				}
				else
				{
					gTpInfo.tpNeedToGetSample = 1;
				}
        	}
 
        	if( gTpInfo.tpNeedToGetSample )    return true;	
        	else    return false;    	
    	}
    }
    else
    {
        gTpInfo.tpCurrINT = 0;
        return false;
    }
}

/**
 * to update the touch status
 * 
 * [HINT 1]: when use "pulling INT", remember to prevent from getting the same sample
 * [HINT 2]: DO NOT loss the first point(each point) in quickly clicking case.
 * [HINT 3]: if not initial yet, TP will sleep 100ms
 * [HINT 4]: Basically, INT is active, then report "TOUCH DOWN" event; INT is not active, then report "TOUCH UP" event
 * [HINT 5]: INT has 2 action type:
    1). pull low until finger is not touch(contact) on TP
    2). pull low as a pulse which width about 2~20 micro-second until finger is not touch(contact) on TP
*/
static void _tpUpdate(struct ts_sample *tpSmp)
{
    if(g_IsTpInitialized!=true)
    {
        printf("WARNING:: TP has not initial, yet~~~\n");
        usleep(TP_IDLE_TIME_NO_INITIAL);
        return;
    }
    
    #ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
    if(gTpInfo.tpStatus != TOUCH_NO_CONTACT)
    	printf("	UpdateSmp:INT=%x, s=%x pxy=(%d,%d,%d)\n",gTpInfo.tpCurrINT, gTpInfo.tpStatus, tpSmp->pressure,tpSmp->x,tpSmp->y);
    #endif
    
   //_tpIntActiveRule_vendor(tpSmp);
    
    _tpUpdateLastXY(tpSmp);
    gTpInfo.tpIntr4Probe = 1;
}

/**
 * the thread for handling real-time touch event(<2ms)
 * 
 * [HINT]: use g_sample for comunication with function "_tpProbeSample()"
 */ 
static void* _tpProbeHandler(void* arg)
{
    struct ts_sample tpSmp[MAX_FINGER_NUM];
    
    while(1)
    {
        if(_tpGetSample(&tpSmp[0], 1) == 0)
        {
            _tpUpdate(&tpSmp[0]);   
        }
        usleep(TP_IDLE_TIME);  
    }
    return NULL;
}

/******************************************************************************
 * do initial flow
 ******************************************************************************/
/**
 * TP initial flow
 *
 * @return: 0 for success, -1 for failure
 *
  */ 
static int _tpDoInitial(void)
{
	int ret;
    int res;
    pthread_t task;
    pthread_attr_t attr;
    
    //initialize the TP SPEC first.
    _tpInitSpec_vendor();
    	
    if(gTpSpec.tpHasPowerOnSeq)     _tpDoPowerOnSeq_vendor();
    
    if(gTpSpec.tpNeedProgB4Init)
    {
    	if(_tpDoInitProgram_vendor()<0)
    	{
    		printf("[TOUCH]warning:: touch panel do initial progeram chip fail\n");
    		return -1;
    	}
	}

    printf("try to init GPIO pin\n");  
    _tpInitTouchGpioPin();
    
    //create touch mutex	
    res = pthread_mutex_init(&gTpMutex, NULL);
   	if(res)
   	{
   	    printf("[Touch Panel]%s() L#%ld: ERROR, init touch mutex fail! res=%ld\r\n", __FUNCTION__, __LINE__, res);
   	    return -1;
   	}

    printf("Create touch pthread~~\n");	
	//create thread	
    pthread_attr_init(&attr);
    res = pthread_create(&task, &attr, _tpProbeHandler, NULL);        
    if(res)
    {
    	printf( "[TouchPanel]%s() L#%ld: ERROR, create _tpProbeHandler() thread fail! res=%ld\n", res );
    	return -1;
    }
	
    g_TouchDownIntr = false;
	g_IsTpInitialized = true;
	printf("TP initial has finished(%d)\n",g_IsTpInitialized);  
	
	return 0;
}

/**
 * Send touch sample(samp->pressure, samp->x, samp->y, and samp->tv)
 *
 * @param samp: the touch samples
 * @param nr: the sample count that upper layer wanna get.
 * @return: the really total touch sample count
 *
 * [HINT 1]:this function will be called by it7260_read(). 
 * [HINT 2]:get the samples from the global variable(g_sample).
 */ 
static int _tpProbeSample(struct ts_sample *samp, int nr)
{
	int ret=0;
	struct ts_sample *s=samp;

	pthread_mutex_lock(&gTpMutex);	
	
	if(gTpInfo.tpIntr4Probe)
	{
        while(ret < nr)
        {
       	    memcpy((void *)s,(void *)&g_sample[0], sizeof(struct ts_sample));
       	    gettimeofday(&s->tv,NULL);
       	    if(s->pressure)	gTpInfo.tpFirstSampHasSend = 1;       	    
       	    
       	    ret++;	    
        }
        //printf("	gfQ, r=(%d,%d) pxy=(%d,%d,%d)\n", ret, nr, s->pressure, s->x, s->y);
	}
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	printf("gfQ, r=%x, INT=%x, s=%x, pxy=(%d,%d,%d)\n", ret, gTpInfo.tpIntr4Probe, gTpInfo.tpStatus, s->pressure, s->x, s->y);
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

/**
 * Send touch sample(samp->pressure, samp->x, samp->y, and samp->tv)
 *
 * @param inf: the module information of tslibo(just need to care "inf->dev")
 * @param samp: the touch samples
 * @param nr: the sample count that upper layer wanna get.
 * @return: the total touch sample count
 *
 * [HINT 1]:this function will be called by SDL every 33 ms. 
 * [HINT 2]:Upper layer(SDL) will judge finger-down(contact on TP) if samp->pressure>0, 
 *          finger-up(no touch) if samp->pressure=0.
 */ 
static int icn8826_read(struct tslib_module_info *inf, struct ts_sample *samp, int nr)
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
		printf("TP first init(INT is GPIO %d)\n",TP_INT_PIN);
		gTpInfo.tpDevFd = tchdev;	
		if(!_tpDoInitial())	return -1;
		else                return 0;
	}
	
	_initSample(s, nr);
	
	//to probe touch sample 
	ret = _tpProbeSample(samp, nr);
	
	#ifdef	ENABLE_TOUCH_PANEL_DBG_MSG
	if(ret)	printf("    deQue-O:(%d,%d,%d)r=%d\n", samp->pressure, samp->x, samp->y, ret);
	#endif

	return nr;
}

static const struct tslib_ops icn8826_ops =
{
	icn8826_read,
};

TSAPI struct tslib_module_info *icn8826_mod_init(struct tsdev *dev, const char *params)
{
	struct tslib_module_info *m;

	m = malloc(sizeof(struct tslib_module_info));
	if (m == NULL)
		return NULL;

	m->ops = &icn8826_ops;
	return m;
}

#ifndef TSLIB_STATIC_CASTOR3_MODULE
	TSLIB_MODULE_INIT(icn8826_mod_init);
#endif
