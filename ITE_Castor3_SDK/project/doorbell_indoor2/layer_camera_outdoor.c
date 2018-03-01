#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

#define CAMERA_TIMEOUT      (30.999f)  // 30 secs

static ITUBackground*   cameraOutdoorBackground;
static ITUText*         cameraOutdoorSourceText;
static ITUText*         cameraOutdoorCountDownText;

static float cameraOutdoorCountDown, LockRecordButtonCountDown;
static uint32_t cameraOutdoorLastTick, LockRecordButtonLastTick;
static int snapshotCountDown;
static bool cameraOutdoorVideoRecordEnable;
static bool LockRecordButton;

#define SNAPSHOT_COUNT (1 * 1000 / 33) // 1 sec

bool CameraOutdoorButtonOnPress(ITUWidget* widget, char* param)
{
    if (!cameraOutdoorBackground)
    {
        cameraOutdoorBackground = ituSceneFindWidget(&theScene, "cameraOutdoorBackground");
        assert(cameraOutdoorBackground);
	}
	LinphoneSetWindowID(cameraOutdoorBackground);
	return true;
}

bool CameraOutdoorRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraOutdoorVideoRecordEnable)
    {
        if(LockRecordButton) return false;
        if(SceneCameraIsViewing() && cameraOutdoorCountDown < 2.0f) return false;
        if (CaptureStartRecord(MEDIA_VIDEO, theEntranceInfo.ip))
            return false;

        LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraOutdoorStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraOutdoorVideoRecordEnable)
    {
        if(LockRecordButton) return false;
        CaptureStopRecord();

        LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraOutdoorUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    SceneOpenDoor(theEntranceInfo.ip);
    return true;
}

bool CameraOutdoorSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
    if (snapshotCountDown)
        return false;

    CaptureSnapshot(theEntranceInfo.ip, 0);
    snapshotCountDown = SNAPSHOT_COUNT;
	return true;
}

bool CameraOutdoorOnTimer(ITUWidget* widget, char* param)
{
    if (LockRecordButtonCountDown > 0.0f && LockRecordButton)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - LockRecordButtonLastTick;
        if (diff >= 1000)
        {
            LockRecordButtonCountDown -= (float)diff / 1000.0f;
            LockRecordButtonLastTick = tick;
        }
        if (LockRecordButtonCountDown <= 0.0f)
        {
            LockRecordButton = false;
        }
	}
    if (cameraOutdoorCountDown > 0.0f)
    {
        uint32_t diff, tick = SDL_GetTicks();

        if (tick >= cameraOutdoorLastTick)
        {
            diff = tick - cameraOutdoorLastTick;
        }
        else
        {
            diff = 0xFFFFFFFF - cameraOutdoorLastTick + tick;
        }

        if (diff >= 1000)
        {
            char buf[4];

            cameraOutdoorCountDown -= (float)diff / 1000.0f;
            cameraOutdoorLastTick = tick;

            if (cameraOutdoorCountDown >= 0.0f)
            {
                sprintf(buf, "%d", (int)cameraOutdoorCountDown);
                ituTextSetString(cameraOutdoorCountDownText, buf);
            }
        }
        if (cameraOutdoorCountDown <= 0.0f)
        {
            if (cameraOutdoorVideoRecordEnable)            
                CaptureStopRecord();

            LinphoneStopCamera();

            // UI receive this event to invisible cameraOutdoorLayer
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
        }
    }
    if (snapshotCountDown > 0)
    {
        snapshotCountDown--;
    }    
	return false;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void cameraOutdoorBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool CameraOutdoorOnEnter(ITUWidget* widget, char* param)
{
    char buf[4];

    if (!cameraOutdoorBackground)
    {
        cameraOutdoorBackground = ituSceneFindWidget(&theScene, "cameraOutdoorBackground");
        assert(cameraOutdoorBackground);
        
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraOutdoorBackground, cameraOutdoorBackgroundDraw);
#endif

        cameraOutdoorSourceText = ituSceneFindWidget(&theScene, "cameraOutdoorSourceText");
        assert(cameraOutdoorSourceText);

        cameraOutdoorCountDownText = ituSceneFindWidget(&theScene, "cameraOutdoorCountDownText");
        assert(cameraOutdoorCountDownText);
    }
    if (theEntranceInfo.alias[0] == '\0')
    {
        char* desc = StringGetCameraPosition(theEntranceInfo.area, theEntranceInfo.building, theEntranceInfo.unit);
        ituTextSetString(cameraOutdoorSourceText, desc);
        free(desc);
    }
    else
    {
        ituTextSetString(cameraOutdoorSourceText, theEntranceInfo.alias);
    }

    SceneEnterVideoState(33);
    LinphoneWatchCamera(theEntranceInfo.ip);
    SceneSetRemoteIP(theEntranceInfo.ip);
    SceneCameraSetViewing(true);

    cameraOutdoorCountDown    = CAMERA_TIMEOUT;
    cameraOutdoorLastTick     = SDL_GetTicks();
    
    // count down
    sprintf(buf, "%d", (int)cameraOutdoorCountDown);
    ituTextSetString(cameraOutdoorCountDownText, buf);

    LinphoneSetWindowID(cameraOutdoorBackground);

    // snapshot
    snapshotCountDown = 0;

    cameraOutdoorVideoRecordEnable = false;

    if (theDeviceInfo.type == DEVICE_INDOOR2 && CaptureGetPath())
        cameraOutdoorVideoRecordEnable = true;

    if (cameraOutdoorVideoRecordEnable)
        return true; // continue to show record button
    else
        return false;
}

bool CameraOutdoorOnLeave(ITUWidget* widget, char* param)
{
    if (cameraOutdoorVideoRecordEnable)
        CaptureStopRecord();

    LinphoneStopCamera();
    cameraOutdoorCountDown = 0.0f;
    SceneCameraSetViewing(false);
    SceneContinuePlayGuardAlarm();

    if(strcmp(param, "calledLayer") && strcmp(param, "videoRecordLayer"))
        SceneLeaveVideoState();
	return true;
}

void CameraOutdoorReset(void)
{
    cameraOutdoorSourceText    = NULL;
	cameraOutdoorBackground    = NULL;
}
