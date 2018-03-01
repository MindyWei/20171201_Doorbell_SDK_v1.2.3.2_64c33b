#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITULayer* guardLogViewLayer;
static ITUBackground* guardLogBackground;
static ITUListBox* guardLogTopicListBox;
static ITUListBox* guardLogTimeListBox;
static ITUBackground* guardLogGrayBackground;
static ITUBackground* guardLogDialogBackground;
static ITUBackground* mainTopBackground;

bool GuardLogTopicListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = GuardGetLogCount();
    count = itcTreeGetChildCount(listbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;
    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }
    node = ((ITCTree*)listbox)->child;

    i = (listbox->pageIndex - 1) * count;
    count = 0;
    if (i < entryCount)
    {
        for (; i < entryCount; i++)
        {
            GuardLog* log;
            scrolltext = (ITUScrollText*) node;

            log = GuardGetLog(i);

            if (strcmp(log->floor, "00"))
                ituTextSetString(scrolltext, StringGetRoomAddress(log->area, log->building, log->unit, log->floor, log->room, NULL));
            else
                ituTextSetString(scrolltext, StringGetCameraPosition(log->area, log->building, log->unit));

			if (log->read)
			{
				ituWidgetSetColor(scrolltext, 255, 128, 128, 128);
				ituScrollTextSetAsRead(scrolltext);
			}
			else
			{
				ituWidgetSetColor(scrolltext, 255, 0, 0, 0);
				ituScrollTextSetAsUnread(scrolltext);
			}
            ituWidgetSetCustomData(scrolltext, i);
            count++;

            node = node->sibling;
            if (node == NULL)
                break;
        }
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituTextSetString(scrolltext, "");
    }
    listbox->itemCount = count;
	return true;
}

bool GuardLogTimeListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = GuardGetLogCount();
    count = itcTreeGetChildCount(listbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;
    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }
    node = ((ITCTree*)listbox)->child;

    i = (listbox->pageIndex - 1) * count;
    count = 0;
    if (i < entryCount)
    {
        for (; i < entryCount; i++)
        {
            time_t rawtime;
            struct tm* timeinfo;
            char buf[80];

            GuardLog* log;
            scrolltext = (ITUScrollText*) node;

            log = GuardGetLog(i);

            rawtime = log->timestamp;
            timeinfo = localtime(&rawtime);
            strftime(buf, sizeof(buf),"%Y.%m.%d %H:%M:%S", timeinfo);

            ituTextSetString(scrolltext, buf);

			if (log->read)
			{
				ituWidgetSetColor(scrolltext, 255, 128, 128, 128);
				ituScrollTextSetAsRead(scrolltext);
			}
			else
			{
				ituWidgetSetColor(scrolltext, 255, 0, 0, 0);
				ituScrollTextSetAsUnread(scrolltext);
			}
            count++;

            node = node->sibling;
            if (node == NULL)
                break;
        }
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituTextSetString(scrolltext, "");
    }

    listbox->itemCount = count;
	return true;
}

bool GuardLogViewButtonOnPress(ITUWidget* widget, char* param)
{
    if (guardLogTopicListBox->focusIndex >= 0)
    {
        ITUScrollText* item = (ITUScrollText*) itcTreeGetChildAt(guardLogTopicListBox, guardLogTopicListBox->focusIndex);
        int i = (int)ituWidgetGetCustomData(item);
        GuardLogViewShow(i);
        return true; //goto guardLogViewLayer
    }
	return false;
}

static void GuardLogReload(int arg)
{
    ituListBoxReload(guardLogTopicListBox);
    ituListBoxReload(guardLogTimeListBox);
}

bool GuardLogDeleteOnPress(ITUWidget* widget, char* param)
{
    if (guardLogTopicListBox->focusIndex >= 0)
	{
        ITUScrollText* item = (ITUScrollText*) itcTreeGetChildAt(guardLogTopicListBox, guardLogTopicListBox->focusIndex);
        int i = (int)ituWidgetGetCustomData(item);
        GuardDeleteLog(i);
        ituSceneExecuteCommand(&theScene, 6, GuardLogReload, 0);
    }
    return true;
}

static bool GuardLogGrayBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        if (ituWidgetIsEnabled(widget))
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;

            if (ituWidgetIsInside(widget, x, y))
            {
                int xx, yy, w, h;

                x = arg2;
                y = arg3;

                ituWidgetGetGlobalPosition(guardLogDialogBackground, &xx, &yy);
                w = ituWidgetGetWidth(guardLogDialogBackground);
                h = ituWidgetGetHeight(guardLogDialogBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituWidgetSetVisible(guardLogDialogBackground, false);
                    ituWidgetSetVisible(widget, false);
                    ituWidgetEnable(guardLogBackground);
                    ituWidgetEnable(mainTopBackground);
                    return widget->dirty;
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool GuardLogOnEnter(ITUWidget* widget, char* param)
{
    if (!guardLogBackground)
    {
        guardLogBackground = ituSceneFindWidget(&theScene, "guardLogBackground");
        assert(guardLogBackground);

        guardLogTopicListBox = ituSceneFindWidget(&theScene, "guardLogTopicListBox");
        assert(guardLogTopicListBox);

        guardLogTimeListBox = ituSceneFindWidget(&theScene, "guardLogTimeListBox");
        assert(guardLogTimeListBox);

        guardLogViewLayer = ituSceneFindWidget(&theScene, "guardLogViewLayer");
        assert(guardLogViewLayer);

        guardLogGrayBackground = ituSceneFindWidget(&theScene, "guardLogGrayBackground");
        assert(guardLogGrayBackground);
        ituWidgetSetUpdate(guardLogGrayBackground, GuardLogGrayBackgroundUpdate);

        guardLogDialogBackground = ituSceneFindWidget(&theScene, "guardLogDialogBackground");
        assert(guardLogDialogBackground);

        mainTopBackground = ituSceneFindWidget(&theScene, "mainTopBackground");
        assert(mainTopBackground);
	}
	return true;
}

bool GuardLogOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void GuardLogReset(void)
{
    guardLogBackground = NULL;
}
