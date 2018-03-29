#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

#define HANGUP_DELAY (500 / 33)  // 0.5 secs

static ITULayer* userLayer;

static ITUBackground* calledConnectingBackground;
static ITUBackground* calledConnectedBackground;
static ITUBackground* calledHangUpBackground;
static ITUText* calledCountDownText;

static float calledCountDown;
static uint32_t calledLastTick;
static DeviceInfo calledDeviceInfo;

bool CalledConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];
    ituWidgetSetVisible(calledConnectingBackground, false);
    ituWidgetSetVisible(calledConnectedBackground, true);

    calledCountDown    = ((theConfig.calling_time > 0) ? (float)theConfig.calling_time + 0.5f : (float)theConfig.calling_time);
    calledLastTick     = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)calledCountDown);
    ituTextSetString(calledCountDownText, buf);
	return true;
}

static void CalledHangUpCommand(int arg)
{
	ituLayerGoto(userLayer);
}

bool CalledEndOnCustom(ITUWidget* widget, char* param)
{
	LinphoneTerminate();
    ituWidgetSetVisible(calledConnectingBackground, false);
    ituWidgetSetVisible(calledConnectedBackground, false);
    ituWidgetSetVisible(calledHangUpBackground, true);
    ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CalledHangUpCommand, 0);
	return true;
}

bool CalledErrorOnCustom(ITUWidget* widget, char* param)
{
    ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CalledHangUpCommand, 0);
	return true;
}

bool CalledKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (calledCountDown <= 0.0f)
    {
        LinphoneAnswer();
    }
	return true;
}

bool CalledKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituWidgetSetVisible(calledConnectingBackground, false);
    ituWidgetSetVisible(calledConnectedBackground, false);
    ituWidgetSetVisible(calledHangUpBackground, true);
    LinphoneTerminate();
    SceneHangUp();
    ituSceneExecuteCommand(&theScene, 6, CalledHangUpCommand, 0);
	return true;
}

bool CalledOnTimer(ITUWidget* widget, char* param)
{
    if (calledCountDown > 0.0f)
    {
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - calledLastTick;
        if (diff >= 1000)
        {
            char buf[4];

            calledCountDown -= (float)diff / 1000.0f;
            calledLastTick = tick;

            if (calledCountDown >= 0.0f)
            {
                sprintf(buf, "%d", (int)calledCountDown);
                ituTextSetString(calledCountDownText, buf);
            }
        }
        if (calledCountDown <= 0.0f)
        {
            ituWidgetSetVisible(calledConnectingBackground, false);
            ituWidgetSetVisible(calledConnectedBackground, false);
            ituWidgetSetVisible(calledHangUpBackground, true);
            LinphoneTerminate();
            SceneHangUp();
            ituSceneExecuteCommand(&theScene, 6, CalledHangUpCommand, 0);
        }
    }
	return true;
}


bool CalledOnEnter(ITUWidget* widget, char* param)
{
    if (!calledConnectingBackground)
    {
        calledConnectingBackground = ituSceneFindWidget(&theScene, "calledConnectingBackground");
        assert(calledConnectingBackground);

        calledConnectedBackground = ituSceneFindWidget(&theScene, "calledConnectedBackground");
        assert(calledConnectedBackground);

        calledHangUpBackground = ituSceneFindWidget(&theScene, "calledHangUpBackground");
        assert(calledHangUpBackground);

        calledCountDownText = ituSceneFindWidget(&theScene, "calledCountDownText");
        assert(calledCountDownText);

        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);
    }
    ituWidgetSetVisible(calledConnectingBackground, true);
    ituWidgetSetVisible(calledConnectedBackground, false);
    ituWidgetSetVisible(calledHangUpBackground, false);
    ituTextSetString(calledCountDownText, NULL);
    calledCountDown = 0;
	return true;
}

bool CalledOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void CalledReset(void)
{
    calledConnectingBackground = NULL;
}
