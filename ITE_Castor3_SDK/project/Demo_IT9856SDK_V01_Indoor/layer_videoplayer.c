#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"
#include "castor3player.h"
#include "ite/itv.h"

#ifdef _WIN32
#undef CFG_VIDEO_ENABLE
#endif

static ITUSprite* videoPlayerStorageSprite;
static ITUBackground* videoPlayerStorageTypeBackground;
static ITURadioBox* videoPlayerSDRadioBox;
static ITURadioBox* videoPlayerUsbRadioBox;
static ITUScrollMediaFileListBox* videoPlayerScrollMediaFileListBox;
static ITUText* videoPlayerTimeText;
static ITUProgressBar* videoPlayerProgressBar;
static ITUSprite* videoPlayerRepeatSprite;
static ITUCheckBox* videoPlayerRandomCheckBox;
static ITUCheckBox* videoPlayerPlayCheckBox;
static ITUCheckBox* videoViewPlayCheckBox;
static ITUCheckBox* videoViewPlay1CheckBox;
static ITUCheckBox* videoViewPlay2CheckBox;
static ITUCheckBox* videoViewRandomCheckBox;

static ITUSprite* videoPlayerVolSprite;
static ITUProgressBar* videoPlayerVolProgressBar;
static ITUTrackBar* videoPlayerVolTrackBar;
static ITUBackground* videoPlayerViewBackground;
static ITUButton* videoPlayerViewButton;

static int x, y, width, height = 0;
static MTAL_SPEC mtal_spec = {0};
int LastMediaPlayerVoice = 0;
bool videoPlayerIsFileEOF = false;
bool videoPlayerIsPlaying = false;
int videoPlayerPercentage = 0;
bool MgrReloadSBCRunning = true;

void VideoPlayerStop(void)
{
    if(videoPlayerIsPlaying) {
#ifdef CFG_VIDEO_ENABLE
        mtal_pb_stop();
#endif
        videoPlayerIsPlaying = false;
    }

    if(!MgrReloadSBCRunning){
        smtkAudioMgrReloadSBC(castor3snd_reinit_for_video_memo_play());
        MgrReloadSBCRunning = true ;
    }
}


static void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
    switch(nEventID)
    {
        case PLAYER_EVENT_EOF:
            printf("File EOF\n");
            videoPlayerIsFileEOF = true;
            break;
        case PLAYER_EVENT_OPEN_FILE_FAIL:
            printf("Open file fail\n");
            videoPlayerIsFileEOF = true;
            break;
        case PLAYER_EVENT_UNSUPPORT_FILE:
            printf("File not support\n");
            videoPlayerIsFileEOF = true;
            break;
        default:
            break;
    }
}

int CalVideoPlayerProgressBarPercentage(void)
{
    int percentage_value;
    int totaltime = 0;
    int currenttime = 0;
    int ret;
#ifdef CFG_VIDEO_ENABLE        
    ret = mtal_pb_get_total_duration(&totaltime);
    if(ret < 0)
        return -1;
    ret = mtal_pb_get_current_time(&currenttime);
    if(ret < 0)
        return -1;
#endif        
    if(totaltime > 0)
    {
        percentage_value = currenttime*100/totaltime;
        if(percentage_value > 100)
            percentage_value = 100;
    }
    else
        percentage_value = 0;

    return percentage_value;
}

bool VideoPlayerSDInsertedOnCustom(ITUWidget* widget, char* param)
{
    if (!ituWidgetIsVisible(videoPlayerStorageSprite))
    {
        StorageType storageType = StorageGetCurrType();

        ituWidgetSetVisible(videoPlayerStorageSprite, true);
        ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, true);
        ituSpriteGoto(videoPlayerStorageSprite, storageType);

        ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
    }
    ituWidgetEnable(videoPlayerSDRadioBox);
    return true;
}

bool VideoPlayerSDRemovedOnCustom(ITUWidget* widget, char* param)
{
    if (videoPlayerStorageSprite->frame == STORAGE_SD)
    {
        StorageType storageType = StorageGetCurrType();

        if (videoPlayerIsPlaying)
        {
#ifdef CFG_VIDEO_ENABLE        
            mtal_pb_stop();
#endif        
            videoPlayerIsPlaying = false;
        }

        ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
        ituCheckBoxSetChecked(videoViewPlayCheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay1CheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay2CheckBox, false);
        ituTextSetString(videoPlayerTimeText, NULL);
        videoPlayerPercentage = 0;
        ituWidgetSetVisible(videoPlayerProgressBar, false);

        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, false);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, true);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, true);
            ituSpriteGoto(videoPlayerStorageSprite, storageType);

            ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }
    }
    ituWidgetDisable(videoPlayerSDRadioBox);
    return true;
}

bool VideoPlayerUsbInsertedOnCustom(ITUWidget* widget, char* param)
{
    if (!ituWidgetIsVisible(videoPlayerStorageSprite))
    {
        StorageType storageType = StorageGetCurrType();

        ituWidgetSetVisible(videoPlayerStorageSprite, true);
        ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, true);
        ituSpriteGoto(videoPlayerStorageSprite, storageType);

        if (storageType == STORAGE_SD)
            ituRadioBoxSetChecked(videoPlayerSDRadioBox, true);
        else if (storageType == STORAGE_USB)
            ituRadioBoxSetChecked(videoPlayerUsbRadioBox, true);

        ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
    }
    ituWidgetEnable(videoPlayerUsbRadioBox);
    return true;
}

bool VideoPlayerUsbRemovedOnCustom(ITUWidget* widget, char* param)
{
    if (videoPlayerStorageSprite->frame == STORAGE_USB)
    {
        StorageType storageType = StorageGetCurrType();

        if (videoPlayerIsPlaying)
        {
#ifdef CFG_VIDEO_ENABLE        
            mtal_pb_stop();
#endif        
            videoPlayerIsPlaying = false;
        }

        ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
        ituCheckBoxSetChecked(videoViewPlayCheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay1CheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay2CheckBox, false);
        ituTextSetString(videoPlayerTimeText, NULL);
        videoPlayerPercentage = 0;
        ituWidgetSetVisible(videoPlayerProgressBar, false);
        
        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, false);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, true);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, true);
            ituSpriteGoto(videoPlayerStorageSprite, storageType);

            if (storageType == STORAGE_SD)
                ituRadioBoxSetChecked(videoPlayerSDRadioBox, true);
            else if (storageType == STORAGE_USB)
                ituRadioBoxSetChecked(videoPlayerUsbRadioBox, true);

            ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }
    }
    ituWidgetDisable(videoPlayerUsbRadioBox);
    return true;
}

bool VideoPlayerStorageRadioBoxOnPress(ITUWidget* widget, char* param)
{
    StorageType storageType = StorageGetCurrType();

    if ((storageType == STORAGE_SD && widget == (ITUWidget*)videoPlayerSDRadioBox) ||
        (storageType == STORAGE_USB && widget == (ITUWidget*)videoPlayerUsbRadioBox))
        return false;

    if (videoPlayerIsPlaying)
    {
#ifdef CFG_VIDEO_ENABLE        
        mtal_pb_stop();
#endif    
        videoPlayerIsPlaying = false;
    }
    ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
    ituCheckBoxSetChecked(videoViewPlayCheckBox, false);
    ituCheckBoxSetChecked(videoViewPlay1CheckBox, false);
    ituCheckBoxSetChecked(videoViewPlay2CheckBox, false);
    ituTextSetString(videoPlayerTimeText, NULL);
    videoPlayerPercentage = 0;
    ituWidgetSetVisible(videoPlayerProgressBar, false);

    if (widget == (ITUWidget*)videoPlayerSDRadioBox)
    {
        StorageSetCurrType(STORAGE_SD);
        ituSpriteGoto(videoPlayerStorageSprite, STORAGE_SD);
        ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(STORAGE_SD));
    }
    else if (widget == (ITUWidget*)videoPlayerUsbRadioBox)
    {
        StorageSetCurrType(STORAGE_USB);
        ituSpriteGoto(videoPlayerStorageSprite, STORAGE_USB);
        ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(STORAGE_USB));
    }
    return true;
}

bool VideoPlayerStorageTypeCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(videoPlayerStorageTypeBackground))
    {
        ituWidgetSetVisible(videoPlayerStorageTypeBackground, false);
        ituWidgetEnable(videoPlayerScrollMediaFileListBox);
    }
    else
    {
        ituWidgetSetVisible(videoPlayerStorageTypeBackground, true);
        ituWidgetDisable(videoPlayerScrollMediaFileListBox);
    }
    return true;
}

bool VideoPlayerScrollMediaFileListBoxOnSelection(ITUWidget* widget, char* param)
{
    return true;
}

bool VideoPlayerRepeatButtonOnPress(ITUWidget* widget, char* param)
{
    ITUMediaRepeatMode mode = videoPlayerRepeatSprite->frame;
    if (mode == ITU_MEDIA_REPEAT_ALL)
        mode = ITU_MEDIA_REPEAT_NONE;
    else
        mode++;

    videoPlayerScrollMediaFileListBox->mflistbox.repeatMode = mode;
    ituSpriteGoto(videoPlayerRepeatSprite, mode);

    return true;
}

bool VideoPlayerRandomCheckBoxOnPress(ITUWidget* widget, char* param)
{
    videoPlayerScrollMediaFileListBox->mflistbox.randomPlay = ituCheckBoxIsChecked(videoPlayerRandomCheckBox);

    if(ituCheckBoxIsChecked(videoPlayerRandomCheckBox))
    {
        ituCheckBoxSetChecked(videoViewRandomCheckBox, true);
    }
    else
    {
        ituCheckBoxSetChecked(videoViewRandomCheckBox, false);
    }
    return true;
}

bool VideoPlayerLastButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* item = ituMediaFileListPrev((ITUMediaFileListBox*)videoPlayerScrollMediaFileListBox);
    if (item && videoPlayerIsPlaying)
    {
        char* filepath = (char*)ituWidgetGetCustomData(item);

        //LinphonePauseKeySound();
        
        strcpy(mtal_spec.srcname, filepath);
        mtal_spec.vol_level = LastMediaPlayerVoice;

        AudioStop();
#ifdef CFG_VIDEO_ENABLE        
        mtal_pb_stop();
        mtal_pb_select_file(&mtal_spec);
        mtal_pb_play();
#endif    
        ituWidgetSetVisible(videoPlayerViewBackground, true);
        ituWidgetSetVisible(videoPlayerViewButton, true);
        ituTextSetString(videoPlayerTimeText, NULL);

        //videoPlayerIsPlaying = true;
        videoPlayerPercentage = 0;
        ituWidgetSetVisible(videoPlayerProgressBar, true);
    }
    return true;
}

bool VideoPlayerPauseButtonOnPress(ITUWidget* widget, char* param)
{
	if(videoPlayerIsPlaying)
	{
#ifdef CFG_VIDEO_ENABLE 
		printf("##################video play/pause \r\n");       
		mtal_pb_pause();
#endif
	}
	return true;
}

bool VideoPlayerPlayCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(videoPlayerPlayCheckBox))
    {
        ITUScrollText* item = ituMediaFileListPlay((ITUMediaFileListBox*)videoPlayerScrollMediaFileListBox);
        if (item)
        {
            char* filepath = (char*)ituWidgetGetCustomData(item);

            //LinphonePauseKeySound();

            // TODO: IMPLEMENT
            strcpy(mtal_spec.srcname, filepath);
            mtal_spec.vol_level = LastMediaPlayerVoice;
            
            AudioStop();
#ifdef CFG_VIDEO_ENABLE            
            mtal_pb_stop();
            mtal_pb_select_file(&mtal_spec);
            mtal_pb_play();
#endif            
            ituWidgetSetVisible(videoPlayerViewBackground, true);
            ituWidgetSetVisible(videoPlayerViewButton, true);
            ituCheckBoxSetChecked(videoViewPlayCheckBox, true);
            ituCheckBoxSetChecked(videoViewPlay1CheckBox, true);
            ituCheckBoxSetChecked(videoViewPlay2CheckBox, true);

            videoPlayerIsPlaying = true;
            videoPlayerPercentage = 0;
            ituWidgetSetVisible(videoPlayerProgressBar, true);
        }
        else
        {
            ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
        }
    }
    else
    {
#ifdef CFG_VIDEO_ENABLE        
        mtal_pb_stop();
#endif
        //LinphoneResumeKeySound();
        ituWidgetSetVisible(videoPlayerViewBackground, false);
        ituWidgetSetVisible(videoPlayerViewButton, false);
        ituCheckBoxSetChecked(videoViewPlayCheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay1CheckBox, false);
        ituCheckBoxSetChecked(videoViewPlay2CheckBox, false);

        videoPlayerIsPlaying = false;
        videoPlayerPercentage = 0;
        ituWidgetSetVisible(videoPlayerProgressBar, false);
    }
    ituTextSetString(videoPlayerTimeText, NULL);
    return true;
}

bool VideoPlayerNextButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* item = ituMediaFileListNext((ITUMediaFileListBox*)videoPlayerScrollMediaFileListBox);
    if (item && videoPlayerIsPlaying)
    {
        char* filepath = (char*)ituWidgetGetCustomData(item);

        //LinphonePauseKeySound();
        
        strcpy(mtal_spec.srcname, filepath);
        mtal_spec.vol_level = LastMediaPlayerVoice;

        AudioStop();
#ifdef CFG_VIDEO_ENABLE        
        mtal_pb_stop();
        mtal_pb_select_file(&mtal_spec);
        mtal_pb_play();
#endif
        ituWidgetSetVisible(videoPlayerViewBackground, true);
        ituWidgetSetVisible(videoPlayerViewButton, true);
        ituTextSetString(videoPlayerTimeText, NULL);

        //videoPlayerIsPlaying = true;
        videoPlayerPercentage = 0;
        ituWidgetSetVisible(videoPlayerProgressBar, true);
    }
    return true;
}

bool VideoPlayerVolTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int vol;

    if (!videoPlayerVolSprite)
        return false;

    vol = atoi(param);
    LastMediaPlayerVoice = vol;

    if (vol > 0)
    {
        ituSpriteGoto(videoPlayerVolSprite, 1);
    }
    else
    {
        ituSpriteGoto(videoPlayerVolSprite, 0);
    }

    
    AudioSetVolume(vol);
    return true;
}

bool VideoPlayerOnTimer(ITUWidget* widget, char* param)
{
    // TODO: IMPLEMENT
    if(videoPlayerIsFileEOF)
    {
        ITUScrollText* item = ituMediaFileListNext((ITUMediaFileListBox*)videoPlayerScrollMediaFileListBox);
        videoPlayerIsFileEOF = false;
#ifdef CFG_VIDEO_ENABLE        
        mtal_pb_stop();
#endif
        if(item)
        {
            char* filepath = (char*)ituWidgetGetCustomData(item);
            strcpy(mtal_spec.srcname, filepath);
            mtal_spec.vol_level = LastMediaPlayerVoice;
            
#ifdef CFG_VIDEO_ENABLE
            mtal_pb_select_file(&mtal_spec);
            mtal_pb_play();
#endif            
        }
        else
        {
            ituWidgetSetVisible(videoPlayerProgressBar, false);
            ituWidgetSetVisible(videoPlayerViewBackground, false);
            ituWidgetSetVisible(videoPlayerViewButton, false);
            ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
            ituCheckBoxSetChecked(videoViewPlayCheckBox, false);
            ituCheckBoxSetChecked(videoViewPlay1CheckBox, false);
            ituCheckBoxSetChecked(videoViewPlay2CheckBox, false);
            videoPlayerIsPlaying = false;
        }    
    }

    if (videoPlayerIsPlaying)
    {
        ScreenSaverRefresh();
#ifdef CFG_VIDEO_ENABLE
        videoPlayerPercentage = CalVideoPlayerProgressBarPercentage();
#endif
        if(videoPlayerPercentage >= 0)
            ituProgressBarSetValue(videoPlayerProgressBar, videoPlayerPercentage);
    }

    if (videoPlayerIsPlaying)
    {
        int h, m, s = 0;
        char buf[32];
        int ret;
#ifdef CFG_VIDEO_ENABLE        
        if(mtal_pb_get_current_time(&s) == 0)
            
#endif
        {
            m = s / 60;
            s %= 60;
            h = m / 60;
            m %= 60;

            if (h > 0)
                sprintf(buf, "%02d:%02d:%02d", h, m, s);
            else
                sprintf(buf, "%02d:%02d", m, s);

            ituTextSetString(videoPlayerTimeText, buf);
        }    
    }
    return false;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool VideoPlayerOnEnter(ITUWidget* widget, char* param)
{
    StorageType storageType;
    int vol;    
    if(!videoPlayerIsPlaying && !LinphoneGetWarnSoundPlaying() && MgrReloadSBCRunning) {
        castor3snd_deinit_for_video_memo_play();
        MgrReloadSBCRunning = false ;
    }

    if (!videoPlayerStorageSprite)
    {
        videoPlayerStorageSprite = ituSceneFindWidget(&theScene, "videoPlayerStorageSprite");
        assert(videoPlayerStorageSprite);

        videoPlayerStorageTypeBackground = ituSceneFindWidget(&theScene, "videoPlayerStorageTypeBackground");
        assert(videoPlayerStorageTypeBackground);

        videoPlayerSDRadioBox = ituSceneFindWidget(&theScene, "videoPlayerSDRadioBox");
        assert(videoPlayerSDRadioBox);

        videoPlayerUsbRadioBox = ituSceneFindWidget(&theScene, "videoPlayerUsbRadioBox");
        assert(videoPlayerUsbRadioBox);

        videoPlayerScrollMediaFileListBox = ituSceneFindWidget(&theScene, "videoPlayerScrollMediaFileListBox");
        assert(videoPlayerScrollMediaFileListBox);

        videoPlayerTimeText = ituSceneFindWidget(&theScene, "videoPlayerTimeText");
        assert(videoPlayerTimeText);

        videoPlayerProgressBar = ituSceneFindWidget(&theScene, "videoPlayerProgressBar");
        assert(videoPlayerProgressBar);

        videoPlayerRepeatSprite = ituSceneFindWidget(&theScene, "videoPlayerRepeatSprite");
        assert(videoPlayerRepeatSprite);

        videoPlayerRandomCheckBox = ituSceneFindWidget(&theScene, "videoPlayerRandomCheckBox");
        assert(videoPlayerRandomCheckBox);

        videoPlayerPlayCheckBox = ituSceneFindWidget(&theScene, "videoPlayerPlayCheckBox");
        assert(videoPlayerPlayCheckBox);

        videoPlayerVolSprite = ituSceneFindWidget(&theScene, "videoPlayerVolSprite");
        assert(videoPlayerVolSprite);

        videoPlayerVolProgressBar = ituSceneFindWidget(&theScene, "videoPlayerVolProgressBar");
        assert(videoPlayerVolProgressBar);

        videoPlayerVolTrackBar = ituSceneFindWidget(&theScene, "videoPlayerVolTrackBar");
        assert(videoPlayerVolTrackBar);

        videoPlayerViewBackground = ituSceneFindWidget(&theScene, "videoPlayerViewBackground");
        assert(videoPlayerViewBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(videoPlayerViewBackground, VideoPlayerViewBackgroundDraw);
#endif

        videoPlayerViewButton = ituSceneFindWidget(&theScene, "videoPlayerViewButton");
        assert(videoPlayerViewButton);

        videoViewPlayCheckBox = ituSceneFindWidget(&theScene, "videoViewPlayCheckBox");
        assert(videoViewPlayCheckBox);

        videoViewPlay1CheckBox = ituSceneFindWidget(&theScene, "videoViewPlay1CheckBox");
        assert(videoViewPlay1CheckBox);

        videoViewPlay2CheckBox = ituSceneFindWidget(&theScene, "videoViewPlay2CheckBox");
        assert(videoViewPlay2CheckBox);

        videoViewRandomCheckBox = ituSceneFindWidget(&theScene, "videoViewRandomCheckBox");
        assert(videoViewRandomCheckBox);
    }

    if (strcmp(param, "videoViewLayer"))
    {
        storageType = StorageGetCurrType();

        if (storageType == STORAGE_NONE)
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, false);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, false);
        }
        else
        {
            ituWidgetSetVisible(videoPlayerStorageSprite, true);
            ituWidgetSetVisible(videoPlayerScrollMediaFileListBox, true);
            ituSpriteGoto(videoPlayerStorageSprite, storageType);

            if (storageType == STORAGE_SD)
                ituRadioBoxSetChecked(videoPlayerSDRadioBox, true);
            else if (storageType == STORAGE_USB)
                ituRadioBoxSetChecked(videoPlayerUsbRadioBox, true);

            if (StorageGetDrivePath(STORAGE_SD))
                ituWidgetEnable(videoPlayerSDRadioBox);
            else
                ituWidgetDisable(videoPlayerSDRadioBox);

            if (StorageGetDrivePath(STORAGE_USB))
                ituWidgetEnable(videoPlayerUsbRadioBox);
            else
                ituWidgetDisable(videoPlayerUsbRadioBox);

            ituMediaFileListSetPath(&videoPlayerScrollMediaFileListBox->mflistbox, StorageGetDrivePath(storageType));
        }
        SceneEnterVideoState(33);
    }
    
    vol = AudioGetVolume();
    ituSpriteGoto(videoPlayerVolSprite, vol > 0 ? 1 : 0);
    ituProgressBarSetValue(videoPlayerVolProgressBar, vol);
    ituTrackBarSetValue(videoPlayerVolTrackBar, vol);
    LastMediaPlayerVoice = vol;

    ituWidgetGetGlobalPosition(videoPlayerViewBackground, &x, &y);
    width = ituWidgetGetWidth(videoPlayerViewBackground);
    height = ituWidgetGetHeight(videoPlayerViewBackground);
#ifdef CFG_VIDEO_ENABLE        
    itv_set_video_window(x, y, width, height);
#endif        
    if (strcmp(param, "videoViewLayer") == 0)
    {
        // TODO: IMPLEMENT
        if(!videoPlayerIsPlaying)
        {
            ituTextSetString(videoPlayerTimeText, NULL);
            ituWidgetSetVisible(videoPlayerViewBackground, false);
            ituWidgetSetVisible(videoPlayerViewButton, false);
            ituWidgetSetVisible(videoPlayerProgressBar, false);
        }
    }
    else
    {
        ituTextSetString(videoPlayerTimeText, NULL);
        ituWidgetSetVisible(videoPlayerViewBackground, false);
        ituWidgetSetVisible(videoPlayerViewButton, false);
        ituWidgetSetVisible(videoPlayerProgressBar, false);
		LinphoneStartMediaVideoPlay();
#ifdef CFG_VIDEO_ENABLE    
        mtal_pb_init(EventHandler);
#endif
        LinphonePauseKeySound();
    }
    return true;
}

bool VideoPlayerOnLeave(ITUWidget* widget, char* param)
{
    if (strcmp(param, "videoViewLayer"))
    {
        LinphoneStopMediaVideoPlay();
#ifdef CFG_VIDEO_ENABLE    
        mtal_pb_stop();
        mtal_pb_exit();
#endif        
        LinphoneResumeKeySound();
        ituCheckBoxSetChecked(videoPlayerPlayCheckBox, false);
        videoPlayerIsPlaying = false;
        videoPlayerPercentage = 0;
		if(strcmp(param, "calledLayer") && strcmp(param, "cameraOutdoorLayer") && strcmp(param, "videoRecordLayer") && strcmp(param, "callingLayer"))
        	SceneLeaveVideoState();

        if(!MgrReloadSBCRunning){
            smtkAudioMgrReloadSBC(castor3snd_reinit_for_video_memo_play());
            MgrReloadSBCRunning = true ;
        }
    }

    return true;
}

void VideoPlayerReset(void)
{
    videoPlayerStorageSprite = NULL;
}
