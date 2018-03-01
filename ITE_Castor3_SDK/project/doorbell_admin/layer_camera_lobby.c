#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

#define CAMERA_TIMEOUT      (30.999f)  // 30 secs

static ITUBackground*   cameraLobbyBackground;
static ITUText*         cameraLobbySourceText;
static ITUText*         cameraLobbyCountDownText;

static float cameraLobbyCountDown, LockRecordButtonCountDown;
static uint32_t cameraLobbyLastTick, LockRecordButtonLastTick;
static int snapshotCountDown;
static bool cameraLobbyVideoRecordEnable;
static bool LockRecordButton;

#define SNAPSHOT_COUNT (1 * 1000 / 33) // 1 sec

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void cameraLobbyBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool CameraLobbyButtonOnPress(ITUWidget* widget, char* param)
{
    if (!cameraLobbyBackground)
    {
        cameraLobbyBackground = ituSceneFindWidget(&theScene, "cameraLobbyBackground");
        assert(cameraLobbyBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraLobbyBackground, cameraLobbyBackgroundDraw);
#endif	
    }
    LinphoneSetWindowID(cameraLobbyBackground);
    return true;
}

bool CameraLobbyRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraLobbyVideoRecordEnable)
    {
		if(LockRecordButton) return false;
		if(SceneCameraIsViewing() && cameraLobbyCountDown < 2.0f) return false;
        if (CaptureStartRecord(MEDIA_VIDEO, theLobbyInfo.ip))
            return false;
        
        LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraLobbyStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraLobbyVideoRecordEnable)
	{
		if(LockRecordButton) return false;
        CaptureStopRecord();
		
		LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
	}
	return true;
}

bool CameraLobbyUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    SceneOpenDoor(theLobbyInfo.ip);
    return true;
}

bool CameraLobbySnapshotButtonOnPress(ITUWidget* widget, char* param)
{
    if (snapshotCountDown)
        return false;

    CaptureSnapshot(theLobbyInfo.ip, 0);
    snapshotCountDown = SNAPSHOT_COUNT;
	return true;
}

bool CameraLobbyOnTimer(ITUWidget* widget, char* param)
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
    if (cameraLobbyCountDown > 0.0f)
    {
        uint32_t diff, tick = SDL_GetTicks();

        if (tick >= cameraLobbyLastTick)
        {
            diff = tick - cameraLobbyLastTick;
        }
        else
        {
            diff = 0xFFFFFFFF - cameraLobbyLastTick + tick;
        }

        if (diff >= 1000)
        {
            char buf[4];

            cameraLobbyCountDown -= (float)diff / 1000.0f;
            cameraLobbyLastTick = tick;

            if (cameraLobbyCountDown >= 0.0f)
            {
                sprintf(buf, "%d", (int)cameraLobbyCountDown);
                ituTextSetString(cameraLobbyCountDownText, buf);
            }
        }
        if (cameraLobbyCountDown <= 0.0f)
        {
            if (cameraLobbyVideoRecordEnable)            
                CaptureStopRecord();

            LinphoneStopCamera();

            // UI receive this event to invisible cameraLobbyLayer
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
        }
    }
    if (snapshotCountDown > 0)
    {
        snapshotCountDown--;
    }    
	return true;
}

bool CameraLobbyOnEnter(ITUWidget* widget, char* param)
{
    char* desc;
    char buf[4];

    if (!cameraLobbyBackground)
    {
        cameraLobbyBackground = ituSceneFindWidget(&theScene, "cameraLobbyBackground");
        assert(cameraLobbyBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraLobbyBackground, cameraLobbyBackgroundDraw);
#endif

        cameraLobbySourceText = ituSceneFindWidget(&theScene, "cameraLobbySourceText");
        assert(cameraLobbySourceText);

        cameraLobbyCountDownText = ituSceneFindWidget(&theScene, "cameraLobbyCountDownText");
        assert(cameraLobbyCountDownText);
    }
    desc = StringGetCameraPosition(theLobbyInfo.area, theLobbyInfo.building, theLobbyInfo.unit);
    ituTextSetString(cameraLobbySourceText, desc);
    free(desc);

#if (CFG_CHIP_FAMILY == 9850)        
        SceneEnterVideoState(33);
#endif

    LinphoneWatchCamera(theLobbyInfo.ip);
    SceneSetRemoteIP(theLobbyInfo.ip);
    SceneCameraSetViewing(true);

    cameraLobbyCountDown    = CAMERA_TIMEOUT;
    cameraLobbyLastTick     = SDL_GetTicks();
    
    // count down
    sprintf(buf, "%d", (int)cameraLobbyCountDown);
    ituTextSetString(cameraLobbyCountDownText, buf);

    LinphoneSetWindowID(cameraLobbyBackground);

    // snapshot
    snapshotCountDown = 0;

    cameraLobbyVideoRecordEnable = false;

    if (CaptureGetPath())
        cameraLobbyVideoRecordEnable = true;

    if (cameraLobbyVideoRecordEnable)
        return true; // continue to show record button
    else
        return false;
}

bool CameraLobbyOnLeave(ITUWidget* widget, char* param)
{
    if (cameraLobbyVideoRecordEnable)
        CaptureStopRecord();

    LinphoneStopCamera();
    cameraLobbyCountDown = 0.0f;
    SceneCameraSetViewing(false);
    SceneHangUp();
#if (CFG_CHIP_FAMILY == 9850)
    if(strcmp(param, "calledLayer"))
        SceneLeaveVideoState();
#endif
	return true;
}

void CameraLobbyReset(void)
{
    cameraLobbySourceText    = NULL;
	cameraLobbyBackground    = NULL;
}
