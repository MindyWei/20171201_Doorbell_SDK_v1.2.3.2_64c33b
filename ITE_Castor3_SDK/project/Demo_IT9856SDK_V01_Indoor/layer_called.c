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
static ITUProgressBar*  calledVolProgressBar;
static ITUTrackBar*     calledVolTrackBar;
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
static ITUButton*	    calledSourceButton;
static ITUScrollListBox* calledSourceScrollListBox;

// status
static bool calledInVideoState;
static bool calledConnected;
static bool calledSpotted;
static float calledCountDown, spotCountDown, LockRecordButtonCountDown;
static uint32_t calledLastTick, spotLastTick, LockRecordButtonLastTick;
static int calledID, calledPendingID;
static bool calledPendingVideo;
static char calledPendingAddr[32];
static DeviceInfo calledDeviceInfo;
static int calledVoiceLevelOld, calledRingLevelOld;
static int snapshotCountDown;
static DeviceInfo calledIPCameraInfoArray[MAX_IPCAMERA_COUNT];
static int calledIPCameraInfoCount;
static bool calledVideoRecordEnable;
static bool LockRecordButton;
static bool SpecialCaseForLockRecordButton = false;

extern SPOTTED_STATUS callSpottedStatus;

#define SNAPSHOT_COUNT (1 * 1000 / MS_PER_FRAME) // 1 sec

#ifdef CFG_AUTO_ANSWER
    #define AUTO_ANSWER_COUNT (5 * 1000 / MS_PER_FRAME) // 5 sec
    static ITUButton* calledAnswerButton;
    static int calledAutoAnswerCountDown;
#endif // CFG_AUTO_ANSWER

static bool CalledWaitingConfirmButtonOnSimulatePressing()
{
    char* desc;

    //LinphoneTerminate(calledID);
    //LinphoneAnswer(calledPendingID);
    CaptureStopRecord();
    AddressBookGetDeviceInfo(&calledDeviceInfo, calledPendingAddr);

    ituWidgetSetVisible(calledUnlockButton, (calledDeviceInfo.type >= DEVICE_OUTDOOR && calledDeviceInfo.type <= DEVICE_WALL) ? true : false);

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
    ituWidgetEnable(calledButtonContainer);
    callSpottedStatus = SPOTTED_ALLOW;
	calledSpotted = true;
    return true;
}


bool CalledVolTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int level = atoi(param);

    if (calledConnected)
    {
        LinphoneSettingMode();//smooth the setting bar
        LinphoneSetVoiceLevel(level);
    }
    else
    {
        LinphoneSettingMode();//smooth the setting bar
        LinphoneSetRingLevel(level, true);
    }
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

    if (calledConnected && !calledSpotted)
    {
        LinphoneTerminate(0);
        SceneHangUp();
        ituLayerGoto(mainMenuLayer);
    }
    else
    {
        LinphoneAnswer(calledID);
		calledSpotted = false;
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
    ITUButton* btn = (ITUButton*)widget;

    ituButtonSetPressed(btn, false);

    //LinphoneTerminate(calledPendingID);
    calledPendingID = 0;
    ituWidgetEnable(calledButtonContainer);
    callSpottedStatus = SPOTTED_REJECT;
	calledSpotted = false;
	return true;
}

bool CalledWaitingConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* desc;

    //LinphoneTerminate(calledID);
    //LinphoneAnswer(calledPendingID);
    CaptureStopRecord();
    AddressBookGetDeviceInfo(&calledDeviceInfo, calledPendingAddr);

    ituWidgetSetVisible(calledUnlockButton, (calledDeviceInfo.type >= DEVICE_OUTDOOR && calledDeviceInfo.type <= DEVICE_WALL) ? true : false);

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
    ituWidgetEnable(calledButtonContainer);
    callSpottedStatus = SPOTTED_ALLOW;
	calledSpotted = true;
    return true;
}

bool CalledSourceButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(calledSourceScrollListBox))
    {
        ituWidgetSetVisible(calledSourceScrollListBox, false);
        return false;
    }
    else
    {
        ituWidgetSetVisible(calledSourceScrollListBox, true);
        return true;
    }
}

bool CalledSourceScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;

    if (calledIPCameraInfoCount == 0)
        calledIPCameraInfoCount = AddressBookGetIPCameraInfoArray(calledIPCameraInfoArray, MAX_IPCAMERA_COUNT);

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = calledIPCameraInfoCount ? (calledIPCameraInfoCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < calledIPCameraInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        DeviceInfo* info = &calledIPCameraInfoArray[j];
        if (info->alias[0] == '\0')
        {
            char* desc = StringGetCameraPosition(info->area, info->building, info->unit);
            ituTextSetString(scrolltext, desc);
            free(desc);
        }
        else
        {
            ituTextSetString(scrolltext, info->alias);
        }
        ituWidgetSetCustomData(scrolltext, info);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool CalledSourceScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(scrolltext);

        if (SceneCameraIsViewing())
        {
            if (calledVideoRecordEnable)
            {
                CaptureStopRecord();
                ituWidgetSetVisible(calledRecordButton, true);
                ituWidgetSetVisible(calledStopRecordButton, false);
            }
            // TODO: IMPLEMENT
        }
        else
        {
            // TODO: IMPLEMENT
        }
    }
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
        ituWidgetDisable(calledButtonContainer);
    	ituWidgetSetVisible(calledWaitingBackground, true);
        //ituWidgetSetVisible(calledAnswerButton, true);
        spotCountDown = (float)theConfig.dial_time;
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
    ituWidgetDrawImpl(widget, dest, x, y, alpha);   
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

    if (theDeviceInfo.type == DEVICE_INDOOR2 && CaptureGetPath())
        calledVideoRecordEnable = true;

    AddressBookGetDeviceInfo(&calledDeviceInfo, addr);
    if (video == 1)
    {
        ituWidgetSetVisible(calledRemoteBackground, true);
        ituWidgetSetVisible(calledRemoteSprite, false);
        ituWidgetSetVisible(calledSnapshotButton, true);
        if (calledVideoRecordEnable)
            ituWidgetSetVisible(calledRecordButton, true);
        else
            ituWidgetSetVisible(calledRecordButton, false);
    }
    else if(video == 2) //for ipcam
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
    ituWidgetSetVisible(calledUnlockButton, (calledDeviceInfo.type >= DEVICE_OUTDOOR && calledDeviceInfo.type <= DEVICE_WALL) ? true : false);

    desc = StringGetRoomAddress(calledDeviceInfo.area, calledDeviceInfo.building, calledDeviceInfo.unit, calledDeviceInfo.floor, calledDeviceInfo.room, NULL);
    ituTextSetString(calledSourceText, desc);
    free(desc);
	calledID = id;
	calledInVideoState = video;

#ifdef CFG_AUTO_ANSWER
    calledAutoAnswerCountDown = AUTO_ANSWER_COUNT;
#endif
}

bool CalledConnectedOnCustom(ITUWidget* widget, char* param)
{
    char buf[4];
    int level;
    calledConnected = true;
#if 1
    level = LinphoneGetVoiceLevel();

    if (level > 100)
        level = 100;
    else if (level < 0)
        level = 0;

    ituProgressBarSetValue(calledVolProgressBar, level);
    ituTrackBarSetValue(calledVolTrackBar, level);
#endif
    
    
    //level = LinphoneGetMicLevel();//mic level
    //LinphoneSetMicLevel(level);

    calledCountDown    = ((theConfig.calling_time > 0) ? (float)theConfig.calling_time + 0.5f : (float)theConfig.calling_time);
    calledLastTick     = SDL_GetTicks();
    
    sprintf(buf, "%d", (int)calledCountDown);
    ituTextSetString(calledCountDownText, buf);
	return true;
}

bool CalledOnTimer(ITUWidget* widget, char* param)
{
#ifdef CFG_AUTO_ANSWER
    if (calledAutoAnswerCountDown > 0 && --calledAutoAnswerCountDown <= 0)
    {
        LinphoneAnswer(calledID);
        ituWidgetSetVisible(calledAnswerButton, false);
        calledAutoAnswerCountDown = 0;
    }
#endif // CFG_AUTO_ANSWER
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
            ituWidgetEnable(calledButtonContainer);
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
            if(callSpottedStatus == SPOTTED_INIT)
            {
                ituWidgetSetVisible(calledWaitingBackground, false);
                CalledWaitingConfirmButtonOnSimulatePressing();
            }
            else
            {
                LinphoneTerminate(0);
                SceneHangUp();
    		    ituLayerGoto(mainMenuLayer);
            }
        }
    }

    if (snapshotCountDown > 0)
    {
        snapshotCountDown--;
    }
	return false;
}

bool CalledOnEnter(ITUWidget* widget, char* param)
{	
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

            calledSourceText = ituSceneFindWidget(&theScene, "calledSourceText");
            assert(calledSourceText);
        }

        calledCountDownText = ituSceneFindWidget(&theScene, "calledCountDownText");
        assert(calledCountDownText);

        calledVolProgressBar = ituSceneFindWidget(&theScene, "calledVolProgressBar");
        assert(calledVolProgressBar);

        calledVolTrackBar = ituSceneFindWidget(&theScene, "calledVolTrackBar");
        assert(calledVolTrackBar);

        calledWaitingBackground = ituSceneFindWidget(&theScene, "calledWaitingBackground");
        assert(calledWaitingBackground);

        calledWaitingTypeText = ituSceneFindWidget(&theScene, "calledWaitingTypeText");
        assert(calledWaitingTypeText);

        calledWaitingRoomText = ituSceneFindWidget(&theScene, "calledWaitingRoomText");
        assert(calledWaitingRoomText);

        calledSnapshotButton = ituSceneFindWidget(&theScene, "calledSnapshotButton");
        assert(calledSnapshotButton);

    #ifdef CFG_AUTO_ANSWER
        calledAnswerButton = ituSceneFindWidget(&theScene, "calledAnswerButton");
        assert(calledAnswerButton);
    #endif // CFG_AUTO_ANSWER

        calledButtonContainer = ituSceneFindWidget(&theScene, "calledButtonContainer");
        assert(calledButtonContainer);

        calledSourceButton = ituSceneFindWidget(&theScene, "calledSourceButton");
        assert(calledSourceButton);

        calledSourceScrollListBox = ituSceneFindWidget(&theScene, "calledSourceScrollListBox");
        assert(calledSourceScrollListBox);
    }

    // volume
    calledConnected = false;
    calledSpotted = false;
    ituProgressBarSetValue(calledVolProgressBar, LinphoneGetRingLevel());
    ituTrackBarSetValue(calledVolTrackBar, LinphoneGetRingLevel());
    calledVoiceLevelOld = LinphoneGetVoiceLevel();
    calledRingLevelOld = LinphoneGetRingLevel();

    // count down
    ituTextSetString(calledCountDownText, NULL);
    calledCountDown = spotCountDown = 0.0f;

    // snapshot
    snapshotCountDown = 0;

    // video
    SceneEnterVideoState(33);
    if (calledInVideoState)
    {
        LinphoneSetWindowID(calledRemoteBackground);
    }

    // waiting
    ituWidgetSetVisible(calledWaitingBackground, false);

    // ip camera
    calledIPCameraInfoCount = 0;
    
    return true;
}

bool CalledOnLeave(ITUWidget* widget, char* param)
{
    CaptureStopRecord();

    if (calledVoiceLevelOld != LinphoneGetVoiceLevel() ||
        calledRingLevelOld != LinphoneGetRingLevel()){
            LinphoneSetVoiceLevel(LinphoneGetVoiceLevel());
            LinphoneSetRingLevel(LinphoneGetRingLevel(), true);
            ConfigSave();
        }

    calledInVideoState = false;
    calledID = calledPendingID = 0;
    
    if(strcmp(param, "cameraLayer"))
    SceneLeaveVideoState();
    else
    {
        LinphoneTerminate(0);
        SceneHangUp();
    }
    
	return true;
}

void CalledReset(void)
{
    mainMenuLayer      = NULL;
    calledRemoteBackground  = NULL;
}
