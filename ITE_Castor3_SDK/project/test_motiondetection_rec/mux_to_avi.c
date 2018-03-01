#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test_capture.h"
#include "ite_avienc.h"

extern PktQueue gJpegInputQueue;
extern PktQueue gAudioInputQueue;
extern bool b_RECORDING;
extern bool motion_detected;

extern pfPktReleaseCb _jpegInputPktRelease(void* pkt);
extern pfPktReleaseCb _audioInputPktRelease(void* pkt);

#if(Motion_Detection)
void *PackageMjpegToAVI(void *arg)
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
#endif
