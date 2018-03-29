#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

ITUBackgroundButton* phoneResidentRoomBackgroundButton;
ITUTextBox* phoneAreaTextBox;
ITUTextBox* phoneBuildingTextBox;
ITUTextBox* phoneUnitTextBox;
ITUTextBox* phoneFloorTextBox;
ITUTextBox* phoneRoomTextBox;

ITUBackground* phoneResidentIPBackground;
ITUTextBox* phoneIPTextBox;

static ITUScrollIconListBox* phoneLogScrollIconListBox;
static ITUScrollListBox* phoneLogScrollListBox;
static ITUButton* phoneLogBlackButton;
static ITUButton* phoneLogWhiteButton;
static ITUText* phoneLogBlackText;
static ITUText* phoneLogWhiteText;

static ITULayer* phoneLayer;
static ITUBackground* phoneResidentBackground;
static ITUBackgroundButton* phoneLogDialogBackgroundButton;

// status
ITUTextBox* activePhoneDialTextBox;
static DeviceInfo phoneLogDeviceInfo;

bool PhoneBackButtonOnPress(ITUWidget* widget, char* param)
{
    if (activePhoneDialTextBox == phoneIPTextBox)
    {
        char* str;
        int count;

        ituTextBoxBack(activePhoneDialTextBox);

        str = ituTextGetString(activePhoneDialTextBox);
        count = str ? strlen(str) : 0;

        if (count == 0)
        {
            ituWidgetSetVisible(phoneResidentRoomBackgroundButton, true);
            ituWidgetSetVisible(phoneResidentIPBackground, false);
            activePhoneDialTextBox = phoneAreaTextBox;
        }
    }
    else
    {
        ituTextSetString(activePhoneDialTextBox, NULL);

        if (activePhoneDialTextBox == phoneRoomTextBox)
        {
            activePhoneDialTextBox = phoneFloorTextBox;
        }
        else if (activePhoneDialTextBox == phoneFloorTextBox)
        {
            activePhoneDialTextBox = phoneUnitTextBox;
        }
        else if (activePhoneDialTextBox == phoneUnitTextBox)
        {
            activePhoneDialTextBox = phoneBuildingTextBox;
        }
        else if (activePhoneDialTextBox == phoneBuildingTextBox)
        {
            activePhoneDialTextBox = phoneAreaTextBox;
        }
    }
	return true;
}

bool PhoneLogScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;

    entryCount = CallLogGetCount();
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
        CallLogEntry* log;
        char buf[8];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        log = CallLogGetEntry(j);
        sprintf(buf, "%d", log->type - 1);
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

bool PhoneLogScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = CallLogGetCount();
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
        struct tm tm;
        char buf[32];
        CallLogEntry* log;
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        log = CallLogGetEntry(i);

        strptime(log->time, "%c", &tm);
        strftime(buf, sizeof(buf),"%Y-%m-%d %H:%M", &tm);
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

bool PhoneLogScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUText* item;
    ITUListBox* listbox = (ITUListBox*)widget;

    assert(listbox);
    item = (ITUText*) ituListBoxGetFocusItem(listbox);
    if (item)
    {
        int i = (int)ituWidgetGetCustomData(item);
        CallLogEntry* log = CallLogGetEntry(i);
        if (log)
        {
            ituWidgetSetVisible(phoneResidentRoomBackgroundButton, true);
            ituWidgetSetVisible(phoneResidentIPBackground, false);
            ituTextSetString(phoneIPTextBox, NULL);

            AddressBookGetDeviceInfo(&phoneLogDeviceInfo, log->ip);

            ituTextBoxSetString(phoneAreaTextBox, phoneLogDeviceInfo.area);
            ituTextBoxSetString(phoneBuildingTextBox, phoneLogDeviceInfo.building);
            ituTextBoxSetString(phoneUnitTextBox, phoneLogDeviceInfo.unit);
            ituTextBoxSetString(phoneFloorTextBox, phoneLogDeviceInfo.floor);
            ituTextBoxSetString(phoneRoomTextBox, phoneLogDeviceInfo.room);
            activePhoneDialTextBox = phoneRoomTextBox;
        }
    }
    return true;
}

bool PhoneLogAddButtonOnPress(ITUWidget* widget, char* param)
{
    char* ip;

    if ((phoneLogDeviceInfo.ext[0] == '\0') || (phoneLogDeviceInfo.type == DEVICE_INDOOR || phoneLogDeviceInfo.type == DEVICE_INDOOR2))
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, "01");
    else
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, phoneLogDeviceInfo.ext);

    if (ip)
    {
        free(ip);
        LinphoneAddFriend(&phoneLogDeviceInfo, false, NULL);
    }
    return true;
}

bool PhoneLogBlackButtonOnPress(ITUWidget* widget, char* param)
{
    char* ip;

    if ((phoneLogDeviceInfo.ext[0] == '\0') || (phoneLogDeviceInfo.type == DEVICE_INDOOR || phoneLogDeviceInfo.type == DEVICE_INDOOR2))
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, "01");
    else
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, phoneLogDeviceInfo.ext);

    if (ip)
    {
        free(ip);
        LinphoneAddFriend(&phoneLogDeviceInfo, true, NULL);
    }
    return true;
}

bool PhoneLogWhiteButtonOnPress(ITUWidget* widget, char* param)
{
    char* ip;

    if ((phoneLogDeviceInfo.ext[0] == '\0') || (phoneLogDeviceInfo.type == DEVICE_INDOOR || phoneLogDeviceInfo.type == DEVICE_INDOOR2))
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, "01");
    else
        ip = AddressBookGetDeviceIP(phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, phoneLogDeviceInfo.ext);

    if (ip)
    {
        free(ip);
        LinphoneAddFriend(&phoneLogDeviceInfo, false, NULL);
    }
    return true;
}

static void PhoneLogReload(int arg)
{
    ituListBoxReload((ITUListBox*)phoneLogScrollIconListBox);
    ituListBoxReload((ITUListBox*)phoneLogScrollListBox);
}

bool PhoneLogDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUScrollText* item = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)phoneLogScrollListBox);
    if (item)
    {
        int i = (int)ituWidgetGetCustomData(item);
        CallLogDeleteEntry(i);
        ituSceneExecuteCommand(&theScene, 6, PhoneLogReload, 0);
    }
    return true;
}

bool PhoneLogClearButtonOnPress(ITUWidget* widget, char* param)
{
    CallLogClearEntries();
    ituSceneExecuteCommand(&theScene, 6, PhoneLogReload, 0);
    return true;
}

bool PhoneResidentRoomBackgroundButtonOnMouseLongPress(ITUWidget* widget, char* param)
{
    char* area = ituTextGetString(phoneAreaTextBox);
    char* building = ituTextGetString(phoneBuildingTextBox);
    char* unit = ituTextGetString(phoneUnitTextBox);
    char* floor = ituTextGetString(phoneFloorTextBox);
    char* room = ituTextGetString(phoneRoomTextBox);

    if (area[0] && building[0] && unit[0] && floor[0] && room[0])
    {
        char buf[32];
        int i;

        phoneLogDeviceInfo.type = DEVICE_INDOOR;
        strcpy(phoneLogDeviceInfo.area, area);
        strcpy(phoneLogDeviceInfo.building, building);
        strcpy(phoneLogDeviceInfo.unit, unit);
        strcpy(phoneLogDeviceInfo.floor, floor);
        strcpy(phoneLogDeviceInfo.room, room);

        ituWidgetDisable(phoneLayer);
        ituWidgetDisable(phoneResidentBackground);
        ituWidgetSetVisible(phoneLogDialogBackgroundButton, true);

        sprintf(buf, "%s-%s-%s-%s-%s", area, building, unit, floor, room);

        for (i = 0; i < linphoneCastor3.friend_count; ++i)
        {
            LinphoneCastor3Friend* fr = &linphoneCastor3.friends[i];

            if (strcmp(fr->refkey, buf) == 0)
            {
                if (fr->blacklist)
                {
                    ituWidgetSetVisible(phoneLogBlackButton, false);
                    ituWidgetSetVisible(phoneLogWhiteButton, true);
                    ituWidgetSetVisible(phoneLogBlackText, false);
                    ituWidgetSetVisible(phoneLogWhiteText, true);
                }
                else
                {
                    ituWidgetSetVisible(phoneLogBlackButton, true);
                    ituWidgetSetVisible(phoneLogWhiteButton, false);
                    ituWidgetSetVisible(phoneLogBlackText, true);
                    ituWidgetSetVisible(phoneLogWhiteText, false);
                }
                return true;
            }
        }
        ituWidgetSetVisible(phoneLogBlackButton, true);
        ituWidgetSetVisible(phoneLogWhiteButton, false);
        ituWidgetSetVisible(phoneLogBlackText, true);
        ituWidgetSetVisible(phoneLogWhiteText, false);
    }
    return true;
}

bool PhoneLogScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param)
{
    char buf[32];
    int i;

    if ((phoneLogDeviceInfo.ext[0] == '\0') || (phoneLogDeviceInfo.type == DEVICE_INDOOR || phoneLogDeviceInfo.type == DEVICE_INDOOR2))
        sprintf(buf, "%s-%s-%s-%s-%s", phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room);
    else
        sprintf(buf, "%s-%s-%s-%s-%s-%s", phoneLogDeviceInfo.area, phoneLogDeviceInfo.building, phoneLogDeviceInfo.unit, phoneLogDeviceInfo.floor, phoneLogDeviceInfo.room, phoneLogDeviceInfo.ext);

    for (i = 0; i < linphoneCastor3.friend_count; ++i)
    {
        LinphoneCastor3Friend* fr = &linphoneCastor3.friends[i];

        if (strcmp(fr->refkey, buf) == 0)
        {
            if (fr->blacklist)
            {
                ituWidgetSetVisible(phoneLogBlackButton, false);
                ituWidgetSetVisible(phoneLogWhiteButton, true);
                ituWidgetSetVisible(phoneLogBlackText, false);
                ituWidgetSetVisible(phoneLogWhiteText, true);
            }
            else
            {
                ituWidgetSetVisible(phoneLogBlackButton, true);
                ituWidgetSetVisible(phoneLogWhiteButton, false);
                ituWidgetSetVisible(phoneLogBlackText, true);
                ituWidgetSetVisible(phoneLogWhiteText, false);
            }
            return true;
        }
    }
    ituWidgetSetVisible(phoneLogBlackButton, true);
    ituWidgetSetVisible(phoneLogWhiteButton, false);
    ituWidgetSetVisible(phoneLogBlackText, true);
    ituWidgetSetVisible(phoneLogWhiteText, false);

    return true;
}

bool PhoneResidentOnEnter(ITUWidget* widget, char* param)
{
	if (!phoneResidentRoomBackgroundButton)
    {
        phoneResidentRoomBackgroundButton = ituSceneFindWidget(&theScene, "phoneResidentRoomBackgroundButton");
        assert(phoneResidentRoomBackgroundButton);

        phoneAreaTextBox = ituSceneFindWidget(&theScene, "phoneAreaTextBox");
        assert(phoneAreaTextBox);

        phoneBuildingTextBox = ituSceneFindWidget(&theScene, "phoneBuildingTextBox");
        assert(phoneBuildingTextBox);

        phoneUnitTextBox = ituSceneFindWidget(&theScene, "phoneUnitTextBox");
        assert(phoneUnitTextBox);

        phoneFloorTextBox = ituSceneFindWidget(&theScene, "phoneFloorTextBox");
        assert(phoneFloorTextBox);

        phoneRoomTextBox = ituSceneFindWidget(&theScene, "phoneRoomTextBox");
        assert(phoneRoomTextBox);

        phoneResidentIPBackground = ituSceneFindWidget(&theScene, "phoneResidentIPBackground");
        assert(phoneResidentIPBackground);

        phoneIPTextBox = ituSceneFindWidget(&theScene, "phoneIPTextBox");
        assert(phoneIPTextBox);

        phoneLogScrollIconListBox = ituSceneFindWidget(&theScene, "phoneLogScrollIconListBox");
        assert(phoneLogScrollIconListBox);

        phoneLogScrollListBox = ituSceneFindWidget(&theScene, "phoneLogScrollListBox");
        assert(phoneLogScrollListBox);

        phoneLogBlackButton = ituSceneFindWidget(&theScene, "phoneLogBlackButton");
        assert(phoneLogBlackButton);

        phoneLogWhiteButton = ituSceneFindWidget(&theScene, "phoneLogWhiteButton");
        assert(phoneLogWhiteButton);

        phoneLogBlackText = ituSceneFindWidget(&theScene, "phoneLogBlackText");
        assert(phoneLogBlackText);

        phoneLogWhiteText = ituSceneFindWidget(&theScene, "phoneLogWhiteText");
        assert(phoneLogWhiteText);

        phoneLayer = ituSceneFindWidget(&theScene, "phoneLayer");
        assert(phoneLayer);

        phoneResidentBackground = ituSceneFindWidget(&theScene, "phoneResidentBackground");
        assert(phoneResidentBackground);

        phoneLogDialogBackgroundButton = ituSceneFindWidget(&theScene, "phoneLogDialogBackgroundButton");
        assert(phoneLogDialogBackgroundButton);
    }
	// status
    activePhoneDialTextBox = phoneAreaTextBox;
    ituTextBoxSetString(phoneAreaTextBox, NULL);
    ituTextBoxSetString(phoneBuildingTextBox, NULL);
    ituTextBoxSetString(phoneUnitTextBox, NULL);
    ituTextBoxSetString(phoneFloorTextBox, NULL);
    ituTextBoxSetString(phoneRoomTextBox, NULL);
    ituTextBoxSetString(phoneIPTextBox, NULL);

    SceneClearMissedCalls();

	return true;
}

bool PhoneResidentOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void PhoneResidentReset(void)
{
    phoneResidentRoomBackgroundButton = NULL;
}

