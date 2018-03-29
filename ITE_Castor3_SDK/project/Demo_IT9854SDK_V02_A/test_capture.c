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
#include "jpg/ite_jpg.h"
#include "ite_avienc.h"
#include "i2s/i2s.h"

static bool  do_once = true;
static pthread_t tid;
static pthread_t isp_tid;
static pthread_t jpeg_enc_tid;
static pthread_t audio_tid;
static pthread_t avi_tid;
static bool     b_RECORDING = false;
//static pthread_mutex_t  recording_mutex = PTHREAD_MUTEX_INITIALIZER;

bool stopTest;

#define CaptureDevName CFG_CAPTURE_MODULE_NAME
#define SecondCaptureDevName CFG_SECOND_CAPTURE_MODULE_NAME
#define Motion_Detection 1
#define CAP_420MODE  3

#define JPEG_BUF_LENGTH (256 << 10)

//ISP
#define SHORT_WAIT_UNTIL(expr)    while (!(expr)) { usleep(1000);  }
static ISP_DEVICE      gIspDev;

//REC
#define REC_FILE_NAME   "a:/rec.avi"
#define REC_FILE_WIDTH  720//800
#define REC_FILE_HEIGHT 576//480
#define REC_FILE_FPS    25
#define FRAME_TO_ENCODE 1000

//Audio
#define SUPPORT_REC_AUDIO 1
#define ADC_BUFFER_SIZE (64*1024-8)
static STRC_I2S_SPEC spec_adI = {0};
uint8_t *adc_buf;


#if Motion_Detection
static uint8_t *ref_outY = NULL;
static bool     firstY = true;
static uint8_t  cnt = 0;
static bool     motion_detected = false;



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
} MOTION_DETECTION;
#endif

typedef enum SEND_STATE_TAG
{
    SEND_BEGIN,
    SEND_START,
    SEND_STOP,
} SEND_STATE;

typedef void (*pfPktReleaseCb) (void *pkt);

typedef struct YUVInputPkt
{
	uint8_t  *ya;       /// address of Y decoded video buffer
	uint8_t  *ua;       /// address of U decoded video buffer
	uint8_t  *va;       /// address of V decoded video buffer
	uint32_t src_w;     /// width of decoded video buffer
	uint32_t src_h;     /// height of decoded video buffer
	uint32_t pitch_y;   /// pitch of Y
	uint32_t pitch_uv;  /// pitch of UV
	uint32_t format;    /// YUV format. see MMP_ISP_INFORMAT.
} YUVInputPkt;

typedef struct DetYUVInputPkt
{
	uint32_t	framePitchY;
	uint32_t	framePitchUV;
	uint32_t	frameWidth;
	uint32_t	frameHeight;
	uint8_t     *pYUVBuffer;
	uint32_t    YUVBufferSize;
} DetYUVInputPkt;

typedef struct JpegInputPkt
{
	uint8_t *pInputBuffer;
	uint32_t bufferSize;
} JpegInputPkt;

typedef struct AudioInputPkt
{
	uint8_t *pInputBuffer;
	uint32_t bufferSize;
} AudioInputPkt;

typedef struct QueuePktList
{
	void* pkt;
	struct QueuePktList *next;
} QueuePktList;

typedef struct PktQueue
{
	QueuePktList *firstPkt, *lastPkt;
	int numPackets;
	int maxPackets;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	pfPktReleaseCb pfPktRelease;
} PktQueue;

PktQueue gYUVInputQueue = { 0 };
PktQueue gDet_YUVInputQueue = { 0 };
PktQueue gJpegInputQueue = { 0 };
PktQueue gAudioInputQueue = { 0 };

static pfPktReleaseCb
_yuvInputPktRelease(
    void* pkt)
{
	YUVInputPkt *ptYUVInputPkt = (YUVInputPkt*)pkt;
	free(pkt);
}

static pfPktReleaseCb
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

static pfPktReleaseCb
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

static pfPktReleaseCb
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


static int
_packetQueuePut(
    PktQueue *q,
    void *pkt)
{
	QueuePktList *pkt1;

	/* duplicate the packet */
	if (q->mutex)
	{
		while(q->numPackets > q->maxPackets)
		{
			printf("queue is full: cur: %u, max: %u\n", q->numPackets, q->maxPackets);
			usleep(1000*100);
		}
		pkt1 = malloc(sizeof(QueuePktList));
		if (!pkt1) return -1;
		pkt1->pkt =  pkt;
		pkt1->next = NULL;

		pthread_mutex_lock(&q->mutex);

		if (!q->lastPkt)
			q->firstPkt = pkt1;
		else
			q->lastPkt->next = pkt1;
		q->lastPkt = pkt1;
		q->numPackets++;

		/* XXX: should duplicate packet data in DV case */
		pthread_cond_signal(&q->cond);
		pthread_mutex_unlock(&q->mutex);
		return 0;
	}
	else
	{
		return -1;
	}
}

/* packet queue handling */
static void
_packetQueueInit(
    PktQueue *q,
    void* pfPktRelease,
    int maxPackets)
{
	memset(q, 0, sizeof(PktQueue));
	pthread_mutex_init(&q->mutex, NULL);
	pthread_cond_init(&q->cond, NULL);
	q->maxPackets = maxPackets;
	q->pfPktRelease = pfPktRelease;
}

static void
_packetQueueFlush(
    PktQueue *q)
{
	QueuePktList *pkt, *pkt1;
	if (q->mutex)
	{
		pthread_mutex_lock(&q->mutex);
		for (pkt = q->firstPkt; pkt != NULL; pkt = pkt1)
		{
			pkt1 = pkt->next;
			if (q->pfPktRelease)
			{
				q->pfPktRelease(pkt->pkt);
			}
			free(pkt);
		}
		q->lastPkt = NULL;
		q->firstPkt = NULL;
		q->numPackets = 0;
		pthread_mutex_unlock(&q->mutex);
	}
}

static void
_packetQueueEnd(
    PktQueue *q)
{
	if (q->mutex)
	{
		_packetQueueFlush(q);
		pthread_mutex_destroy(&q->mutex);
		pthread_cond_destroy(&q->cond);
		memset(q, 0, sizeof(PktQueue));
	}
}

static int
_packetQueueGet(
    PktQueue *q,
    void **pkt,
    int block)
{
	QueuePktList *pkt1;
	int ret;
	if (q->mutex)
	{
		pthread_mutex_lock(&q->mutex);

		for (;;)
		{
			pkt1 = q->firstPkt;
			if (pkt1)
			{
				q->firstPkt = pkt1->next;
				if (!q->firstPkt)
					q->lastPkt = NULL;
				q->numPackets--;
				*pkt = pkt1->pkt;
				free(pkt1);
				ret = 1;
				break;
			}
			else if (!block)
			{
				ret = 0;
				break;
			}
			else
			{
				pthread_cond_wait(&q->cond, &q->mutex);
			}
		}
		pthread_mutex_unlock(&q->mutex);
		return ret;
	}
	else
	{
		return -1;
	}
}

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

#if 0
void argb8888toyuv420(char *yuv_dst, char *argb_src, int width, int height)
{
	int line, i = 0;
	int u_pos = width * height;
	int v_pos = u_pos + u_pos/4;
	int pixel_count = 0;

	for(line = 0; line < height; line++)
	{
		if(line % 2 == 0)
		{
			for(i = 0; i < width; i+=2)
			{
				char r, g, b;
				b = argb_src[4 * pixel_count + 0];
				g = argb_src[4 * pixel_count + 1];
				r = argb_src[4 * pixel_count + 2];

				yuv_dst[pixel_count++] = ((66*r + 129*g + 25*b) >> 8) + 16;
				yuv_dst[u_pos++] = ((-38*r + -74*g + 112*b) >> 8) + 128;
				yuv_dst[v_pos++] = ((112*r + -94*g + -18*b) >> 8) + 128;

				b = argb_src[4 * pixel_count + 0];
				g = argb_src[4 * pixel_count + 1];
				r = argb_src[4 * pixel_count + 2];

				yuv_dst[pixel_count++] = ((66*r + 129*g + 25*b) >> 8) + 16;
			}
		}
		else
		{
			for(i = 0; i < width; i++)
			{
				char r, g, b;
				b = argb_src[4 * pixel_count + 0];
				g = argb_src[4 * pixel_count + 1];
				r = argb_src[4 * pixel_count + 2];

				yuv_dst[pixel_count++] = ((66*r + 129*g + 25*b) >> 8) + 16;
			}
		}
	}
}
#endif

static void ayuv444toyuv420(char *yuv420_dst, char *ayuv444_src, int width, int height)
{
	int line, i = 0;
	int u_pos = width * height;
	int v_pos = u_pos + u_pos/4;
	int pixel_count = 0;

	for(line = 0; line < height; line++)
	{
		if(line % 2 == 0)
		{
			for(i = 0; i < width; i+=2)
			{
				char y, u, v;
				v = ayuv444_src[4 * pixel_count];
				u = ayuv444_src[4 * pixel_count + 1];
				y = ayuv444_src[4 * pixel_count + 2];

				yuv420_dst[pixel_count++] = y;
				yuv420_dst[u_pos++] = u;
				yuv420_dst[v_pos++] = v;

				y = ayuv444_src[4 * pixel_count + 2];

				yuv420_dst[pixel_count++] = y;
			}
		}
		else
		{
			for(i = 0; i < width; i++)
			{
				char y;
				y = ayuv444_src[4 * pixel_count + 2];
				yuv420_dst[pixel_count++] = y;
			}
		}
	}
}

static void *Isp_process(void)
{
	//uint32_t tick1, tick2;
	//static bool do_once = true;
	YUVInputPkt *ptYUVPkt = NULL;
	unsigned char *RGB_Buffer, *YUV_Buffer;

	MMP_ISP_OUTPUT_INFO out_info  = {0};
	MMP_ISP_SHARE       isp_share = {0};

	mmpIspInitialize(&gIspDev);

	while(b_RECORDING)
	{
		if(_packetQueueGet(&gYUVInputQueue, (void**) &ptYUVPkt, 0) > 0)
		{
			//tick1 = SDL_GetTicks();
			isp_share.addrY   = (uint32_t)ptYUVPkt->ya;
			isp_share.addrU   = (uint32_t)ptYUVPkt->ua;
			isp_share.addrV   = (uint32_t)ptYUVPkt->va;
			isp_share.width   = ptYUVPkt->src_w;
			isp_share.height  = ptYUVPkt->src_h;
			isp_share.pitchY  = ptYUVPkt->pitch_y;
			isp_share.pitchUv = ptYUVPkt->pitch_uv;
			isp_share.format  = ptYUVPkt->format;

			RGB_Buffer = (char*) malloc (2*1024*1024);

			out_info.addrRGB = (uint32_t)RGB_Buffer;
			out_info.width = REC_FILE_WIDTH;//ithLcdGetWidth();
			out_info.height = REC_FILE_HEIGHT;//ithLcdGetHeight();
			out_info.pitchRGB = REC_FILE_WIDTH * 4;//ithLcdGetPitch() * 2;
			out_info.format = MMP_ISP_OUT_RGB888;

			mmpIspEnable(gIspDev, MMP_ISP_DEINTERLACE);
			mmpIspSetMode(gIspDev, MMP_ISP_MODE_TRANSFORM);
			mmpIspEnable(gIspDev, MMP_ISP_DUMMY_CS);
			mmpIspSetOutputWindow(gIspDev, &out_info);
			mmpIspPlayImageProcess(gIspDev, &isp_share);
			//printf("YC: %s, %d\n", __FUNCTION__, __LINE__);
			SHORT_WAIT_UNTIL(mmpIspIsEngineIdle());
			//printf("YC: %s, %d, out_info.width = %d, out_info.height = %d\n", __FUNCTION__, __LINE__, out_info.width, out_info.height);
#if 0
			if(do_once)
			{
				do_once = false;
				FILE *fp;
				fp = fopen("A:/aaa.rgb", "wb");
				fwrite(RGB_Buffer, 1, out_info.width * out_info.height * 4, fp);
				fclose(fp);
			}
#endif

			YUV_Buffer = (char*) malloc (out_info.width * out_info.height * 3/2);
			ayuv444toyuv420(YUV_Buffer, RGB_Buffer, out_info.width, out_info.height);
			{
				DetYUVInputPkt *Det_YUVPkt = (DetYUVInputPkt*) malloc(sizeof(DetYUVInputPkt));
				if (Det_YUVPkt)
				{
					Det_YUVPkt->frameWidth = out_info.width;
					Det_YUVPkt->frameHeight = out_info.height;
					Det_YUVPkt->framePitchY = out_info.width;
					Det_YUVPkt->framePitchUV = out_info.width/2;
					Det_YUVPkt->pYUVBuffer = YUV_Buffer;
					Det_YUVPkt->YUVBufferSize = out_info.width * out_info.height * 3/2;
					_packetQueuePut(&gDet_YUVInputQueue, Det_YUVPkt);
				}
			}
			_yuvInputPktRelease(ptYUVPkt);
			free(RGB_Buffer);
			//tick2 = SDL_GetTicks();
			//if(tick2 - tick1 > 20)
			//printf("ISP process waste %d ms\n", tick2 - tick1);
		}
		else
			usleep(3000);
	}

	usleep(100*1000);

	_packetQueueEnd(&gYUVInputQueue);
	pthread_exit(NULL);
}

static void *Jpeg_Enc(void)
{
	//uint32_t tick1, tick2;
	DetYUVInputPkt *ptYUVPkt = NULL;
	unsigned char *jpegStream;

	HJPG            *pHJpeg        = 0;
	JPG_INIT_PARAM  initParam      = {0};
	JPG_STREAM_INFO inStreamInfo   = {0};
	JPG_STREAM_INFO outStreamInfo  = {0};
	JPG_BUF_INFO    entropyBufInfo = {0};
	JPG_USER_INFO   jpgUserInfo    = {0};
	uint32_t        jpgEncSize     = 0;

	while(b_RECORDING)
	{
		if(_packetQueueGet(&gDet_YUVInputQueue, (void**) &ptYUVPkt, 0) > 0)
		{
			//tick1 = SDL_GetTicks();
			//printf("gYUVInputQueue number = %d\n", gYUVInputQueue.numPackets);
			//pthread_mutex_lock(&recording_mutex);
			unsigned char *pAddr_y = 0, *pAddr_u = 0, *pAddr_v = 0;

			pAddr_y = ptYUVPkt->pYUVBuffer;
			pAddr_u = ptYUVPkt->pYUVBuffer + (ptYUVPkt->framePitchY * ptYUVPkt->frameHeight);
			pAddr_v = pAddr_u + (ptYUVPkt->framePitchUV * ptYUVPkt->frameHeight/2);

			initParam.codecType = JPG_CODEC_ENC_JPG;
			initParam.outColorSpace = JPG_COLOR_SPACE_YUV420;
			initParam.width         = ptYUVPkt->frameWidth;
			initParam.height        = ptYUVPkt->frameHeight;
			initParam.encQuality    = 70;//85;
			iteJpg_CreateHandle(&pHJpeg, &initParam, 0);
			inStreamInfo.streamIOType         = JPG_STREAM_IO_READ;
			inStreamInfo.streamType           = JPG_STREAM_MEM;
			// Y
			inStreamInfo.jstream.mem[0].pAddr = (uint8_t *)pAddr_y; //YUV_Save;
			inStreamInfo.jstream.mem[0].pitch = ptYUVPkt->framePitchY;      // src_w_out;

			// U
			inStreamInfo.jstream.mem[1].pAddr = (uint8_t *)pAddr_u; //(inStreamInfo.jstream.mem[0].pAddr+H264_pitch_y*src_h_out);
			inStreamInfo.jstream.mem[1].pitch = ptYUVPkt->framePitchUV;     //src_w_out/2;

			// V
			inStreamInfo.jstream.mem[2].pAddr = (uint8_t *)pAddr_v; //(inStreamInfo.jstream.mem[1].pAddr+H264_pitch_y*src_h_out);
			inStreamInfo.jstream.mem[2].pitch = ptYUVPkt->framePitchUV;     //src_w_out/2;

			inStreamInfo.validCompCnt         = 3;

			jpegStream = (char*) malloc (sizeof(char)*JPEG_BUF_LENGTH);

			outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
			outStreamInfo.streamType            = JPG_STREAM_MEM;
			outStreamInfo.jpg_reset_stream_info = 0;

			outStreamInfo.jstream.mem[0].pAddr  = jpegStream;
			outStreamInfo.jstream.mem[0].pitch  = ptYUVPkt->frameWidth;
			outStreamInfo.jstream.mem[0].length = JPEG_BUF_LENGTH;

			outStreamInfo.validCompCnt          = 1;

			iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, &outStreamInfo, 0);
			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);

			iteJpg_Setup(pHJpeg, 0);

			iteJpg_Process(pHJpeg, &entropyBufInfo, &jpgEncSize, 0);

			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
			//printf("\n\tresult = %d, encode size = %f KB\n", jpgUserInfo.status, (float)jpgEncSize / 1024);

			iteJpg_DestroyHandle(&pHJpeg, 0);
			//pthread_mutex_unlock(&recording_mutex);
			{
				JpegInputPkt *jpegPkt = (JpegInputPkt*) malloc(sizeof(JpegInputPkt));
				if (jpegPkt)
				{
					jpegPkt->pInputBuffer = jpegStream;
					jpegPkt->bufferSize = jpgEncSize + 4;
					_packetQueuePut(&gJpegInputQueue, jpegPkt);
				}
			}

			_detYuvInputPktRelease(ptYUVPkt);
			//tick2 = SDL_GetTicks();
			//if(tick2 - tick1 > 20)
			//printf("JPEG ENC waste %d ms\n", tick2 - tick1);
		}
		else
			usleep(5000);
	}

	usleep(100*1000);
	_packetQueueEnd(&gDet_YUVInputQueue);
	pthread_exit(NULL);
}

static void initAD()
{
	/* init ADC */
	adc_buf = (uint8_t*)malloc(ADC_BUFFER_SIZE);
	memset((uint8_t*) adc_buf, 0, ADC_BUFFER_SIZE);
	memset((void*)&spec_adI, 0, sizeof(STRC_I2S_SPEC));
	spec_adI.channels                   = 1;
	spec_adI.sample_rate                = 8000;
	spec_adI.buffer_size                = ADC_BUFFER_SIZE;
	spec_adI.is_big_endian              = 0;
	spec_adI.base_i2s                   = (uint8_t*) adc_buf;
	spec_adI.sample_size                = 16;
	spec_adI.record_mode                = 1;
	spec_adI.from_LineIN                = 0;
	spec_adI.from_MIC_IN                = 1;
	i2s_init_ADC(&spec_adI);
	i2s_pause_ADC(1);
}

#if (SUPPORT_REC_AUDIO)
static void *GetAudioRawDataFromADC(void *arg)
{
	initAD();
	i2s_ADC_set_direct_volstep(80);

	i2s_pause_ADC(0);

	while(b_RECORDING)
	{
		int bsize = 0;
		int sizecount = 0;
		uint8_t* audio_data = NULL;
		audio_data = (uint8_t*)malloc(16000);

		usleep(980*1000);

		while(sizecount < 16000)
		{
			uint32_t AD_r = I2S_AD32_GET_RP();//9850
			uint32_t AD_w = I2S_AD32_GET_WP();//9850

			if (AD_r <= AD_w)
			{
				bsize = AD_w - AD_r;
				if (sizecount + bsize > 16000)
					bsize = 16000 - sizecount;

				if (bsize)
				{
					ithInvalidateDCacheRange(adc_buf + AD_r, bsize);
					memcpy(audio_data + sizecount, adc_buf + AD_r, bsize);
					AD_r += bsize;
					I2S_AD32_SET_RP(AD_r);
					sizecount += bsize;
				}
			}
			else
			{
				// AD_r > AD_w
				bsize = (ADC_BUFFER_SIZE - AD_r) + AD_w;
				if (sizecount + bsize > 16000)
					bsize = 16000 - sizecount;

				if (bsize)
				{
					//printf("AD_r %u, AD_w %u bsize %u \n", AD_r, AD_w, bsize );
					uint32_t szsec0, szsec1;
					szsec0 = ADC_BUFFER_SIZE - AD_r;
					if(bsize < szsec0)
						szsec0 = bsize;
					szsec1 = bsize - szsec0;

					ithInvalidateDCacheRange(adc_buf + AD_r, szsec0);
					memcpy(audio_data + sizecount, adc_buf + AD_r, szsec0);
					if(szsec1)
					{
						ithInvalidateDCacheRange(adc_buf, szsec1);
						memcpy(audio_data + sizecount + szsec0, adc_buf, szsec1);
					}

					if(szsec1)
						AD_r = szsec1;
					else
						AD_r += szsec0;
					I2S_AD32_SET_RP(AD_r);

					sizecount += bsize;
				}
			}
		}

		//put into _packetqueue
		AudioInputPkt *audioPkt = (AudioInputPkt*) malloc(sizeof(AudioInputPkt));
		if (audioPkt)
		{
			audioPkt->pInputBuffer = audio_data;
			audioPkt->bufferSize = sizecount;
			_packetQueuePut(&gAudioInputQueue, audioPkt);
		}
		//usleep(600*1000);
	}

	i2s_pause_ADC(1);
	pthread_exit(NULL);

}
#endif

static void *PackageMjpegToAVI(void *arg)
{
	//uint32_t tick1, tick2 = 0;
	JpegInputPkt *ptJpegPkt = NULL;
	struct ite_avi_t *avi;
	int enc_count = 0;

	AudioInputPkt *ptAudioPkt = NULL;
	struct ite_avi_audio_t auds;
	auds.bits = 16;
	auds.channels = 1;
	auds.samples_per_second = 8000;

#if (SUPPORT_REC_AUDIO)
	avi = ite_avi_open(REC_FILE_NAME, REC_FILE_WIDTH, REC_FILE_HEIGHT, "MJPG", REC_FILE_FPS, &auds);
#else
	avi = ite_avi_open(REC_FILE_NAME, REC_FILE_WIDTH, REC_FILE_HEIGHT, "MJPG", REC_FILE_FPS, NULL);
#endif
	printf("Encode to AVI Start.\n");
	while(1)
	{
		if(_packetQueueGet(&gJpegInputQueue, (void**) &ptJpegPkt, 0) > 0)
		{
			ite_avi_add_frame(avi, ptJpegPkt->pInputBuffer, ptJpegPkt->bufferSize);
			_jpegInputPktRelease(ptJpegPkt);
#if (SUPPORT_REC_AUDIO)
			if((enc_count+1)%REC_FILE_FPS == 0)
			{
				while(1)
				{
					if (_packetQueueGet(&gAudioInputQueue, (void**) &ptAudioPkt, 0) > 0)
					{
						ite_avi_add_audio(avi, ptAudioPkt->pInputBuffer, ptAudioPkt->bufferSize);
						_audioInputPktRelease(ptAudioPkt);
						break;
					}
					else
					{
						//printf("wait 1 ms\n");
						usleep(1000);
					}
				}
			}
#endif
			enc_count++;
			if(enc_count == FRAME_TO_ENCODE)
			{
				b_RECORDING = false;
				motion_detected = false;
				break;
			}
		}
		else
			usleep(1000);
	}
	ite_avi_close(avi);
	printf("Encode to AVI Finish.\n");
	usleep(100*1000);
	_packetQueueEnd(&gJpegInputQueue);
#if (SUPPORT_REC_AUDIO)
	_packetQueueEnd(&gAudioInputQueue);
#endif
	pthread_exit(NULL);
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
#endif

void SettingISPAnd_FilpLCD(
    void)
{
	static int         New_state   = 0;
	static int         Old_state   = 0;
	static int         state_count = 0;
	static int         cap_idx     = 0;
	uint8_t            *dbuf       = NULL;
	ITV_DBUF_PROPERTY  dbufprop    = {0};
	ITE_CAP_VIDEO_INFO outdata     = {0};
	unsigned char *YUVRawData;
	int YUVRawData_Size = 0;

	ithCaptureGetNewFrame(&outdata);
#if Motion_Detection
	if(++cnt > 3)
	{
		MOTION_DETECTION   motion_info;

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
		if(ref_outY && !b_RECORDING)
			motion_detected = Check_Motion_Detection(motion_info, ref_outY, outdata.DisplayAddrY);
		//else
		//printf("motion check buffer invalid!\n");
		cnt = 0;
	}
#endif
	dbuf = itv_get_dbuf_anchor();
	if (dbuf == NULL)
		return;

	if(outdata.IsInterlaced)
		itv_enable_isp_feature(MMP_ISP_DEINTERLACE);

	if(motion_detected && !b_RECORDING && do_once)
	{
		do_once = false;
		b_RECORDING = true;

		_packetQueueInit(&gYUVInputQueue, _yuvInputPktRelease, 20);
		pthread_create(&isp_tid, NULL, Isp_process, NULL);

		_packetQueueInit(&gJpegInputQueue, _jpegInputPktRelease, 20);
		pthread_create(&avi_tid, NULL, PackageMjpegToAVI, NULL);

		_packetQueueInit(&gDet_YUVInputQueue, _detYuvInputPktRelease, 20);
		pthread_create(&jpeg_enc_tid, NULL, Jpeg_Enc, NULL);
#if (SUPPORT_REC_AUDIO)
		_packetQueueInit(&gAudioInputQueue, _audioInputPktRelease, 20);
		pthread_create(&audio_tid, NULL, GetAudioRawDataFromADC, NULL);
#endif
	}

	if(motion_detected && b_RECORDING)
	{
		YUVInputPkt *InPkt = (YUVInputPkt*) malloc(sizeof(YUVInputPkt));
		if (InPkt)
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
	}

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
		}
		while (!stopTest);
	}
	return NULL;
}
