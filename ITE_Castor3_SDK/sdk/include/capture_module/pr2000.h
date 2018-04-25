#ifndef __PR2000_H__
#define __PR2000_H__

#include "ite/itp.h"
#include "ite/ith_defs.h"
#include "ite/ith.h"

#include "encoder/encoder_types.h"
#include "capture_module.h" //Benson

#ifdef __cplusplus
extern "C" {
#endif

/*
 *
 */
//#define	MMP_BOOL	unsigned char

#ifndef MMP_TRUE
    #define MMP_TRUE  1
#endif
#ifndef MMP_FALSE
    #define MMP_FALSE 0
#endif

#define COMPOSITE_DEV //Benson

typedef enum _PR2000_INPUT_MODE
{
    PR2000_INPUT_CVBS_PAL   = 0,
    PR2000_INPUT_CVBS_NTSC   = 1,
    PR2000_INPUT_YPBPR_PAL  = 2,
    PR2000_INPUT_YPBPR_NTSC  = 3,
    PR2000_INPUT_TOTAL = 4,
} PR2000_INPUT_MODE;

typedef enum _PR2000_INPUT_STANDARD
{
    PR2000_NTSM_M_J          = 0x0,
    PR2000_NTSC_4_43         = 0x1,
    PR2000_PAL_M             = 0x2,
    PR2000_PAL_60            = 0x3,
    PR2000_PAL_B_G_H_I_D     = 0x4,
    PR2000_SECAM             = 0x5,
    PR2000_PAL_COMBINATION_N = 0x6,
    PR2000_SECAM_525         = 0x7,
} PR2000_INPUT_STANDARD;

typedef enum CAP_FRAMERATE_TAG
{
    CAP_FRAMERATE_UNKNOW = 0,
    CAP_FRAMERATE_25HZ,
    CAP_FRAMERATE_50HZ,
    CAP_FRAMERATE_30HZ,
    CAP_FRAMERATE_60HZ,
    CAP_FRAMERATE_29_97HZ,
    CAP_FRAMERATE_59_94HZ,
    CAP_FRAMERATE_23_97HZ,
    CAP_FRAMERATE_24HZ,
    CAP_FRAMERATE_56HZ,
    CAP_FRAMERATE_70HZ,
    CAP_FRAMERATE_72HZ,
    CAP_FRAMERATE_75HZ,
    CAP_FRAMERATE_85HZ,
    CAP_FRAMERATE_VESA_30HZ,
    CAP_FRAMERATE_VESA_60HZ
} CAP_FRAMERATE;

uint16_t PR2000_InWidth;
uint16_t PR2000_InHeight;
uint16_t PR2000_InFrameRate;
extern PR2000_INPUT_MODE input_mode;

void Set_PR2000_Tri_State_Enable();

void Set_PR2000_Tri_State_Disable();

PR2000_INPUT_STANDARD Get_Auto_Detection_Result_1();

void PR2000Initial(PR2000_INPUT_MODE mode);

void PR2000_Input_Mode(PR2000_INPUT_MODE mode);

MMP_BOOL PR2000_IsStable();

void PR2000_PowerDown(
    MMP_BOOL enable);

//X10LightDriver_t1.h
typedef struct PR2000CaptureModuleDriverStruct *PR2000CaptureModuleDriver;
CaptureModuleDriver PR2000CaptureModuleDriver_Create();
static void PR2000CaptureModuleDriver_Destory(CaptureModuleDriver base);
void PR2000Initialize(void);
void PR2000Terminate(void);
void PR2000OutputPinTriState(unsigned char flag);
void PR2000GetProperty(CAP_GET_PROPERTY *pGetProperty);
void PR2000PowerDown(unsigned char enable);
void PR2000ForCaptureDriverSetting(CAP_CONTEXT *Capctxt );
unsigned char PR2000IsSignalStable(void);
//end of X10LightDriver_t1.h

#ifdef __cplusplus
}
#endif

#endif