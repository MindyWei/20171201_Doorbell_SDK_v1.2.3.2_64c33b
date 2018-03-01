#include "test_item_isp.h"
#include "ite/itv.h"
#include "isp/mmp_isp.h"

//=============================================================================
//				  Constant Definition
//=============================================================================

//=============================================================================
//				  Macro Definition
//=============================================================================

//=============================================================================
//				  Structure Definition
//=============================================================================

//=============================================================================
//				  Global Data Definition
//=============================================================================
unsigned int g_bOffOtherFF = 1;      // ctrl use 2 frame function or not
ISP_DEVICE   gIspDev;

//=============================================================================
//				  Private Function Definition
//=============================================================================

//=============================================================================
//				  Public Function Definition
//=============================================================================
void
test_isp_colorTrans(
    uint8_t         *srcAddr_rgby,
    uint8_t         *srcAddr_u,
    uint8_t         *srcAddr_v,
    DATA_COLOR_TYPE colorType,
    CLIP_WND_INFO   *clipInfo,
    BASE_RECT       *srcRect)
{
    int                 result   = 0, OutputWidth = 0, OutputHeight = 0;
    MMP_ISP_OUTPUT_INFO outInfo  = {0};
    MMP_ISP_SHARE       ispInput = {0};

    ispInput.width        = srcRect->w;
    ispInput.height       = srcRect->h;
    ispInput.isAdobe_CMYK = 0;

    switch (colorType)
    {
    case DATA_COLOR_YUV444:
    case DATA_COLOR_YUV422:
    case DATA_COLOR_YUV422R:
    case DATA_COLOR_YUV420:
        ispInput.addrY = (uint32_t)srcAddr_rgby;
        ispInput.addrU = (uint32_t)srcAddr_u;
        ispInput.addrV = (uint32_t)srcAddr_v;
        switch (colorType)
        {
        case DATA_COLOR_YUV444:
            ispInput.format  = MMP_ISP_IN_YUV444;
            ispInput.pitchY  = srcRect->w;
            ispInput.pitchUv = srcRect->w;
            break;

        case DATA_COLOR_YUV422:
            ispInput.format  = MMP_ISP_IN_YUV422;
            ispInput.pitchY  = srcRect->w;
            ispInput.pitchUv = (srcRect->w >> 1);
            break;

        case DATA_COLOR_YUV422R:
            ispInput.format  = MMP_ISP_IN_YUV422R;
            ispInput.pitchY  = srcRect->w;
            ispInput.pitchUv = srcRect->w;
            break;

        case DATA_COLOR_YUV420:
            ispInput.format  = MMP_ISP_IN_YUV420;
            ispInput.pitchY  = srcRect->w;
            ispInput.pitchUv = (srcRect->w >> 1);
            break;
        }
        break;

    case DATA_COLOR_ARGB8888:
    case DATA_COLOR_ARGB4444:
    case DATA_COLOR_RGB565:
        ispInput.addrY = (uint32_t)srcAddr_rgby;
        switch (colorType)
        {
        case DATA_COLOR_ARGB8888:
            ispInput.format = MMP_ISP_IN_RGB888;
            ispInput.pitchY = (srcRect->w << 2);
            break;

        case DATA_COLOR_RGB565:
            ispInput.format = MMP_ISP_IN_RGB565;
            ispInput.pitchY = (srcRect->w << 1);
            break;

        case DATA_COLOR_ARGB4444:
            _err(" not support ARGB4444 !");
            return;
        }
        break;

    case DATA_COLOR_NV12:
    case DATA_COLOR_NV21:
        ispInput.addrY   = (uint32_t)srcAddr_rgby;
        ispInput.addrU   = (uint32_t)srcAddr_u;
        ispInput.pitchY  = srcRect->w;
        ispInput.pitchUv = srcRect->w;
        ispInput.format  = (colorType == DATA_COLOR_NV12) ? MMP_ISP_IN_NV12 : MMP_ISP_IN_NV21;
        break;
    }

    // ---------------------------------------
    // initial isp
    mmpIspTerminate(&gIspDev);

    result = mmpIspInitialize(&gIspDev);
    if (result)
        _err("mmpIspInitialize() error (0x%x) !!\n", result);

	#if (CFG_CHIP_FAMILY == 9850)
	result = mmpIspSetMode(gIspDev, MMP_ISP_MODE_JPEG_TRANSFORM);
#else
	result = mmpIspSetMode(gIspDev, MMP_ISP_MODE_TRANSFORM);
#endif
	
    if (result)
        _err("mmpIspSetMode() error (0x%x) !! \n", result);

    //Benson add
    OutputWidth  = _Get_Lcd_Width();
    OutputHeight = _Get_Lcd_Height();
    mmpIspSetDisplayWindow(gIspDev, 0, 0, OutputWidth, OutputHeight);
    mmpIspSetVideoWindow(gIspDev, 0, 0, OutputWidth, OutputHeight);
    mmpIspSetOutputFormat(gIspDev, MMP_ISP_OUT_DITHER565);
    //end of Benson

    // set isp output to LCD Buffer
#if 0
    outInfo.startX = 0;
    outInfo.startY = 0;
    outInfo.width  = _Get_Lcd_Width();
    outInfo.height = _Get_Lcd_Height();
    result         = mmpIspSetDisplayWindow(
        outInfo.startX, outInfo.startY,
        outInfo.width, outInfo.height);
#else
    outInfo.startX   = 0;
    outInfo.startY   = 0;
    outInfo.width    = _Get_Lcd_Width();
    outInfo.height   = _Get_Lcd_Height();
    outInfo.addrRGB  = _Get_Lcd_Addr_A();
    outInfo.pitchRGB = (uint16_t)_Get_Lcd_Pitch();
    outInfo.format   = MMP_ISP_OUT_DITHER565;
    mmpIspSetOutputWindow(gIspDev, &outInfo);

    //printf("outInfo.width = %d ,outInfo.height=%d ,outInfo.addrRGB=0x%x\n",outInfo.width,outInfo.height ,outInfo.addrRGB);
#endif
    if (result)
        _err("mmpIspSetDisplayWindow() error (0x%x) !!\n", result);

    // set clip window info
    if (clipInfo && clipInfo->bClipEnable)
    {
        BASE_RECT rect = {0, 50, 100, 150};
        if (clipInfo->bClipOutside == 0)
            Draw_Rect((uint8_t *)_Get_Lcd_Addr_A(), _Get_Lcd_Pitch(), &rect, ITH_RGB565(0xff, 0, 0xff));

        switch (clipInfo->clipWndId)
        {
        case 0:  mmpIspEnable(gIspDev, MMP_ISP_CLIPPING0);   break;
        case 1:  mmpIspEnable(gIspDev, MMP_ISP_CLIPPING1);   break;
        case 2:  mmpIspEnable(gIspDev, MMP_ISP_CLIPPING2);   break;
        }

        mmpIspSetClipWindow(
            gIspDev,
            clipInfo->clipWndId, clipInfo->bClipOutside,
            clipInfo->clipRect.x, clipInfo->clipRect.y,
            clipInfo->clipRect.w, clipInfo->clipRect.h);
    }

    // set isp source info and fire
    result = mmpIspPlayImageProcess(gIspDev, &ispInput);
    if (result)
        _err("mmpIspPlayImageProcess() error (0x%x) !!\n", result);

    result = mmpIspWaitEngineIdle(); //mmpIspTerminate();
    if (result)
        _err("mmpIspTerminate() error (0x%x) !!\n", result);
}