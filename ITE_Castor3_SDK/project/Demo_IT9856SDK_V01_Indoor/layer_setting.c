#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingDisplayLayer;
static ITULayer* settingSoundLayer;
static ITULayer* settingVoiceLayer;
static ITULayer* settingDisturbLayer;
static ITULayer* settingLangLayer;
static ITULayer* settingNetworkLayer;
static ITULayer* settingWiFiSsidLayer;
static ITULayer* settingSensorLayer;
static ITULayer* settingTerminalLayer;
static ITULayer* settingDoorCameraLayer;
static ITULayer* settingMiscLayer;
static ITULayer* settingSysInfoLayer;
static ITULayer* settingPasswordLayer;

static ITUTextBox* settingPasswordTextBox;
static ITULayer* settingLastLayer;

int settingPageIndex;
int settingFocusIndex;
static bool settingVerified;

void SettingResetVerify(void)
{
    settingVerified = false;
}

bool SettingPasswordConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* password = ituTextGetString(settingPasswordTextBox);
    if (!password ||
        strcmp(password, theConfig.engineer_password))
    {
        ituTextBoxSetString(settingPasswordTextBox, NULL);
        settingVerified = false;
		return true;
    }
    settingVerified = true;
    assert(settingLastLayer);
    ituLayerGoto(settingLastLayer);
    return true;
}

bool SettingIconScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;

    entryCount = SETTING_COUNT;
    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        if (settingPageIndex > 0)
        {
            listbox->pageIndex = settingPageIndex;
            ituListBoxSelect(listbox, settingFocusIndex);
        }
        else
        {
            // initialize
            listbox->pageIndex = 1;
            listbox->focusIndex = 0;
            settingPageIndex = listbox->pageIndex;
            settingFocusIndex = listbox->focusIndex;
        }
    }
    else if (listbox->pageIndex == settingPageIndex)
    {
        ituListBoxSelect(listbox, settingFocusIndex);
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

bool SettingScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = SETTING_COUNT ? (SETTING_COUNT + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        if (settingPageIndex > 0)
        {
            listbox->pageIndex = settingPageIndex;
            ituListBoxSelect(listbox, settingFocusIndex);
        }
        else
        {
            // initialize
            listbox->pageIndex = 1;
            listbox->focusIndex = 0;
            settingPageIndex = listbox->pageIndex;
            settingFocusIndex = listbox->focusIndex;
        }
    }
    else if (listbox->pageIndex == settingPageIndex)
    {
        ituListBoxSelect(listbox, settingFocusIndex);
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

    for (; j < SETTING_COUNT; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        const char* s = StringGetSettingType(j);
        ituTextSetString(scrolltext, s);
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

bool SettingScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* item;

    if (!settingDisplayLayer)
    {
        settingDisplayLayer = ituSceneFindWidget(&theScene, "settingDisplayLayer");
        assert(settingDisplayLayer);

        settingSoundLayer = ituSceneFindWidget(&theScene, "settingSoundLayer");
        assert(settingSoundLayer);

        settingVoiceLayer = ituSceneFindWidget(&theScene, "settingVoiceLayer");
        assert(settingVoiceLayer);

        settingDisturbLayer = ituSceneFindWidget(&theScene, "settingDisturbLayer");
        assert(settingDisturbLayer);

        settingLangLayer = ituSceneFindWidget(&theScene, "settingLangLayer");
        assert(settingLangLayer);

        settingNetworkLayer = ituSceneFindWidget(&theScene, "settingNetworkLayer");
        assert(settingNetworkLayer);

        settingWiFiSsidLayer = ituSceneFindWidget(&theScene, "settingWiFiSsidLayer");
        assert(settingWiFiSsidLayer);

        settingSensorLayer = ituSceneFindWidget(&theScene, "settingSensorLayer");
        assert(settingSensorLayer);

        settingTerminalLayer = ituSceneFindWidget(&theScene, "settingTerminalLayer");
        assert(settingTerminalLayer);

        settingDoorCameraLayer = ituSceneFindWidget(&theScene, "settingDoorCameraLayer");
        assert(settingDoorCameraLayer);

        settingMiscLayer = ituSceneFindWidget(&theScene, "settingMiscLayer");
        assert(settingMiscLayer);

        settingSysInfoLayer = ituSceneFindWidget(&theScene, "settingSysInfoLayer");
        assert(settingSysInfoLayer);

        settingPasswordLayer = ituSceneFindWidget(&theScene, "settingPasswordLayer");
        assert(settingPasswordLayer);

        settingPasswordTextBox = ituSceneFindWidget(&theScene, "settingPasswordTextBox");
        assert(settingPasswordTextBox);
    }

    item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
    if (item)
    {
        int i = (int)ituWidgetGetCustomData(item);
        switch (i)
        {
        case SETTING_DISPLAY:
            if (!ituWidgetIsVisible(settingDisplayLayer))
                ituLayerGoto(settingDisplayLayer);
            break;

        case SETTING_SOUND:
            if (!ituWidgetIsVisible(settingSoundLayer))
                ituLayerGoto(settingSoundLayer);
            break;

        case SETTING_VOICE:
            if (!ituWidgetIsVisible(settingVoiceLayer))
                ituLayerGoto(settingVoiceLayer);
            break;

        case SETTING_DISTURB:
            if (!ituWidgetIsVisible(settingDisturbLayer))
                ituLayerGoto(settingDisturbLayer);
            break;

        case SETTING_LANGUAGE:
            if (!ituWidgetIsVisible(settingLangLayer))
                ituLayerGoto(settingLangLayer);
            break;

        case SETTING_NETWORK:
        #ifdef CFG_NET_WIFI
            if (!ituWidgetIsVisible(settingWiFiSsidLayer))
            {
                if (settingVerified)
                {
                    ituLayerGoto(settingWiFiSsidLayer);
                }
                else
                {
                    settingLastLayer = settingWiFiSsidLayer;
                    ituLayerGoto(settingPasswordLayer);
                }
            }
        #else
            if (!ituWidgetIsVisible(settingNetworkLayer))
            {
                if (settingVerified)
                {
                    ituLayerGoto(settingNetworkLayer);
                }
                else
                {
                    settingLastLayer = settingNetworkLayer;
                    ituLayerGoto(settingPasswordLayer);
                }
            }
        #endif // CFG_NET_WIFI
            break;

        case SETTING_SENSOR:
            if (!ituWidgetIsVisible(settingSensorLayer))
            {
                if (settingVerified)
                {
                    ituLayerGoto(settingSensorLayer);
                }
                else
                {
                    settingLastLayer = settingSensorLayer;
                    ituLayerGoto(settingPasswordLayer);
                }
            }
            break;

        case SETTING_TERMINAL:
            if (!ituWidgetIsVisible(settingTerminalLayer))
            {
                if (settingVerified)
                {
                    ituLayerGoto(settingTerminalLayer);
                }
                else
                {
                    settingLastLayer = settingTerminalLayer;
                    ituLayerGoto(settingPasswordLayer);
                }
            }
            break;

        case SETTING_OUTDOOR:
            if (!ituWidgetIsVisible(settingDoorCameraLayer))
                ituLayerGoto(settingDoorCameraLayer);
            break;

        case SETTING_MISC:
            if (!ituWidgetIsVisible(settingMiscLayer))
            {
                if (settingVerified)
                {
                    ituLayerGoto(settingMiscLayer);
                }
                else
                {
                    settingLastLayer = settingMiscLayer;
                    ituLayerGoto(settingPasswordLayer);
                }
            }
            break;

        case SETTING_SYSTEM:
            if (!ituWidgetIsVisible(settingSysInfoLayer))
                ituLayerGoto(settingSysInfoLayer);
            break;
        }
        settingPageIndex = listbox->pageIndex;
        settingFocusIndex = listbox->focusIndex;

        return true;
    }
    return false;
}

void SettingReset(void)
{
    settingDisplayLayer = NULL;
    settingLastLayer = NULL;
}