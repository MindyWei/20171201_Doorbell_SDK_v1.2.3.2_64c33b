#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* mainMenuLayer;
static ITUText* videoMemoCountDownText;

static float videoMemoCountDown;
static uint32_t videoMemoLastTick;

void VideoMemoCall(DeviceInfo* info, bool videoFromIPCam)
{
    LinphoneRecordVideoMessage(info, videoFromIPCam);
}

bool VideoMemoConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];

    videoMemoCountDown  = (float)theConfig.videomemo_time + 0.999f;
    videoMemoLastTick   = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)videoMemoCountDown);
    ituTextSetString(videoMemoCountDownText, buf);
	return true;
}

bool VideoMemoErrorOnCustom(ITUWidget* widget, char* param)
{
	ituLayerGoto(mainMenuLayer);
	return true;
}

bool VideoMemoHangUpButtonOnPress(ITUWidget* widget, char* param)
{
    LinphoneTerminate(0);
    SceneHangUp();
	ituLayerGoto(mainMenuLayer);
	return true;
}

bool VideoMemoOnTimer(ITUWidget* widget, char* param)
{
    if (videoMemoCountDown > 0.0f)
    {
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - videoMemoLastTick;
        if (diff >= 1000)
        {
            char buf[4];

            videoMemoCountDown -= (float)diff / 1000.0f;
            videoMemoLastTick = tick;

            sprintf(buf, "%d", (int)videoMemoCountDown);
            ituTextSetString(videoMemoCountDownText, buf);
        }
        if (videoMemoCountDown <= 0.0f)
        {
            LinphoneTerminate(0);
            SceneHangUp();
	        ituLayerGoto(mainMenuLayer);
        }
    }
    return true;
}

bool VideoMemoOnEnter(ITUWidget* widget, char* param)
{
    if (!videoMemoCountDownText)
    {
        videoMemoCountDownText = ituSceneFindWidget(&theScene, "videoMemoCountDownText");
        assert(videoMemoCountDownText);

        mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);
    }
    ituTextSetString(videoMemoCountDownText, NULL);
    videoMemoCountDown = 0.0f;
	return true;
}

void VideoMemoReset(void)
{
    videoMemoCountDownText = NULL;
}
