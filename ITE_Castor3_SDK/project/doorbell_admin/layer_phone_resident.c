#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

ITUBackground* phoneResidentRoomBackground;
ITUTextBox* phoneAreaTextBox;
ITUTextBox* phoneBuildingTextBox;
ITUTextBox* phoneUnitTextBox;
ITUTextBox* phoneFloorTextBox;
ITUTextBox* phoneRoomTextBox;
ITUBackground* phoneResidentAliasBackground;
ITUText* phoneAliasText;

// status
ITUTextBox* activePhoneDialTextBox;
static phoneLogCount;

bool PhoneBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituWidgetSetVisible(phoneResidentRoomBackground, true);
    ituWidgetSetVisible(phoneResidentAliasBackground, false);

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
	return true;
}

bool PhoneLogIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = phoneLogCount;
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
            CallLogEntry* log;
            char buf[8];
            scrolltext = (ITUScrollText*) node;
    
            log = CallLogGetEntry(i);

            sprintf(buf, "%d", log->type - 1);
            ituTextSetString(scrolltext, buf);

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

bool PhoneLogListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = phoneLogCount;
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
            struct tm tm;
            char buf[32];
            CallLogEntry* log;
            scrolltext = (ITUScrollText*) node;
    
            log = CallLogGetEntry(i);

            strptime(log->time, "%c", &tm);
            strftime(buf, sizeof(buf),"%Y-%m-%d %H:%M", &tm);
            ituTextSetString(scrolltext, buf);
            ituWidgetSetCustomData(scrolltext, log);
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

bool PhoneLogListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUText* item;
    ITUListBox* listbox = (ITUListBox*)widget;

    assert(listbox);
    item = (ITUText*) ituListBoxGetFocusItem(listbox);
    if (item)
    {
        CallLogEntry* log = (CallLogEntry*)ituWidgetGetCustomData(item);
        if (log)
        {
            DeviceInfo info;
                
            AddressBookGetDeviceInfo(&info, log->ip);

            ituTextBoxSetString(phoneAreaTextBox, info.area);
            ituTextBoxSetString(phoneBuildingTextBox, info.building);
            ituTextBoxSetString(phoneUnitTextBox, info.unit);
            ituTextBoxSetString(phoneFloorTextBox, info.floor);
            ituTextBoxSetString(phoneRoomTextBox, info.room);
            activePhoneDialTextBox = phoneRoomTextBox;

            ituTextSetString(phoneAliasText, info.alias);

            if (info.alias[0] == '\0')
            {
                ituWidgetSetVisible(phoneResidentRoomBackground, true);
                ituWidgetSetVisible(phoneResidentAliasBackground, false);
            }
            else
            {
                ituWidgetSetVisible(phoneResidentRoomBackground, false);
                ituWidgetSetVisible(phoneResidentAliasBackground, true);
            }
        }
    }
    return true;
}

bool PhoneResidentOnEnter(ITUWidget* widget, char* param)
{
	if (!phoneAreaTextBox)
    {
        phoneResidentRoomBackground = ituSceneFindWidget(&theScene, "phoneResidentRoomBackground");
        assert(phoneResidentRoomBackground);

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

        phoneResidentAliasBackground = ituSceneFindWidget(&theScene, "phoneResidentAliasBackground");
        assert(phoneResidentAliasBackground);

        phoneAliasText = ituSceneFindWidget(&theScene, "phoneAliasText");
        assert(phoneAliasText);
    }
	// status
    activePhoneDialTextBox = phoneAreaTextBox;
    ituTextBoxSetString(phoneAreaTextBox, NULL);
    ituTextBoxSetString(phoneBuildingTextBox, NULL);
    ituTextBoxSetString(phoneUnitTextBox, NULL);
    ituTextBoxSetString(phoneFloorTextBox, NULL);
    ituTextBoxSetString(phoneRoomTextBox, NULL);

    phoneLogCount = CallLogGetCount();

    SceneClearMissedCalls();

	return true;
}

bool PhoneResidentOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void PhoneResidentReset(void)
{
    phoneAreaTextBox = NULL;
}

