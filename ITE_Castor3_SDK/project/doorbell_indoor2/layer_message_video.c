#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITUScrollListBox* messageVideoTopicScrollListBox;
static ITUScrollListBox* messageVideoTimeScrollListBox;
static Message* messageVideos;
static int messageVideoCount;

bool MessageVideoTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    if (theDeviceInfo.type == DEVICE_INDOOR2)
        entryCount = VideoMemoGetCount();
    else
        entryCount = messageVideoCount;

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
        if (theDeviceInfo.type == DEVICE_INDOOR2)
        {
            VideoMemoEntry* entry;
            ITUScrollText* scrolltext = (ITUScrollText*) node;
    
            entry = VideoMemoGetEntry(j);

            ituTextSetString(scrolltext, entry->name);
            ituWidgetSetCustomData(scrolltext, j);

            ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
        }
        else
        {
            Message* ptr = &messageVideos[j];
            ITUScrollText* scrolltext = (ITUScrollText*) node;
    
            ituTextSetString(scrolltext, ptr->topic);

            ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, ptr->read ? true : false);
    
            ituWidgetSetCustomData(scrolltext, ptr->id);
        }
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

bool MessageVideoTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    if (theDeviceInfo.type == DEVICE_INDOOR2)
        entryCount = VideoMemoGetCount();
    else
        entryCount = messageVideoCount;

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
        if (theDeviceInfo.type == DEVICE_INDOOR2)
        {
            VideoMemoEntry* entry;
            struct tm* timeinfo;
            char buf[32];
            ITUScrollText* scrolltext = (ITUScrollText*) node;

            entry = VideoMemoGetEntry(j);

            timeinfo = localtime((const time_t*)&entry->time);
            if (timeinfo)
                strftime(buf, sizeof(buf),"%Y/%m/%d %H:%M", timeinfo);

            ituTextSetString(scrolltext, buf);

            ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
        }
        else
        {
            Message* ptr = &messageVideos[j];
            ITUScrollText* scrolltext = (ITUScrollText*) node;
    
            ituTextSetString(scrolltext, ptr->time);

            ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, ptr->read ? true : false);
        }
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

bool MessageVideoScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageVideoTopicScrollListBox);

    if (topicItem)
    {
        ITUScrollText* timeItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageVideoTimeScrollListBox);
        int id = (int)ituWidgetGetCustomData(topicItem);
        MessageVideoViewShow(ituTextGetString(&topicItem->text), ituTextGetString(&timeItem->text), id);
        return true; //goto messageVideoViewLayer
    }
	return false;
}

static void MessageVideoReload(int arg)
{
    ituListBoxReload((ITUListBox*)messageVideoTopicScrollListBox);
    ituListBoxReload((ITUListBox*)messageVideoTimeScrollListBox);
}

static void MessageVideoClear(void)
{
    int i, count;

    if (messageVideoCount == 0)
        return;

    count = messageVideoCount;
    messageVideoCount = 0;

    for (i = 0; i < count; i++)
    {
        Message* msg = &messageVideos[i];
        free(msg->topic);
        free(msg->time);
    }
    free(messageVideos);
    messageVideos = NULL;
}

static void MessageVideoGetList(Message* msgs, int count)
{
    MessageVideoClear();

    messageVideos = msgs;
    messageVideoCount = count;
    ituSceneExecuteCommand(&theScene, 1, MessageVideoReload, 0);
}

static void MessageVideoDeleteComplete(void)
{
    MessageVideoListGet(MessageVideoGetList);
}

bool MessageVideoDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageVideoTopicScrollListBox);

    if (topicItem)
    {
        int id = (int)ituWidgetGetCustomData(topicItem);
        
        if (theDeviceInfo.type == DEVICE_INDOOR2)
        {
           VideoMemoDeleteEntry(id);
           ituSceneExecuteCommand(&theScene, 6, MessageVideoReload, 0);
        }
        else
            MessageVideoDelete(id, MessageVideoDeleteComplete);
    }
	return true;
}

bool MessageVideoOnEnter(ITUWidget* widget, char* param)
{
    if (!messageVideoTopicScrollListBox)
    {
        messageVideoTopicScrollListBox = ituSceneFindWidget(&theScene, "messageVideoTopicScrollListBox");
        assert(messageVideoTopicScrollListBox);

        messageVideoTimeScrollListBox = ituSceneFindWidget(&theScene, "messageVideoTimeScrollListBox");
        assert(messageVideoTimeScrollListBox);
	}
    if (theDeviceInfo.type == DEVICE_INDOOR2)
        MessageVideoReload(0);
    else
        MessageVideoListGet(MessageVideoGetList);

    MessageNotifyUnreadRecordMessage(CaptureGetUnplayCount());
	return true;
}

bool MessageVideoOnLeave(ITUWidget* widget, char* param)
{
    if (theDeviceInfo.type != DEVICE_INDOOR2)
        MessageVideoClear();

	return true;
}

void MessageVideoReset(void)
{
    messageVideoTopicScrollListBox = NULL;
}
