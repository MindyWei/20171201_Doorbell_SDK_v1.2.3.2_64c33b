#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "ite/itv.h"
#include "leaf_mediastream.h"
#include "config.h"
#include "doorbell.h"

static ITULayer*        mainMenuLayer;
static ITUBackground*   calledRemoteBackground;
static ITUSprite*       calledRemoteSprite;
static ITUText*         calledSourceText;
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

//call

#define MKV_REC_TEST 1

extern LeafCall *call;

static bool SnapShotSuccess;

static void SnapShotSuccessCallback(void *arg)
{
	char *GetSaveSuccessFilename = NULL;
	GetSaveSuccessFilename = (char*)arg;
	SnapShotSuccess = true;
}

bool CalledVolTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int level = atoi(param);
    if(call->audiostream){
        leaf_set_play_level(call,level);
        theConfig.play_lev = level;
    }
    else{
        leaf_set_ring_level(call,level);
        theConfig.ring_lev = level;
    }
	return true;
}

bool  CalledRecordButtonOnPress(ITUWidget* widget, char* param)
{
#if MKV_REC_TEST
    remove("E:/rec.mkv");
    leaf_start_video_memo_record(call, "E:/rec.mkv");
#else
    remove("D:/test.wav");
    leaf_start_audio_memo_record(call, "D:/test.wav");   
#endif

    return true;
}

bool CalledStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
#if MKV_REC_TEST    
    leaf_stop_video_memo_record(call);
#else
    leaf_stop_audio_memo_record(call);
#endif    
    //leaf_stop_voice_memo_record(call);//test stop rec recVoice sound
	return true;
}

bool CalledSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
	SnapShotSuccess = false;
    remove("E:/snapshot.jpg");
	leaf_take_video_snapshot(call, "E:/snapshot.jpg", SnapShotSuccessCallback);
	//leaf_take_video_snapshot(call, "E:/snapshot.jpg", NULL);
	return true;
}

bool CalledAnswerButtonOnPress(ITUWidget* widget, char* param)
{
    LeafStartConnect();
	return true;
}

bool CalledUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool CalledHangUpButtonOnPress(ITUWidget* widget, char* param)
{
    ituButtonSetPressed((ITUButton*)widget, false);

    //leaf_stop_voice_memo_playback(call);//just for test stop recVoice sound
    if(leaf_video_memo_is_recording(call))
    {
        leaf_stop_video_memo_record(call);
    }
    
    LeafTerminate();
    
	return true;
}

bool CalledWaitingCancelButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool CalledWaitingConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool CalledSourceButtonOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool CalledSourceScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool CalledSourceScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    return true;
}

bool CalledIncomingOnCustom(ITUWidget* widget, char* param)
{
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
}

bool CalledConnectedOnCustom(ITUWidget* widget, char* param)
{
	return true;
}

bool CalledOnTimer(ITUWidget* widget, char* param)
{ 
	return true;
}

bool CalledOnEnter(ITUWidget* widget, char* param)
{    
    int x, y, width, height;

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

        calledAnswerButton = ituSceneFindWidget(&theScene, "calledAnswerButton");
        assert(calledAnswerButton);

        calledButtonContainer = ituSceneFindWidget(&theScene, "calledButtonContainer");
        assert(calledButtonContainer);

        calledSourceButton = ituSceneFindWidget(&theScene, "calledSourceButton");
        assert(calledSourceButton);

        calledSourceScrollListBox = ituSceneFindWidget(&theScene, "calledSourceScrollListBox");
        assert(calledSourceScrollListBox);

        calledRecordButton = ituSceneFindWidget(&theScene, "calledRecordButton");
        assert(calledRecordButton);    

        calledStopRecordButton = ituSceneFindWidget(&theScene, "calledStopRecordButton");
        assert(calledStopRecordButton);
    }

    // video
    ituWidgetGetGlobalPosition(calledRemoteBackground, &x, &y);
    width = ituWidgetGetWidth(calledRemoteBackground);
    height = ituWidgetGetHeight(calledRemoteBackground);

    SceneEnterVideoState(33);
#ifdef CFG_VIDEO_ENABLE        
    itv_set_video_window(x, y, width, height);
#endif

    ituWidgetSetVisible(calledWaitingBackground, false); 
    ituWidgetSetVisible(calledRemoteSprite, false);
    ituWidgetSetVisible(calledRecordButton, true);
    ituWidgetSetVisible(calledStopRecordButton, false);
   
	//call->dir = CallIncoming;
    //leaf_init_audio_streams(call, 6666);
    //leaf_start_audio_stream(call, "192.168.190.132", 6666);
    //leaf_init_video_streams(call, 5555);
    //leaf_start_video_stream(call, "192.168.190.132", 5555);
    LeafStratRingPreview();
#ifdef CFG_AUTO_ANSWER
	LeafStartConnect();
#endif
    return true;
}

bool CalledOnLeave(ITUWidget* widget, char* param)
{
    if(leaf_video_memo_is_recording(call))
    {
        leaf_stop_video_memo_record(call);
    }
    
    NetworkCallSend(CALL_STOP);
	if (NetworkCheckAck())
	{
		LeafTerminate();
		NetworkCallIdle();
	}

    SceneLeaveVideoState();
	return true;
}

void CalledReset(void)
{
    mainMenuLayer      = NULL;
    calledRemoteBackground  = NULL;
}
