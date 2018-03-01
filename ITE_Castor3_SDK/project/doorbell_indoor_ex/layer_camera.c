#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "config.h"
#include "leaf_mediastream.h"

#define TESTVIDEO 0

static ITUScrollListBox* cameraScrollListBox;
static ITUBackground* cameraScreenBackground;
static ITUText* cameraCountDownText;
static ITUButton* cameraRecordButton;
static ITUButton* cameraStopRecordButton;

extern LeafCall *call;

static bool SnapShotSuccess;

static void SnapShotSuccessCallback(void *arg)
{
	char *GetSaveSuccessFilename = NULL;
	GetSaveSuccessFilename = (char*)arg;
	SnapShotSuccess = true;
}

static void SoundPlayEofCallback(int state)
{
     switch (state)
    {
        case SOUND_START_PLAY:
            printf("sound start play\n");
        break;
        case SOUND_FINISH_PLAY:
            printf("aound finish play\n");
            break;
        default:
            break;
    }
}

bool CameraScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool CameraScrollListOnSelection(ITUWidget* widget, char* param)
{
    return true;
}

bool CameraRecordButtonOnPress(ITUWidget* widget, char* param)
{
#if TESTVIDEO
    remove("E:/rec.mkv");
    leaf_start_video_memo_record(call, "E:/rec.mkv");
#else
    leaf_pause_sound_play(call,false);//resume sound play
#endif
	return true;
}

bool CameraStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
#if TESTVIDEO
    leaf_stop_video_memo_record(call);
#else
	leaf_pause_sound_play(call,true);//pause sound play
#endif
    return true;
}

bool CameraUnlockButtonOnPress(ITUWidget* widget, char* param)
{
#if TESTVIDEO

#else
    leaf_set_ring_level(call,50);
    leaf_start_sound_play(call,"D:/test.wav",Normalplay,SoundPlayEofCallback);
        
#endif    
    return true;
}

bool CameraSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
#if TESTVIDEO
    SnapShotSuccess = false;
    remove("E:/snapshot.jpg");
	leaf_take_video_snapshot(call, "E:/snapshot.jpg", SnapShotSuccessCallback);
#else
    leaf_stop_sound_play(call);
#endif
	return true;
}

bool CameraDisconnectButtonOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool CameraOnTimer(ITUWidget* widget, char* param)
{
    return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void cameraScreenBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;

    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);	
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool CameraOnEnter(ITUWidget* widget, char* param)
{
    int x, y, width, height;
    if (!cameraScreenBackground)
    {
        cameraScreenBackground = ituSceneFindWidget(&theScene, "cameraScreenBackground");
        assert(cameraScreenBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraScreenBackground, cameraScreenBackgroundDraw);
#endif 

        cameraScrollListBox = ituSceneFindWidget(&theScene, "cameraScrollListBox");
        assert(cameraScrollListBox);

        cameraCountDownText = ituSceneFindWidget(&theScene, "cameraCountDownText");
        assert(cameraCountDownText);
     
        cameraRecordButton = ituSceneFindWidget(&theScene, "cameraRecordButton");
        assert(cameraRecordButton);

        cameraStopRecordButton = ituSceneFindWidget(&theScene, "cameraStopRecordButton");
        assert(cameraStopRecordButton);
    }
    ituWidgetSetVisible(cameraRecordButton, true);
    ituWidgetSetVisible(cameraStopRecordButton, false);

    ituWidgetGetGlobalPosition(cameraScreenBackground, &x, &y);
    width = ituWidgetGetWidth(cameraScreenBackground);
    height = ituWidgetGetHeight(cameraScreenBackground);
    SceneEnterVideoState(33);
#ifdef CFG_VIDEO_ENABLE        
    itv_set_video_window(x, y, width, height);
#endif        
    leaf_init_video_streams(call, 554);
    leaf_start_ipcam_stream(call, "admin:admin@192.168.190.31", 554);

    LeafPauseKeySound();
    return true;

}

bool CameraOnLeave(ITUWidget* widget, char* param)
{
    if(!leaf_video_memo_is_recording(call))
    {
        leaf_stop_video_memo_record(call);
    }    
    leaf_stop_ipcam_stream(call);
    SceneLeaveVideoState();
    LeafResumeKeySound();
    return true;
}

void CameraReset(void)
{
    cameraScreenBackground = NULL;
}
