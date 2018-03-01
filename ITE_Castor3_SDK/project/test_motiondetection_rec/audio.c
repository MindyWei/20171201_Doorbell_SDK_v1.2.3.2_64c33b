#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "test_capture.h"
#include "i2s/i2s.h"

#define ADC_BUFFER_SIZE (64*1024-8)
static STRC_I2S_SPEC spec_adI = {0};
uint8_t *adc_buf;

extern PktQueue gAudioInputQueue;
extern bool b_RECORDING;

static void initAD(){
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

#if (Motion_Detection && SUPPORT_REC_AUDIO)
void *GetAudioRawDataFromADC(void *arg)
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
            { // AD_r > AD_w
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


