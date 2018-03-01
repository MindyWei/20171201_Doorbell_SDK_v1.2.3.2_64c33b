#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUBackground* recordBackground;
static ITUListBox* recordTopicListBox;
static ITUListBox* recordTimeListBox;
static ITUBackground* recordDialogBackground;

bool RecordTopicListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = CaptureGetCount();
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
            CaptureEntry* entry;
            scrolltext = (ITUScrollText*) node;
    
            entry = CaptureGetEntry(i);

            ituTextSetString(scrolltext, entry->name);
            ituWidgetSetCustomData(scrolltext, i);

            if (entry->played)
                ituWidgetSetColor(scrolltext, 255, 128, 128, 128);
            else
                ituWidgetSetColor(scrolltext, 255, 0, 0, 0);

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

bool RecordTimeListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = CaptureGetCount();
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
            CaptureEntry* entry;
            int hour, min, sec;
            char buf[32];
            scrolltext = (ITUScrollText*) node;
    
            entry = CaptureGetEntry(i);

            hour = entry->sec / (60 * 60);
            min = entry->sec / 60 - hour * 60;
            sec = entry->sec % 60;
            if (hour > 0)
                sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
            else
                sprintf(buf, "%02d:%02d", min, sec);

            ituTextSetString(scrolltext, buf);

            if (entry->played)
                ituWidgetSetColor(scrolltext, 255, 128, 128, 128);
            else
                ituWidgetSetColor(scrolltext, 255, 0, 0, 0);

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

bool RecordViewButtonOnPress(ITUWidget* widget, char* param)
{
    ITUText* item = (ITUText*)ituListBoxGetFocusItem(recordTopicListBox);
    if (item)
    {
        int id = (int)ituWidgetGetCustomData(item);
        CaptureEntry* entry = CaptureGetEntry(id);
        RecordViewShow(ituTextGetString(item), id, entry);
		ituButtonSetPressed((ITUButton*)widget, false);
        return true; //goto recordViewLayer
    }
    return false;
}

bool RecordDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUText* item = (ITUText*)ituListBoxGetFocusItem(recordTopicListBox);
    if (item)
    {
        int id = (int)ituWidgetGetCustomData(item);
        CaptureDeleteEntry(id);
        return true;
    }
    return false;
}

static bool RecordBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        int x = arg2 - widget->rect.x;
        int y = arg3 - widget->rect.y;

        if (ituWidgetIsInside(widget, x, y))
        {
            int xx, yy, w, h;

            x = arg2;
            y = arg3;

            if (ituWidgetIsVisible(recordDialogBackground))
            {
                ituWidgetGetGlobalPosition(recordDialogBackground, &xx, &yy);
                w = ituWidgetGetWidth(recordDialogBackground);
                h = ituWidgetGetHeight(recordDialogBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituWidgetSetVisible(recordDialogBackground, false);
                    ituWidgetEnable(widget);
                    return widget->dirty;
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool RecordOnEnter(ITUWidget* widget, char* param)
{
    if (!recordBackground)
    {
        recordBackground = ituSceneFindWidget(&theScene, "recordBackground");
        assert(recordBackground);
        ituWidgetSetUpdate(recordBackground, RecordBackgroundUpdate);

        recordTopicListBox = ituSceneFindWidget(&theScene, "recordTopicListBox");
        assert(recordTopicListBox);

        recordTimeListBox = ituSceneFindWidget(&theScene, "recordTimeListBox");
        assert(recordTimeListBox);

        recordDialogBackground = ituSceneFindWidget(&theScene, "recordDialogBackground");
        assert(recordDialogBackground);
    }
    return true;
}

void RecordReset(void)
{
    recordBackground = NULL;
}
