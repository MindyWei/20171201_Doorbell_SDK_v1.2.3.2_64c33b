#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itu.h"
#include "config.h"
#include "scene.h"
#include "leaf/leaf_mediastream.h"

//extern LeafCall *call;
LeafCall *call = NULL;
static bool leafKeySoundPaused;
static bool leafSettingMode;

void LeafInit(void)
{
   	call = leaf_init();
    leaf_set_play_level(call, 80);
    leaf_set_ring_level(call, 70);
    leaf_set_rec_level(call, 70); 

    //call->dir = CallOutgoing;
    //leafKeySoundPaused=false;
    //leafSettingMode=false;
}

void LeafPlayKeySound(void)
{
    if(leafKeySoundPaused)
        return;
    
    leaf_start_sound_play(call,"A:/rings/key1.wav",Normalplay,NULL);
}

void LeafStratRingPreview(void)
{
    leafKeySoundPaused = true;
    call->dir = CallIncoming;
    //leaf_start_sound_play(call,"A:/rings/Let_It_Go.mp3",0,NULL);
    leaf_start_sound_play(call,"A:/rings/ring1.wav",RepeatPlay,NULL);
    leaf_init_audio_streams(call, 6666);
    leaf_init_video_streams(call, 5555);
    leaf_start_video_stream(call, "192.168.190.132", 5555);
}

void LeafStartConnect(
    char* broadcast_addr,
    int broadcast_port)
{
    leaf_init_audio_streams(call, broadcast_port);
    leaf_start_audio_stream(call, broadcast_addr, broadcast_port);
}

void LeafPlayfile(char* filename)
{
    leaf_start_sound_play(call,filename,Normalplay,NULL);
}

void LeafStopSound(void)
{
    leaf_stop_sound_play(call);
}

void LeafTerminate(void)
{
    //leaf_stop_sound_play(call);
    leaf_stop_media_streams(call);
    leafKeySoundPaused = false;
}

void LeafPauseKeySound(void)
{
    leafKeySoundPaused = true;
}

void LeafResumeKeySound(void)
{
    leafKeySoundPaused = false;
}
