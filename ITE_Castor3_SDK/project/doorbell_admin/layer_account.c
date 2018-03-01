#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define MAX_ACCOUNT_COUNT 999

static ITUBackground* accountBackground;
static ITUListBox* accountListBox;
static ITUBackground* accountDialogBackground;

static int accountCount;
static char accountArray[MAX_ACCOUNT_COUNT][64];

bool AccountListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    accountCount = UserListGetUserIDArray(accountArray, MAX_ACCOUNT_COUNT);
    entryCount = accountCount;
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
            scrolltext = (ITUScrollText*) node;

            ituTextSetString(scrolltext, accountArray[i]);

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

bool AccountListBoxOnSelection(ITUWidget* widget, char* param)
{
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    //theCenterInfo.ip[0] = '\0'; // TEST ONLY
    if (theCenterInfo.ip[0] == '\0')
    {
        ituWidgetSetVisible(accountDialogBackground, true);
        ituWidgetDisable(accountBackground);
        return true;
    }
    else
    {
        ituListBoxSelect(listbox, -1);
        return false;
    }
}

bool AccountDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    ITUText* item = (ITUText*)ituListBoxGetFocusItem(accountListBox);
    if (item)
    {
        UserListDeleteUser(ituTextGetString(item));
    }
    return true;
}

static bool AccountBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        int x = arg2 - widget->rect.x;
        int y = arg3 - widget->rect.y;

        if (ituWidgetIsInside(widget, x, y))
        {
            int xx, yy, w, h;

            x = arg2;
            y = arg3;

            if (ituWidgetIsVisible(accountDialogBackground))
            {
                ituWidgetGetGlobalPosition(accountDialogBackground, &xx, &yy);
                w = ituWidgetGetWidth(accountDialogBackground);
                h = ituWidgetGetHeight(accountDialogBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituWidgetSetVisible(accountDialogBackground, false);
                    ituWidgetEnable(widget);
                    return widget->dirty;
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool AccountOnEnter(ITUWidget* widget, char* param)
{
    if (!accountBackground)
    {
        accountBackground = ituSceneFindWidget(&theScene, "accountBackground");
        assert(accountBackground);
        ituWidgetSetUpdate(accountBackground, AccountBackgroundUpdate);

        accountListBox = ituSceneFindWidget(&theScene, "accountListBox");
        assert(accountListBox);

        accountDialogBackground = ituSceneFindWidget(&theScene, "accountDialogBackground");
        assert(accountDialogBackground);
    }
    return true;
}

void AccountReset(void)
{
    accountBackground = NULL;
}
