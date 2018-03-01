#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "scene.h"

#define SNAPSHOT_PATH CFG_TEMP_DRIVE ":/capture"
static ITULayer* messageRecordLayer;
static ITUText* messageRecordViewTopicText;
static ITUText* messageRecordViewTimeText;
static ITUBackground* messageRecordViewImageBackground;
static ITUIcon* messageRecordViewImageIcon;
static ITUBackground* messageRecordViewVideoBackground;
static ITUBackground* messageRecordViewAudioBackground;

bool MessageRecordViewOnTimer(ITUWidget* widget, char* param)
{
    return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void messageRecordViewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
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
    return true;
}

bool MessageRecordViewOnLeave(ITUWidget* widget, char* param)
{    
	return true;
}

void MessageRecordViewReset(void)
{
    messageRecordViewTopicText          = NULL;
	messageRecordViewImageBackground    = NULL;
    messageRecordLayer                  = NULL;
}
