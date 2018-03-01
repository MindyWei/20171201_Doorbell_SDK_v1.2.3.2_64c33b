#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "scene.h"
#include "leaf_mediastream.h"

#define TEST_RTSP_STREAMING 0

static ITULayer* messageVideoLayer;
static ITUText* messageVideoViewTopicText;
static ITUText* messageVideoViewTimeText;
static ITUBackground* messageVideoViewBackground;
static ITUTrackBar* messageVideoViewVolTrackBar;
static int messageVideoViewID;

extern LeafCall *call;


bool MessageVideoViewVolTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int level = atoi(param);
    leaf_set_ring_level(call, level);
    theConfig.ring_lev = level;
    return true;
}

bool MessageVideoViewOnTimer(ITUWidget* widget, char* param)
{
    int cur_time, total_time;
    
    if (!messageVideoLayer)
    {
        messageVideoLayer = ituSceneFindWidget(&theScene, "messageVideoLayer");
        assert(messageVideoLayer);
    }

#if TEST_RTSP_STREAMING
#else
    cur_time = leaf_get_video_memo_current_time();
    total_time = leaf_get_video_memo_total_time();
    //if(cur_time != -1 && total_time != -1)
        //printf("curr time: %d sec, total time: %d sec\n", cur_time, total_time);
    
    if(leaf_video_memo_is_play_finished())
    {
        //leaf_stop_video_memo_playback(call);
        ituLayerGoto(messageVideoLayer);
    }
#endif    
    return true;
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

        messageVideoViewVolTrackBar = ituSceneFindWidget(&theScene, "messageVideoViewVolTrackBar");
        assert(messageVideoViewVolTrackBar);
	}
    SceneEnterVideoState(33);
#if TEST_RTSP_STREAMING
    leaf_start_rtsp_stream("192.168.190.100", 3232, "D:/test.mkv");
#else
    leaf_start_video_memo_playback(call, "E:/rec.mkv");
#endif
	return true;
}

bool MessageVideoViewOnLeave(ITUWidget* widget, char* param)
{
#if TEST_RTSP_STREAMING
    leaf_stop_rtsp_stream();
#else
    leaf_stop_video_memo_playback(call);
#endif
    SceneLeaveVideoState();
	return true;
}

void MessageVideoViewReset(void)
{
    messageVideoViewBackground  = NULL;
    messageVideoViewTopicText   = NULL;
    messageVideoLayer = NULL;
}
