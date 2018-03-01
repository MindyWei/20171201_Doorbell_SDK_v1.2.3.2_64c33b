#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "config.h"
#include "leaf/leaf_mediastream.h"

#define HANGUP_DELAY (500 / 33)  // 0.5 secs

// scene
extern ITUScene theScene;
// call
extern LeafCall *call;

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
static int callingManagerIndex;
static int callingManagerCount;
static bool callingPlayRedialSound;

bool CallingConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];

    ituWidgetSetVisible(callingConnectingBackground, false);
    ituWidgetSetVisible(callingConnectedBackground, true);

    dialCountDown       = 0.0f;
    callingCountDown    = 30;
    callingLastTick     = SDL_GetTicks();
    callingManagerIndex = -1;

    sprintf(buf, "%d", (int)callingCountDown);
    ituTextSetString(callingCountDownText, buf);
	return true;
}

bool CallingEndOnCustom(ITUWidget* widget, char* param)
{
    ituWidgetSetVisible(callingConnectingBackground, false);
    ituWidgetSetVisible(callingConnectedBackground, false);
    ituWidgetSetVisible(callingHangUpBackground, true);
	NetworkCallSend(CALL_STOP);
	if (NetworkCheckAck())
	{
		leaf_stop_media_streams(call);
		NetworkCallIdle();
    	ituLayerGoto(userLayer);
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

	call->dir = CallOutgoing;
	leaf_init_video_streams(call, 5555);
	leaf_start_video_stream(call, "192.168.190.66", 5555);
	leaf_init_audio_streams(call, 6666);
	leaf_start_audio_stream(call, "192.168.190.66", 6666);
	return true;
}

bool CallingOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

