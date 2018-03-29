#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITULayer* messageVideoLayer;
static ITUText* messageVideoViewTopicText;
static ITUText* messageVideoViewTimeText;
static ITUBackground* messageVideoViewBackground;
static ITUProgressBar* messageVideoViewVolProgressBar;
static ITUTrackBar* messageVideoViewVolTrackBar;
static int messageVideoViewID;
static bool messageVideoViewPlaying;

bool MessageVideoViewVolTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int level = atoi(param);
    
    LinphoneSettingMode();//smooth the setting bar
    LinphoneSetVoiceLevel(level);

    return true;
}

bool MessageVideoViewOnTimer(ITUWidget* widget, char* param)
{
    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
       if (!messageVideoLayer)
       {
           messageVideoLayer = ituSceneFindWidget(&theScene, "messageVideoLayer");
           assert(messageVideoLayer);
       }
   
       if (messageVideoViewPlaying && VideoMemoIsPlayFinished())
       {
           ituLayerGoto(messageVideoLayer);
           messageVideoViewPlaying = false;
       }
    }
    return false;
}

void MessageVideoViewShow(char* topic, char* time, int id)
{
    if (!messageVideoViewTopicText)
    {
        messageVideoViewTopicText = ituSceneFindWidget(&theScene, "messageVideoViewTopicText");
        assert(messageVideoViewTopicText);

        messageVideoViewTimeText = ituSceneFindWidget(&theScene, "messageVideoViewTimeText");
        assert(messageVideoViewTimeText);
    }
    ituTextSetString(messageVideoViewTopicText, topic);
    ituTextSetString(messageVideoViewTimeText, time);
    messageVideoViewID = id;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void messageVideoViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool MessageVideoViewOnEnter(ITUWidget* widget, char* param)
{
	if(!messageVideoViewBackground) 
	{
	    messageVideoViewBackground = ituSceneFindWidget(&theScene, "messageVideoViewBackground");
        assert(messageVideoViewBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(messageVideoViewBackground, messageVideoViewBackgroundDraw);
#endif

        messageVideoViewVolProgressBar = ituSceneFindWidget(&theScene, "messageVideoViewVolProgressBar");
        assert(messageVideoViewVolProgressBar);

        messageVideoViewVolTrackBar = ituSceneFindWidget(&theScene, "messageVideoViewVolTrackBar");
        assert(messageVideoViewVolTrackBar);
	}
    ituProgressBarSetValue(messageVideoViewVolProgressBar, LinphoneGetVoiceLevel());
    ituTrackBarSetValue(messageVideoViewVolTrackBar, LinphoneGetVoiceLevel());
    SceneEnterVideoState(33);
    LinphoneSetWindowID(messageVideoViewBackground);

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        VideoMemoStartPlay(messageVideoViewID);
        messageVideoViewPlaying = true;
    }
    else
    {
        LinphonePlayVideoMessage(theCenterInfo.ip, messageVideoViewID);
        SceneCameraSetViewing(true);
    }
	return true;
}

bool MessageVideoViewOnLeave(ITUWidget* widget, char* param)
{
    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        VideoMemoStopPlay();
        messageVideoViewPlaying = false;
    }
    else
    {
       SceneCameraSetViewing(false);
       LinphoneTerminate(0);
       SceneHangUp();
    }
    if(strcmp(param, "calledLayer") && strcmp(param, "cameraOutdoorLayer") && strcmp(param, "videoRecordLayer"))
            SceneLeaveVideoState();
    return true;
}

void MessageVideoViewReset(void)
{
    messageVideoViewBackground  = NULL;
    messageVideoViewTopicText   = NULL;
    messageVideoLayer           = NULL;
}
