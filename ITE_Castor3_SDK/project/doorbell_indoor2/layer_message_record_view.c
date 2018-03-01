#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

#define SNAPSHOT_PATH CFG_TEMP_DRIVE ":/capture"
static ITULayer* messageRecordLayer;
static ITUText* messageRecordViewTopicText;
static ITUText* messageRecordViewTimeText;
static ITUBackground* messageRecordViewImageBackground;
static ITUIcon* messageRecordViewImageIcon;
static ITUBackground* messageRecordViewVideoBackground;
static ITUBackground* messageRecordViewAudioBackground;
static int messageRecordViewID;
static CaptureEntry* messageRecordCaptureEntry;
static bool messageRecordViewPlaying;

bool MessageRecordViewOnTimer(ITUWidget* widget, char* param)
{
    if (!messageRecordLayer)
    {
        messageRecordLayer = ituSceneFindWidget(&theScene, "messageRecordLayer");
        assert(messageRecordLayer);
    }

    if (messageRecordViewPlaying && CaptureIsPlayFinished())
    {
        ituLayerGoto(messageRecordLayer);
        messageRecordViewPlaying = false;
    }
    return false;
}

void MessageRecordViewShow(char* topic, char* time, int id, CaptureEntry* entry)
{
    if (!messageRecordViewTopicText)
    {
        messageRecordViewTopicText = ituSceneFindWidget(&theScene, "messageRecordViewTopicText");
        assert(messageRecordViewTopicText);

        messageRecordViewTimeText = ituSceneFindWidget(&theScene, "messageRecordViewTimeText");
        assert(messageRecordViewTimeText);
    }
    ituTextSetString(messageRecordViewTopicText, topic);
    ituTextSetString(messageRecordViewTimeText, time);
    messageRecordViewID = id;
    messageRecordCaptureEntry = entry;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void messageRecordViewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool MessageRecordViewOnEnter(ITUWidget* widget, char* param)
{
    if (!messageRecordViewImageBackground)
    {
        messageRecordViewImageBackground = ituSceneFindWidget(&theScene, "messageRecordViewImageBackground");
        assert(messageRecordViewImageBackground);

        messageRecordViewImageIcon = ituSceneFindWidget(&theScene, "messageRecordViewImageIcon");
        assert(messageRecordViewImageIcon);

        messageRecordViewVideoBackground = ituSceneFindWidget(&theScene, "messageRecordViewVideoBackground");
        assert(messageRecordViewVideoBackground);    
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(messageRecordViewVideoBackground, messageRecordViewVideoBackgroundDraw);
#endif
        messageRecordViewAudioBackground = ituSceneFindWidget(&theScene, "messageRecordViewAudioBackground");
        assert(messageRecordViewAudioBackground);
    }

    if (messageRecordCaptureEntry->type == MEDIA_IMAGE)
    {
        char buf[PATH_MAX];
        FILE* f;
        //char* capturePath = CaptureGetPath();
        //if (!capturePath)
        //    return true;

        strcpy(buf, SNAPSHOT_PATH);
        strcat(buf, "/");
        strcat(buf, messageRecordCaptureEntry->filename);

        // try to load jpeg file if exists
        printf("filename =%s\n",buf);
        f = fopen(buf, "rb");
        if (f)
        {
            uint8_t* data;
            int size;
            struct stat sb;

            if (fstat(fileno(f), &sb) != -1)
            {
                size = sb.st_size;
                data = malloc(size);
                if (data)
                {			    
                    size = fread(data, 1, size, f);
                    ituIconLoadJpegData(messageRecordViewImageIcon, data, size);
                    free(data);
                }
            }
            fclose(f);
        }
        ituWidgetSetVisible(messageRecordViewImageBackground, true);
        ituWidgetSetVisible(messageRecordViewVideoBackground, false);
        ituWidgetSetVisible(messageRecordViewAudioBackground, false);
        CaptureStartPlay(messageRecordViewID);
    }
    else if (messageRecordCaptureEntry->type == MEDIA_VIDEO)
    {
        ituWidgetSetVisible(messageRecordViewImageBackground, false);
        ituWidgetSetVisible(messageRecordViewVideoBackground, true);
        ituWidgetSetVisible(messageRecordViewAudioBackground, false);
        CaptureStartPlay(messageRecordViewID);
        SceneEnterVideoState(33);
        LinphoneSetWindowID(messageRecordViewVideoBackground);
        messageRecordViewPlaying = true;
    }
    else if (messageRecordCaptureEntry->type == MEDIA_AUDIO)
    {
        ituWidgetSetVisible(messageRecordViewImageBackground, false);
        ituWidgetSetVisible(messageRecordViewVideoBackground, false);
        ituWidgetSetVisible(messageRecordViewAudioBackground, true);
        CaptureStartPlay(messageRecordViewID);
        messageRecordViewPlaying = true;
    }
    MessageNotifyUnreadRecordMessage(CaptureGetUnplayCount());
    SceneCameraSetViewing(true);
    return true;
}

bool MessageRecordViewOnLeave(ITUWidget* widget, char* param)
{    
    CaptureStopPlay();
    SceneCameraSetViewing(false);
    SceneContinuePlayGuardAlarm();
    if(strcmp(param, "calledLayer") && strcmp(param, "cameraOutdoorLayer") && strcmp(param, "videoRecordLayer"))
        SceneLeaveVideoState();
    messageRecordViewPlaying = false;
 
	return true;
}

void MessageRecordViewReset(void)
{
    messageRecordViewTopicText          = NULL;
	messageRecordViewImageBackground    = NULL;
    messageRecordLayer                  = NULL;
}
