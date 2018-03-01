#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"
#include "capture_s/ite_capture.h"
#include "isp/mmp_isp.h"   //for ISP
#include "ite/itv.h"       //for VideoInit()
#include "ite/ith_video.h" //for VideoInit()
#include "capture_s/capture_types.h"
//#include "jpg/ite_jpg.h"
//#include "ite_avienc.h"
//#include "i2s/i2s.h"
#include "test_capture.h"

static pthread_t tid;
bool stopTest;

#define CaptureDevName CFG_CAPTURE_MODULE_NAME
#define SecondCaptureDevName CFG_SECOND_CAPTURE_MODULE_NAME
#define CAP_420MODE  3

#if Motion_Detection
static uint8_t *ref_outY = NULL;
static bool     firstY = true;
static uint8_t  cnt = 0;

static bool  do_once = true;
static pthread_t isp_tid;
static pthread_t sw_yuv_tid;
static pthread_t jpeg_enc_tid;
static pthread_t audio_tid;
static pthread_t avi_tid;
bool     record_720p_bypass = false;
bool     b_RECORDING = false;
bool     motion_detected = false;

typedef struct MOTION_DETECTION_TAG
{   
    uint32_t motion_sensitivity;
    uint8_t  motion_threshold;
    uint32_t motion_start_x;
    uint32_t motion_end_x;
    uint32_t motion_start_y;
    uint32_t motion_end_y;
    bool     motion_mask_enable;
    uint32_t motion_mask_start_x;
    uint32_t motion_mask_end_x;
    uint32_t motion_mask_start_y;
    uint32_t motion_mask_end_y;
}MOTION_DETECTION;
#endif

typedef enum SEND_STATE_TAG
{
    SEND_BEGIN,
    SEND_START,
    SEND_STOP,
} SEND_STATE;

#if Motion_Detection
PktQueue gYUVInputQueue = { 0 };
PktQueue gDet_YUVInputQueue = { 0 };
PktQueue gJpegInputQueue = { 0 };
PktQueue gAudioInputQueue = { 0 };
PktQueue gSW_YUVInputQueue = { 0 };

pfPktReleaseCb
_yuvInputPktRelease(
    void* pkt)
{
    YUVInputPkt *ptYUVInputPkt = (YUVInputPkt*)pkt;    
    free(pkt);
}

pfPktReleaseCb
_sw_yuvInputPktRelease(
    void* pkt)
{
	SWYUVInputPkt *ptSWYUVInputPkt = (SWYUVInputPkt*)pkt;
	free(pkt);
}

pfPktReleaseCb
_detYuvInputPktRelease(
    void* pkt)
{
    DetYUVInputPkt *ptDetYUVInputPkt = (DetYUVInputPkt*)pkt;
    if (ptDetYUVInputPkt && ptDetYUVInputPkt->pYUVBuffer && ptDetYUVInputPkt->YUVBufferSize)
    {
        free(ptDetYUVInputPkt->pYUVBuffer);
    }
    free(pkt);
}

pfPktReleaseCb
_jpegInputPktRelease(
    void* pkt)
{
    JpegInputPkt *ptJpegInputPkt = (JpegInputPkt*)pkt;
    if (ptJpegInputPkt && ptJpegInputPkt->pInputBuffer && ptJpegInputPkt->bufferSize)
    {
        free(ptJpegInputPkt->pInputBuffer);
    }
    free(pkt);
}

pfPktReleaseCb
_audioInputPktRelease(
    void* pkt)
{
    AudioInputPkt *ptAudioInputPkt = (AudioInputPkt*)pkt;
    if (ptAudioInputPkt && ptAudioInputPkt->pInputBuffer && ptAudioInputPkt->bufferSize)
    {
        free(ptAudioInputPkt->pInputBuffer);
    }
    free(pkt);
}
#endif

static void
VideoInit(
    void)
{
    ithVideoInit(NULL);
    itv_init();
}

static void
VideoExit(
    void)
{
    /* release dbuf & itv */
    itv_flush_dbuf();
    itv_deinit();

    /* release decoder stuff */
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
    ithVideoExit();
    printf("%s(%d)\n", __FUNCTION__, __LINE__);
}

#if Motion_Detection
static uint8_t*
Check_Motion_Init(
    void)
{
    return malloc(CFG_CAPTURE_HEIGHT*CFG_CAPTURE_WIDTH);
}

static void
Check_Motion_Terminate(
    uint8_t* y_data)
{
    free(y_data);
}

static bool
Check_Motion_Detection(
    MOTION_DETECTION   motion_info,
    uint8_t* ref_data,
    uint8_t* y_data)
{
    uint32_t i, j, k, pi_diff = 0, cell_diff = 0;

    if(firstY == false)
    {
        for(i = motion_info.motion_start_y; i < motion_info.motion_end_y; i+=8)
        {
            for(j = motion_info.motion_start_x; j < motion_info.motion_end_x; j++)
            {
                if(motion_info.motion_mask_enable && (j >= motion_info.motion_mask_start_x) && (j <= motion_info.motion_mask_end_x) && (i >= motion_info.motion_mask_start_y) && (i <= motion_info.motion_mask_end_y))
                    continue;
                for(k = i; k < i+8; k++)
                {                 
                    if(ref_data[CFG_CAPTURE_HEIGHT*k+j] - y_data[CFG_CAPTURE_HEIGHT*k+j] > motion_info.motion_threshold)
                        pi_diff++;
                }
                if(!(j%8))
                {
                    if(pi_diff > 8)
                        cell_diff++;
                    pi_diff = 0;
                }
            }
        }
        printf("y diff=%d\n", cell_diff);    
    }
    memcpy(ref_data, y_data, CFG_CAPTURE_HEIGHT*CFG_CAPTURE_WIDTH);
    firstY = false;

    if(cell_diff > motion_info.motion_sensitivity)
        return true;
    else
        return false;
}

static void Motion_Info_Initialize(MOTION_DETECTION   motion_info)
{
    motion_info.motion_threshold = 10;
    motion_info.motion_sensitivity = 10;
    motion_info.motion_start_x = 0;
    motion_info.motion_end_x = 720;
    motion_info.motion_start_y = 288;
    motion_info.motion_end_y = 576;
    motion_info.motion_mask_enable = true;
    motion_info.motion_mask_start_x = 360;
    motion_info.motion_mask_end_x =720;
    motion_info.motion_mask_start_y = 288;
    motion_info.motion_mask_end_y = 576;
}

static Motion_Detection_PreStart()
{
    if(record_720p_bypass == false)
    {
	    _packetQueueInit(&gYUVInputQueue, _yuvInputPktRelease, 50);
	    pthread_create(&isp_tid, NULL, Isp_process, NULL);

        _packetQueueInit(&gSW_YUVInputQueue, _sw_yuvInputPktRelease, 50);
	    pthread_create(&sw_yuv_tid, NULL, sw_yuv, NULL);
    }
    
    _packetQueueInit(&gJpegInputQueue, _jpegInputPktRelease, 20);
    pthread_create(&avi_tid, NULL, PackageMjpegToAVI, NULL);
    
    if(record_720p_bypass == false)
    {
	    _packetQueueInit(&gDet_YUVInputQueue, _detYuvInputPktRelease, 50);
	    pthread_create(&jpeg_enc_tid, NULL, Jpeg_Enc, NULL);
    }
    else
    {
        _packetQueueInit(&gDet_YUVInputQueue, _yuvInputPktRelease, 50);
	    pthread_create(&jpeg_enc_tid, NULL, Jpeg_Enc, NULL);
    }
#if (SUPPORT_REC_AUDIO)
    _packetQueueInit(&gAudioInputQueue, _audioInputPktRelease, 20);
    pthread_create(&audio_tid, NULL, GetAudioRawDataFromADC, NULL);
#endif
}
#endif

void SettingISPAnd_FilpLCD(
    void)
{
    static int         New_state   = 0;
    static int         Old_state   = 0;
    static int         state_count = 0;
    static int         cap_idx     = 0;
    ITV_DBUF_PROPERTY  dbufprop    = {0};
    ITE_CAP_VIDEO_INFO outdata     = {0};
    unsigned char *YUVRawData;
    int YUVRawData_Size = 0;

    ithCaptureGetNewFrame(&outdata);
#if Motion_Detection
    if(++cnt > 3)
    {
        MOTION_DETECTION   motion_info;        
        Motion_Info_Initialize(motion_info);
        
        if(ref_outY && !b_RECORDING)
            motion_detected = Check_Motion_Detection(motion_info, ref_outY, outdata.DisplayAddrY);
        //else
            //printf("motion check buffer invalid!\n");
        cnt = 0; 
    }
#endif

	if(outdata.IsInterlaced)
		itv_enable_isp_feature(MMP_ISP_DEINTERLACE); 

#if Motion_Detection
    if(motion_detected && !b_RECORDING && do_once)
    {
        do_once = false;
        b_RECORDING = true;
        Motion_Detection_PreStart();
    }
    
    if(motion_detected && b_RECORDING)
    {
        YUVInputPkt *InPkt = (YUVInputPkt*) malloc(sizeof(YUVInputPkt));
        if (InPkt)
        {
            if(record_720p_bypass == false)
            {
                InPkt->src_w    = outdata.OutWidth;
    			InPkt->src_h    = outdata.OutHeight;
    			InPkt->pitch_y  = outdata.PitchY;
    			InPkt->pitch_uv = outdata.PitchUV;
    			InPkt->format   = MMP_ISP_IN_YUV420;
    			InPkt->ya       = outdata.DisplayAddrY;
    			InPkt->ua       = outdata.DisplayAddrU;
    			InPkt->va       = outdata.DisplayAddrV;
    			_packetQueuePut(&gYUVInputQueue, InPkt);
            } 
            else
            {
        		InPkt->src_w    = outdata.OutWidth;
        		InPkt->src_h    = outdata.OutHeight;
        		InPkt->pitch_y  = outdata.PitchY;
        		InPkt->pitch_uv = outdata.PitchUV;
        		InPkt->format   = MMP_ISP_IN_YUV420;
        		InPkt->ya       = outdata.DisplayAddrY;
        		InPkt->ua       = outdata.DisplayAddrU;
        		InPkt->va       = outdata.DisplayAddrV;
        		_packetQueuePut(&gDet_YUVInputQueue, InPkt);
            }
        }        
    }     
#endif    
    dbufprop.src_w    = outdata.OutWidth;
    dbufprop.src_h    = outdata.OutHeight;
    dbufprop.pitch_y  = outdata.PitchY;
    dbufprop.pitch_uv = outdata.PitchUV;
    dbufprop.format   = MMP_ISP_IN_YUV420;
    dbufprop.ya       = outdata.DisplayAddrY;
    dbufprop.ua       = outdata.DisplayAddrU;
    dbufprop.va       = outdata.DisplayAddrV;

    //printf("dbufprop.ya=0x%x,dbufprop.ua=0x%x,dbufprop.va=0x%x,dbufprop.src_w=%d,dbufprop.src_h=%d,dbufprop.pitch_y=%d,dbufprop.pitch_uv=%d\n",dbufprop.ya,dbufprop.ua,dbufprop.va,dbufprop.src_w,dbufprop.src_h,dbufprop.pitch_y,dbufprop.pitch_uv);
    itv_update_dbuf_anchor(&dbufprop);

    return;
}

#ifndef WIN32
static void* DrawVideoSurface(void* arg)
{
	ITUSurface* lcdSurf;
    lcdSurf = ituGetDisplaySurface();

	while(!stopTest)
    {
        ituDrawVideoSurface(lcdSurf, 0, 0, ithLcdGetWidth(), ithLcdGetHeight());
        ituFlip(lcdSurf);
        usleep(20000);
    }
    pthread_exit(NULL);
}
#endif

void *TestFunc(void *arg)
{
    itpInit();
    ioctl(ITP_DEVICE_SCREEN, ITP_IOCTL_POST_RESET, NULL);
    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_RESET, NULL);

    ioctl(ITP_DEVICE_BACKLIGHT, ITP_IOCTL_ON, NULL);

    while (!stopTest)
    {
        struct  timeval     start,end;
        unsigned long       diff;
		uint16_t            bSignalStable = 0;
		CaptureModuleDriver IrSensor;
        SEND_STATE          gState = SEND_BEGIN;
        sleep(3);


#ifndef WIN32

		// init itu
		ituLcdInit();

#ifdef CFG_M2D_ENABLE
		ituM2dInit();
#ifdef CFG_VIDEO_ENABLE
		ituFrameFuncInit();
#endif // CFG_VIDEO_ENABLE
#else
		ituSWInit();
#endif // CFG_M2D_ENABLE
		
#if defined(CFG_BUILD_ITV) && !defined(CFG_TEST_VIDEO) 
		itv_set_pb_mode(1);
#endif
		pthread_create(&tid, NULL, DrawVideoSurface, NULL);
#else
		VideoInit();
#endif

#if Motion_Detection
        if(ref_outY == NULL)
            ref_outY = Check_Motion_Init();
#endif        

        do
        {
            switch (gState)
            {
            case SEND_BEGIN:
                ithCapInitialize();
                ithCapSetMode(CAP_420MODE);
            
 				IrSensor = (CaptureModuleDriver)CaptureModuleDriver_GetDevice(CaptureDevName);
				ithCaptureSetModule(IrSensor);

				//bSignalStable = ithCapDeviceIsSignalStable();
				while (!ithCapDeviceIsSignalStable()) printf("Capture device not stable!!\n");
 

                printf("ith9850CaptureRun\n");
                ithCapFire();
                //gettimeofday(&start, NULL);
                gState = SEND_START;
                break;

            case SEND_START:
                SettingISPAnd_FilpLCD();
               // mmpDumpReg();
                //gettimeofday(&end, NULL);
                //diff = (end.tv_sec) - (start.tv_sec);
                //if (diff >= 30)
                //{
                //    gState = SEND_STOP;
                //}
                break;

            case SEND_STOP:
                printf("ith9850CaptureTerminate\n");
                ithCapTerminate();
#if Motion_Detection                
                if(ref_outY)
                    Check_Motion_Terminate(ref_outY);
#endif                
                usleep(1000 * 1000 * 3);
                gState = SEND_BEGIN;
                break;
            }
        } while (!stopTest);
    }
    return NULL;
}
