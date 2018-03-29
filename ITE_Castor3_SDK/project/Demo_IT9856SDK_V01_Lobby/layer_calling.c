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
static ITULayer* voiceMemoLayer;
static ITUBackground* callingConnectingBackground;
static ITUBackground* callingConnectedBackground;
static ITUBackground* callingHangUpBackground;
static ITUText* callingCountDownText;
static ITUBackground* callingVoiceMemoBackground;
static ITUText* callingVoiceMemoCountDownText;

static float dialCountDown, callingCountDown, callingVoiceMemoCountDown;
static uint32_t dialLastTick, callingLastTick, callingVoiceMemoLastTick;
static DeviceInfo callingDeviceInfo;
static DeviceInfo callingManagerInfoArray[MAX_MANAGER_COUNT];
static int callingManagerIndex;
static int callingManagerCount;
static bool callingPlayRedialSound;

int CallingCall(DeviceInfo* info)
{
    callingManagerIndex = -1;

    if (info)
    {
        DeviceInfo infos[LINPHONE_CASTOR3_MAX_IP_COUNT];
        int len;

        memcpy(&callingDeviceInfo, info, sizeof (DeviceInfo));

        len = AddressBookGetDeviceInfoArray(infos, LINPHONE_CASTOR3_MAX_IP_COUNT, callingDeviceInfo.area, callingDeviceInfo.building, callingDeviceInfo.unit, callingDeviceInfo.floor, callingDeviceInfo.room);
        if (len > 0)
        {
            memcpy(&callingDeviceInfo, &infos[0], sizeof (DeviceInfo));

            LinphoneCallDevices(infos, len, true, true);
            SceneSetRemoteIP(infos[0].ip);
            dialCountDown = (float)theConfig.dial_time;
            dialLastTick = SDL_GetTicks();

            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        callingManagerCount = AddressBookGetManagerInfoArray(callingManagerInfoArray, MAX_MANAGER_COUNT);
        if (callingManagerCount > 0)
        {
            callingManagerIndex = 0;
            memcpy(&callingDeviceInfo, &callingManagerInfoArray[0], sizeof (DeviceInfo));
            LinphoneCall(callingDeviceInfo.ip, true);
            SceneSetRemoteIP(callingDeviceInfo.ip);
            dialCountDown = (float)theConfig.dial_time;
            dialLastTick = SDL_GetTicks();
        }
        else if (theCenterInfo.ip[0] != '\0')
        {
            // call to center
            memcpy(&callingDeviceInfo, &theCenterInfo, sizeof (DeviceInfo));

            LinphoneCall(callingDeviceInfo.ip, true);
            SceneSetRemoteIP(callingDeviceInfo.ip);
            dialCountDown = (float)theConfig.dial_time;
            dialLastTick = SDL_GetTicks();
        }
        return 0;
    }
}

bool CallingConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];

    ituWidgetSetVisible(callingConnectingBackground, false);
    ituWidgetSetVisible(callingConnectedBackground, true);

    dialCountDown       = 0.0f;
    callingCountDown    = (float)theConfig.calling_time;
    callingLastTick     = SDL_GetTicks();
    callingManagerIndex = -1;

    sprintf(buf, "%d", (int)callingCountDown);
    ituTextSetString(callingCountDownText, buf);
	return true;
}

static void CallingHangUpCommand(int arg)
{
    if (callingManagerIndex >= 0)
    {
        ituWidgetSetVisible(callingConnectingBackground, true);
        ituWidgetSetVisible(callingConnectedBackground, false);
        ituWidgetSetVisible(callingHangUpBackground, false);

        if (++callingManagerIndex >= callingManagerCount)
        {
            if (theCenterInfo.ip[0] == '\0')
            {
                ituLayerGoto(userLayer);
		        if (callingPlayRedialSound)
		        {
			        LinphonePlayHintSound(HINT_SOUND_REDIAL);
			        callingPlayRedialSound = false;
		        }
            }
            else
            {
                // call to center
                memcpy(&callingDeviceInfo, &theCenterInfo, sizeof (DeviceInfo));
                callingManagerIndex = -1;
                LinphoneCall(callingDeviceInfo.ip, true);
		        SceneSetRemoteIP(callingDeviceInfo.ip);
                dialCountDown = (float)theConfig.dial_time;
                dialLastTick = SDL_GetTicks();
            }
        }
        else
        {
            memcpy(&callingDeviceInfo, &callingManagerInfoArray[callingManagerIndex], sizeof (DeviceInfo));
            LinphoneCall(callingDeviceInfo.ip, true);
		    SceneSetRemoteIP(callingDeviceInfo.ip);
            dialCountDown = (float)theConfig.dial_time;
            dialLastTick = SDL_GetTicks();
        }
    }
    else
    {
        ituLayerGoto(userLayer);
		if (callingPlayRedialSound)
		{
			LinphonePlayHintSound(HINT_SOUND_REDIAL);
			callingPlayRedialSound = false;
		}
    }
}

bool CallingKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (callingVoiceMemoCountDown > 0.0f)
    {
        VoiceMemoCall(&callingDeviceInfo);
		ituLayerGoto(voiceMemoLayer);
    }
	return true;
}

bool CallingKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituWidgetSetVisible(callingConnectingBackground, false);
    ituWidgetSetVisible(callingConnectedBackground, false);
    ituWidgetSetVisible(callingVoiceMemoBackground, false);
    ituWidgetSetVisible(callingHangUpBackground, true);
    LinphoneTerminate();
    SceneHangUp();
    ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);
	return true;
}

bool CallingOnTimer(ITUWidget* widget, char* param)
{
	if (dialCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - dialLastTick;
        if (diff >= 1000)
        {
            dialCountDown -= (float)diff / 1000.0f;
            dialLastTick = tick;
        }
        if (dialCountDown <= 0.0f)
        {
            ituWidgetSetVisible(callingConnectingBackground, false);
            ituWidgetSetVisible(callingConnectedBackground, false);
            ituWidgetSetVisible(callingHangUpBackground, true);
            LinphoneTerminate();
            SceneHangUp();
			callingPlayRedialSound = true;
            ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);
        }
	}
    else if (callingCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - callingLastTick;
        if (diff >= 1000)
        {
            char buf[4];

            callingCountDown -= (float)diff / 1000.0f;
            callingLastTick = tick;

            if (callingCountDown >= 0.0f)
            {
                sprintf(buf, "%d", (int)callingCountDown);
                ituTextSetString(callingCountDownText, buf);
            }
        }
        if (callingCountDown <= 0.0f)
        {
            ituWidgetSetVisible(callingConnectingBackground, false);
            ituWidgetSetVisible(callingConnectedBackground, false);
            ituWidgetSetVisible(callingHangUpBackground, true);
            LinphoneTerminate();
            SceneHangUp();
            ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);
        }
	}
	else if (callingVoiceMemoCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - callingVoiceMemoLastTick;
        if (diff >= 1000)
        {
            char buf[4];

            callingVoiceMemoCountDown -= (float)diff / 1000.0f;
            callingVoiceMemoLastTick = tick;

            sprintf(buf, "%d", (int)callingVoiceMemoCountDown);
            ituTextSetString(callingVoiceMemoCountDownText, buf);
        }
        if (callingVoiceMemoCountDown <= 0.0f)
        {
            ituWidgetSetVisible(callingVoiceMemoBackground, false);
            ituWidgetSetVisible(callingHangUpBackground, true);
            ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);
        }
	}
	return true;
}

bool CallingEndOnCustom(ITUWidget* widget, char* param)
{
	LinphoneTerminate();
    ituWidgetSetVisible(callingConnectingBackground, false);
    ituWidgetSetVisible(callingConnectedBackground, false);
    ituWidgetSetVisible(callingHangUpBackground, true);
    ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);
	return true;
}

bool CallingErrorOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];
    int errorCode = atoi(param);

    if (callingManagerIndex >= 0)
    {
        if (++callingManagerIndex >= callingManagerCount)
        {
            if (theCenterInfo.ip[0] == '\0')
            {
                if (errorCode == 600)
                {
                    ituWidgetSetVisible(callingConnectingBackground, false);
                    ituWidgetSetVisible(callingConnectedBackground, false);
                    ituWidgetSetVisible(callingHangUpBackground, false);
                    ituWidgetSetVisible(callingVoiceMemoBackground, true);

                    dialCountDown               = 0.0f;
                    callingVoiceMemoCountDown   = 10.9999f;
                    callingVoiceMemoLastTick    = SDL_GetTicks();
                    
                    sprintf(buf, "%d", (int)callingVoiceMemoCountDown);
                    ituTextSetString(callingVoiceMemoCountDownText, buf);

                    LinphonePlayHintSound(HINT_SOUND_VOICEMEMO);
                }
                else
                {
                    ituWidgetSetVisible(callingConnectingBackground, false);
                    ituWidgetSetVisible(callingConnectedBackground, false);
                    ituWidgetSetVisible(callingHangUpBackground, true);
                    ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);

                    LinphonePlayHintSound(HINT_SOUND_REDIAL);
                }
            }
            else
            {
                // call to center
                memcpy(&callingDeviceInfo, &theCenterInfo, sizeof (DeviceInfo));
                callingManagerIndex = -1;
            }
        }
        else
        {
            memcpy(&callingDeviceInfo, &callingManagerInfoArray[callingManagerIndex], sizeof (DeviceInfo));
        }
        LinphoneCall(callingDeviceInfo.ip, true);
		SceneSetRemoteIP(callingDeviceInfo.ip);
        dialCountDown = (float)theConfig.dial_time;
        dialLastTick = SDL_GetTicks();
    }
    else
    {
        if (errorCode == 600)
        {
            ituWidgetSetVisible(callingConnectingBackground, false);
            ituWidgetSetVisible(callingConnectedBackground, false);
            ituWidgetSetVisible(callingHangUpBackground, false);
            ituWidgetSetVisible(callingVoiceMemoBackground, true);

            dialCountDown               = 0.0f;
            callingVoiceMemoCountDown   = 10.9999f;
            callingVoiceMemoLastTick    = SDL_GetTicks();
            
            sprintf(buf, "%d", (int)callingVoiceMemoCountDown);
            ituTextSetString(callingVoiceMemoCountDownText, buf);

            LinphonePlayHintSound(HINT_SOUND_VOICEMEMO);
        }
        else
        {
            ituWidgetSetVisible(callingConnectingBackground, false);
            ituWidgetSetVisible(callingConnectedBackground, false);
            ituWidgetSetVisible(callingHangUpBackground, true);
            ituSceneExecuteCommand(&theScene, HANGUP_DELAY, CallingHangUpCommand, 0);

            LinphonePlayHintSound(HINT_SOUND_REDIAL);
        }
    }
	return true;
}

bool CallingOnEnter(ITUWidget* widget, char* param)
{
    if (!userLayer)
    {
        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);

        callingConnectingBackground = ituSceneFindWidget(&theScene, "callingConnectingBackground");
        assert(callingConnectingBackground);

        callingConnectedBackground = ituSceneFindWidget(&theScene, "callingConnectedBackground");
        assert(callingConnectedBackground);
    
        callingHangUpBackground = ituSceneFindWidget(&theScene, "callingHangUpBackground");
        assert(callingHangUpBackground);

        callingCountDownText = ituSceneFindWidget(&theScene, "callingCountDownText");
        assert(callingCountDownText);

        voiceMemoLayer = ituSceneFindWidget(&theScene, "voiceMemoLayer");
        assert(voiceMemoLayer);

        callingVoiceMemoBackground = ituSceneFindWidget(&theScene, "callingVoiceMemoBackground");
        assert(callingVoiceMemoBackground);

        callingVoiceMemoCountDownText = ituSceneFindWidget(&theScene, "callingVoiceMemoCountDownText");
        assert(callingVoiceMemoCountDownText);
    }
    ituWidgetSetVisible(callingConnectingBackground, true);
    ituWidgetSetVisible(callingConnectedBackground, false);
    ituWidgetSetVisible(callingHangUpBackground, false);
    ituTextSetString(callingCountDownText, NULL);
    ituWidgetSetVisible(callingVoiceMemoBackground, false);
    ituTextSetString(callingVoiceMemoCountDownText, NULL);
    callingCountDown = callingVoiceMemoCountDown = 0.0f;
	callingPlayRedialSound = false;
	return true;
}

bool CallingOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void CallingReset(void)
{
    userLayer = NULL;
}
