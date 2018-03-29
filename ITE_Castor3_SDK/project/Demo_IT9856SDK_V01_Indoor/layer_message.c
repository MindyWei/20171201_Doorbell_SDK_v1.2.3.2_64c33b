#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"

static ITUBackground* messageVideoUnreadBackground;
static ITUText* messageVideoUnreadText;
static ITUBackground* messageRecordUnreadBackground;
static ITUText* messageRecordUnreadText;
static ITUBackground* messageServiceUnreadBackground;
static ITUText* messageServiceUnreadText;

void MessageNotifyUnreadRecordMessage(int count)
{
    if (!messageRecordUnreadBackground)
    {
        messageRecordUnreadBackground = ituSceneFindWidget(&theScene, "messageRecordUnreadBackground");
        assert(messageRecordUnreadBackground);

        messageRecordUnreadText = ituSceneFindWidget(&theScene, "messageRecordUnreadText");
        assert(messageRecordUnreadText);
    }
    if (count > 0)
    {
        char buf[8];

        ituWidgetSetVisible(messageRecordUnreadBackground, true);

        sprintf(buf, "%d", count);
        ituTextSetString(messageRecordUnreadText, buf);
    }
    else
    {
        ituWidgetSetVisible(messageRecordUnreadBackground, false);
    }
}

bool MessageNotifyVideoMessageOnCustom(ITUWidget* widget, char* param)
{
	int count = atoi(param);
    if (!messageVideoUnreadBackground)
    {
        messageVideoUnreadBackground = ituSceneFindWidget(&theScene, "messageVideoUnreadBackground");
        assert(messageVideoUnreadBackground);

        messageVideoUnreadText = ituSceneFindWidget(&theScene, "messageVideoUnreadText");
        assert(messageVideoUnreadText);
    }

    if (count > 0)
    {
        char buf[8];

        ituWidgetSetVisible(messageVideoUnreadBackground, true);

        sprintf(buf, "%d", count);
        ituTextSetString(messageVideoUnreadText, buf);
    }
    else
    {
        ituWidgetSetVisible(messageVideoUnreadBackground, false);
    }
	return true;
}

bool MessageNotifyTextMessageOnCustom(ITUWidget* widget, char* param)
{
	int count = atoi(param);
    if (!messageServiceUnreadBackground)
    {
        messageServiceUnreadBackground = ituSceneFindWidget(&theScene, "messageServiceUnreadBackground");
        assert(messageServiceUnreadBackground);

        messageServiceUnreadText = ituSceneFindWidget(&theScene, "messageServiceUnreadText");
        assert(messageServiceUnreadText);
    }

    if (count > 0)
    {
        char buf[8];

        ituWidgetSetVisible(messageServiceUnreadBackground, true);

        sprintf(buf, "%d", count);
        ituTextSetString(messageServiceUnreadText, buf);
    }
    else
    {
        ituWidgetSetVisible(messageServiceUnreadBackground, false);
    }
	return true;
}

void MessageReset(void)
{
	messageServiceUnreadBackground = NULL;
	messageVideoUnreadBackground = NULL;
	messageRecordUnreadBackground = NULL;
}
