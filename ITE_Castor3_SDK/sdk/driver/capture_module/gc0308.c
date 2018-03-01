#include <unistd.h>
#include "iic/mmp_iic.h"
#include "capture_module/gc0308.h"
#include "capture_module.h" 

//=============================================================================
//                Constant Definition
//=============================================================================
static uint8_t GC0308_IICADDR = 0x42 >> 1;  
#define POWER_MANAGEMENT 0x1A
#define RESET_MASK       (1 << 7)

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

typedef struct GC0308CaptureModuleDriverStruct
{
    CaptureModuleDriverStruct base;
} GC0308CaptureModuleDriverStruct;

//=============================================================================
//                Global Data Definition
//=============================================================================
static MMP_BOOL gtGC0308InitDone = MMP_FALSE;

static REGPAIR  INPUT_REG[] =
{
    {0x03,0x02},
    {0x04,0x65},
    {0x0E,0x02},
    {0x14,0x12},
    {0x17,0x01},
    {0x1A,0x1E},
    {0x1C,0x49},
    {0x1D,0x98},
    {0x1E,0x60},
    {0x1F,0x16},
    {0x20,0x18},
    {0x21,0x00},
    {0x23,0x02},
    {0x24,0xa2},
    {0x25,0x0F},
    {0x26,0x02},
    {0x30,0xF7},
    {0x32,0x00},
    {0x33,0x32},
    {0x34,0x31},
    {0x35,0x32},
    {0x36,0x01},
    {0x37,0x01},
    {0x38,0x01},
    {0x39,0x08},
    {0x3B,0x20},
    {0x51,0x60},
    {0x52,0x80},
    {0xB3,0x30},
};    

static CAP_TIMINFO_TABLE GC0308_TABLE [] = {
    //Index, HActive, VActive,  Rate,            FrameRate,                 Hpor,   Vpor,  HStar,     HEnd,   VStar1,   VEnd1,  VStar2,   VEnd2,
    {0,     640,    480,        1000,   CAP_FRAMERATE_10HZ,                   0,      0,      0,        0,        0,      0,   0,       0   }, //480i60     // Benson
};      

//=============================================================================
//                Private Function Definition
//=============================================================================
static uint8_t _GC0308_ReadI2c_Byte(uint8_t RegAddr)
{
    uint8_t    data;
    MMP_RESULT flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicReceiveData(IIC_PORT_0, IIC_MASTER_MODE, GC0308_IICADDR, &RegAddr, 1, &data, sizeof(data))))
    {
        printf("GC0308 I2C Read error, reg = %02x\n", RegAddr);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return data;
}

static MMP_RESULT _GC0308_WriteI2c_Byte(uint8_t RegAddr, uint8_t data)
{
    MMP_RESULT flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicSendData(IIC_PORT_0, IIC_MASTER_MODE, GC0308_IICADDR, RegAddr, &data, sizeof(data))))
    {
        printf("GC0308 I2c write error, reg = %02x val =%02x\n", RegAddr, data);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return flag;
}

static MMP_RESULT _GC0308_WriteI2c_ByteMask(uint8_t RegAddr, uint8_t data, uint8_t mask)
{
    uint8_t    value;
    MMP_RESULT flag;

    //mmpIicLockModule(IIC_PORT_0);
    if (0 != (flag = mmpIicReceiveData(IIC_PORT_0, IIC_MASTER_MODE, GC0308_IICADDR, &RegAddr, 1, &value, 1)))
    {
        printf("GC0308 I2C Read error, reg = %02x\n", RegAddr);
        mmpIicGenStop(IIC_PORT_0);
    }

    value = ((value & ~mask) | (data & mask));

    if (0 != (flag = mmpIicSendData(IIC_PORT_0, IIC_MASTER_MODE, GC0308_IICADDR, RegAddr, &value, 1)))
    {
        printf("GC0308 I2c write error, reg = %02x val =%02x\n", RegAddr, value);
        mmpIicGenStop(IIC_PORT_0);
    }
    //mmpIicReleaseModule(IIC_PORT_0);
    return flag;
}

static void _GC0308_SWReset()
{
    _GC0308_WriteI2c_ByteMask(0xFE, RESET_MASK, RESET_MASK);
}

static void _Set_GC0308_Input_Setting(void)
{
    uint16_t i;

    for (i = 0; i < (sizeof(INPUT_REG) / sizeof(REGPAIR)); i++)
    {
        _GC0308_WriteI2c_Byte(INPUT_REG[i].addr, INPUT_REG[i].value);
    }
}

//=============================================================================
//                Public Function Definition
//=============================================================================

void GC0308Initial()
{
    printf("GC0308Initial\n");
    _Set_GC0308_Input_Setting();
    gtGC0308InitDone = MMP_TRUE;
}


void GC0308_PowerDown(
    MMP_BOOL enable)
{
    //When PDBP is set to 1, setting the PWRDWN bit switches the GC0308 to a chip-wide power-down mode.
    if (enable)
    {
        gtGC0308InitDone = MMP_FALSE;
        _GC0308_WriteI2c_ByteMask(POWER_MANAGEMENT, 0x01, 0x01);
        _GC0308_WriteI2c_ByteMask(0X25, 0x0, 0xff);
    }
    else
    {
        _GC0308_WriteI2c_ByteMask(POWER_MANAGEMENT, 0x00, 0x01);
        _GC0308_WriteI2c_ByteMask(0X25, 0xff, 0xff);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//X10LightDriver_t1.c
void GC0308Initialize(void)
{
    printf("GC0308Initialize here\n");
    //TODO: change to PLL1Out2 17.7MHZ MCLK, 0x320/3/15=17.7
	ithWriteRegMaskH(ITH_SENSOR_CLK_REG, 0x01 << 8 , ITH_SENSOR_CLK_SEL_MASK); //17.7Mhz
    ithWriteRegMaskH(ITH_SENSOR_CLK_REG, 0x0E, ITH_SENSOR_CLK_RATIO_MASK);  //17.7Mhz //from PLL1Out2
    usleep(10000);
    GC0308Initial();
}

void GC0308Terminate(void)
{
    printf("GC0308Terminate\n");
}

void GC0308OutputPinTriState(unsigned char flag)
{
}

unsigned char GC0308IsSignalStable(void)
{
	return true;
}

void GC0308GetProperty(CAP_GET_PROPERTY * pGetProperty)
{
    uint16_t i;
    pGetProperty->GetTopFieldPolarity = MMP_FALSE;
    pGetProperty->GetHeight = 480;
    pGetProperty->GetWidth  = 640;
    pGetProperty->Rate = 1000;
    pGetProperty->GetModuleIsInterlace = 0;

    for (i = 0; i < (sizeof(GC0308_TABLE) / sizeof(CAP_TIMINFO_TABLE)); ++i)
    {
       if ((pGetProperty->GetWidth == GC0308_TABLE[i].HActive) && (pGetProperty->GetHeight == GC0308_TABLE[i].VActive) && pGetProperty->Rate == GC0308_TABLE[i].Rate)
        {
            pGetProperty->HPolarity       = GC0308_TABLE[i].HPolarity; 
            pGetProperty->VPolarity       = GC0308_TABLE[i].VPolarity;
            pGetProperty->FrameRate       = GC0308_TABLE[i].FrameRate; 
            pGetProperty->matchResolution = MMP_TRUE;

            pGetProperty->HStar  = GC0308_TABLE[i].HStar;
            pGetProperty->HEnd   = GC0308_TABLE[i].HEnd;
            pGetProperty->VStar1 = GC0308_TABLE[i].VStar1;
            pGetProperty->VEnd1  = GC0308_TABLE[i].VEnd1;
            pGetProperty->VStar2 = GC0308_TABLE[i].VStar2; 
            pGetProperty->VEnd2  = GC0308_TABLE[i].VEnd2; 
        }
    }
}

void GC0308PowerDown(unsigned char enable)
{
    GC0308_PowerDown(enable);
}

void GC0308ForCaptureDriverSetting(CAP_CONTEXT *Capctxt )
{
    printf("GC0308ForCaptureDriverSetting\n");
    /* Input Format default Setting */
    Capctxt->Interleave   = Progressive;

    /* Input Data Format Setting */
    // 8bit bus
    Capctxt->YUV422Format  = CAP_IN_YUV422_YUYV;
    Capctxt->EnDEMode             = MMP_TRUE;
    Capctxt->input_protocol       = BT_601;
    
    Capctxt->input_video_source.HSYNCDE =   MMP_TRUE;
}

static void GC0308CaptureModuleDriver_Destory(CaptureModuleDriver base)
{
    GC0308CaptureModuleDriver self = (GC0308CaptureModuleDriver)base;
    if(self)
        free(self);
}

static CaptureModuleDriverInterfaceStruct interface =
{
    GC0308Initialize,
    GC0308Terminate,
    GC0308OutputPinTriState,
    GC0308IsSignalStable,
    GC0308GetProperty,
    GC0308PowerDown,
    GC0308ForCaptureDriverSetting,
    GC0308CaptureModuleDriver_Destory
};

CaptureModuleDriver GC0308CaptureModuleDriver_Create()
{
    GC0308CaptureModuleDriver self = calloc(1, sizeof(GC0308CaptureModuleDriverStruct));
    self->base.vtable = &interface;
    self->base.type = "GC0308";
    return (GC0308CaptureModuleDriver)self;
}
//end of X10LightDriver_t1.c
