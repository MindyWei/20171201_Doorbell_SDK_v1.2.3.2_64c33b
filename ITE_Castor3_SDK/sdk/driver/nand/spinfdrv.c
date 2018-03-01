/*
 * Copyright (c) 2015 iTE Corp. All Rights Reserved.
 */
/** @file
 * SMedia SPI NAND Driver API header file.
 *
 */
 
#include "spinfdrv.h"

#ifdef USE_MMP_DRIVER
#include "ssp/mmp_spi.h"
#else
#include <stdio.h>
#include <string.h>
#include "stdint.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

//#define ENABLE_THREAD
//#define ENABLE_DUMP_VCD
//#define ENABLE_WRITE_THREAD_QUEUE
#define ENABLE_PATCH_WINBOND_SPARE_ISSUE
//=============================================================================
//                              Include Files
//=============================================================================
#ifdef ENABLE_THREAD
#include <pthread.h>
#define MAX_NF_PRIORITY 5
#endif
/***************************
 *
 **************************/ 

#ifdef	USE_MMP_DRIVER
#ifdef	CFG_SPI_NAND_USE_SPI0
#define SPI_PORT	SPI_0
#endif

#ifdef	CFG_SPI_NAND_USE_SPI1
#define SPI_PORT	SPI_1
#endif

#ifndef SPI_PORT
    #error "ERROR: spinfdrv.c MUST define the SPI NAND bus(SPI0 or SPI1)"
#endif

#if (CFG_CHIP_FAMILY == 9850)
    #define SPI_OK		(1)	//(true)
#else
    #define SPI_OK		(0)	//(true)
    #define ENABLE_CTRL_1
#endif
#else
#define SPI_OK		(0)	//(true)
#endif

#define SPINF_OK		(0)	//(true)
#define SPINF_FAIL		(1)	//(true)

#ifdef ENABLE_WRITE_THREAD_QUEUE
#define MAX_QUEUE_SIZE  (128)
#define MAX_NF_PRIORITY 4
#endif
//=============================================================================
//                              spi Function WRAP
//=============================================================================
#ifdef ENABLE_WRITE_THREAD_QUEUE
typedef struct write_queue_tag { /* Used in the IBM Arctic II */
	uint32_t blk;
	uint32_t ppo;
	uint8_t  *ptr;
}MY_WT_QUE;
#endif

//=============================================================================
//                              spi Function WRAP
//=============================================================================
static SPI_NF_INFO gSpiNfInfo;

static SPINF_CFG g_SpiNfCfgArray[] = 
{
	{ "GD_5F1GQ4UC", 0xC8, 0xB1, 0x48, 1, 2048, 64, 1024, 128 },
	{ "GD_5F2GQ4UC", 0xC8, 0xB2, 0x48, 1, 2048, 64, 2048, 128 },
	{ "GD_5F4GQ4UC", 0xC8, 0xB4, 0x68, 1, 4096, 64, 2048, 256 },
	{ "GD_5F1GQ4UB", 0xC8, 0xD1, 0x00, 1, 2048, 64, 1024, 128 },
	{ "GD_5F2GQ4UB", 0xC8, 0xD2, 0x00, 1, 2048, 64, 2048, 128 },
	{ "MX35LF1GE4AB", 0xC2, 0x12, 0x00, 2, 2048, 64, 1024, 128 },
	{ "MX35LF2GE4AB", 0xC2, 0x22, 0x00, 2, 2048, 64, 2048, 128 },
	{ "W25N01GV", 0xEF, 0xAA, 0x21, 1, 2048, 64, 1024, 64 },
};

static int gTimeOutInterval = 0;

static int gHasDummyByteAddr = 0;

#ifdef	USE_MMP_DRIVER
struct timeval startT, endT;
#else
static int gCurrTv = 0;
#endif

#ifdef	ENABLE_CTRL_1
static int gEnableRecoverNorGpioPin = 0;
#endif

#if (CFG_CHIP_FAMILY == 9850)
static uint32_t gEnSpiCsCtrl = 0;	//default: diable GPIO control SPI CS pin
#else
static uint32_t gEnSpiCsCtrl = 1;	//default: enable GPIO control SPI CS pin
#endif

static uint32_t gAddrHasDummyByte = 0;
static uint32_t gUseDummyReadId = 0;

#ifdef ENABLE_DUMP_VCD
extern volatile int dump_enable;
static int count = 0;
#endif

static int gForceToStopErasing = 0;
static int gEnWinbondContinueRead = 0;

#ifdef ENABLE_WRITE_THREAD_QUEUE
static	pthread_mutex_t     wq_mutex = PTHREAD_MUTEX_INITIALIZER;
static	pthread_mutex_t     spi_mutex = PTHREAD_MUTEX_INITIALIZER;
static uint8_t   gShowWqMsg         = 0;
static uint8_t   gWqRdWtErsMsg      = 0;
static uint8_t   gFirstInitWtQue    = 1;
volatile uint8_t gSpiWqMutex        = 1;
volatile int     gFrtPtr            = 0;
volatile int     gRrPtr             = 0;
volatile MY_WT_QUE wq[MAX_QUEUE_SIZE] = {0};
#endif

//=============================================================================
//                              spi Function WRAP
//=============================================================================
#ifdef	USE_MMP_DRIVER
static int spi_read(uint8_t *cBuf, uint32_t cLen, uint8_t *dBuf, uint32_t dLen)	//SPI_COMMAND_INFO *cmd)
{
	uint32_t rst;

	#ifdef	ENABLE_CTRL_1
	if(gEnSpiCsCtrl)	mmpSpiSetControlMode(SPI_CONTROL_NAND);
	#endif	
	
	if(dLen<16)	rst = mmpSpiPioRead(SPI_PORT, cBuf, cLen, dBuf, dLen, 8);
	else	rst = mmpSpiDmaRead(SPI_PORT, cBuf, cLen, dBuf, dLen, 8);
	
	#ifdef	ENABLE_CTRL_1
	if(gEnSpiCsCtrl)
	{
		mmpSpiResetControl();
		if(gEnableRecoverNorGpioPin)
		{
			mmpSpiSetControlMode(SPI_CONTROL_NOR);
			mmpSpiResetControl();
		}
	}
	#endif

	if(rst==SPI_OK)	return SPINF_OK;
	else			return SPINF_FAIL;
}

static int spi_write(uint8_t *cBuf, uint32_t cLen, uint8_t *dBuf, uint32_t dLen)	//(SPI_COMMAND_INFO *cmd)
{
	uint32_t rst;

	#ifdef	ENABLE_CTRL_1
	if(gEnSpiCsCtrl)	mmpSpiSetControlMode(SPI_CONTROL_NAND);
	#endif
	
	if(dLen<16)	rst = mmpSpiPioWrite(SPI_PORT, cBuf, cLen, dBuf, dLen, 8);
	else	rst = mmpSpiDmaWrite(SPI_PORT, cBuf, cLen, dBuf, dLen, 8);
	
	#ifdef	ENABLE_CTRL_1
	if(gEnSpiCsCtrl)
	{
		mmpSpiResetControl();
		if(gEnableRecoverNorGpioPin)
		{
			mmpSpiSetControlMode(SPI_CONTROL_NOR);
			mmpSpiResetControl();
		}
	}
	#endif
	
	if(rst==SPI_OK)	return SPINF_OK;
	else			return SPINF_FAIL;
}

static void spinf_error(uint8_t errCode)
{
	printf("[SPINF ERROR] error code = %x\n",errCode);
}
#else	//use win32 spi driver
static int spi_read(uint8_t *cBuf, uint32_t cLen, uint8_t *dBuf, uint32_t dLen)	//(SPI_COMMAND_INFO *cmd)
{
	uint32_t rst;
	rst = SpiRead(cLen, (PWriteControlByteBuffer)cBuf, dLen, (PReadDataByteBuffer)dBuf);
	if(rst==SPI_OK)	return SPINF_OK;
	else		return SPINF_FAIL;
}

static int spi_write(uint8_t *cBuf, uint32_t cLen, uint8_t *dBuf, uint32_t dLen)	//(SPI_COMMAND_INFO *cmd)
{
	uint32_t rst;
	rst = SpiWrite(cLen, (PWriteControlByteBuffer)cBuf, dLen, (PWriteDataByteBuffer)dBuf);

	if(rst==SPI_OK)	return SPINF_OK;
	else		return SPINF_FAIL;
}

static void spinf_error(uint8_t errCode)
{
	printf("[SPINF ERROR] error code = %x\n",errCode);
}
#endif

void _setTimeOutT(uint32_t tv)
{
#ifdef	USE_MMP_DRIVER
	gettimeofday(&startT, NULL);
	gTimeOutInterval = tv;
#else	
	gCurrTv = 0;
	gTimeOutInterval = tv*10;
#endif
}

static int _waitTimeOut(void)
{
#ifdef	USE_MMP_DRIVER
    gettimeofday(&endT, NULL);
    if(itpTimevalDiff(&startT, &endT) > gTimeOutInterval)    return 1;
	else	return 0;
#else
	gCurrTv++;
	if(gCurrTv==gTimeOutInterval)	return 1;
	else							return 0;
#endif
}

#ifdef ENABLE_WRITE_THREAD_QUEUE
static uint8_t _wqPageWrite(uint32_t blk, uint32_t ppo, uint8_t *dBuf);
#endif
//=============================================================================
//                              Private Function Definition
//=============================================================================
#ifdef	USE_MMP_DRIVER
void showData(uint8_t *buf, uint32_t len)
{
    uint32_t i;
    
    printf(" *** buf = %x, len = %x ***\n",buf,len);
	for(i=0; i<len; i++)
	{
	    printf("%02x ",buf[i]);
	    if( (i&0x0F)==0x0F )    printf("\n");
	    if( (i&0x1FF)==0x1FF )    printf("\n");
	}		
	printf("\n");
}
#endif

void _SpiNf_InitAttr(unsigned char *pBuf)
{
	memset( (unsigned char*)&gSpiNfInfo, 0, sizeof(SPI_NF_INFO) );
	
    if(pBuf[0]==0xC8)	gSpiNfInfo.MID = (unsigned char)pBuf[0];
    else	printf("Error: MID not support!!\n");
    
    if(pBuf[1]==0xB1)	gSpiNfInfo.DID0 = (unsigned char)pBuf[1];
    else	printf("Error: DID0 not support!!\n");

    if(pBuf[2]==0x48)	gSpiNfInfo.DID1 = (unsigned char)pBuf[2];
    else	printf("Error: DID1 not support!!\n");
    
    if(gSpiNfInfo.MID!=0xC8)
    {
		//vendor not support
    }
    
    if( (gSpiNfInfo.DID0==0xB1) && (gSpiNfInfo.DID1==0x48) )
    {
    	printf("SPI NF ID is VALID!!\n");
    	gSpiNfInfo.Init = 1;
    	gSpiNfInfo.PageSize = 2048;
    	gSpiNfInfo.PageInBlk = 64;
    	gSpiNfInfo.TotalBlk = 256;
    	gSpiNfInfo.SprSize = 128;    	
    }
}

static int _setAttribute(SPI_NF_INFO *cInfo, unsigned char *id)
{
	SPINF_CFG *snCfg;
	int snCfgCnt = 0;
	int isGotAttr = 0;
	int i = 0;

	//printf("setInfo:[%x,%x][%x,%x]\n",cInfo,cInfo->fromCfg,id,id[0]);

	if(cInfo->fromCfg)
	{
		if(!cInfo->CfgArray)	return (0x18);

		if(!cInfo->CfgCnt)	return (0x19);

		snCfg = (SPINF_CFG*)cInfo->CfgArray;
		snCfgCnt = cInfo->CfgCnt;
	}
	else
	{
		snCfg = g_SpiNfCfgArray;
		snCfgCnt = sizeof(g_SpiNfCfgArray)/sizeof(SPINF_CFG);
	}

	for(i=0; i<snCfgCnt; i++)
	{
		if( (id[0]==snCfg[i].cfgMID) && (id[1]==snCfg[i].cfgDID0) && (id[2]==snCfg[i].cfgDID1) )
		{
			isGotAttr = 1;
			break;
		}
	}

	if(gUseDummyReadId && isGotAttr)    gHasDummyByteAddr = 1;

	if(!isGotAttr)	return (0x1A);	//ID not support

	gSpiNfInfo.Init = 1;
	gSpiNfInfo.MID = id[0];
	gSpiNfInfo.DID0 = id[1];
	gSpiNfInfo.DID1 = id[2];
	gSpiNfInfo.BBM_type = snCfg[i].cfgBBM_type;
	gSpiNfInfo.BootRomSize = 0;	//16MB = 2048*64*32;
	gSpiNfInfo.PageInBlk = snCfg[i].cfgPageInBlk;
	gSpiNfInfo.PageSize = snCfg[i].cfgPageSize;
	gSpiNfInfo.TotalBlk = snCfg[i].cfgTotalBlk;
	gSpiNfInfo.SprSize = snCfg[i].cfgSprSize;
	gSpiNfInfo.FtlSprSize = 24;

	memcpy((char*)gSpiNfInfo.name, (char*)&snCfg[i].cfgDevName, 32);

	memcpy( (unsigned char*)cInfo, (unsigned char*)&gSpiNfInfo, sizeof(SPI_NF_INFO) );

	printf("isGotAttr=%x, dummyAddr=%x\n",isGotAttr, gHasDummyByteAddr);

	return (0);
}

static uint8_t _cmdReset(void)
{
	unsigned char SpiCmd[1];
	unsigned char dBuf;
	int spiret;
	uint8_t  status=0x01;
	uint8_t  result = 0x31;
	
	//printf("_cmdReset.1\n");
	
    //FFH (reset)
    SpiCmd[0] = 0xFF;
    
    spiret = spi_write(&SpiCmd[0], 1, &dBuf, 0);	
    
    //0F (get feature)
	_setTimeOutT(500);
	while(status & NF_STATUS_OIP)
	{
	    if(spiNf_getFeature(0xC0, &status))	
	    {
	    	printf(" get feature FAIL(reset1)!!\n");
	    	goto resetEnd;
	    }
	    //else                            	printf(" status = %x\n",status);
	    
	    if(_waitTimeOut())	goto resetEnd;
	    
	    ithDelay(0); 
	}
	result = 0;

resetEnd:
		
	printf("leave reset cmd R=%x\n",status);
	return result;
}

static uint8_t _checkEccStatus_GD(uint32_t blk, uint32_t ppo, uint8_t s)
{
    switch(s>>4)
    {
        case 0x7:
            printf("spiNfRdGdE3(%x,%x,%x)\n",blk,ppo,s);
            return (20);
            break;
        case 0x6:
        case 0x5:
        case 0x4:
        case 0x3:
        case 0x2:
        #if 0
            printf("spiNfRdGdE2(%x,%x), b=%d, s=%x\n",blk,ppo,(s>>4)+2,s);
            break;
        case 0x1:
            printf("spiNfRdGdE1(%x,%x), b=%d, s=%x\n",blk,ppo,s);
            break;
        case 0x0:
            //printf("spiNfRdGdE0(%x,%x), b=%d, s=%x\n",blk,ppo,s);
            break;
        default:
            printf("incorrect status of feature register!!!reg[C0]=%x\n",s);
            return (22);
            break;
        #else
        case 0x1:
            {
            	uint8_t sF0 = 0;
            	
            	if(!spiNf_getFeature(0xF0, &sF0))
            	{
            		uint8_t s1 = (sF0>>4)&0x03;
            		if(s1)	printf("NfRdGdE2(%x,%x,%x)(%x,%x)\n",blk,ppo,s1+4,s,sF0);
            		//else	printf("NfRdGdE1(%x,%x)(%x,%x)\n",blk,ppo,s,sF0);
            	}
            }
            break;
        case 0x0: 
            //printf("No bit errors were detected during the previous read algorithm, reg[C0]=%x\n",s);
            break;
        default:
            printf("incorrect status of feature register!!!reg[C0]=%x\n",s);
            return (22);
            break;        
        #endif
    }  
    return (0);
}

static uint8_t _checkEccStatus_MXIC(uint32_t blk, uint32_t ppo, uint8_t s)
{
    switch(s>>4)
    {
        case 0x2:
            printf("NfRdMxicE2(%x,%x,%x)\n",blk,ppo,s);
            return (23);
        case 0x1:
            printf("NfRdMxicE1(%x,%x,%x)\n",blk,ppo,s);
            break;
        case 0x0:
            //printf("NfRdMxicE0(%x,%x,%x)\n",blk,ppo,s);
            break;

        case 0x3:            
        default:
            printf("incorrect status of feature register!!!reg[C0]=%x\n",s);
            return (22);
            break;
    }  
    return (0);
}

static uint8_t _checkEccStatus_WINBOND(uint32_t blk, uint32_t ppo, uint8_t s)
{
    switch(s>>4)
    {
        case 0x3:      
        case 0x2:
            printf("NfRdWbE2(%x,%x,%x)\n",blk,ppo,s);
            return (23);
        case 0x1:
            printf("NfRdWbE1(%x,%x,%x)\n",blk,ppo,s);
            break;
        case 0x0:
            //printf("NfRdWbE0(%x,%x,%x)\n",blk,ppo,s);
            break;
          
        default:
            printf("incorrect status of feature register!!!reg[C0]=%x\n",s);
            return (22);
            break;
    }  
    return (0);
}

static uint8_t _checkEccStatus(uint32_t b, uint32_t p)
{
	uint8_t rst=0;
	uint8_t status=0;	
	
	if(!spiNf_getFeature(0xC0, &status))
	{	
		//printf(" get feature OK for ECC check, status=%x!!\n",status);
	    switch(gSpiNfInfo.MID)
	    {
	    	case 0xC8:
	   			rst = _checkEccStatus_GD(b,p,status);
	   			break;
	   			
	   		case 0xC2:
	   			rst = _checkEccStatus_MXIC(b,p,status);
	   			break;
	   			
	   		case 0xEF:
	   			rst = _checkEccStatus_WINBOND(b,p,status);
	   			break;
	   				
	   		default:
	   			printf("_checkEccStatus(): incorrect Factory ID!!\n");
	   			rst = 0x23;
	   			break;
	   	}
   	}

   	return rst;
}

#ifdef ENABLE_WRITE_THREAD_QUEUE
static bool _QueEmpty(void)
{
    int a,b;
    
    pthread_mutex_lock(&wq_mutex);
    a = gFrtPtr;
    b = gRrPtr;
    pthread_mutex_unlock(&wq_mutex);
    
    if(a==b)	return true;
    else      return false;
}

static bool _QueFull(void)
{
    int a,b;
    int tmpWtPtr;
    
    pthread_mutex_lock(&wq_mutex);
    a = gFrtPtr;
    b = gRrPtr+1;
    pthread_mutex_unlock(&wq_mutex);    
    
    if(b>=MAX_QUEUE_SIZE)    b = 0;
    
    if(a==b)	return true;
    else      return false;
}

static int _checkWtQueBuf(uint32_t b, uint32_t p)
{
    int tmpEnd;
    int i,fp,rp;
    uint32_t wqB,wqP;
    #ifdef	ENABLE_MEASURE
    struct timeval T1, T2;
    int myT=0;
    
    gettimeofday(&T1, NULL);
    #endif
    
    //return (int)-1;
    
    if(_QueEmpty())
    {
        #ifdef	ENABLE_MEASURE
        gettimeofday(&T2, NULL);
        myT = itpTimevalDiff(&T1, &T2);
        if(myT) printf("rc0:T=%d\n",myT);
        #endif
        return -1;
    }
    
    pthread_mutex_lock(&wq_mutex);
    fp = gFrtPtr;
    rp = gRrPtr;
    pthread_mutex_unlock(&wq_mutex);
    
    if(fp<rp)    tmpEnd = rp;    
    if(fp>rp)    tmpEnd = MAX_QUEUE_SIZE;
 
    for(i=fp; i<tmpEnd; i++)
    {
        if( (wq[i].blk == b) && (wq[i].ppo == p) )
        {
            #ifdef	ENABLE_MEASURE
            gettimeofday(&T2, NULL);
            myT = itpTimevalDiff(&T1, &T2);
            if(myT) printf("rc1:T=%d\n",myT);
            #endif
            return i;
        }
    }
    
    if(fp>rp)
    {
        for(i=0; i<rp; i++)
        {
            if( (wq[i].blk == b) && (wq[i].ppo == p) )	
            {
                #ifdef	ENABLE_MEASURE
                gettimeofday(&T2, NULL);
                myT = itpTimevalDiff(&T1, &T2);
                printf("rc2:=%d\n",myT);
                #endif
                return i;
            }
        }
    }
    
    #ifdef	ENABLE_MEASURE
    gettimeofday(&T2, NULL);
    myT = itpTimevalDiff(&T1, &T2);
    if(myT) printf("rc3:T=%d\n",myT);
    #endif
    
    return -1;
}

static void* _WtHandler(void* arg)
{
    uint8_t idx = 0;
    uint8_t rst;
    int a;
    uint32_t b;
    uint32_t c;
    uint8_t  *d;
    
    /*
    {
        int policy;
        struct sched_param param;
        
        pthread_getschedparam(pthread_self(), &policy, &param);
        param.sched_priority = MAX_NF_PRIORITY;
        pthread_setschedparam(pthread_self(), policy, &param);
    }
    */
           
    //
    while(1)
    {
        if(_QueEmpty())
        {
            if(gShowWqMsg) printf("QueEpt!!\n");
            ithDelay(1000);
        }
        else
        {
            pthread_mutex_lock(&wq_mutex);
            a=gFrtPtr;
            b=wq[a].blk;
            c=wq[a].ppo;
            d=wq[a].ptr;
            if(gShowWqMsg) printf("wQ:i=%x,%x,%x,%x\n",a, b, c, d);
            pthread_mutex_unlock(&wq_mutex);
            
            pthread_mutex_lock(&spi_mutex);
            gSpiWqMutex = 0;
            rst = _wqPageWrite(b, c, d);
            gSpiWqMutex = 1;
            pthread_mutex_unlock(&spi_mutex);
            
            //deQue(idx);            
            pthread_mutex_lock(&wq_mutex);
            gFrtPtr++;
            if(gFrtPtr>=MAX_QUEUE_SIZE)	gFrtPtr = 0;
            pthread_mutex_unlock(&wq_mutex);
        }    
        //ithDelay(0);    
    }
}

static bool _initWtQue(void)
{
    int i;
    int res;
    uint8_t *tmpPtr;
    pthread_t task;
    pthread_attr_t attr;    
	
    if(gShowWqMsg) printf("wq-init:1\n");
    
    tmpPtr = (uint8_t*)malloc( (gSpiNfInfo.PageSize + gSpiNfInfo.SprSize) * MAX_QUEUE_SIZE );
    if(!tmpPtr)    return false;
    
    if(gShowWqMsg) printf("wq-init:2, wq_base = %08x\n",tmpPtr);
    
    for(i=0; i<MAX_QUEUE_SIZE; i++)
    {
        wq[i].ptr = (uint8_t*)(tmpPtr + ((gSpiNfInfo.PageSize + gSpiNfInfo.SprSize) * i) );
        wq[i].blk = -1;
        wq[i].ppo = -1;
        //printf("[%02x]:[%03x,%02x,%08x]\n",i,wq[i].blk,wq[i].ppo,wq[i].ptr);
    }
    
    if(gShowWqMsg) printf("wq-init:3\n");

	  //create thread	
    pthread_attr_init(&attr);
    res = pthread_create(&task, &attr, _WtHandler, NULL);
    if(res)
    {
    	printf( "[SPI NAND]%s() L#%ld: ERROR, create _WtHandler() thread fail! res=%ld\n", res );
    	return false;
    }
    
    gFirstInitWtQue = 0;
    
    printf("wq-init-finished:\n");
    
    return true;
}

#endif
//=============================================================================
//                              Public Function Definition
//=============================================================================

//=============================================================================
/**
* read ID of SPI NAND
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_ReadId(unsigned char *id)
{
	unsigned char SpiCmd[2];
	unsigned char SpiData[3];
	unsigned char regStatus;
	int spiret,i;	
    
	//reset 0xFF
    if(_cmdReset())
    {
        printf("[SPINF ERR] reset commnad fail:0\n");
        spiret = SPINF_ERROR_RESET_CMD_FAIL;
        goto errEnd;
    }
    
    for(i=0; i<4; i++)
    {
	      if( spiNf_getFeature(0xA0+0x10*i, &regStatus) )
	      {
	          printf("[SPINF ERR] get feature commnad fail\n");
	          spiret = SPINF_ERROR_GET_FEATURE_CMD_FAIL;
	          goto errEnd;
	      }    
    }
    
	SpiData[0]=0;SpiData[1]=0;SpiData[2]=0;
	SpiCmd[0] = 0x9F;
	spiret = spi_read(SpiCmd, 1, SpiData, 3);
	
    id[0] = SpiData[0];
    id[1] = SpiData[1];
    id[2] = SpiData[2];

	if(spiret==SPINF_OK)
	{
		printf("SPI READ ID PASS, data=%02x,%02x,%02x\n",id[0],id[1],id[2]);
		spiret = 0;
	}
	else	
	{
		printf("SPI READ ID FAIL,result=%x\n",spiret);  
	}

errEnd:
	
	return (spiret);
}

//=============================================================================
/**
* read ID with dummy byte of SPI NAND
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_DummyReadId(unsigned char *id)
{
	unsigned char SpiCmd[2];
	unsigned char SpiData[3];
	unsigned char regStatus;
	int spiret,i;
	
	//reset 0xFF
    if(_cmdReset())
    {
        printf("[SPINF ERR] reset commnad fail:0\n");
        spiret = SPINF_ERROR_RESET_CMD_FAIL;
        goto errEnd;
    }
	
    for(i=0; i<4; i++)
    {
	      if( spiNf_getFeature(0xA0+0x10*i, &regStatus) )
	      {
	          //printf("[SPINF ERR] get feature commnad fail\n");
	          spiret = SPINF_ERROR_GET_FEATURE_CMD_FAIL;
	          goto errEnd;
	      }
    }
	
	SpiData[0]=0;SpiData[1]=0;SpiData[2]=0;	
	SpiCmd[0] = 0x9F; SpiCmd[1] = 0x00;
	spiret = spi_read(SpiCmd, 2, SpiData, 3);

	id[0] = SpiData[0];
	id[1] = SpiData[1];
	
    //GD & MXIC has 2 IDs, but Winbond has 3 IDs.
	if(SpiData[0]!=0xEF)
	    id[2] = 00;
	else
	    id[2] = SpiData[2];

	if(spiret==SPINF_OK)
	{
		printf("SPI READ ID(with dummy byte) PASS, data=%02x,%02x,%02x\n",id[0],id[1],id[2]);
		gUseDummyReadId = 1;
		spiret = 0;
	}
	else	
	{
		//printf("SPI READ ID FAIL,result=%x\n",spiret);  
	}
	
errEnd:

	return (spiret);
}

//=============================================================================
/**
* initialization of SPI NAND
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_Initial(SPI_NF_INFO *info)
{
	unsigned char idBuf[4];

	gUseDummyReadId = 0;

	//readId
	if( spiNf_ReadId(idBuf) )  return 0x11;

	if( idBuf[0]==0xFF || idBuf[0]==0x00 )
	{
		if( spiNf_DummyReadId(idBuf) )	return 0x11;
	}

	//get attribute from cfg(cfgArray)
	if(_setAttribute(info, idBuf))	return 0x12;

	if(gSpiNfInfo.MID==0xEF)
	{
	    uint8_t  status = 0;
	    //do winbond initial flow
	    if(!spiNf_getFeature(0xA0, &status))   printf(" get feature(A0)=%x!!\n",status);
	    if(!spiNf_getFeature(0xB0, &status))   printf(" get feature(B0)=%x!!\n",status);
	    if( !(status&0x08) )	spiNf_setFeature(0xB0, 0x18);
	    if(!spiNf_getFeature(0xC0, &status))   printf(" get feature(C0)=%x!!\n",status);
	}

	#ifdef ENABLE_WRITE_THREAD_QUEUE
	if(gFirstInitWtQue)    _initWtQue();
    #endif

	return 0x00;
}
//=============================================================================
/**
* Get feature of SPI NAND
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_getFeature(uint8_t addr, uint8_t *buf)
{
	unsigned char SpiCmd[2];
	unsigned char SpiData[4];
	int spiret;
	
	SpiCmd[0] = 0x0F;	

	if(!buf)	return (4);
	
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	pthread_mutex_lock(&spi_mutex);
	#endif
	
	SpiCmd[1] = addr;
	spiret = spi_read(SpiCmd, 2, &SpiData[0], 1);

	#ifdef ENABLE_WRITE_THREAD_QUEUE
	pthread_mutex_unlock(&spi_mutex);
	#endif

	if(spiret==SPINF_OK)
	{
	    *buf = SpiData[0];
	    //printf("SPI NF GET feature OK, data[%02X]=%02x\n",addr,SpiData[0]);
	    return (0);
	}
	else
	{
	    printf("SPI NAND GET feature FAIL,result=%x\n",spiret); 
	    return (5);
	}
}

//=============================================================================
/**
* Set feature of SPI NAND
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_setFeature(uint8_t addr, uint8_t Reg)
{
	unsigned char SpiCmd[2];
	unsigned char SpiData[2];
	int spiret;
	uint8_t  mask[4] = {0xBE,0xD1,0x00,0xC0};	//reg[C0] is read only (status)
	
	SpiCmd[0] = 0x1F;
	SpiCmd[1] = addr;

	if(gSpiNfInfo.MID==0xC8)
	{
	    SpiData[0] = Reg & mask[(addr - 0xA0) >> 4];		
	    if(SpiData[0]!=Reg)    printf("warning: THE VALUE=%x,set=%x, index=%x\n",Reg,SpiData[0],(addr - 0xA0) >> 4);
	}
	else
	{
	    SpiData[0] = Reg;	
	}

	spiret = spi_write(SpiCmd, 2, &SpiData[0], 1);
    
    if(spiret==SPINF_OK)
	{
	    //printf("Set Feature OK!!\n");
	    return (0);
	}
	else
	{
	    printf("SPI NAND SET feature FAIL,result=%x\n",spiret);
	    return (1);
	}
}

uint8_t spiNf_ByteRead(uint32_t blk, uint32_t ppo, uint8_t *dBuf, uint32_t offset, uint32_t rLen)
{
	unsigned char SpiCmd[4];
	unsigned char SpiData[3];
	int spiret;
	uint8_t  status=0x01;
	uint32_t addr = blk*gSpiNfInfo.PageInBlk + ppo;
	uint8_t result = 1;
#ifdef ENABLE_PATCH_WINBOND_SPARE_ISSUE
	uint8_t readWbSpr = 0;
	unsigned char tmpData[128];	
#endif
	
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	
	if(gWqRdWtErsMsg) printf("r2[%x,%x]\n",blk,ppo);
	
	if(gShowWqMsg) printf("Rp2[%x,%x][%x]\n",blk,ppo,dBuf,offset,rLen);	
	{
	    int qIndex = _checkWtQueBuf(blk,ppo);
	    if(qIndex!=(int)-1)
	    {
	        //printf("wq_chRd:i=%x[%x,%x,%x]\n",qIndex,wq[qIndex].blk,wq[qIndex].ppo,wq[qIndex].ptr);
	        memcpy(dBuf, &wq[qIndex].ptr[offset], rLen);
	        return (0);
	    }
	}

	pthread_mutex_lock(&spi_mutex);
	#endif

#ifdef ENABLE_PATCH_WINBOND_SPARE_ISSUE
	if( (gSpiNfInfo.MID==0xEF) && (offset>=gSpiNfInfo.PageSize) )
	{
		readWbSpr = 1;
	}
#else
	if( gSpiNfInfo.MID == 0xEF )
	{
		if( (offset>=gSpiNfInfo.PageSize) && (rLen==1) )
	    {
            //rearrange the spare data
            //0~3 -> 4~7, 4~7 -> 20~23, 8~11 -> 36~39, 12~15 -> 52~55,
            //16~19(good block code) -> 0~4(NO ECC protect)
            //20~23 -> 16~19(NO ECC protect)
	        if(offset<gSpiNfInfo.PageSize+16)
	        {
	            uint32_t tmpAddr = offset - gSpiNfInfo.PageSize;
	            offset = gSpiNfInfo.PageSize + 4 + 16*(tmpAddr/4) + (tmpAddr%4);
	        }
	        else
	        {
	            if(offset<gSpiNfInfo.PageSize+20)
	                offset = gSpiNfInfo.PageSize + (offset%4);
	            else
	                offset = 16;
	        }
	    }
	}
#endif
	
	SpiCmd[0] = 0x13;
	SpiData[0] = (addr>>16)&0xFF;
	SpiData[1] = (addr>>8)&0xFF;
	SpiData[2] = addr&0xFF;
	spiret = spi_write(SpiCmd, 1, &SpiData[0], 3);
	if(spiret!=SPINF_OK)
	{
		printf("send cmd 0x13 FAIL,R1=(%x,%x)\n",spiret,SPINF_OK);
		goto errRdEnd;
	}
	
	_setTimeOutT(500);
	while(status & NF_STATUS_OIP)
	{
	    if(spiNf_getFeature(0xC0, &status))   printf(" get feature FAIL(PgRd)!!\n");
	    if(_waitTimeOut())	goto errRdEnd;
	    
	    ithDelay(0); 
	}
	
	SpiCmd[0] = 0x03;

	if(gHasDummyByteAddr)
	{
		SpiCmd[1] = (unsigned char)((offset>>8) & 0x0F);
		SpiCmd[2] = (unsigned char)(offset & 0xFF);
		SpiCmd[3] = 0x00;
	}
	else
	{
		SpiCmd[1] = 0x00;
		SpiCmd[2] = (unsigned char)((offset>>8) & 0xFF);
		SpiCmd[3] = (unsigned char)(offset & 0xFF);
	}

#ifdef ENABLE_PATCH_WINBOND_SPARE_ISSUE
    if(readWbSpr)	spiret = spi_read(SpiCmd, 4, tmpData, 64);
	else            spiret = spi_read(SpiCmd, 4, dBuf, rLen);
#else
	spiret = spi_read(SpiCmd, 4, dBuf, rLen);
#endif
	
	if(spiret!=SPINF_OK)
	{
		printf("send cmd 0x13 FAIL,R2=(%x,%x)\n",spiret,SPINF_OK);
		goto errRdEnd;
	}
	
#ifdef ENABLE_PATCH_WINBOND_SPARE_ISSUE
	if(readWbSpr)	ithInvalidateDCacheRange((uint32_t*)tmpData, 64);
	else			ithInvalidateDCacheRange((uint32_t*)&dBuf[0], rLen);
	
	if(readWbSpr)
	{
    	int j;
    	uint8_t tmp[24];
    	uint8_t *ptr;
    	
        //re-arrange the spare data
        //0~3 -> 4~7, 4~7 -> 20~23, 8~11 -> 36~39, 12~15 -> 52~55,
        //16~19(good block code) -> 0~3(NO ECC protect); 20~23 -> 16~19(NO ECC protect)

        if( rLen == 24 )	
        	ptr = (uint8_t*)dBuf;
        else
        	ptr = (uint8_t*)&tmp[0];       	

        for(j= 0; j<16; j++)	ptr[j] = tmpData[4 + 16*(j/4) + (j%4)];            
        for(j=16; j<20; j++)	ptr[j] = tmpData[j-16];            
        for(j=20; j<24; j++)	ptr[j] = tmpData[j-4]; 
        
        if( rLen != 24 )   
        	for(j= 0; j<rLen; j++)	dBuf[j] = ptr[offset-gSpiNfInfo.PageSize+j];
	}
#else
    ithInvalidateDCacheRange((uint32_t*)&dBuf[0], rLen);
#endif

	result = _checkEccStatus(blk,ppo);
    
errRdEnd:

  #ifdef ENABLE_WRITE_THREAD_QUEUE
  pthread_mutex_unlock(&spi_mutex);
  #endif

	return result;
}

//=============================================================================
/**
* SPI NAND read page data
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_PageRead(uint32_t blk, uint32_t ppo, uint8_t *dBuf)	//(uint32_t addr, uint8_t *dBuf)
{
	unsigned char SpiCmd[5];
	unsigned char SpiData[3];
	int spiret;
	uint8_t  status=0x01;
	uint32_t addr = blk*gSpiNfInfo.PageInBlk + ppo;
	uint8_t result = 1;
	#ifdef ENABLE_THREAD
	int policy;
	struct sched_param old_param;
	struct sched_param new_param;
	
	pthread_getschedparam(pthread_self(), &policy, &old_param);
	memcpy(&new_param, &old_param, sizeof(struct sched_param));
	new_param.sched_priority = MAX_NF_PRIORITY;
	#endif
	
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	
	if(gWqRdWtErsMsg) printf("r1[%x,%x]\n",blk,ppo);
	
	if(gShowWqMsg) printf("Rp1[%x,%x][%x]\n",blk,ppo,dBuf);
	
	{
	    int qIndex = _checkWtQueBuf(blk,ppo);
	    if(gShowWqMsg) printf("sRd:%x\n",qIndex);
	    if(qIndex!=(int)-1)
	    {
	        //printf("wq_chRd:i=%x[%x,%x,%x]\n",qIndex,wq[qIndex].blk,wq[qIndex].ppo,wq[qIndex].ptr);
	        memcpy(dBuf, wq[qIndex].ptr, gSpiNfInfo.PageSize+gSpiNfInfo.FtlSprSize);
	        return (0);
	    }
	}

	pthread_mutex_lock(&spi_mutex);
	#endif

	SpiCmd[0] = 0x13;
	SpiData[0] = (addr>>16)&0xFF;
	SpiData[1] = (addr>>8)&0xFF;
	SpiData[2] = addr&0xFF;
	spiret = spi_write(SpiCmd, 1, &SpiData[0], 3);
	
	if(spiret!=SPINF_OK)
	{
		printf("send cmd 0x13 FAIL,R1=(%x,%x)\n",spiret,SPINF_OK);
		goto errRdEnd;
	}
	
	_setTimeOutT(500);
	while(status & NF_STATUS_OIP)
	{
	    if(spiNf_getFeature(0xC0, &status))   printf(" get feature FAIL(PgRd)!!\n");
		if(_waitTimeOut())	goto errRdEnd;
		ithDelay(0);
	}
	
	#ifdef ENABLE_THREAD
	pthread_setschedparam(pthread_self(), policy, &new_param);
	#endif
	
	SpiCmd[0] = 0x03;
	SpiCmd[1] = 0x00;
	SpiCmd[2] = 0x00;
	SpiCmd[3] = 0x00;

	if(gSpiNfInfo.MID==0xEF)
	{
	    /*
	    if(gEnWinbondContinueRead)
	    {
	        SpiCmd[4] = 0x00;
	        spiret = spi_read(SpiCmd, 5, &dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.SprSize);
	    }
	    else
	    {
	        spiret = spi_read(SpiCmd, 4, &dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.SprSize);
	    }
	    */
	    spiret = spi_read(SpiCmd, 4, &dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.SprSize);
	}
	else
	{
	    spiret = spi_read(SpiCmd, 4, &dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.FtlSprSize);
	}
	
	ithInvalidateDCacheRange((uint32_t*)&dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.FtlSprSize);
	
	if(gSpiNfInfo.MID==0xEF)
	{
	    uint8_t tmpBuf[24];
	    memset(tmpBuf, 0xFF, 24);
	    memcpy(&tmpBuf[0], &dBuf[gSpiNfInfo.PageSize+4], 4);        //0~3
	    memcpy(&tmpBuf[4], &dBuf[gSpiNfInfo.PageSize+4+16], 4);     //4~7
	    memcpy(&tmpBuf[8], &dBuf[gSpiNfInfo.PageSize+4+32], 4);     //8~11
	    memcpy(&tmpBuf[12], &dBuf[gSpiNfInfo.PageSize+4+48], 4);    //12~15
	    memcpy(&tmpBuf[16], &dBuf[gSpiNfInfo.PageSize], 4);			//good block code
	    memcpy(&dBuf[gSpiNfInfo.PageSize], &tmpBuf[0], 24);
	}
	
	#ifdef ENABLE_THREAD
	pthread_setschedparam(pthread_self(), policy, &old_param);
	#endif	
	
	if(spiret!=SPINF_OK)
	{
		printf("send cmd 0x13 FAIL,R2=(%x,%x)\n",spiret,SPINF_OK);
		goto errRdEnd;
	}
	result = _checkEccStatus(blk,ppo);
    
errRdEnd:

	#ifdef ENABLE_WRITE_THREAD_QUEUE
	pthread_mutex_unlock(&spi_mutex);
	#endif
	
	return result;
}

//=============================================================================
/**
* SPI NAND program page data
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
#ifdef ENABLE_WRITE_THREAD_QUEUE
uint8_t spiNf_PageProgram(uint32_t blk, uint32_t ppo, uint8_t *dBuf)
{
    int wtIdx = 0;
    uint8_t *tmpPtr;
    
    if(gWqRdWtErsMsg) printf("w1:%x,%x\n",blk,ppo);
    
    if(gShowWqMsg) printf("wq.1\n");
    //check if queue full
    if(gFirstInitWtQue)    _initWtQue();
    
    if(gShowWqMsg) printf("wq.2\n");

    while(_QueFull())
    {
        ithDelay(0);
    }
    
    if(gShowWqMsg) printf("wq.3\n");
    
    pthread_mutex_lock(&wq_mutex);
    wtIdx = gRrPtr;
    tmpPtr = wq[wtIdx].ptr;
    pthread_mutex_unlock(&wq_mutex);
    
    if(gShowWqMsg) printf("wq.4\n");
    
    if(wtIdx>=MAX_QUEUE_SIZE)	wtIdx = 0;    
    
    ithFlushDCacheRange((void*)&dBuf[0], g_NfAttrib.PageSize+g_NfAttrib.FtlSprSize);
    ithFlushMemBuffer();
    
    //memcopy to buffer
    memcpy(tmpPtr, dBuf, gSpiNfInfo.PageSize+gSpiNfInfo.FtlSprSize);
    
    if(gShowWqMsg) printf("wq.5\n");

    //enQueue(wtIdx);
    pthread_mutex_lock(&wq_mutex);
    wq[wtIdx].blk = blk;
    wq[wtIdx].ppo = ppo;
    gRrPtr = wtIdx+1;
    if(gRrPtr>=MAX_QUEUE_SIZE)	gRrPtr = 0;
    pthread_mutex_unlock(&wq_mutex);
    
    //if(gShowWqMsg) 
    if(gShowWqMsg) printf("wq.6[%x][%x,%x]\n",wtIdx,wq[wtIdx].blk,wq[wtIdx].ppo);
    
    //return OK
    return (0);
}

static uint8_t _wqPageWrite(uint32_t blk, uint32_t ppo, uint8_t *dBuf)
#else
uint8_t spiNf_PageProgram(uint32_t blk, uint32_t ppo, uint8_t *dBuf)
#endif
{
	unsigned char SpiCmd[4];
	int spiret;
	uint8_t  status=0x01;
	uint32_t addr = blk*gSpiNfInfo.PageInBlk + ppo;
	uint8_t  wpStatus=0x0;
	
	#ifdef ENABLE_THREAD
	int policy;
	struct sched_param old_param;
	struct sched_param new_param;
	
	pthread_getschedparam(pthread_self(), &policy, &old_param);
	memcpy(&new_param, &old_param, sizeof(struct sched_param));
	new_param.sched_priority = MAX_NF_PRIORITY;
	
	pthread_setschedparam(pthread_self(), policy, &new_param);
	#endif
	
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	if(gWqRdWtErsMsg) printf("wq:%x,%x\n",blk,ppo);
	#endif
	
	#ifdef ENABLE_DUMP_VCD
	if (dump_enable)
	    count++;
	else
	    count = 0;
	
	if (dump_enable) {
            itpTaskVcdSetTag(0, count);
            itpTaskVcdSetTag(1, 1);	
	}
	#endif
	
	//if(gShowWqMsg) printf("ithWt.01(%x,%x,%x)\n",blk,ppo,dBuf);
	//showData(dBuf,2048+24);
		
	#ifdef ENABLE_DUMP_VCD
	if (dump_enable) {
            itpTaskVcdSetTag(1, 2);
	}
	#endif
	
	if(spiNf_getFeature(0xC0, &wpStatus))   printf(" get feature [0xC0] = %x!!\n",wpStatus);
	if(spiNf_getFeature(0xA0, &status))   printf(" get feature [0xA0] = %x!!\n",status);		
				
	if(status&0x38)
	{
	    //unlock blocks
	    spiNf_setFeature(0xA0, 0x00);
	}
		
    //06H (write enable)
    if(wpStatus & NF_STATUS_WEL)
    {
        printf("WEL=1\n");
    }
    else
    {
        //printf("WEL=0\n");
	    SpiCmd[0] = 0x06;
	    spiret = spi_write(SpiCmd, 1, &SpiCmd[0], 0);
	    if(spiret!=SPINF_OK)	spinf_error(SPINF_ERROR_CMD_WT_EN_ERR);
	}
		
	#ifdef ENABLE_DUMP_VCD
	if (dump_enable) {
            itpTaskVcdSetTag(1, 3);
	}
	#endif
	
    ithFlushDCacheRange((void*)&dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.FtlSprSize);
    ithFlushMemBuffer();
	
    //02H(program load)
	SpiCmd[0] = 0x02;	
    SpiCmd[1] = 0x00;
    SpiCmd[2] = 0x00;
    if(gSpiNfInfo.MID==0xEF)
    {
        uint8_t tmpBuf[24];
        memcpy(&tmpBuf[0], &dBuf[gSpiNfInfo.PageSize+0], 24);
        memset(&dBuf[gSpiNfInfo.PageSize+0], 0xFF, 64);
        memcpy(&dBuf[gSpiNfInfo.PageSize+0], &tmpBuf[16], 24);	//good block code
        
        memcpy(&dBuf[gSpiNfInfo.PageSize+4], &tmpBuf[0], 4);	//0~3
        memcpy(&dBuf[gSpiNfInfo.PageSize+4+16], &tmpBuf[4], 4);	//4~7
        memcpy(&dBuf[gSpiNfInfo.PageSize+4+32], &tmpBuf[8], 4);	//8~11
        memcpy(&dBuf[gSpiNfInfo.PageSize+4+48], &tmpBuf[12], 4);	//12~15
        
        ithFlushDCacheRange((void*)&dBuf[gSpiNfInfo.PageSize], gSpiNfInfo.SprSize);
        ithFlushMemBuffer();        
    }

	spiret = spi_write(SpiCmd, 3, &dBuf[0], gSpiNfInfo.PageSize+gSpiNfInfo.SprSize);
	
	#ifdef ENABLE_DUMP_VCD
	if (dump_enable) {
            itpTaskVcdSetTag(1, 4);
	}
    #endif
    
    //10H (program execute)
	SpiCmd[0] = 0x10;	
	SpiCmd[1] = (addr>>16)&0xFF;
	SpiCmd[2] = (addr>>8)&0xFF;
	SpiCmd[3] = addr&0xFF;
	spiret = spi_write(SpiCmd, 4, &SpiCmd[0], 0);

	#ifdef ENABLE_WRITE_THREAD_QUEUE
	ithDelay(0); 
	#endif

	#ifdef ENABLE_THREAD
	pthread_setschedparam(pthread_self(), policy, &old_param);
	#endif
	
	#ifdef ENABLE_DUMP_VCD
        if (dump_enable) {
            itpTaskVcdSetTag(1, 5);
	}
	#endif
	
    //0FH (get feature) (wait busy)
	_setTimeOutT(500);
    status=NF_STATUS_OIP;

	while(status & NF_STATUS_OIP)
	{
	    if(spiNf_getFeature(0xC0, &status))   printf(" get feature FAIL(PgPg)!!\n");
		if(_waitTimeOut())	return (12);
		ithDelay(0); 
	}
	
    //04H (write Disable)
	SpiCmd[0] = 0x04;	
	spiret = spi_write(SpiCmd, 1, &SpiCmd[0], 0);
	
	#ifdef ENABLE_DUMP_VCD
        if (dump_enable) {
            itpTaskVcdSetTag(1, 6);
	}
	#endif
	
	if(status & NF_STATUS_P_FAIL)
	{
		if(_cmdReset())	printf("[SPINF ERR] reset commnad fail:1\n");	
		
		return (9);
	}
	else               return (0);
}

//=============================================================================
/**
* SPI NAND erase block data
*
* @return 0 If successful. Otherwise, return a nonzero value.
*/
//=============================================================================
uint8_t spiNf_BlockErase(uint32_t blk)
{
	unsigned char SpiCmd[4];
	int spiret;
	uint8_t  status=0x01;
	uint8_t  wpStatus=0x0;
	
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	pthread_mutex_lock(&spi_mutex);
	if(gWqRdWtErsMsg) printf("er:%x\n",blk);
	#endif
	
	if(!spiNf_getFeature(0xC0, &status))
	{
		if(status & NF_STATUS_E_FAIL)
		{
			if(_cmdReset())	
			{
				printf("[SPINF ERR] reset commnad fail:2\n");	
			}
		}
		wpStatus = status;
	}

	if(spiNf_getFeature(0xA0, &status))   printf(" get feature [0xA0] = %x!!\n",status);		
	if(status&0x38)
	{
	    //unlock blocks
	    spiNf_setFeature(0xA0, 0x00);
	    spiNf_getFeature(0xA0, &status);
	    printf(" get [0xA0] = %x!!\n",status);
	}

    //06H (write enable)
    if(wpStatus & NF_STATUS_WEL)
    {
        printf("WEL=1\n");
    }
    else
    {
        //printf("WEL=0\n");
	    SpiCmd[0] = 0x06;
	    spiret = spi_write(SpiCmd, 1, &SpiCmd[0], 0);
	    if(spiret!=SPINF_OK)	spinf_error(SPINF_ERROR_CMD_WT_EN_ERR);
	    //ithDelay(100);
	}

    //D8H
	SpiCmd[0] = 0xD8;	
    SpiCmd[1] = ((blk*gSpiNfInfo.PageInBlk)>>16)&0xFF;
	SpiCmd[2] = ((blk*gSpiNfInfo.PageInBlk)>>8)&0xFF;
	SpiCmd[3] = (blk*gSpiNfInfo.PageInBlk)&0xFF;
	spiret = spi_write(SpiCmd, 4, &SpiCmd[0], 0);
	if(spiret!=SPINF_OK)	spinf_error(SPINF_ERROR_CMD_ERS_ERR);
    
    //0F (get feature)
	_setTimeOutT(500);
    status = NF_STATUS_OIP;
	while(status & NF_STATUS_OIP)
	{
	    if(spiNf_getFeature(0xC0, &status))	printf(" get feature FAIL(Ers)!!\n");
	    //else                            	printf(" status = %x\n",status);
		
		if(_waitTimeOut())
		{
		    #ifdef ENABLE_WRITE_THREAD_QUEUE
		    pthread_mutex_unlock(&spi_mutex);
		    #endif
		    return (13);
		}
		ithDelay(0); 
	}
		
	//04H (write Disable)
	SpiCmd[0] = 0x04;
	spiret = spi_write(SpiCmd, 1, &SpiCmd[0], 0);
	if(spiret!=SPINF_OK)	spinf_error(SPINF_ERROR_CMD_WT_EN_ERR);
		
	//printf("Ers(%x,%x)\n",blk,status);
	#ifdef ENABLE_WRITE_THREAD_QUEUE
	pthread_mutex_unlock(&spi_mutex);
	#endif
	
	if(status&0x04)    return (10);
	else               return (0);
}

void spiNf_SetSpiCsCtrl(uint32_t csCtrl)
{
	printf("spiNf_SetSpiCsCtrl(%x)\n",csCtrl);
	gEnSpiCsCtrl = csCtrl;
}

#ifdef __cplusplus
}
#endif

