#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

static ITUBackground* phoneContactBackground;
static ITUScrollListBox* phoneContactNameScrollListBox;
static ITUScrollListBox* phoneContactCommentScrollListBox;
static ITUTextBox* phoneContactKeyboardCommentTextBox;
static ITUButton* phoneContactBlackButton;
static ITUButton* phoneContactWhiteButton;
static ITUText* phoneContactBlackText;
static ITUText* phoneContactWhiteText;
static ITUScrollText* phoneContactKeyboardCommentScrollText;

static ITUBackground* phoneContactAddBackground;
static ITUTextBox* phoneContactAddAreaTextBox;
static ITUTextBox* phoneContactAddBuildingTextBox;
static ITUTextBox* phoneContactAddUnitTextBox;
static ITUTextBox* phoneContactAddFloorTextBox;
static ITUTextBox* phoneContactAddRoomTextBox;
static ITUBackground* phoneKeypadBackground;
static ITUBackgroundButton* phoneContactErrorBackgroundButton;

static DeviceInfo phoneContactDeviceInfo;
static char phoneContactComment[256];

// status
ITUTextBox* activePhoneContactAddTextBox;

bool PhoneContactNameScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = linphoneCastor3.friend_count;
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
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        DeviceInfo info;
        char* desc;
        LinphoneCastor3Friend* fr = &linphoneCastor3.friends[j];
        scrolltext = (ITUScrollText*) node;

        AddressBookGetDeviceInfo(&info, fr->uri + 4);
        desc = StringGetRoomAddress(info.area, info.building, info.unit, info.floor, info.room, NULL);
        ituTextSetString(scrolltext, desc);
        free(desc);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, fr->blacklist ? true : false);

        ituWidgetSetCustomData(scrolltext, fr);

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

bool PhoneContactCommentScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    entryCount = linphoneCastor3.friend_count;
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
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        LinphoneCastor3Friend* fr = &linphoneCastor3.friends[j];
        scrolltext = (ITUScrollText*) node;

        ituTextSetString(scrolltext, fr->comment ? fr->comment : "");

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, fr->blacklist ? true : false);

        ituWidgetSetCustomData(scrolltext, fr);

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

bool PhoneContactScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param)
{
    phoneContactKeyboardCommentScrollText = (ITUScrollText*)ituListBoxGetFocusItem(&phoneContactCommentScrollListBox->listbox);
    if (phoneContactKeyboardCommentScrollText)
    {
        LinphoneCastor3Friend* fr = ituWidgetGetCustomData(phoneContactKeyboardCommentScrollText);
        AddressBookGetDeviceInfo(&phoneContactDeviceInfo, fr->uri + 4);

        if (fr->blacklist)
        {
            ituWidgetSetVisible(phoneContactBlackButton, false);
            ituWidgetSetVisible(phoneContactWhiteButton, true);
            ituWidgetSetVisible(phoneContactBlackText, false);
            ituWidgetSetVisible(phoneContactWhiteText, true);
        }
        else
        {
            ituWidgetSetVisible(phoneContactBlackButton, true);
            ituWidgetSetVisible(phoneContactWhiteButton, false);
            ituWidgetSetVisible(phoneContactBlackText, true);
            ituWidgetSetVisible(phoneContactWhiteText, false);
        }
        return true;
    }
    return false;
}

bool PhoneContactAddButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardCommentScrollText)
    {
        ituTextBoxSetString(phoneContactKeyboardCommentTextBox, ituTextGetString(phoneContactKeyboardCommentScrollText));
        return true;
    }
    return false;
}

void PhoneContactReload(int arg)
{
    ituListBoxReload((ITUListBox*)phoneContactNameScrollListBox);
    ituListBoxReload((ITUListBox*)phoneContactCommentScrollListBox);
}

bool PhoneContactBlackButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardCommentScrollText)
    {
        LinphoneCastor3Friend* fr = ituWidgetGetCustomData(phoneContactKeyboardCommentScrollText);

        if (fr->comment)
            strcpy(phoneContactComment, fr->comment);
        else
            phoneContactComment[0] = '\0';

        LinphoneUpdateFriend(fr->refkey, true, phoneContactComment);
        ituSceneExecuteCommand(&theScene, 6, PhoneContactReload, 0);
        return true;
    }
    return false;
}

bool PhoneContactWhiteButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardCommentScrollText)
    {
        LinphoneCastor3Friend* fr = ituWidgetGetCustomData(phoneContactKeyboardCommentScrollText);

        if (fr->comment)
            strcpy(phoneContactComment, fr->comment);
        else
            phoneContactComment[0] = '\0';

        LinphoneUpdateFriend(fr->refkey, false, phoneContactComment);
        ituSceneExecuteCommand(&theScene, 6, PhoneContactReload, 0);
        return true;
    }
    return false;
}

bool PhoneContactDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardCommentScrollText)
    {
        LinphoneCastor3Friend* fr = ituWidgetGetCustomData(phoneContactKeyboardCommentScrollText);
        LinphoneDeleteFriend(fr->refkey);
        ituSceneExecuteCommand(&theScene, 6, PhoneContactReload, 0);
        return true;
    }
    return false;
}

bool PhoneContactClearButtonOnPress(ITUWidget* widget, char* param)
{
    LinphoneClearFriends();
    ituSceneExecuteCommand(&theScene, 6, PhoneContactReload, 0);
    return true;
}

bool PhoneContactAddBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activePhoneContactAddTextBox, NULL);

    if (activePhoneContactAddTextBox == phoneContactAddRoomTextBox)
    {
        activePhoneContactAddTextBox = phoneContactAddFloorTextBox;
    }
    else if (activePhoneContactAddTextBox == phoneContactAddFloorTextBox)
    {
        activePhoneContactAddTextBox = phoneContactAddUnitTextBox;
    }
    else if (activePhoneContactAddTextBox == phoneContactAddUnitTextBox)
    {
        activePhoneContactAddTextBox = phoneContactAddBuildingTextBox;
    }
    else if (activePhoneContactAddTextBox == phoneContactAddBuildingTextBox)
    {
        activePhoneContactAddTextBox = phoneContactAddAreaTextBox;
    }
    return true;
}

bool PhoneContactAddConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* area = ituTextGetString(phoneContactAddAreaTextBox);
    char* building = ituTextGetString(phoneContactAddBuildingTextBox);
    char* unit = ituTextGetString(phoneContactAddUnitTextBox);
    char* floor = ituTextGetString(phoneContactAddFloorTextBox);
    char* room = ituTextGetString(phoneContactAddRoomTextBox);

    if (area[0] && building[0] && unit[0] && floor[0] && room[0])
    {
        char* ip = AddressBookGetDeviceIP(area, building, unit, floor, room, "01");
        if (ip)
        {
            phoneContactDeviceInfo.type = DEVICE_INDOOR;
            strcpy(phoneContactDeviceInfo.area, area);
            strcpy(phoneContactDeviceInfo.building, building);
            strcpy(phoneContactDeviceInfo.unit, unit);
            strcpy(phoneContactDeviceInfo.floor, floor);
            strcpy(phoneContactDeviceInfo.room, room);
            strcpy(phoneContactDeviceInfo.ip, ip);

            free(ip);

            LinphoneAddFriend(&phoneContactDeviceInfo, false, NULL);
        }
        else
        {
            ituWidgetDisable(phoneContactBackground);
            ituWidgetSetVisible(phoneContactAddBackground, false);
            ituWidgetSetVisible(phoneContactErrorBackgroundButton, true);
            return false;
        }
        return true;
    }
    return false;
}

static bool PhoneContactAddBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        if (ituWidgetIsEnabled(widget))
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;

            if (!ituWidgetIsInside(widget, x, y))
            {
                int xx, yy, w, h;

                x = arg2;
                y = arg3;

                ituWidgetGetGlobalPosition(phoneKeypadBackground, &xx, &yy);
                w = ituWidgetGetWidth(phoneKeypadBackground);
                h = ituWidgetGetHeight(phoneKeypadBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituWidgetSetVisible(phoneContactAddBackground, false);
                    ituWidgetEnable(phoneContactBackground);

                    return widget->dirty;
                }

            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool PhoneContactOnEnter(ITUWidget* widget, char* param)
{
    if (!phoneContactBackground)
    {
        phoneContactBackground = ituSceneFindWidget(&theScene, "phoneContactBackground");
        assert(phoneContactBackground);    

        phoneContactNameScrollListBox = ituSceneFindWidget(&theScene, "phoneContactNameScrollListBox");
        assert(phoneContactNameScrollListBox);    

        phoneContactCommentScrollListBox = ituSceneFindWidget(&theScene, "phoneContactCommentScrollListBox");
        assert(phoneContactCommentScrollListBox);    

        phoneContactKeyboardCommentTextBox = ituSceneFindWidget(&theScene, "phoneContactKeyboardCommentTextBox");
        assert(phoneContactKeyboardCommentTextBox);    

        phoneContactBlackButton = ituSceneFindWidget(&theScene, "phoneContactBlackButton");
        assert(phoneContactBlackButton);

        phoneContactWhiteButton = ituSceneFindWidget(&theScene, "phoneContactWhiteButton");
        assert(phoneContactWhiteButton);

        phoneContactBlackText = ituSceneFindWidget(&theScene, "phoneContactBlackText");
        assert(phoneContactBlackText);

        phoneContactWhiteText = ituSceneFindWidget(&theScene, "phoneContactWhiteText");
        assert(phoneContactWhiteText);

        phoneContactAddBackground = ituSceneFindWidget(&theScene, "phoneContactAddBackground");
        assert(phoneContactAddBackground);
        ituWidgetSetUpdate(phoneContactAddBackground, PhoneContactAddBackgroundUpdate);

        phoneContactAddAreaTextBox = ituSceneFindWidget(&theScene, "phoneContactAddAreaTextBox");
        assert(phoneContactAddAreaTextBox);

        phoneContactAddBuildingTextBox = ituSceneFindWidget(&theScene, "phoneContactAddBuildingTextBox");
        assert(phoneContactAddBuildingTextBox);

        phoneContactAddUnitTextBox = ituSceneFindWidget(&theScene, "phoneContactAddUnitTextBox");
        assert(phoneContactAddUnitTextBox);

        phoneContactAddFloorTextBox = ituSceneFindWidget(&theScene, "phoneContactAddFloorTextBox");
        assert(phoneContactAddFloorTextBox);

        phoneContactAddRoomTextBox = ituSceneFindWidget(&theScene, "phoneContactAddRoomTextBox");
        assert(phoneContactAddRoomTextBox);

        phoneKeypadBackground = ituSceneFindWidget(&theScene, "phoneKeypadBackground");
        assert(phoneKeypadBackground);

        phoneContactErrorBackgroundButton = ituSceneFindWidget(&theScene, "phoneContactErrorBackgroundButton");
        assert(phoneContactErrorBackgroundButton);
    }

    if (strcmp(param, "phoneContactKeyboardLayer") == 0)
    {
        LinphoneCastor3Friend* fr = NULL;

        if (phoneContactKeyboardCommentScrollText)
        {
            fr = ituWidgetGetCustomData(phoneContactKeyboardCommentScrollText);
            ituScrollTextSetString(phoneContactKeyboardCommentScrollText, ituTextBoxGetString(phoneContactKeyboardCommentTextBox));
        }
        else
        {
            int i;
            for (i = 0; i < linphoneCastor3.friend_count; i++)
            {
                fr = &linphoneCastor3.friends[i];
                if (strcmp(fr->uri + 4, phoneContactDeviceInfo.ip) == 0)
                    break;
            }
        }
        if (fr)
            LinphoneUpdateFriend(fr->refkey, fr->blacklist ? true : false, ituTextBoxGetString(phoneContactKeyboardCommentTextBox));

        ituSceneExecuteCommand(&theScene, 6, PhoneContactReload, 0);
        ituWidgetSetVisible(phoneContactAddBackground, false);
        ituWidgetEnable(phoneContactBackground);
    }
    else
    {
        ituListBoxReload((ITUListBox*)phoneContactNameScrollListBox);
        ituListBoxReload((ITUListBox*)phoneContactCommentScrollListBox);
        phoneContactKeyboardCommentScrollText = NULL;
        ituWidgetSetVisible(phoneContactAddBackground, true);
        ituWidgetDisable(phoneContactBackground);
    }

	// status
    activePhoneContactAddTextBox = phoneContactAddAreaTextBox;
    ituTextBoxSetString(phoneContactAddAreaTextBox, NULL);
    ituTextBoxSetString(phoneContactAddBuildingTextBox, NULL);
    ituTextBoxSetString(phoneContactAddUnitTextBox, NULL);
    ituTextBoxSetString(phoneContactAddFloorTextBox, NULL);
    ituTextBoxSetString(phoneContactAddRoomTextBox, NULL);

    return true;
}

void PhoneContactReset(void)
{
    phoneContactBackground = NULL;
}

