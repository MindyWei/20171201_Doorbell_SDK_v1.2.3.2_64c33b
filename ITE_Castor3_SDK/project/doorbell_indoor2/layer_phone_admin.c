#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static DeviceInfo phoneCallManagerInfoArray[MAX_MANAGER_COUNT];
static int phoneCallManagerInfoCount;

bool PhoneAdminScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    DeviceInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = phoneCallManagerInfoCount ? (phoneCallManagerInfoCount + count - 1) / count : 1;

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

    for (; j < phoneCallManagerInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &phoneCallManagerInfoArray[j];
        ituTextSetString(scrolltext, info->alias);
        ituWidgetSetCustomData(scrolltext, info);

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

bool PhoneAdminOnEnter(ITUWidget* widget, char* param)
{
    if (phoneCallManagerInfoCount == 0)
        phoneCallManagerInfoCount = AddressBookGetManagerInfoArray(phoneCallManagerInfoArray, MAX_MANAGER_COUNT);

    return true;
}

void PhoneAdminReset(void)
{
}

