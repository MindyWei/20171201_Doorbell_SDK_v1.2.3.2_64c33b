#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

ITUTextBox* redirectAreaTextBox;
ITUTextBox* redirectBuildingTextBox;
ITUTextBox* redirectUnitTextBox;
ITUTextBox* redirectFloorTextBox;
ITUTextBox* redirectRoomTextBox;

// status
ITUTextBox* activeRedirectDialTextBox;
static redirectLogCount;

bool RedirectBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activeRedirectDialTextBox, NULL);

    if (activeRedirectDialTextBox == redirectRoomTextBox)
    {
        activeRedirectDialTextBox = redirectFloorTextBox;
    }
    else if (activeRedirectDialTextBox == redirectFloorTextBox)
    {
        activeRedirectDialTextBox = redirectUnitTextBox;
    }
    else if (activeRedirectDialTextBox == redirectUnitTextBox)
    {
        activeRedirectDialTextBox = redirectBuildingTextBox;
    }
    else if (activeRedirectDialTextBox == redirectBuildingTextBox)
    {
        activeRedirectDialTextBox = redirectAreaTextBox;
    }
	return true;
}

bool RedirectLogIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = redirectLogCount;
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

bool RedirectLogListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = redirectLogCount;
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

bool RedirectLogListBoxOnSelection(ITUWidget* widget, char* param)
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

            ituTextBoxSetString(redirectAreaTextBox, info.area);
            ituTextBoxSetString(redirectBuildingTextBox, info.building);
            ituTextBoxSetString(redirectUnitTextBox, info.unit);
            ituTextBoxSetString(redirectFloorTextBox, info.floor);
            ituTextBoxSetString(redirectRoomTextBox, info.room);
            activeRedirectDialTextBox = redirectRoomTextBox;
        }
    }
    return true;
}

bool RedirectResidentOnEnter(ITUWidget* widget, char* param)
{
	if (!redirectAreaTextBox)
    {
        redirectAreaTextBox = ituSceneFindWidget(&theScene, "redirectAreaTextBox");
        assert(redirectAreaTextBox);

        redirectBuildingTextBox = ituSceneFindWidget(&theScene, "redirectBuildingTextBox");
        assert(redirectBuildingTextBox);

        redirectUnitTextBox = ituSceneFindWidget(&theScene, "redirectUnitTextBox");
        assert(redirectUnitTextBox);

        redirectFloorTextBox = ituSceneFindWidget(&theScene, "redirectFloorTextBox");
        assert(redirectFloorTextBox);

        redirectRoomTextBox = ituSceneFindWidget(&theScene, "redirectRoomTextBox");
        assert(redirectRoomTextBox);
    }
	// status
    activeRedirectDialTextBox = redirectAreaTextBox;
    ituTextBoxSetString(redirectAreaTextBox, NULL);
    ituTextBoxSetString(redirectBuildingTextBox, NULL);
    ituTextBoxSetString(redirectUnitTextBox, NULL);
    ituTextBoxSetString(redirectFloorTextBox, NULL);
    ituTextBoxSetString(redirectRoomTextBox, NULL);

    redirectLogCount = CallLogGetCount();

	return true;
}

bool RedirectResidentOnLeave(ITUWidget* widget, char* param)
{
    if (strcmp(param, "redirectAdminLayer") && strcmp(param, "redirectMiscLayer"))
    {
        ituSceneExecuteCommand(&theScene, 10, RedirectHangUp, 0);
    }
	return true;
}

void RedirectResidentReset(void)
{
    redirectAreaTextBox = NULL;
}

