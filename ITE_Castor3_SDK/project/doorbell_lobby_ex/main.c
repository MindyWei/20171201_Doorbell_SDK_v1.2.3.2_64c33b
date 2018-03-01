#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "scene.h"
#include "config.h"
#include "xcpu_master/itx.h"
#include "ssp/mmp_spi.h"
#include "ssp/ssp_reg.h"
#include "tsi/mmp_tsi.h"

#ifdef CFG_XCPU_MSGQ
static void bootup_encoder(void)
{
    uint32_t     mask = 0;
    uint32_t     value = 0;
    SENSOR_IMAGE_MIRROR pCtrl ={true, true};
    MMP_RESULT   result = MMP_RESULT_SUCCESS;
    uint32_t     timeOutCnt = 0;

#ifdef CFG_CHIP_PKG_IT9079
    //Set GPIO20.21.22 HIGH
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x00, (0x1 << 20) | (0x1 << 21) | (0x1 << 22), (0x1 << 20) | (0x1 << 21) | (0x1 << 22));

    //Set GPIO20 output mode and GPIO21 output mode (control 9919)
    mask = (1 << 20) | (1 << 21) | (1 << 22);
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x8, (0x1 << 20) | (0x1 << 21) | (0x1 << 22), mask);

    value = (0x0 << 8) | (0x0 << 10) | (0x0 << 12);
    mask  = (0x3 << 8) | (0x3 << 10) | (0x3 << 12);
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x94, value, mask);
#endif
    usleep(5000);


//--------------------
#ifdef CFG_CHIP_PKG_IT9079
#if 1
    //Set GPIO51 HIGH
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x40, (0x1 << 19), (0x1 << 19));
    //Set GPIO51 output mode (control 9919)
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x1 << 19), (0x1 << 19));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x9C, (0x0 << 6), (0x3 << 6));

    //Set GPIO50 input mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x0 << 18), (0x1 << 18));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x9C, (0x0 << 4), (0x3 << 4));
#else
    //Set GPIO18 HIGH
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x00, (0x1 << 18), (0x1 << 18));
    //Set GPIO18 output mode (control 9919)
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x8, (0x1 << 18), (0x1 << 18));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x94, (0x0 << 4), (0x3 << 4));

    //Set GPIO33 input mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x0 << 1), (0x1 << 1));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x98, (0x0 << 2), (0x3 << 2));
#endif
#endif
//--------------------


    printf("[NOR]slck_divider = 24\n");
#ifdef CFG_CHIP_PKG_IT9079
    // SPI clock 2MHz
    ithWriteRegMaskA(REG_SSP_CONTROL_1 + SPI_0 * REG_SSP_PORT_OFFSET, 24, REG_MASK_SCLK_DIV);


//--------------------
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x40, (0x1 << 13), (0x1 << 13));
    //Set GPIO45 to reset 9919
    mask = (1 << 13);
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (1 << 13), mask);

    value = (0x0 << 26);
    mask  = (0x3 << 26);
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x98, value, mask);
//--------------------
#else	
	ithWriteRegMaskA(REG_SSP_CONTROL_1 + SPI_1 * REG_SSP_PORT_OFFSET, 24, REG_MASK_SCLK_DIV);	
	//ithGpioClear(75);
	ithGpioSetOut(75);
	ithGpioSetMode(75,ITH_GPIO_MODE0);
	//usleep(300000);
	ithGpioSet(75);	
#endif
    usleep(50000);

//--------------------
#ifdef CFG_CHIP_PKG_IT9079
#if 1
    //Set GPIO51 input mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x50, (0x1 << 19), (0x1 << 19));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x0 << 19), (0x1 << 19));

    //Set GPIO50 HIGH
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x40, (0x1 << 18), (0x1 << 18));
    //Set GPIO50 output mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x1 << 18), (0x1 << 18));
#else
    //Set GPIO18 input mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x10, (0x1 << 18), (0x1 << 18));

    ithWriteRegMaskA(ITH_GPIO_BASE + 0x8, (0x0 << 18), (0x1 << 18));

    //Set GPIO33 HIGH
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x40, (0x1 << 1), (0x1 << 1));
    //Set GPIO33 output mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x48, (0x1 << 1), (0x1 << 1));
#endif
#endif
//--------------------
    //boot up JEDI
    mmpSpiSetControlMode(SPI_CONTROL_SLAVE);
    mmpSpiResetControl();            
    result = itxBootup(ITX_SPI_BOOT);
    
    while (result != MMP_RESULT_SUCCESS && timeOutCnt < 5)
    {
    	unsigned int ResetPin;
    	
    	timeOutCnt++;
    	printf("Boot fail retry (%d) !!\n", timeOutCnt);
    	//reset 9917
#ifdef CFG_CHIP_PKG_IT9079   
        ResetPin = 45;
#else
        ResetPin = 75;
#endif 	
    	ithGpioSet(ResetPin);
    	usleep(10000);
    	ithGpioClear(ResetPin);
    	usleep(10000);
    	ithGpioSet(ResetPin);
    	usleep(10000);
    	result = itxBootup(ITX_SPI_BOOT);    	    	
    }
    // Frequency Divider
    printf("[NOR]slck_divider = 11\n");
#ifdef CFG_CHIP_PKG_IT9079	
    // SPI clock 4MHz
    ithWriteRegMaskA(REG_SSP_CONTROL_1 + SPI_0 * REG_SSP_PORT_OFFSET, 11, REG_MASK_SCLK_DIV);

    //---Set GPIO20.21.22 back to tsi0 mode
    ithWriteRegMaskA(ITH_GPIO_BASE + 0x94, (0x1 << 8 | 0x1 << 10 | 0x1 << 12) , (0x3 << 8 | 0x3 << 10 | 0x3 << 12));
    //---
#endif
    //---encoder TSO clock,clock = 150M/(1+1)=75M;
    xCpuIO_WriteRegister(0x005C, 0xD003);
    xCpuIO_WriteRegister(0x005C, 0x9003);
    //enhance TSO IO driving
    value = xCpuIO_ReadRegister(0x7CA2);
    xCpuIO_WriteRegister(0x7CA2, value | 0xF000);
    value = xCpuIO_ReadRegister(0x7CA4);
    xCpuIO_WriteRegister(0x7CA4, value | 0x0003);
    printf("Encoder TSO clock state: 0x%X\n",xCpuIO_ReadRegister(0x005C));
    //set encode resolution and bit rate
#ifndef SENSOR_TI_TVP5150
    itxSetOutputBitRate(ITX_INPUT_DEVICE_SENSOR, ITX_INPUT_VIDEO_INFO_CAMERA, (SENSOR_BITRATE<5000)? SENSOR_BITRATE:5000, 0);
    //itxSetOutputEncRes(ITX_INPUT_DEVICE_SENSOR, ITX_INPUT_VIDEO_INFO_CAMERA, !(SENSOR_WIDTH%16)? SENSOR_WIDTH:1280, !(SENSOR_HEIGHT%16)? SENSOR_HEIGHT:720, 0);
    itxSetOutputEncRes(ITX_INPUT_DEVICE_SENSOR, ITX_INPUT_VIDEO_INFO_CAMERA, (SENSOR_WIDTH<1280)? SENSOR_WIDTH:1280, (SENSOR_HEIGHT<720)? SENSOR_HEIGHT:720, 0);
#if defined(CFG_DUAL_STREAM)
    itxSetOutputBitRate(ITX_INPUT_DEVICE_SENSOR, ITX_INPUT_VIDEO_INFO_CAMERA, (SENSOR_BITRATE_2<1000)? SENSOR_BITRATE_2:1000, 1);
    itxSetOutputEncRes(ITX_INPUT_DEVICE_SENSOR, ITX_INPUT_VIDEO_INFO_CAMERA, (SENSOR_WIDTH_2<640)? SENSOR_WIDTH_2:640, (SENSOR_HEIGHT_2<480)? SENSOR_HEIGHT_2:480, 1);
#endif
	//encoder parameter update
    itxEncoderParaUpdate(); 
#else
    itxSetOutputBitRate(ITX_INPUT_DEVICE_CVBS, ITX_INPUT_VIDEO_INFO_CAMERA, (SENSOR_BITRATE<5000)? SENSOR_BITRATE:5000, 0);
    itxSetOutputEncRes(ITX_INPUT_DEVICE_CVBS, ITX_INPUT_VIDEO_INFO_CAMERA, !(SENSOR_WIDTH%16)? SENSOR_WIDTH:1280, !(SENSOR_HEIGHT%16)? SENSOR_HEIGHT:720, 0);
#endif
    //itxEncStart();
    //itxEncStop();
    
#ifndef SENSOR_TI_TVP5150
#ifdef CFG_SENSOR_FLICK_50HZ
    itxSetSensorFlickMode(SENSOR_CTRL_FLICK_MODE_50HZ);
#elif defined(CFG_SENSOR_FLICK_60HZ)
    itxSetSensorFlickMode(SENSOR_CTRL_FLICK_MODE_60HZ);
#else
    itxSetSensorFlickMode(SENSOR_CTRL_FLICK_MODE_AUTO);
#endif
    itxSetSensorImageMirror(pCtrl);
#endif
    //---
    mmpSpiResetControl();
    
    VideoEncoder_Init();
}
#endif

int SDL_main(int argc, char *argv[])
{
    int ret = 0;

#ifdef CFG_XCPU_MSGQ
    bootup_encoder();
#endif

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    ConfigLoad();
	NetworkInit();
    SceneInit();
    SceneLoad();
    ret = SceneRun();

    SceneExit();

    exit(ret);
    return ret;
}
