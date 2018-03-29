#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* guardUserSettingDetailSprite;
static ITUText* guardUserSettingDetailText;

static guardUserSettingChanged;

bool GuardUserSettingDetailScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GUARD_SENSOR_COUNT;
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
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        buf[0] = '\0';

        sprintf(buf, "%d", j);
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

bool GuardUserSettingDetailSensorScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = GUARD_SENSOR_COUNT;
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
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        ptr = StringGetGuardSensor(j);

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

bool GuardUserSettingDetailStatusScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = GUARD_SENSOR_COUNT ? (GUARD_SENSOR_COUNT + count - 1) / count : 1;

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

    for (; j < GUARD_SENSOR_COUNT; j++)
    {
        char buf[32];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        if (guardUserSettingDetailSprite->frame == 0)
        {
            if (theConfig.guard_indoor_sensors[j] == '1')
            {
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
            else
            {
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
        }
        else if (guardUserSettingDetailSprite->frame == 1)
        {
            if (theConfig.guard_outdoor_sensors[j] == '1')
            {
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
            else
            {
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
        }
        else
        {
            if (theConfig.guard_sleep_sensors[j] == '1')
            {
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
            else
            {
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
        }
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

bool GuardUserSettingDetailStatusScrollListBoxOnSelect(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);

    if (scrolltext)
    {
        int i = (int)ituWidgetGetCustomData(scrolltext);
        char buf[32];

        if (guardUserSettingDetailSprite->frame == 0)
        {
            if (theConfig.guard_indoor_sensors[i] == '1')
            {
                theConfig.guard_indoor_sensors[i] = '0';
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
            else
            {
                theConfig.guard_indoor_sensors[i] = '1';
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
        }
        else if (guardUserSettingDetailSprite->frame == 1)
        {
            if (theConfig.guard_outdoor_sensors[i] == '1')
            {
                theConfig.guard_outdoor_sensors[i] = '0';
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
            else
            {
                theConfig.guard_outdoor_sensors[i] = '1';
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
        }
        else
        {
            if (theConfig.guard_sleep_sensors[i] == '1')
            {
                theConfig.guard_sleep_sensors[i] = '0';
                strcpy(buf, StringGetGuardSensorState(false));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, true);
            }
            else
            {
                theConfig.guard_sleep_sensors[i] = '1';
                strcpy(buf, StringGetGuardSensorState(true));
                ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, false);
            }
        }
        ituTextSetString(scrolltext, buf);
        guardUserSettingChanged = true;
    }
    return true;
}

bool GuardUserSettingDetailOnEnter(ITUWidget* widget, char* param)
{
    if (!guardUserSettingDetailSprite)
    {
        guardUserSettingDetailSprite = ituSceneFindWidget(&theScene, "guardUserSettingDetailSprite");
        assert(guardUserSettingDetailSprite);

        guardUserSettingDetailText = ituSceneFindWidget(&theScene, "guardUserSettingDetailText");
        assert(guardUserSettingDetailText);
    }

    switch (guardUserSettingDetailSprite->frame)
    {
    case 0:
        ituTextSetString(guardUserSettingDetailText, StringGetGuardMode(GUARD_INDOOR));
        break;

    case 1:
        ituTextSetString(guardUserSettingDetailText, StringGetGuardMode(GUARD_OUTDOOR));
        break;

    case 2:
        ituTextSetString(guardUserSettingDetailText, StringGetGuardMode(GUARD_SLEEP));
        break;
    }

    guardUserSettingChanged = false;
    return true;
}

bool GuardUserSettingDetailOnLeave(ITUWidget* widget, char* param)
{
    if (guardUserSettingChanged)
    {
        GuardSetMode(theConfig.guard_mode);
        ConfigSave();
    }
    return true;
}

void GuardUserSettingDetailReset(void)
{
    guardUserSettingDetailSprite = NULL;
}
