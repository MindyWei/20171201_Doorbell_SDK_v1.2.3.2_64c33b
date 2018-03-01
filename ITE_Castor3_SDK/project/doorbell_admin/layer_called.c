#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer*        mainMenuLayer;
static ITUBackground*   calledRemoteBackground;
static ITUSprite*       calledRemoteSprite;
static ITUText*         calledSourceText;
static ITUBackground*   calledVolBackground;	
static ITUCheckBox*     calledVol0CheckBox;
static ITUCheckBox*     calledVol1CheckBox;
static ITUCheckBox*     calledVol2CheckBox;
static ITUCheckBox*     calledVol3CheckBox;
static ITUCheckBox*     calledVol4CheckBox;
static ITUCheckBox*     calledVol5CheckBox;
static ITUCheckBox*     calledVol6CheckBox;
static ITUText*         calledCountDownText;
static ITUBackground*   calledWaitingBackground;
static ITUText*         calledWaitingTypeText;
static ITUText*         calledWaitingRoomText;
static ITUButton*       calledUnlockButton;
static ITUButton*       calledSnapshotButton;
static ITUButton*       calledRecordButton;
static ITUButton*       calledStopRecordButton;
static ITUButton*       calledAnswerButton;
static ITUButton*       calledButtonContainer;

// status
static int calledMaxVol;
static bool calledInVideoState;
static bool calledConnected;
static float calledCountDown, spotCountDown, LockRecordButtonCountDown;
static uint32_t calledLastTick, spotLastTick, LockRecordButtonLastTick;
static int calledID, calledPendingID;
static bool calledPendingVideo;
static char calledPendingAddr[32];
static DeviceInfo calledDeviceInfo;
static int calledVoiceLevelOld, calledRingLevelOld;
static int snapshotCountDown;
static bool calledVideoRecordEnable;
static bool LockRecordButton;

extern SPOTTED_STATUS callSpottedStatus;

#define SNAPSHOT_COUNT (1 * 1000 / 33) // 1 sec

bool CalledVolCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(calledVolBackground))
    {
        ituWidgetSetVisible(calledVolBackground, false);
    }
    else
    {
        ituWidgetSetVisible(calledVolBackground, true);
    }
	return true;
}

bool CalledVolNumberCheckBoxOnPress(ITUWidget* widget, char* param)
{
    ITUCheckBox* checkbox = (ITUCheckBox*) widget;
    int level, vol;

    vol = atoi(ituTextGetString(&checkbox->btn.text));
    level = vol * 100 / calledMaxVol;

    if (level > 100)
        level = 100;
    else if (level < 0)
        level = 0;

    if (calledConnected)
    {
        LinphoneSetVoiceLevel(level);
    }
    else
    {
        LinphoneSetRingLevel(level, true);
    }

    ituCheckBoxSetChecked(calledVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(calledVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(calledVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(calledVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(calledVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(calledVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(calledVol6CheckBox, vol >= 6);

	return true;
}

bool  CalledRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if(LockRecordButton) return false;
    if(calledConnected && calledCountDown < 2.0f) return false;
    if (CaptureStartRecord(calledInVideoState ? MEDIA_VIDEO : MEDIA_AUDIO, calledDeviceInfo.ip))
        return false;

    LockRecordButton = true;
    LockRecordButtonCountDown = 1.0f;
    LockRecordButtonLastTick = SDL_GetTicks();

    return true;
}

bool CalledStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if(LockRecordButton) return false;
    CaptureStopRecord();

    LockRecordButton = true;
    LockRecordButtonCountDown = 1.0f;
    LockRecordButtonLastTick = SDL_GetTicks();
    
	return true;
}

bool CalledSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
  if (snapshotCountDown)
        return false;

    CaptureSnapshot(calledDeviceInfo.ip, 0);
    snapshotCountDown = SNAPSHOT_COUNT;
	return true;
}

bool CalledAnswerButtonOnPress(ITUWidget* widget, char* param)
{
    if (calledConnected)
    {
        LinphoneTerminate(0);
        SceneHangUp();
        ituLayerGoto(mainMenuLayer);
    }
    else
    {
        LinphoneAnswer(calledID);
    }
	return true;
}

bool CalledUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    SceneOpenDoor(NULL);
	return true;
}

bool CalledHangUpButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*)widget;

    ituButtonSetPressed(btn, false);
    LinphoneTerminate(0);
    SceneHangUp();
	return true;
}

bool CalledWaitingCancelButtonOnPress(ITUWidget* widget, char* param)
{
    //LinphoneTerminate(calledPendingID);
    calledPendingID = 0;
    //ituWidgetEnable(calledButtonContainer);
    callSpottedStatus = SPOTTED_REJECT;
	return true;
}

bool CalledWaitingConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* desc;

    //LinphoneTerminate(calledID);
    //LinphoneAnswer(calledPendingID);
    CaptureStopRecord();
    AddressBookGetDeviceInfo(&calledDeviceInfo, calledPendingAddr);

    ituWidgetSetVisible(calledUnlockButton, (calledDeviceInfo.type >= DEVICE_OUTDOOR && calledDeviceInfo.type <= DEVICE_WALL) || calledDeviceInfo.type == DEVICE_INTERCOM ? true : false);

    desc = StringGetRoomAddress(calledDeviceInfo.area, calledDeviceInfo.building, calledDeviceInfo.unit, calledDeviceInfo.floor, calledDeviceInfo.room, NULL);
    ituTextSetString(calledSourceText, desc);
    free(desc);

    if (calledPendingVideo)
    {
        ituWidgetSetVisible(calledRemoteBackground, true);
        ituWidgetSetVisible(calledRemoteSprite, false);
        ituWidgetSetVisible(calledSnapshotButton, true);
        ituWidgetSetVisible(calledStopRecordButton, false);
        
        if (calledVideoRecordEnable)        
            ituWidgetSetVisible(calledRecordButton, true);
        else
            ituWidgetSetVisible(calledRecordButton, false);

        ituWidgetSetVisible(calledAnswerButton, true);
    }
    else
    {
        ituWidgetSetVisible(calledRemoteBackground, false);
        ituWidgetSetVisible(calledRemoteSprite, true);
        ituWidgetSetVisible(calledSnapshotButton, false);
        ituWidgetSetVisible(calledStopRecordButton, false);
        ituWidgetSetVisible(calledRecordButton, false);
        ituWidgetSetVisible(calledAnswerButton, true);
    }
    
    if(!calledInVideoState && calledPendingVideo)
    {
        LinphoneSetWindowID(calledRemoteBackground);
	}
    calledInVideoState = calledPendingVideo;
    calledID = calledPendingID;
    calledPendingID = 0;
    //ituWidgetEnable(calledButtonContainer);
    callSpottedStatus = SPOTTED_ALLOW;
    return true;
}

bool CalledIncomingOnCustom(ITUWidget* widget, char* param)
{
    DeviceInfo info;
    char* desc;
	char addr[16];
	int id=0, video=0;

	sscanf(param, "%s %d %d", addr, &id, &video);
    AddressBookGetDeviceInfo(&info, addr);

    ituTextSetString(calledWaitingTypeText, StringGetDeviceType(info.type));
    desc = StringGetRoomAddress(info.area, info.building, info.unit, info.floor, info.room, NULL);
    ituTextSetString(calledWaitingRoomText, desc);
    free(desc);

    if(callSpottedStatus == SPOTTED_INIT)
    {
        //ituWidgetDisable(calledButtonContainer);
        ituWidgetSetVisible(calledWaitingBackground, true);
        //ituWidgetSetVisible(calledAnswerButton, true);
        spotCountDown = 5.0f;
        spotLastTick = SDL_GetTicks();
    }    
    calledPendingID = id;
    strcpy(calledPendingAddr, addr);
    calledPendingVideo = video;

	return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void calledRemoteBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}
#endif

void CalledIncomingShow(int id, char* addr, int video)
{
    char* desc;

    if (!calledRemoteBackground)
    {
        calledRemoteBackground = ituSceneFindWidget(&theScene, "calledRemoteBackground");
        assert(calledRemoteBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(calledRemoteBackground, calledRemoteBackgroundDraw);
#endif	

        calledRemoteSprite = ituSceneFindWidget(&theScene, "calledRemoteSprite");
        assert(calledRemoteSprite);

        calledUnlockButton = ituSceneFindWidget(&theScene, "calledUnlockButton");
        assert(calledUnlockButton);

        calledSourceText = ituSceneFindWidget(&theScene, "calledSourceText");
        assert(calledSourceText);

        calledSnapshotButton = ituSceneFindWidget(&theScene, "calledSnapshotButton");
        assert(calledSnapshotButton);    

        calledRecordButton = ituSceneFindWidget(&theScene, "calledRecordButton");
        assert(calledRecordButton);    

        calledStopRecordButton = ituSceneFindWidget(&theScene, "calledStopRecordButton");
        assert(calledStopRecordButton);

        calledAnswerButton = ituSceneFindWidget(&theScene, "calledAnswerButton");
        assert(calledAnswerButton);
    }
    calledCountDown = 0.0f;

    calledVideoRecordEnable = false;

    if (CaptureGetPath())
        calledVideoRecordEnable = true;

    AddressBookGetDeviceInfo(&calledDeviceInfo, addr);
    if (video)
    {
        ituWidgetSetVisible(calledRemoteBackground, true);
        ituWidgetSetVisible(calledRemoteSprite, false);
        ituWidgetSetVisible(calledSnapshotButton, true);
        if (calledVideoRecordEnable)
            ituWidgetSetVisible(calledRecordButton, true);
        else
            ituWidgetSetVisible(calledRecordButton, false);
    }
    else
    {
        ituWidgetSetVisible(calledRemoteBackground, false);
        ituWidgetSetVisible(calledRemoteSprite, true);
        ituWidgetSetVisible(calledSnapshotButton, false);
        ituWidgetSetVisible(calledRecordButton, false);
    }
    ituWidgetSetVisible(calledUnlockButton, (calledDeviceInfo.type >= DEVICE_OUTDOOR && calledDeviceInfo.type <= DEVICE_WALL) || calledDeviceInfo.type == DEVICE_INTERCOM ? true : false);

    desc = StringGetRoomAddress(calledDeviceInfo.area, calledDeviceInfo.building, calledDeviceInfo.unit, calledDeviceInfo.floor, calledDeviceInfo.room, NULL);
    ituTextSetString(calledSourceText, desc);
    free(desc);
	calledID = id;
	calledInVideoState = video;
}

bool CalledConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];
    int vol;

    ituCheckBoxSetChecked(calledVol0CheckBox, false);
    ituCheckBoxSetChecked(calledVol1CheckBox, false);
    ituCheckBoxSetChecked(calledVol2CheckBox, false);
    ituCheckBoxSetChecked(calledVol3CheckBox, false);
    ituCheckBoxSetChecked(calledVol4CheckBox, false);
    ituCheckBoxSetChecked(calledVol5CheckBox, false);
    ituCheckBoxSetChecked(calledVol6CheckBox, false);

    vol = LinphoneGetVoiceLevel() * calledMaxVol / 100;
    if (vol >= 0)
        ituCheckBoxSetChecked(calledVol0CheckBox, true);
    if (vol >= 1)
        ituCheckBoxSetChecked(calledVol1CheckBox, true);
    if (vol >= 2)
        ituCheckBoxSetChecked(calledVol2CheckBox, true);
    if (vol >= 3)
        ituCheckBoxSetChecked(calledVol3CheckBox, true);
    if (vol >= 4)
        ituCheckBoxSetChecked(calledVol4CheckBox, true);
    if (vol >= 5)
        ituCheckBoxSetChecked(calledVol5CheckBox, true);
    if (vol >= 6)
        ituCheckBoxSetChecked(calledVol6CheckBox, true);

    calledConnected = true;

    calledCountDown    = ((theConfig.calling_time > 0) ? (float)theConfig.calling_time + 0.5f : (float)theConfig.calling_time);
    calledLastTick     = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)calledCountDown);
    ituTextSetString(calledCountDownText, buf);
	return true;
}

bool CalledOnTimer(ITUWidget* widget, char* param)
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
    if (spotCountDown > 0.0f && callSpottedStatus == SPOTTED_INIT)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - spotLastTick;
        if (diff >= 1000)
        {
            spotCountDown -= (float)diff / 1000.0f;
            spotLastTick = tick;
        }
        if (spotCountDown <= 0.0f)
        {
            calledPendingID = 0;
            //ituWidgetEnable(calledButtonContainer);
            ituWidgetSetVisible(calledWaitingBackground, false);
            callSpottedStatus = SPOTTED_REJECT;
        }
	}
    if (calledCountDown > 0.0f)
    {
        uint32_t diff, tick = SDL_GetTicks();

        if (tick >= calledLastTick)
        {
            diff = tick - calledLastTick;
        }
        else
        {
            diff = 0xFFFFFFFF - calledLastTick + tick;
        }

        if (diff >= 1000)
        {
            char buf[4] = {0};

            calledCountDown -= (float)diff / 1000.0f;
            calledLastTick = tick;

            if ((0 <= ((int)calledCountDown)) && (((int)calledCountDown) < 1000))
            {
                sprintf(buf, "%d", (int)calledCountDown);
                ituTextSetString(calledCountDownText, buf);
            }
        }
        if (calledCountDown <= 0.0f)
        {
            LinphoneTerminate(0);
            SceneHangUp();
		    ituLayerGoto(mainMenuLayer);
        }
    }

    if (snapshotCountDown > 0)
    {
        snapshotCountDown--;
    }
	return true;
}

bool CalledOnEnter(ITUWidget* widget, char* param)
{
    int vol;

    if (!mainMenuLayer)
    {
        mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);

        if (!calledRemoteBackground)
        {
            calledRemoteBackground = ituSceneFindWidget(&theScene, "calledRemoteBackground");
            assert(calledRemoteBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
            ituWidgetSetDraw(calledRemoteBackground, calledRemoteBackgroundDraw);
#endif	    
            calledRemoteSprite = ituSceneFindWidget(&theScene, "calledRemoteSprite");
            assert(calledRemoteSprite);

            calledUnlockButton = ituSceneFindWidget(&theScene, "calledUnlockButton");
            assert(calledUnlockButton);
        }

        calledCountDownText = ituSceneFindWidget(&theScene, "calledCountDownText");
        assert(calledCountDownText);

        calledWaitingBackground = ituSceneFindWidget(&theScene, "calledWaitingBackground");
        assert(calledWaitingBackground);

        calledWaitingTypeText = ituSceneFindWidget(&theScene, "calledWaitingTypeText");
        assert(calledWaitingTypeText);

        calledWaitingRoomText = ituSceneFindWidget(&theScene, "calledWaitingRoomText");
        assert(calledWaitingRoomText);

        calledSnapshotButton = ituSceneFindWidget(&theScene, "calledSnapshotButton");
        assert(calledSnapshotButton);

        //calledButtonContainer = ituSceneFindWidget(&theScene, "calledButtonContainer");
        //assert(calledButtonContainer);

        calledVolBackground = ituSceneFindWidget(&theScene, "calledVolBackground");
        assert(calledVolBackground);

        calledVol0CheckBox = ituSceneFindWidget(&theScene, "calledVol0CheckBox");
        assert(calledVol0CheckBox);

        calledVol1CheckBox = ituSceneFindWidget(&theScene, "calledVol1CheckBox");
        assert(calledVol1CheckBox);

        calledVol2CheckBox = ituSceneFindWidget(&theScene, "calledVol2CheckBox");
        assert(calledVol2CheckBox);

        calledVol3CheckBox = ituSceneFindWidget(&theScene, "calledVol3CheckBox");
        assert(calledVol3CheckBox);

        calledVol4CheckBox = ituSceneFindWidget(&theScene, "calledVol4CheckBox");
        assert(calledVol4CheckBox);

        calledVol5CheckBox = ituSceneFindWidget(&theScene, "calledVol5CheckBox");
        assert(calledVol5CheckBox);

        calledVol6CheckBox = ituSceneFindWidget(&theScene, "calledVol6CheckBox");
        assert(calledVol6CheckBox);

        // status
        calledMaxVol = 6;
    }
    
    // volume
    calledConnected = false;

    vol = LinphoneGetRingLevel() * calledMaxVol / 100;
    ituCheckBoxSetChecked(calledVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(calledVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(calledVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(calledVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(calledVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(calledVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(calledVol6CheckBox, vol >= 6);

    calledVoiceLevelOld = LinphoneGetVoiceLevel();
    calledRingLevelOld = LinphoneGetRingLevel();

    // count down
    ituTextSetString(calledCountDownText, NULL);
    calledCountDown = spotCountDown = 0.0f;

    // snapshot
    snapshotCountDown = 0;

    // video
#if (CFG_CHIP_FAMILY == 9850)    
    SceneEnterVideoState(33);
#endif    
    if (calledInVideoState)
    {
        LinphoneSetWindowID(calledRemoteBackground);
    }

    // waiting
    ituWidgetSetVisible(calledWaitingBackground, false);
	return true;
}

bool CalledOnLeave(ITUWidget* widget, char* param)
{
    CaptureStopRecord();

    if (calledVoiceLevelOld != LinphoneGetVoiceLevel() ||
        calledRingLevelOld != LinphoneGetRingLevel())
        ConfigSave();

    calledInVideoState = false;
    calledID = calledPendingID = 0;
#if (CFG_CHIP_FAMILY == 9850)
    if(strcmp(param, "redirectResidentLayer") && strcmp(param, "cameraLayer"))
        SceneLeaveVideoState();
    else if(!strcmp(param, "cameraLayer"))
    {
        LinphoneTerminate(0);
        SceneHangUp();
    }
#endif    
	return true;
}

void CalledReset(void)
{
    mainMenuLayer      = NULL;
    calledRemoteBackground  = NULL;
}
