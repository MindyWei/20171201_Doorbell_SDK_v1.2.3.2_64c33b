#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUScrollIconListBox* settingVoiceDeleteUserScrollIconListBox;
static ITUScrollListBox* settingVoiceDeleteUserScrollListBox;

static int settingVoiceDeleteUserCount;
static char settingVoiceDeleteUserArray[5][64];
static bool settingVoiceDeleteUserSelectedArray[5];

bool SettingVoiceDeleteUserScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*) widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*) listbox;
    ITCTree* node;
    int i, j, count;
    assert(listbox);

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);

    listbox->pageCount = settingVoiceDeleteUserCount ? (settingVoiceDeleteUserCount + count - 1) / count : 1;

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

    for (; j < settingVoiceDeleteUserCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        sprintf(buf, "%d", settingVoiceDeleteUserSelectedArray[j] ? 1 : 0);

        ituScrollTextSetString(scrolltext, buf);
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
        listbox->itemCount = i % count;
        if (listbox->itemCount == 0)
            listbox->itemCount = count;
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingVoiceDeleteUserScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*) widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*) listbox;
    ITCTree* node;
    int i, j, count;
    assert(listbox);

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);

    listbox->pageCount = settingVoiceDeleteUserCount ? (settingVoiceDeleteUserCount + count - 1) / count : 1;

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

    for (; j < settingVoiceDeleteUserCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        ituScrollTextSetString(scrolltext, settingVoiceDeleteUserArray[j]);

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
        listbox->itemCount = i % count;
        if (listbox->itemCount == 0)
            listbox->itemCount = count;
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingVoiceDeleteUserScrollIconListBoxOnSelect(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*) widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*) listbox;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);

    if (scrolltext)
    {
        int index = (int)ituWidgetGetCustomData(scrolltext);
        if (settingVoiceDeleteUserSelectedArray[index])
        {
            settingVoiceDeleteUserSelectedArray[index] = false;
            ituScrollTextSetString(scrolltext, "0");
        }
        else
        {
            settingVoiceDeleteUserSelectedArray[index] = true;
            ituScrollTextSetString(scrolltext, "1");
        }
    }
    return true;
}

bool SettingVoiceDeleteUserScrollListBoxOnSelect(ITUWidget* widget, char* param)
{
    return true;
}

bool SettingVoiceDeleteUserButtonOnPress(ITUWidget* widget, char* param)
{
    int i;
    bool saveCfg = false;

    for (i = 0; i < 5; i++)
    {
        if (settingVoiceDeleteUserSelectedArray[i])
        {
            settingVoiceDeleteUserArray[i][0] = '\0';
            settingVoiceDeleteUserSelectedArray[i] = false;
            saveCfg = true;
        }
    }

    if (!saveCfg)
        return false;

    theConfig.mobile_user0[0] = '\0';
    theConfig.mobile_user1[0] = '\0';
    theConfig.mobile_user2[0] = '\0';
    theConfig.mobile_user3[0] = '\0';
    theConfig.mobile_user4[0] = '\0';

    for (i = 0; i < 5; i++)
    {
        if (settingVoiceDeleteUserArray[i][0] != '\0')
        {
            if (theConfig.mobile_user0[0] == '\0')
                strcpy(theConfig.mobile_user0, settingVoiceDeleteUserArray[i]);
            else if (theConfig.mobile_user1[0] == '\0')
                strcpy(theConfig.mobile_user1, settingVoiceDeleteUserArray[i]);
            else if (theConfig.mobile_user2[0] == '\0')
                strcpy(theConfig.mobile_user2, settingVoiceDeleteUserArray[i]);
            else if (theConfig.mobile_user3[0] == '\0')
                strcpy(theConfig.mobile_user3, settingVoiceDeleteUserArray[i]);
            else if (theConfig.mobile_user4[0] == '\0')
                strcpy(theConfig.mobile_user4, settingVoiceDeleteUserArray[i]);
        }
    }

    i = 0;

    if (theConfig.mobile_user0[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user0);
    if (theConfig.mobile_user1[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user1);
    if (theConfig.mobile_user2[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user2);
    if (theConfig.mobile_user3[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user3);
    if (theConfig.mobile_user4[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user4);

    settingVoiceDeleteUserCount = i;

    for (; i < 5; i++)
        settingVoiceDeleteUserArray[i][0] = '\0';

    for (i = 0; i < 5; i++)
        settingVoiceDeleteUserSelectedArray[i] = false;

    ituListBoxReload((ITUListBox*)settingVoiceDeleteUserScrollIconListBox);
    ituListBoxReload((ITUListBox*)settingVoiceDeleteUserScrollListBox);

    ConfigSave();

    return true;
}

bool SettingVoiceDeleteUserOnEnter(ITUWidget* widget, char* param)
{
    int i = 0;

    if (!settingVoiceDeleteUserScrollIconListBox)
    {
        settingVoiceDeleteUserScrollIconListBox = ituSceneFindWidget(&theScene, "settingVoiceDeleteUserScrollIconListBox");
        assert(settingVoiceDeleteUserScrollIconListBox);
        
        settingVoiceDeleteUserScrollListBox = ituSceneFindWidget(&theScene, "settingVoiceDeleteUserScrollListBox");
        assert(settingVoiceDeleteUserScrollListBox);        
    }

    if (theConfig.mobile_user0[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user0);
    if (theConfig.mobile_user1[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user1);
    if (theConfig.mobile_user2[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user2);
    if (theConfig.mobile_user3[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user3);
    if (theConfig.mobile_user4[0] != '\0')
        strcpy(settingVoiceDeleteUserArray[i++], theConfig.mobile_user4);

    settingVoiceDeleteUserCount = i;

    for (; i < 5; i++)
        settingVoiceDeleteUserArray[i][0] = '\0';

    for (i = 0; i < 5; i++)
        settingVoiceDeleteUserSelectedArray[i] = false;

    return true;
}

bool SettingVoiceDeleteUserOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void SettingVoiceDeleteUserReset(void)
{
    settingVoiceDeleteUserScrollIconListBox = NULL;
}
