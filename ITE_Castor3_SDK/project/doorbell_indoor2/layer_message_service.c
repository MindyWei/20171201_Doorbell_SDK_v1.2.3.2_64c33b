#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITULayer* messageServiceViewLayer;
static ITUScrollListBox* messageServiceTopicScrollListBox;
static ITUScrollListBox* messageServiceTimeScrollListBox;
static Message* messageTexts;
static int messageTextCount;

bool MessageServiceTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = messageTextCount;

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
        Message* ptr = &messageTexts[j];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        ituTextSetString(scrolltext, ptr->topic);
        ituWidgetSetCustomData(scrolltext, ptr);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, ptr->read ? true: false);

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

bool MessageServiceTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = messageTextCount;

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
        Message* ptr = &messageTexts[j];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        ituTextSetString(scrolltext, ptr->time);
        ituWidgetSetCustomData(scrolltext, ptr);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, ptr->read ? true: false);

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

bool MessageServiceScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageServiceTopicScrollListBox);

    if (topicItem)
    {
        ITUScrollText* timeItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageServiceTimeScrollListBox);
        Message* msg = (Message*)ituWidgetGetCustomData(topicItem);
        MessageServiceViewShow(msg->image);
        return true; //goto messageServiceViewLayer
    }
	return false;
}

static void MessageServiceReload(int arg)
{
    ituListBoxReload((ITUListBox*)messageServiceTopicScrollListBox);
    ituListBoxReload((ITUListBox*)messageServiceTimeScrollListBox);
}

static void MessageServiceClear(void)
{
    int i, count;

    if (messageTextCount == 0)
        return;

    count = messageTextCount;
    messageTextCount = 0;

    for (i = 0; i < count; i++)
    {
        Message* msg = &messageTexts[i];
        free(msg->topic);
        free(msg->time);
        free(msg->image);
    }
    free(messageTexts);
    messageTexts = NULL;
}

static void MessageServiceGetList(Message* msgs, int count)
{
    MessageServiceClear();

    messageTexts = msgs;
    messageTextCount = count;
    ituSceneExecuteCommand(&theScene, 1, MessageServiceReload, 0);
}

static void MessageServiceDeleteComplete(void)
{
	MessageTextListGet(MessageServiceGetList);
}

bool MessageServiceDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* topicItem = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)messageServiceTopicScrollListBox);

    if (topicItem)
	{
        Message* msg = (Message*)ituWidgetGetCustomData(topicItem);
        MessageTextDelete(msg->id, MessageServiceDeleteComplete);
    }
	return true;
}

bool MessageServiceOnEnter(ITUWidget* widget, char* param)
{
    if (!messageServiceTopicScrollListBox)
    {
        messageServiceTopicScrollListBox = ituSceneFindWidget(&theScene, "messageServiceTopicScrollListBox");
        assert(messageServiceTopicScrollListBox);

        messageServiceTimeScrollListBox = ituSceneFindWidget(&theScene, "messageServiceTimeScrollListBox");
        assert(messageServiceTimeScrollListBox);

        messageServiceViewLayer = ituSceneFindWidget(&theScene, "messageServiceViewLayer");
        assert(messageServiceViewLayer);
	}
    MessageTextListGet(MessageServiceGetList);
	return true;
}

bool MessageServiceOnLeave(ITUWidget* widget, char* param)
{
    MessageServiceClear();
	return true;
}

void MessageServiceReset(void)
{
    messageServiceTopicScrollListBox = NULL;
}
