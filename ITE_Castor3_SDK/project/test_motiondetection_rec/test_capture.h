#ifndef TEST_CAPTURE_H
#define TEST_CAPTURE_H

#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "ite/itp.h"

#define Motion_Detection 1
#define SUPPORT_REC_AUDIO 1

#define REC_FILE_NAME   "a:/rec.avi"
#define REC_FILE_WIDTH  720//800
#define REC_FILE_HEIGHT 576//480
#define REC_FILE_FPS    25
#define FRAME_TO_ENCODE 1000

typedef void (*pfPktReleaseCb) (void *pkt);
typedef struct YUVInputPkt {
    uint8_t  *ya;       /// address of Y decoded video buffer
    uint8_t  *ua;       /// address of U decoded video buffer
    uint8_t  *va;       /// address of V decoded video buffer
    uint32_t src_w;     /// width of decoded video buffer
    uint32_t src_h;     /// height of decoded video buffer
    uint32_t pitch_y;   /// pitch of Y
    uint32_t pitch_uv;  /// pitch of UV
    uint32_t format;    /// YUV format. see MMP_ISP_INFORMAT.
} YUVInputPkt;

typedef struct DetYUVInputPkt {
    uint32_t	framePitchY;
    uint32_t	framePitchUV;
    uint32_t	frameWidth;
    uint32_t	frameHeight;
    uint8_t     *pYUVBuffer;
    uint32_t    YUVBufferSize;
} DetYUVInputPkt;

typedef struct SWYUVInputPkt{	
    uint32_t    frameWidth; 
    uint32_t    frameHeight;    
    uint8_t     *pRGBBuffer;
} SWYUVInputPkt;

typedef struct JpegInputPkt {
    uint8_t *pInputBuffer;
    uint32_t bufferSize;
} JpegInputPkt;

typedef struct AudioInputPkt {
    uint8_t *pInputBuffer;
    uint32_t bufferSize;
} AudioInputPkt;

typedef struct QueuePktList {
    void* pkt;
    struct QueuePktList *next;
} QueuePktList;

typedef struct PktQueue {
    QueuePktList *firstPkt, *lastPkt;
    int numPackets;
    int maxPackets;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pfPktReleaseCb pfPktRelease;
} PktQueue;

void _packetQueuePut(PktQueue *q, void *pkt);
void _packetQueueInit(PktQueue *q, void* pfPktRelease, int maxPackets);
void _packetQueueFlush(PktQueue *q);
void _packetQueueEnd(PktQueue *q);
int _packetQueueGet(PktQueue *q, void **pkt, int block);

void *sw_yuv(void);
void *Isp_process(void);
void *Jpeg_Enc(void);
void *GetAudioRawDataFromADC(void *arg);
void *PackageMjpegToAVI(void *arg);

#endif
