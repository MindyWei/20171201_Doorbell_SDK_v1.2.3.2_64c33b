#include <unistd.h>
#include "iic/mmp_iic.h"
#include "capture_module/pr2000.h"
#include "capture_module.h" //Benson add

//=============================================================================
//                Constant Definition
//=============================================================================
static uint8_t PR2000_IICADDR = 0x5C;

#define POWER_MANAGEMENT 0x0F
#define REG_STATUS1      0x10
#define REG_IDENT        0x11
#define REG_STATUS2      0x12
#define REG_STATUS3      0x13

#define RESET_MASK       (1 << 7)
#define TRI_STATE_ENABLE (1 << 7)

//#define AUTO_DETECT_INPUT

//=============================================================================
//                Macro Definition
//=============================================================================

//=============================================================================
//                Structure Definition
//=============================================================================
typedef struct _REGPAIR
{
	uint8_t  addr;
	uint16_t value;
} REGPAIR;

typedef enum _INPUT_TYPE
{
    _NTSM_M_J = 0,
    _NTSC_4_43,
    _PAL_M,
    _PAL_60,
    _PAL_B_G_H_I_D,
    _SECAM,
    _PAL_COMBINATION_N,
    _SECAM_525
} INPUT_TYPE;

typedef struct PR2000CaptureModuleDriverStruct
{
	CaptureModuleDriverStruct base;
} PR2000CaptureModuleDriverStruct;

//=============================================================================
//                Global Data Definition
//=============================================================================
static uint16_t gtPR2000CurMode  = 0xFF;
static uint16_t gtPR2000PreMode  = 0xFF;
static uint16_t gtPR2000CurDev   = 0xFF;
static MMP_BOOL gtPR2000InitDone = MMP_FALSE;
PR2000_INPUT_MODE input_mode = PR2000_INPUT_YPBPR_PAL;

/* 32-Lead LFCSP , page 108*/
static REGPAIR  CVBS_INPUT[]      =
{
	{0x00, 0x04}, //AIN3
	{0x04, 0x54},
	{0x17, 0x41},
	{0x31, 0x02},
	{0x3D, 0xA2},
	{0x3E, 0x6A},
	{0x3F, 0xA0},
	{0x58, 0x01},
	{0x0E, 0x80},
	{0x55, 0x81},
	{0x0E, 0x00},

	//Autodetect enable PAL_B/NTSC/N443
	{0x07, 0x23},  // Benson

};

static REGPAIR  SVIDEO_INPUT [] =
{
	{0x00, 0x06}, //AIN1 AIN2
	{0x04, 0x54},
	{0x31, 0x02},
	{0x3D, 0xA2},
	{0x3E, 0x6A},
	{0x3F, 0xA0},
	{0x58, 0x05},
	{0x0E, 0x80},
	{0x55, 0x81},
	{0x0E, 0x00},

	//Autodetect enable PAL_B/NTSC/N443
	{0x07, 0x23},

	// Figure35 and Figure 40 for BT601 NTSC and PAL
	{0x31, 0x1A},
	{0x32, 0x81},
	{0x33, 0x84},
	{0x34, 0x00},
	{0x35, 0x00},
	{0x36, 0x7D},
	{0x37, 0xA1},
	//NTSC
	{0xE5, 0x41},
	{0xE6, 0x84},
	{0xE7, 0x06},
	//PAL
	{0xE8, 0x41},
	{0xE9, 0x84},
	{0xEA, 0x06},
};

static REGPAIR  YPrPb_INPUT [] =
{
	{0x00, 0x09},
	{0x31, 0x02},
	{0x3D, 0xA2},
	{0x3E, 0x6A},
	{0x3F, 0xA0},
	{0x58, 0x01},
	{0x0E, 0x80},
	{0x55, 0x81},
	{0x0E, 0x00},

	//Autodetect enable PAL_B/NTSC/N443
	{0x07, 0x23},

	// Figure35 and Figure 40 for BT601 NTSC and PAL
	{0x31, 0x1A},
	{0x32, 0x81},
	{0x33, 0x84},
	{0x34, 0x00},
	{0x35, 0x00},
	{0x36, 0x7D},
	{0x37, 0xA1},
	//NTSC
	{0xE5, 0x41},
	{0xE6, 0x84},
	{0xE7, 0x06},
	//PAL
	{0xE8, 0x41},
	{0xE9, 0x84},
	{0xEA, 0x06},
};

//PR2000 Table
static CAP_TIMINFO_TABLE PR2000_TABLE [] =
{
    //Index, HActive, VActive,  Rate,            FrameRate,                 Hpor,   Vpor,  HStar,     HEnd,   VStar1,   VEnd1,  VStar2,   VEnd2,
//    {0,     720,    487,        2997,   CAP_FRAMERATE_29_97HZ,            0,      0, 238 + 32,   1677 + 32,     22 - 7,   261 - 7,   285 - 7,   524 - 7   }, //480i60     // Benson
    {0,     720,    487,        2997,   CAP_FRAMERATE_29_97HZ,            0,      0, 238 + 32,   1677 + 32,     22 - 7,   261 - 7,   285 - 7,   524 - 7   }, //480i60     // Benson
    {1,     720 + 16,    478,        2997,   CAP_FRAMERATE_29_97HZ,            0,      0, 0,     1440 + 32,     1,   239,   1,   239   }, //480i60     
    {2,     720 + 16,    574,        2500,   CAP_FRAMERATE_25HZ,               0,      0, 0,     1440 + 32,     1,   287,   1,   287   }, //576i50
    {3,     1280 + 16,   720,   3000,   CAP_FRAMERATE_30HZ,               0,      0,        0,   2560 + 32,          0,       720,    23 - 4,   23 + 722 - 4   }, //720p30
    {4,     1280 + 16,   720,   2500,   CAP_FRAMERATE_25HZ,               0,      0,        0,   2560 + 32,          10,       729,    23 - 4,   23 + 722 - 4   }, //720p25
};      

//=============================================================================
//                Private Function Definition
//=============================================================================

static uint8_t _PR2000_ReadI2c_Byte(uint8_t slaveAddr,uint8_t regAddr)
{
	uint8_t val;
	if(!mmpIicReceiveData(IIC_PORT_1, IIC_MASTER_MODE, slaveAddr>>1, &regAddr, 1,&val, 1))
		printf("PR2000 I2C Read error, reg = %02x\n", regAddr);
	return val;
}
static void _PR2000_WriteI2c_Byte(uint8_t slaveAddr,uint8_t regAddr,uint8_t val)
{
	if(!mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, slaveAddr>>1, regAddr, &val,1))
		printf("PR2000 I2c write error, reg = %02x val =%02x\n", regAddr, val);
	return;
}
static MMP_RESULT _PR2000_WriteI2c_ByteMask(uint8_t RegAddr, uint8_t data, uint8_t mask)
{
	uint8_t    value;
	MMP_RESULT flag;

	//mmpIicLockModule(IIC_PORT_0);

	if (0 != (flag = mmpIicReceiveData(IIC_PORT_0, IIC_MASTER_MODE, PR2000_IICADDR, &RegAddr, 1, &value, 1)))
	{
		printf("PR2000 I2C Read error, reg = %02x\n", RegAddr);
		mmpIicGenStop(IIC_PORT_0);
	}

	value = ((value & ~mask) | (data & mask));

	if (0 != (flag = mmpIicSendData(IIC_PORT_0, IIC_MASTER_MODE, PR2000_IICADDR, RegAddr, &value, 1)))
	{
		if(RegAddr!= 0x0F)
			printf("PR2000 I2c write error, reg = %02x val =%02x\n", RegAddr, value);
		mmpIicGenStop(IIC_PORT_0);
	}
	//mmpIicReleaseModule(IIC_PORT_0);
	return flag;
}

static void _PR2000_SWReset()
{
	_PR2000_WriteI2c_Byte(0xb8,0xff,0x00);
	_PR2000_WriteI2c_Byte(0xb8,0xf8,0xff);
	usleep(100*1000);
	_PR2000_WriteI2c_Byte(0xb8,0xf8,0x00);
}

static void _Set_PR2000_Input_CVBS(void)
{
	//PAL_register_write();
}

static void _Set_PR2000_Input_SVIDEO(void)
{
	uint16_t i;

	//for (i = 0; i < (sizeof(SVIDEO_INPUT) / sizeof(REGPAIR)); i++)
		//_PR2000_WriteI2c_Byte(SVIDEO_INPUT[i].addr, SVIDEO_INPUT[i].value);
}

static void _Set_PR2000_Input_YPrPb(void)
{
	uint16_t i;

	//for (i = 0; i < (sizeof(YPrPb_INPUT) / sizeof(REGPAIR)); i++)
		//_PR2000_WriteI2c_Byte(YPrPb_INPUT[i].addr, YPrPb_INPUT[i].value);
}

//=============================================================================
//                Public Function Definition
//=============================================================================
void Set_PR2000_Tri_State_Enable()
{
/*
	//LLC pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x1D, 0x80, 0x80);

	//TIM_OE pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x04, 0x00, 0x08);

	//SFL Pin Disable (DE)
	_PR2000_WriteI2c_ByteMask(0x04, 0x00, 0x02);

	//HS, VS, FIELD Data pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x03, 0x40, 0x40);
*/
}

void Set_PR2000_Tri_State_Disable()
{
/*
	//LLC pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x1D, 0x00, 0x80);

	//TIM_OE pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x04, 0x08, 0x08);

	//SFL Pin Disable (DE)
	_PR2000_WriteI2c_ByteMask(0x04, 0x00, 0x02);

	//HS, VS, FIELD Data pin Tri-State
	_PR2000_WriteI2c_ByteMask(0x03, 0x00, 0x40);
*/
}

PR2000_INPUT_STANDARD Get_Auto_Detection_Result_1()
{
	uint8_t result;
/*

	result = _PR2000_ReadI2c_Byte(REG_STATUS1);
	result = (result & 0x70) >> 4;

	switch (result)
	{
	case PR2000_NTSM_M_J:
		PR2000_InWidth     = 720;
		PR2000_InHeight    = 487; //480;
		PR2000_InFrameRate = 2997;
		gtPR2000CurMode    = PR2000_NTSM_M_J;
		printf("NTSM M/J\n");
		break;

	case PR2000_NTSC_4_43:
		PR2000_InWidth     = 720;
		PR2000_InHeight    = 480;
		PR2000_InFrameRate = 2997;
		gtPR2000CurMode    = PR2000_NTSC_4_43;
		printf("NTSC 4.43\n");
		break;

		//case PR2000_PAL_M:
		//  PR2000_InWidth = 720;
		//  PR2000_InHeight = 480;
		//  PR2000_InFrameRate = 2997;
		//  gtPR2000CurMode = PR2000_PAL_M;
		//  printf("PAL_M\n");
		//  break;
		//case PR2000_PAL_60:
		//  PR2000_InWidth = 720;
		//  PR2000_InHeight = 480;
		//  PR2000_InFrameRate = 3000;
		//  gtPR2000CurMode = PR2000_PAL_60;
		//  printf("PAL_60\n");
		//  break;
	case PR2000_PAL_B_G_H_I_D:
		PR2000_InWidth     = 720;
		PR2000_InHeight    = 576;
		PR2000_InFrameRate = 2500;
		gtPR2000CurMode    = PR2000_PAL_B_G_H_I_D;
		printf("0 PAL B/G/H/I/D\n");
		break;

		//case PR2000_SECAM:
		//  PR2000_InWidth = 720;
		//  PR2000_InHeight = 576;
		//  PR2000_InFrameRate = 2500;
		//  gtPR2000CurMode = PR2000_SECAM;
		//  printf("SECAM\n");
		//  break;
		//case PR2000_PAL_COMBINATION_N:
		//  PR2000_InWidth = 720;
		//  PR2000_InHeight = 576;
		//  PR2000_InFrameRate = 2500;
		//  gtPR2000CurMode = PR2000_PAL_COMBINATION_N;
		//  printf("PAL Combination N\n");
		//  break;
		//case PR2000_SECAM_525:
		//  PR2000_InWidth = 720;
		//  PR2000_InHeight = 480;
		//  PR2000_InFrameRate = 2997;
		//  gtPR2000CurMode = PR2000_SECAM_525;
		//  printf("SECAM 525\n");
		//  break;
	default:
		printf("Can not recognize\n");
		break;
	}
	*/
	return result;
}

uint16_t _PR2000_InputSelection()
{
	return PR2000_INPUT_YPBPR_PAL;
#if 0//def AUTO_DETECT_INPUT
	uint16_t Value;

	Value = _PR2000_ReadI2c_Byte(REG_STATUS1);
	if ((Value & 0x05) != 0x05)
	{
		Value = _PR2000_ReadI2c_Byte(0x00);
		if ((Value & 0x0F) == 0x06)
		{
			PR2000_Input_Mode(PR2000_INPUT_CVBS);
			return PR2000_INPUT_CVBS;
		}
		else
		{
			PR2000_Input_Mode(PR2000_INPUT_SVIDEO);
			return PR2000_INPUT_SVIDEO;
		}
	}
	else
	{
		Value = _PR2000_ReadI2c_Byte(0x00);
		if ((Value & 0x0F) == 0x06)
			return PR2000_INPUT_SVIDEO;
		else
			return PR2000_INPUT_CVBS;
	}
//#else
	uint16_t Value;
	Value = _PR2000_ReadI2c_Byte(0x00);
	if ((Value & 0x0F) == 0x06)
		return PR2000_INPUT_SVIDEO;
	else
		return PR2000_INPUT_CVBS;
#endif
}

void PR2000Initial(PR2000_INPUT_MODE mode)
{
	//PR2000 INIT

}

void PR2000_Input_Mode(PR2000_INPUT_MODE mode)
{
    input_mode = mode;
		//_Set_PR2000_Input_CVBS();
	/*
	//if (mode == PR2000_INPUT_CVBS)
		_Set_PR2000_Input_CVBS();
	else if (mode == PR2000_INPUT_SVIDEO)
		_Set_PR2000_Input_SVIDEO();
	else if (mode == PR2000_INPUT_YPBPR)
		_Set_PR2000_Input_YPrPb();

	//Y Range 16 - 235, UV Range 16 - 240
	_PR2000_WriteI2c_ByteMask(0x04, 0x00, 0x01);

	//_PR2000_WriteI2c_ByteMask(0x04, 0x01, 0x02);  // Benson test
	//Lock status set by horizontal lock and subcarrier lock
	//_PR2000_WriteI2c_ByteMask(0x51, 0x80, 0x80);

	//drive strength
	_PR2000_WriteI2c_Byte(0xF4, 0x04);
	*/
}

MMP_BOOL PR2000_IsStable()
{
/*
	uint16_t Value;
	uint16_t IsStable;

	if (!gtPR2000InitDone)
		return MMP_FALSE;

	Value = _PR2000_ReadI2c_Byte(REG_STATUS1);
	if (((Value & 0x85) == 0x05) || ((Value & 0x85) == 0x81)) //Color Burst or No Color Burst
	{
		Get_Auto_Detection_Result();

		if (gtPR2000CurMode != gtPR2000PreMode)
		{
			printf("--------PR2000 Resolution = \n");
			if (gtPR2000CurMode == PR2000_NTSM_M_J)
				printf("NTSM_M_J\n");
			else if (gtPR2000CurMode == PR2000_NTSC_4_43)
				printf( "NTSC_4_43\n");
			else if (gtPR2000CurMode == PR2000_PAL_M)
				printf("PAL_M\n");
			else if (gtPR2000CurMode == PR2000_PAL_60)
				printf("PAL_60\n");
			else if (gtPR2000CurMode == PR2000_PAL_B_G_H_I_D)
				printf("PAL_B_G_H_I_D\n");
			else if (gtPR2000CurMode == PR2000_SECAM)
				printf("SECAM\n");
			else if (gtPR2000CurMode == PR2000_PAL_COMBINATION_N)
				printf("PAL_COMBINATION_N\n");
			else if (gtPR2000CurMode == PR2000_SECAM_525)
				printf("SECAM_525\n");
			else
				printf("Unknow Format\n");

			if (gtPR2000CurDev == PR2000_INPUT_CVBS)
				printf("----CVBS ---------\n");
			else if (gtPR2000CurDev == PR2000_INPUT_SVIDEO)
				printf("----S-Video ------\n");

			gtPR2000PreMode = gtPR2000CurMode;
		}
		IsStable = MMP_TRUE;
	}
	else
	{
		gtPR2000CurDev = _PR2000_InputSelection();
		IsStable        = MMP_FALSE;
	}

	return IsStable;
	*/
	return MMP_TRUE;
}

void PR2000_PowerDown(
    MMP_BOOL enable)
{
/*
	//When PDBP is set to 1, setting the PWRDWN bit switches the PR2000 to a chip-wide power-down mode.
	_PR2000_WriteI2c_ByteMask(POWER_MANAGEMENT, 0x04, 0x04);

	if (enable)
	{
		gtPR2000InitDone = MMP_FALSE;
		_PR2000_WriteI2c_ByteMask(POWER_MANAGEMENT, 0x20, 0x20);
	}
	else
		_PR2000_WriteI2c_ByteMask(POWER_MANAGEMENT, 0x00, 0x20);
*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//X10LightDriver_t1.c
void PR2000Initialize(void)
{
	printf("PR2000Initialize here\n");
	//PR2000Initial(PR2000_INPUT_CVBS);
}

void PR2000Terminate(void)
{
	printf("PR2000Terminate\n");
}

void PR2000OutputPinTriState(unsigned char flag)
{
/*
	if (flag == MMP_TRUE)
		Set_PR2000_Tri_State_Enable();
	else
		Set_PR2000_Tri_State_Disable();
		*/
}

unsigned char PR2000IsSignalStable(void)
{
	MMP_BOOL isStable;
	isStable = PR2000_IsStable();
	return isStable;
}

void PR2000GetProperty(CAP_GET_PROPERTY * pGetProperty)
{
	uint16_t i;

	//Get_Auto_Detection_Result();
	//pGetProperty->GetTopFieldPolarity = MMP_TRUE;
    if(input_mode == PR2000_INPUT_YPBPR_PAL)// || input_mode == PR2000_INPUT_YPBPR_NTSC)
    {
	    pGetProperty->GetHeight = 720;//PR2000_InHeight;
	    pGetProperty->GetWidth  = 1280+16;//PR2000_InWidth;
	    pGetProperty->Rate = 2500;//PR2000_InFrameRate;
	    pGetProperty->GetModuleIsInterlace = 0;
		pGetProperty->GetTopFieldPolarity = MMP_TRUE;
    }
    else if(input_mode == PR2000_INPUT_YPBPR_NTSC)
    {
	    pGetProperty->GetHeight = 720;//PR2000_InHeight;
	    pGetProperty->GetWidth  = 1280+16;//PR2000_InWidth;
	    pGetProperty->Rate = 3000;//PR2000_InFrameRate;
	    pGetProperty->GetModuleIsInterlace = 0;
		pGetProperty->GetTopFieldPolarity = MMP_TRUE;
    }
    else if(input_mode == PR2000_INPUT_CVBS_PAL )//|| input_mode == PR2000_INPUT_CVBS_NTSC)
    {   
        pGetProperty->GetHeight = 576 - 2;//PR2000_InHeight;
	    pGetProperty->GetWidth  = 720 +16;//PR2000_InWidth;
	    pGetProperty->Rate = 2500;//PR2000_InFrameRate;
	    pGetProperty->GetModuleIsInterlace = 1;
		pGetProperty->GetTopFieldPolarity = MMP_FALSE;
    }
    else if(input_mode == PR2000_INPUT_CVBS_NTSC)
    {   
        pGetProperty->GetHeight = 480 - 2;//PR2000_InHeight;
	    pGetProperty->GetWidth  = 720 + 16;//PR2000_InWidth;
	    pGetProperty->Rate = 2997;//PR2000_InFrameRate;
	    pGetProperty->GetModuleIsInterlace = 1;
		pGetProperty->GetTopFieldPolarity = MMP_FALSE;
    }
    for (i = 0; i < (sizeof(PR2000_TABLE) / sizeof(CAP_TIMINFO_TABLE)); ++i)
    {
       if ((pGetProperty->GetWidth == PR2000_TABLE[i].HActive) && (pGetProperty->GetHeight == PR2000_TABLE[i].VActive) && pGetProperty->Rate == PR2000_TABLE[i].Rate)
        {
            pGetProperty->HPolarity       = PR2000_TABLE[i].HPolarity; 
            pGetProperty->VPolarity       = PR2000_TABLE[i].VPolarity;
            pGetProperty->FrameRate       = PR2000_TABLE[i].FrameRate; 
            pGetProperty->matchResolution = MMP_TRUE;

			pGetProperty->HStar  = PR2000_TABLE[i].HStar;
			pGetProperty->HEnd   = PR2000_TABLE[i].HEnd;
			pGetProperty->VStar1 = PR2000_TABLE[i].VStar1;
			pGetProperty->VEnd1  = PR2000_TABLE[i].VEnd1;
			pGetProperty->VStar2 = PR2000_TABLE[i].VStar2;
			pGetProperty->VEnd2  = PR2000_TABLE[i].VEnd2;
		}
	}
}

void PR2000PowerDown(unsigned char enable)
{
	PR2000_PowerDown(enable);
}

void PR2000ForCaptureDriverSetting(CAP_CONTEXT *Capctxt )
{
    /* Input Format default Setting */
    if(input_mode == PR2000_INPUT_YPBPR_PAL || input_mode == PR2000_INPUT_YPBPR_NTSC)
        Capctxt->Interleave   = Progressive;
    else
	    Capctxt->Interleave   = Interleaving;

	/* Input Data Format Setting */
	// 8bit bus
	Capctxt->YUV422Format  = CAP_IN_YUV422_UYVY;
	Capctxt->EnDEMode             = MMP_FALSE; //MMP_TRUE;//my.wei enable DE mode for BT_601
	Capctxt->input_protocol       = BT_601;
}

static void PR2000CaptureModuleDriver_Destory(CaptureModuleDriver base)
{
	PR2000CaptureModuleDriver self = (PR2000CaptureModuleDriver)base;
	if(self)
		free(self);
}

static CaptureModuleDriverInterfaceStruct interface =
{
	PR2000Initialize,
	PR2000Terminate,
	PR2000OutputPinTriState,
	PR2000IsSignalStable,
	PR2000GetProperty,
	PR2000PowerDown,
	PR2000ForCaptureDriverSetting,
	PR2000CaptureModuleDriver_Destory
};

CaptureModuleDriver PR2000CaptureModuleDriver_Create()
{
	PR2000CaptureModuleDriver self = calloc(1, sizeof(PR2000CaptureModuleDriverStruct));
	self->base.vtable = &interface;
	self->base.type = "PR2000";
	return (PR2000CaptureModuleDriver)self;
}
//end of X10LightDriver_t1.c
