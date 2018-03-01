#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* userLayer;
static ITUText* voiceMemoCountDownText;

static float voiceMemoCountDown;
static uint32_t voiceMemoLastTick;

void VoiceMemoCall(DeviceInfo* info)
{
    LinphoneRecordVideoMessage(info);
}

bool VoiceMemoConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];

    voiceMemoCountDown  = (float)theConfig.voicememo_time + 0.999f;
    voiceMemoLastTick   = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)voiceMemoCountDown);
    ituTextSetString(voiceMemoCountDownText, buf);
	return true;
}

bool VoiceMemoErrorOnCustom(ITUWidget* widget, char* param)
{
	ituLayerGoto(userLayer);
	return true;
}

bool VoiceMemoKeyStarOnPress(ITUWidget* widget, char* param)
{
    LinphoneTerminate();
    SceneHangUp();
	ituLayerGoto(userLayer);
	return true;
}

bool VoiceMemoKeySharpOnPress(ITUWidget* widget, char* param)
{
    LinphoneTerminate();
    SceneHangUp();
	ituLayerGoto(userLayer);
	return true;
}

bool VoiceMemoKeyNumberOnPress(ITUWidget* widget, char* param)
{
    LinphoneTerminate();
    SceneHangUp();
	ituLayerGoto(userLayer);
	return true;
}

bool VoiceMemoOnTimer(ITUWidget* widget, char* param)
{
    if (voiceMemoCountDown > 0.0f)
    {
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - voiceMemoLastTick;
        if (diff >= 1000)
        {
            char buf[4];

            voiceMemoCountDown -= (float)diff / 1000.0f;
            voiceMemoLastTick = tick;

            sprintf(buf, "%d", (int)voiceMemoCountDown);
            ituTextSetString(voiceMemoCountDownText, buf);
        }
        if (voiceMemoCountDown <= 0.0f)
        {
            LinphoneTerminate();
            SceneHangUp();
	        ituLayerGoto(userLayer);
        }
    }
    return true;
}

bool VoiceMemoOnEnter(ITUWidget* widget, char* param)
{
    if (!voiceMemoCountDownText)
    {
        voiceMemoCountDownText = ituSceneFindWidget(&theScene, "voiceMemoCountDownText");
        assert(voiceMemoCountDownText);

        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);
    }
    ituTextSetString(voiceMemoCountDownText, NULL);
    voiceMemoCountDown = 0.0f;
	return true;
}

void VoiceMemoReset(void)
{
    voiceMemoCountDownText = NULL;
}
