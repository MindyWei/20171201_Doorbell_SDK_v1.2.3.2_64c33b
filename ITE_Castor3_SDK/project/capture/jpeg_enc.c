#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test_capture.h"
#include "jpg/ite_jpg.h"

#define JPEG_BUF_LENGTH (256 << 10)

extern PktQueue gJpegInputQueue;
extern PktQueue gDet_YUVInputQueue;
extern bool b_RECORDING;
extern bool record_720p_bypass;

extern pfPktReleaseCb _detYuvInputPktRelease(void* pkt);

#if(Motion_Detection)
void *Jpeg_Enc(void)
{
    //uint32_t tick1, tick2;
    unsigned char *jpegStream;
    
    HJPG            *pHJpeg        = 0;
    JPG_INIT_PARAM  initParam      = {0};
    JPG_STREAM_INFO inStreamInfo   = {0};
    JPG_STREAM_INFO outStreamInfo  = {0};
    JPG_BUF_INFO    entropyBufInfo = {0};
    JPG_USER_INFO   jpgUserInfo    = {0};
    uint32_t        jpgEncSize     = 0;
	struct timeval tv;
	struct tm* timeinfo;
	int len;
	char filename[34] = "e:/photo/IMG_";
	char name_buf[20];
	
	len = strlen(filename);
	len++;
	gettimeofday(&tv, NULL);
	timeinfo = localtime((const time_t*)&tv.tv_sec);
	strftime(name_buf, 20, "%Y%m%d_%H%M%S.jpg", timeinfo);   
	strcat(filename, name_buf);
	if(b_SNAPSHOT)
	{
		printf("--------------->%s\n",filename);
		show_snap_rec = 1;
	}

	while(b_RECORDING || b_SNAPSHOT)
	{
	    jpegStream = NULL;
	    if(record_720p_bypass == false && b_SNAPSHOT == false)
        {
            DetYUVInputPkt *ptYUVPkt = NULL;
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
    			inStreamInfo.jstream.mem[0].pitch = ptYUVPkt->framePitchY;// src_w_out;

    			// U
    			inStreamInfo.jstream.mem[1].pAddr = (uint8_t *)pAddr_u; //(inStreamInfo.jstream.mem[0].pAddr+H264_pitch_y*src_h_out);
    			inStreamInfo.jstream.mem[1].pitch = ptYUVPkt->framePitchUV;     //src_w_out/2;

    			// V
    			inStreamInfo.jstream.mem[2].pAddr = (uint8_t *)pAddr_v; //(inStreamInfo.jstream.mem[1].pAddr+H264_pitch_y*src_h_out);
    			inStreamInfo.jstream.mem[2].pitch = ptYUVPkt->framePitchUV;     //src_w_out/2;

    			inStreamInfo.validCompCnt         = 3;

    			outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
    			outStreamInfo.streamType            = JPG_STREAM_MEM;

		        if (filename && b_SNAPSHOT)
		        {
		         	outStreamInfo.streamType   = JPG_STREAM_FILE;
		          	outStreamInfo.jstream.path = (void *)filename;
				    outStreamInfo.jpg_reset_stream_info = 0;
		        }
			    else
			    {
			        jpegStream = (char*) malloc (sizeof(char)*JPEG_BUF_LENGTH);
				    outStreamInfo.jpg_reset_stream_info = 0;

				    outStreamInfo.jstream.mem[0].pAddr  = jpegStream;
				    outStreamInfo.jstream.mem[0].pitch  = ptYUVPkt->frameWidth;
				    outStreamInfo.jstream.mem[0].length = JPEG_BUF_LENGTH;

				    outStreamInfo.validCompCnt          = 1;
			    }
    			iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, &outStreamInfo, 0);
    			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);

    			iteJpg_Setup(pHJpeg, 0);

    			iteJpg_Process(pHJpeg, &entropyBufInfo, &jpgEncSize, 0);

    			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    			//printf("\n\tresult = %d, encode size = %f KB\n", jpgUserInfo.status, (float)jpgEncSize / 1024);

    			iteJpg_DestroyHandle(&pHJpeg, 0);
    			//pthread_mutex_unlock(&recording_mutex);
			    if(b_RECORDING)
			    {
				    JpegInputPkt *jpegPkt = (JpegInputPkt*) malloc(sizeof(JpegInputPkt));
				    if (jpegPkt)
				    {
					    jpegPkt->pInputBuffer = jpegStream;
					    jpegPkt->bufferSize = jpgEncSize + 4;
					    _packetQueuePut(&gJpegInputQueue, jpegPkt);
				    }
			    }
                else
                {
                    if(jpegStream)
                        free(jpegStream);                      
                }

			    _detYuvInputPktRelease(ptYUVPkt);

			    if(b_SNAPSHOT)
			    {
				    b_SNAPSHOT = false;
				    show_snap_rec = 4;
					snap_stop = true;
			    }
			    //tick2 = SDL_GetTicks();
			    //if(tick2 - tick1 > 20)
			    //printf("JPEG ENC waste %d ms\n", tick2 - tick1);
		    }
		    else
			    usleep(5000);
        }
        else
        {
            YUVInputPkt *ptYUVPkt = NULL;
    		if(_packetQueueGet(&gDet_YUVInputQueue, (void**) &ptYUVPkt, 0) > 0)
    		{
    			//tick1 = SDL_GetTicks();
    			//printf("gYUVInputQueue number = %d\n", gYUVInputQueue.numPackets);
    			//pthread_mutex_lock(&recording_mutex);
    			unsigned char *pAddr_y = 0, *pAddr_u = 0, *pAddr_v = 0;
                
                pAddr_y = ptYUVPkt->ya;
                pAddr_u = ptYUVPkt->ua;
                pAddr_v = ptYUVPkt->va;

			    initParam.codecType = JPG_CODEC_ENC_JPG;
			    initParam.outColorSpace = JPG_COLOR_SPACE_YUV420;

                initParam.width         = ptYUVPkt->src_w;
			    initParam.height        = ptYUVPkt->src_h;
            
    			initParam.encQuality    = 70;//85;
    			iteJpg_CreateHandle(&pHJpeg, &initParam, 0);
    			inStreamInfo.streamIOType         = JPG_STREAM_IO_READ;
    			inStreamInfo.streamType           = JPG_STREAM_MEM;
    			// Y
    			inStreamInfo.jstream.mem[0].pAddr = (uint8_t *)pAddr_y; //YUV_Save;
    			inStreamInfo.jstream.mem[0].pitch = ptYUVPkt->pitch_y;// src_w_out;

    			// U
    			inStreamInfo.jstream.mem[1].pAddr = (uint8_t *)pAddr_u; //(inStreamInfo.jstream.mem[0].pAddr+H264_pitch_y*src_h_out);
    			inStreamInfo.jstream.mem[1].pitch = ptYUVPkt->pitch_uv;     //src_w_out/2;

    			// V
    			inStreamInfo.jstream.mem[2].pAddr = (uint8_t *)pAddr_v; //(inStreamInfo.jstream.mem[1].pAddr+H264_pitch_y*src_h_out);
    			inStreamInfo.jstream.mem[2].pitch = ptYUVPkt->pitch_uv;     //src_w_out/2;

    			inStreamInfo.validCompCnt         = 3;

    			outStreamInfo.streamIOType          = JPG_STREAM_IO_WRITE;
    			outStreamInfo.streamType            = JPG_STREAM_MEM;

		        if (filename && b_SNAPSHOT)
		        {
		         	outStreamInfo.streamType   = JPG_STREAM_FILE;
		          	outStreamInfo.jstream.path = (void *)filename;
				    outStreamInfo.jpg_reset_stream_info = 0;
		        }
			    else
			    {
			        jpegStream = (char*) malloc (sizeof(char)*JPEG_BUF_LENGTH);
				    outStreamInfo.jpg_reset_stream_info = 0;

				    outStreamInfo.jstream.mem[0].pAddr  = jpegStream;
				    outStreamInfo.jstream.mem[0].pitch  = ptYUVPkt->src_w;
				    outStreamInfo.jstream.mem[0].length = JPEG_BUF_LENGTH;

				    outStreamInfo.validCompCnt          = 1;
			    }
    			iteJpg_SetStreamInfo(pHJpeg, &inStreamInfo, &outStreamInfo, 0);
    			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);

    			iteJpg_Setup(pHJpeg, 0);

    			iteJpg_Process(pHJpeg, &entropyBufInfo, &jpgEncSize, 0);

    			iteJpg_GetStatus(pHJpeg, &jpgUserInfo, 0);
    			//printf("\n\tresult = %d, encode size = %f KB\n", jpgUserInfo.status, (float)jpgEncSize / 1024);

    			iteJpg_DestroyHandle(&pHJpeg, 0);
    			//pthread_mutex_unlock(&recording_mutex);
			    if(b_RECORDING)
			    {
				    JpegInputPkt *jpegPkt = (JpegInputPkt*) malloc(sizeof(JpegInputPkt));
				    if (jpegPkt)
				    {
					    jpegPkt->pInputBuffer = jpegStream;
					    jpegPkt->bufferSize = jpgEncSize + 4;
					    _packetQueuePut(&gJpegInputQueue, jpegPkt);
				    }
			    }
                else
                {
                    if(jpegStream)
                        free(jpegStream);                      
                }

			    _yuvInputPktRelease(ptYUVPkt);

			    if(b_SNAPSHOT)
			    {
				    b_SNAPSHOT = false;
				    show_snap_rec = 4;
					snap_stop = true;
					printf("snap_stop!\r\n");
			    }
			    //tick2 = SDL_GetTicks();
			    //if(tick2 - tick1 > 20)
			    //printf("JPEG ENC waste %d ms\n", tick2 - tick1);
		    }
		    else
			    usleep(5000);
        }
	}

	//usleep(100*1000);
	//_packetQueueEnd(&gDet_YUVInputQueue);
	pthread_exit(NULL);
}
#endif


