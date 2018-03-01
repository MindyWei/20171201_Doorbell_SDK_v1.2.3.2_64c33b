#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUWidget* guardLogContainer;
static ITUScrollIconListBox* guardLogScrollIconListBox;
static ITUScrollListBox* guardLogSensorScrollListBox;
static ITUScrollListBox* guardLogTypeScrollListBox;
static ITUScrollListBox* guardLogTimeScrollListBox;

bool GuardLogScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GuardGetLogCount();
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
        char buf[4];
        GuardLog* log;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        buf[0] = '\0';

        log = GuardGetLog(j);
        sprintf(buf, "%d", log->sensor);
        ituTextSetString(scrolltext, buf);

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

bool GuardLogSensorScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GuardGetLogCount();
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
        const char* ptr;
        GuardLog* log;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        log = GuardGetLog(j);
        if (log->state == GUARD_HANDLED)
            ptr = StringGetGuardType(log->type);
        else
            ptr = StringGetGuardSensor(log->sensor);

        ituTextSetString(scrolltext, ptr);

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

bool GuardLogTypeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GuardGetLogCount();
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
        const char* ptr;
        GuardLog* log;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        log = GuardGetLog(j);
        if (log->state == GUARD_HANDLED)
            ptr = StringGetGuardState(log->state);
        else
            ptr = StringGetGuardStatus(log->status);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, log->status == GUARD_DISABLE || log->status == GUARD_ALARM ? true : false);

        ituTextSetString(scrolltext, ptr);

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

bool GuardLogTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GuardGetLogCount();
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
        time_t rawtime;
        struct tm* timeinfo;
        char buf[80];

        GuardLog* log;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        log = GuardGetLog(j);

        rawtime = log->timestamp;
        timeinfo = localtime(&rawtime);
        strftime(buf, sizeof(buf),"%Y.%m.%d %H:%M:%S", timeinfo);

        ituTextSetString(scrolltext, buf);

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

static void GuardLogReload(int arg)
{
    ituListBoxReload((ITUListBox*)guardLogScrollIconListBox);
    ituListBoxReload((ITUListBox*)guardLogSensorScrollListBox);
    ituListBoxReload((ITUListBox*)guardLogTypeScrollListBox);
    ituListBoxReload((ITUListBox*)guardLogTimeScrollListBox);
}

bool GuardLogClearButtonOnPress(ITUWidget* widget, char* param)
{
    GuardClearLog();
    ituSceneExecuteCommand(&theScene, 6, GuardLogReload, 0);
    return true;
}

bool GuardLogOnEnter(ITUWidget* widget, char* param)
{
    if (!guardLogContainer)
    {
        guardLogContainer = ituSceneFindWidget(&theScene, "guardLogContainer");
        assert(guardLogContainer);

        guardLogScrollIconListBox = ituSceneFindWidget(&theScene, "guardLogScrollIconListBox");
        assert(guardLogScrollIconListBox);

        guardLogSensorScrollListBox = ituSceneFindWidget(&theScene, "guardLogSensorScrollListBox");
        assert(guardLogSensorScrollListBox);

        guardLogTypeScrollListBox = ituSceneFindWidget(&theScene, "guardLogTypeScrollListBox");
        assert(guardLogTypeScrollListBox);

        guardLogTimeScrollListBox = ituSceneFindWidget(&theScene, "guardLogTimeScrollListBox");
        assert(guardLogTimeScrollListBox);
	}
    return true;
}

void GuardLogReset(void)
{
    guardLogContainer = NULL;
}
