#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITUScrollListBox* messageRecordTopicScrollListBox;
static ITUScrollListBox* messageRecordTimeScrollListBox;
static ITUScrollIconListBox* messageRecordScrollIconListBox;

bool MessageRecordScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;

    entryCount = CaptureGetCount();

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        char buf[8];
        CaptureEntry* entry;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        entry = CaptureGetEntry(j);

        sprintf(buf, "%d", entry->type);
        ituTextSetString(scrolltext, buf);
        ituWidgetSetCustomData(scrolltext, j);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool MessageRecordTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = CaptureGetCount();

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        CaptureEntry* entry;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        entry = CaptureGetEntry(j);

        ituTextSetString(scrolltext, entry->name);
        ituWidgetSetCustomData(scrolltext, j);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool MessageRecordTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = CaptureGetCount();

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        CaptureEntry* entry;
        int hour, min, sec;
        char buf[32];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        entry = CaptureGetEntry(j);

        hour = entry->sec / (60 * 60);
        min = entry->sec / 60 - hour * 60;
        sec = entry->sec % 60;
        if (hour > 0)
            sprintf(buf, "%02d:%02d:%02d", hour, min, sec);
        else
            sprintf(buf, "%02d:%02d", min, sec);

        ituTextSetString(scrolltext, buf);
        ituWidgetSetCustomData(scrolltext, j);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool MessageRecordScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageRecordTopicScrollListBox);

    if (topicItem)
    {
        ITUScrollText* timeItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageRecordTimeScrollListBox);
        int id = (int)ituWidgetGetCustomData(topicItem);
        CaptureEntry* entry = CaptureGetEntry(id);
        MessageRecordViewShow(ituTextGetString(&topicItem->text), ituTextGetString(&timeItem->text), id, entry);
        return true; //goto messageRecordViewLayer
    }
	return false;
}

bool MessageRecordDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageRecordTopicScrollListBox);

    if (topicItem)
    {
        int id = (int)ituWidgetGetCustomData(topicItem);
        CaptureEntry* entry = CaptureGetEntry(id);

        if (entry->type  == MEDIA_IMAGE)
        {        	
            CaptureSnapshotDeleteEntry(id);
        }
        else
        {
            CaptureDeleteEntry(id);
        }

#ifdef BACKUP_FLAG
    ConfigSaveTempForBackup();
#else
    ConfigSaveTemp();
#endif		

        MessageNotifyUnreadRecordMessage(CaptureGetUnplayCount());
        ituListBoxReload((ITUListBox*)messageRecordTopicScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordTimeScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordScrollIconListBox);
    }
	return true;
}

bool MessageRecordOnEnter(ITUWidget* widget, char* param)
{
    if (!messageRecordTopicScrollListBox)
    {
        messageRecordTopicScrollListBox = ituSceneFindWidget(&theScene, "messageRecordTopicScrollListBox");
        assert(messageRecordTopicScrollListBox);

        messageRecordTimeScrollListBox = ituSceneFindWidget(&theScene, "messageRecordTimeScrollListBox");
        assert(messageRecordTimeScrollListBox);
	
        messageRecordScrollIconListBox = ituSceneFindWidget(&theScene, "messageRecordScrollIconListBox");
        assert(messageRecordScrollIconListBox);    
    }

    if (strcmp(param, "messageRecordViewLayer"))
    {
        ituListBoxReload((ITUListBox*)messageRecordTopicScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordTimeScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordScrollIconListBox);
    }
    else
    {
        MessageRecordTopicScrollListBoxOnLoad((ITUWidget*)messageRecordTopicScrollListBox, NULL);
        MessageRecordTimeScrollListBoxOnLoad((ITUWidget*)messageRecordTimeScrollListBox, NULL);
        MessageRecordScrollIconListBoxOnLoad((ITUWidget*)messageRecordScrollIconListBox, NULL);
    }
	return true;
}

void MessageRecordReset(void)
{
    messageRecordTopicScrollListBox = NULL;
}
