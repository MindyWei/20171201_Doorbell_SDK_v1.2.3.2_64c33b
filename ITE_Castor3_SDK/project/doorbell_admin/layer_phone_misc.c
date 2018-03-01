#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static DeviceInfo phoneCallLobbyInfoArray[MAX_MISC_COUNT];
static int phoneCallLobbyInfoCount;

bool PhoneMiscListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = phoneCallLobbyInfoCount;
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
            DeviceInfo* info;
            scrolltext = (ITUScrollText*) node;
    
            info = &phoneCallLobbyInfoArray[i];

            ituTextSetString(scrolltext, info->alias);
            ituWidgetSetCustomData(scrolltext, info);
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

bool PhoneMiscOnEnter(ITUWidget* widget, char* param)
{
    if (phoneCallLobbyInfoCount == 0)
        phoneCallLobbyInfoCount = AddressBookGetMiscInfoArray(phoneCallLobbyInfoArray, MAX_MISC_COUNT);

    return true;
}

void PhoneMiscReset(void)
{
}

