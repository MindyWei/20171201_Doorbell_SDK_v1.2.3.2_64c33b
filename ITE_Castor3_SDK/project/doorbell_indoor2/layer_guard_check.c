#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static int guardCheckSensorCount;
static char* guardSensors;

bool GuardCheckScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, k, count;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = guardCheckSensorCount ? (guardCheckSensorCount + count - 1) / count : 1;

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

    k = 0;
    for (; j < guardCheckSensorCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        buf[0] = '\0';
        for (; k < GUARD_SENSOR_COUNT; k++)
        {
            if (guardSensors && guardSensors[k] == '1')
            {
                sprintf(buf, "%d", k++);
                break;
            }
        }
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

bool GuardCheckSensorScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, k, count;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = guardCheckSensorCount ? (guardCheckSensorCount + count - 1) / count : 1;

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

    k = 0;
    for (; j < guardCheckSensorCount; j++)
    {
        const char* ptr = NULL;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        for (; k < GUARD_SENSOR_COUNT; k++)
        {
            if (guardSensors && guardSensors[k] == '1')
            {
                ptr = StringGetGuardSensor(k++);
                break;
            }
        }

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

bool GuardCheckStatusScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, k, count;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = guardCheckSensorCount ? (guardCheckSensorCount + count - 1) / count : 1;

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

    k = 0;
    for (; j < guardCheckSensorCount; j++)
    {
        const char* ptr = "";
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        for (; k < GUARD_SENSOR_COUNT; k++)
        {
            if (guardSensors && guardSensors[k] == '1')
            {
                GuardStatus status = GuardGetSensorStatus(k++);
                ptr = StringGetGuardSensorStatus(status == GUARD_ALARM ? true : false);
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, status == GUARD_DISABLE || status == GUARD_ALARM ? true : false);
                break;
            }
        }
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

bool GuardCheckOnEnter(ITUWidget* widget, char* param)
{
    switch (theConfig.guard_mode)
    {
    case GUARD_INDOOR:
        guardSensors = theConfig.guard_indoor_sensors;
        break;

    case GUARD_OUTDOOR:
        guardSensors = theConfig.guard_outdoor_sensors;
        break;

    case GUARD_SLEEP:
        guardSensors = theConfig.guard_sleep_sensors;
        break;

    default:
        guardSensors = NULL;
    }

    guardCheckSensorCount = 0;

    if (guardSensors)
    {
        int i;

        for (i = 0; i < GUARD_SENSOR_COUNT; i++)
        {
            if (guardSensors[i] == '1')
                guardCheckSensorCount++;
        }
    }
	return true;
}

void GuardCheckReset(void)
{
}
