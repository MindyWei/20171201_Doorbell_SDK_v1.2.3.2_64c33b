#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer*        mainMenuLayer;
static ITUBackground*   videoRecordRemoteBackground;
static ITUBackground*   videoRecordBlackBackground;
static ITUText*         videoRecordSourceText;
static ITUText*         videoRecordCountDownText;

// status
static bool videoRecordInVideoState;
static float videoRecordCountDown;
static uint32_t videoRecordLastTick;
static int videoRecordID;

bool VideoRecordHangUpButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*)widget;

    ituButtonSetPressed(btn, false);

    LinphoneTerminate(0);
    SceneHangUp();
	return true;
}


#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void videoRecordRemoteBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

void VideoRecordIncomingShow(int id, char* addr, int video)
{
    char* desc;
    DeviceInfo info;
    char buf[128];

    if (!videoRecordRemoteBackground)
    {
        videoRecordRemoteBackground = ituSceneFindWidget(&theScene, "videoRecordRemoteBackground");
        assert(videoRecordRemoteBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(videoRecordRemoteBackground, videoRecordRemoteBackgroundDraw);
#endif        

        videoRecordBlackBackground = ituSceneFindWidget(&theScene, "videoRecordBlackBackground");
        assert(videoRecordBlackBackground);

        videoRecordSourceText = ituSceneFindWidget(&theScene, "videoRecordSourceText");
        assert(videoRecordSourceText);

        videoRecordCountDownText = ituSceneFindWidget(&theScene, "videoRecordCountDownText");
        assert(videoRecordCountDownText);            
    }
    videoRecordCountDown = 0.0f;

    AddressBookGetDeviceInfo(&info, addr);
    if (video)
    {
        ituWidgetSetVisible(videoRecordRemoteBackground, true);
        ituWidgetSetVisible(videoRecordBlackBackground, false);
    }
    else
    {
        ituWidgetSetVisible(videoRecordRemoteBackground, false);
        ituWidgetSetVisible(videoRecordBlackBackground, true);
    }

    desc = StringGetRoomAddress(info.area, info.building, info.unit, info.floor, info.room, NULL);
    ituTextSetString(videoRecordSourceText, desc);

	videoRecordID = id;
	videoRecordInVideoState = video;

    videoRecordCountDown    = (float)theConfig.calling_time;
    videoRecordLastTick     = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)videoRecordCountDown);
    ituTextSetString(videoRecordCountDownText, buf);

    VideoMemoStartRecord(video ? MEDIA_VIDEO : MEDIA_AUDIO, addr);
}

bool VideoRecordOnTimer(ITUWidget* widget, char* param)
{
    if (videoRecordCountDown > 0.0f)
    {
        uint32_t diff, tick = SDL_GetTicks();

        if (tick >= videoRecordLastTick)
        {
            diff = tick - videoRecordLastTick;
        }
        else
        {
            diff = 0xFFFFFFFF - videoRecordLastTick + tick;
        }

        if (diff >= 1000)
        {
            char buf[4] = {0};

            videoRecordCountDown -= (float)diff / 1000.0f;
            videoRecordLastTick = tick;

            if ((0 <= ((int)videoRecordCountDown)) && (((int)videoRecordCountDown) < 1000))
            {
                sprintf(buf, "%d", (int)videoRecordCountDown);
                ituTextSetString(videoRecordCountDownText, buf);
            }
        }
        if (videoRecordCountDown <= 0.0f)
        {
            LinphoneTerminate(0);
            SceneHangUp();
		    ituLayerGoto(mainMenuLayer);
        }
    }
	return false;
}

bool VideoRecordOnEnter(ITUWidget* widget, char* param)
{
    if (!mainMenuLayer)
    {
        mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);

        if (!videoRecordRemoteBackground)
        {
            videoRecordRemoteBackground = ituSceneFindWidget(&theScene, "videoRecordRemoteBackground");
            assert(videoRecordRemoteBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
            ituWidgetSetDraw(videoRecordRemoteBackground, videoRecordRemoteBackgroundDraw);
#endif       

            videoRecordBlackBackground = ituSceneFindWidget(&theScene, "videoRecordBlackBackground");
            assert(videoRecordBlackBackground);

            videoRecordSourceText = ituSceneFindWidget(&theScene, "videoRecordSourceText");
            assert(videoRecordSourceText);
        
            videoRecordCountDownText = ituSceneFindWidget(&theScene, "videoRecordCountDownText");
            assert(videoRecordCountDownText);        
        }
    }
    // video
    if (videoRecordInVideoState)
    {
        SceneEnterVideoState(33);
        LinphoneSetWindowID(videoRecordRemoteBackground);
    }
    return true;
}

bool VideoRecordOnLeave(ITUWidget* widget, char* param)
{
    VideoMemoStopRecord();
    videoRecordInVideoState = false;
    videoRecordID = 0;
    SceneLeaveVideoState();
	return true;
}

void VideoRecordReset(void)
{
    mainMenuLayer             = NULL;
    videoRecordRemoteBackground    = NULL;
}
