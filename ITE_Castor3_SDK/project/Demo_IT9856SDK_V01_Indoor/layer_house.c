#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

#define MAX_MODE_COUNT 10

static ITURadioBox* houseModeIndoorRadioBox;
static ITURadioBox* houseModeOutdoorRadioBox;
static ITURadioBox* houseModeSleepRadioBox;
static ITUText* houseModeSettingLabelText;

static HouseInfo houseLightInfoArray[HOUSE_MAX_LIGHT_COUNT];
static int houseLightInfoCount;

static HouseInfo houseAirConditionerInfoArray[HOUSE_MAX_AIRCONDITIONER_COUNT];
static int houseAirConditionerInfoCount;

static HouseInfo houseCurtainInfoArray[HOUSE_MAX_CURTAIN_COUNT];
static int houseCurtainInfoCount;

static HouseInfo housePlugInfoArray[HOUSE_MAX_PLUG_COUNT];
static int housePlugInfoCount;

static HouseInfo houseTVInfoArray[HOUSE_MAX_TV_COUNT];
static int houseTVInfoCount;

static HouseInfo houseModeInfoArray[MAX_MODE_COUNT];
static HouseMode houseMode;
static int houseModeInfoCount;
static bool houseModeUpdated;

static void HouseModeUpdateInfoArray(void)
{
	int i, index, count;
    char *airconditioners, *curtains, *lights, *plugs, *tvs;
    
    ituTextSetString(houseModeSettingLabelText, StringGetHouseMode(houseMode));
    
    houseModeInfoCount = 0;

    switch (houseMode)
    {
    case HOUSE_INDOOR:
        airconditioners = theConfig.house_indoor_airconditioners;
        curtains = theConfig.house_indoor_curtains;
        lights = theConfig.house_indoor_lights;
        plugs = theConfig.house_indoor_plugs;
        tvs = theConfig.house_indoor_tvs;
        break;

    case HOUSE_OUTDOOR:
        airconditioners = theConfig.house_outdoor_airconditioners;
        curtains = theConfig.house_outdoor_curtains;
        lights = theConfig.house_outdoor_lights;
        plugs = theConfig.house_outdoor_plugs;
        tvs = theConfig.house_outdoor_tvs;
        break;

    case HOUSE_SLEEP:
        airconditioners = theConfig.house_sleep_airconditioners;
        curtains = theConfig.house_sleep_curtains;
        lights = theConfig.house_sleep_lights;
        plugs = theConfig.house_sleep_plugs;
        tvs = theConfig.house_sleep_tvs;
        break;

    default:
        return;
    }

    count = HouseGetInfoArray(HOUSE_AIRCONDITIONER, &houseModeInfoArray[houseModeInfoCount], MAX_MODE_COUNT - houseModeInfoCount);
    index = houseModeInfoCount;
    houseModeInfoCount += count;
    for (i = 0; i < count; i++)
    {
        if (airconditioners[i] == '0')
           houseModeInfoArray[index + i].status = HOUSE_OFF;
        else
           houseModeInfoArray[index + i].status = HOUSE_ON;
    }

    count = HouseGetInfoArray(HOUSE_CURTAIN, &houseModeInfoArray[houseModeInfoCount], MAX_MODE_COUNT - houseModeInfoCount);
    index = houseModeInfoCount;
    houseModeInfoCount += count;
    for (i = 0; i < count; i++)
    {
        if (curtains[i] == '0')
           houseModeInfoArray[index + i].status = HOUSE_OFF;
        else
           houseModeInfoArray[index + i].status = HOUSE_ON;
    }

    count = HouseGetInfoArray(HOUSE_LIGHT, &houseModeInfoArray[houseModeInfoCount], MAX_MODE_COUNT - houseModeInfoCount);
    index = houseModeInfoCount;
    houseModeInfoCount += count;
    for (i = 0; i < count; i++)
    {
        if (lights[i] == '0')
           houseModeInfoArray[index + i].status = HOUSE_OFF;
        else
           houseModeInfoArray[index + i].status = HOUSE_ON;
    }

    count = HouseGetInfoArray(HOUSE_PLUG, &houseModeInfoArray[houseModeInfoCount], MAX_MODE_COUNT - houseModeInfoCount);
    index = houseModeInfoCount;
    houseModeInfoCount += count;
    for (i = 0; i < count; i++)
    {
        if (plugs[i] == '0')
           houseModeInfoArray[index + i].status = HOUSE_OFF;
        else
           houseModeInfoArray[index + i].status = HOUSE_ON;
    }

    count = HouseGetInfoArray(HOUSE_TV, &houseModeInfoArray[houseModeInfoCount], MAX_MODE_COUNT - houseModeInfoCount);
    index = houseModeInfoCount;
    houseModeInfoCount += count;
    for (i = 0; i < count; i++)
    {
        if (tvs[i] == '0')
           houseModeInfoArray[index + i].status = HOUSE_OFF;
        else
           houseModeInfoArray[index + i].status = HOUSE_ON;
    }
}

void HouseModeUpdateConfig(void)
{
    int i, index = 0;

    if (houseMode == HOUSE_INDOOR)
    {
        for (i = 0; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[index].device != HOUSE_AIRCONDITIONER)
                break;

            if (houseModeInfoArray[index].status == HOUSE_ON)
                theConfig.house_indoor_airconditioners[i] = '1';
            else
                theConfig.house_indoor_airconditioners[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_CURTAIN)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_indoor_curtains[i] = '1';
            else
                theConfig.house_indoor_curtains[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_LIGHT)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_indoor_lights[i] = '1';
            else
                theConfig.house_indoor_lights[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_PLUG)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_indoor_plugs[i] = '1';
            else
                theConfig.house_indoor_plugs[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_TV)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_indoor_tvs[i] = '1';
            else
                theConfig.house_indoor_tvs[i] = '0';
        }
    }
    else if (houseMode == HOUSE_OUTDOOR)
    {
        for (i = 0; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[index].device != HOUSE_AIRCONDITIONER)
                break;

            if (houseModeInfoArray[index].status == HOUSE_ON)
                theConfig.house_outdoor_airconditioners[i] = '1';
            else
                theConfig.house_outdoor_airconditioners[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_CURTAIN)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_outdoor_curtains[i] = '1';
            else
                theConfig.house_outdoor_curtains[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_LIGHT)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_outdoor_lights[i] = '1';
            else
                theConfig.house_outdoor_lights[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_PLUG)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_outdoor_plugs[i] = '1';
            else
                theConfig.house_outdoor_plugs[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_TV)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_outdoor_tvs[i] = '1';
            else
                theConfig.house_outdoor_tvs[i] = '0';
        }
    }
    else if (houseMode == HOUSE_SLEEP)
    {
        for (i = 0; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[index].device != HOUSE_AIRCONDITIONER)
                break;

            if (houseModeInfoArray[index].status == HOUSE_ON)
                theConfig.house_sleep_airconditioners[i] = '1';
            else
                theConfig.house_sleep_airconditioners[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_CURTAIN)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_sleep_curtains[i] = '1';
            else
                theConfig.house_sleep_curtains[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_LIGHT)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_sleep_lights[i] = '1';
            else
                theConfig.house_sleep_lights[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_PLUG)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_sleep_plugs[i] = '1';
            else
                theConfig.house_sleep_plugs[i] = '0';
        }

        for (; i < houseModeInfoCount; i++, index++)
        {
            if (houseModeInfoArray[i].device != HOUSE_TV)
                break;

            if (houseModeInfoArray[i].status == HOUSE_ON)
                theConfig.house_sleep_tvs[i] = '1';
            else
                theConfig.house_sleep_tvs[i] = '0';
        }
    }
}

bool HouseModeIndoorRadioBoxOnPress(ITUWidget* widget, char* param)
{
    ITURadioBox* radiobox = (ITURadioBox*) widget;

    HouseModeUpdateConfig();

    if (theConfig.house_mode == HOUSE_INDOOR)
    {
        ituRadioBoxSetChecked(radiobox, false);
        HouseSetMode(HOUSE_NONE);
        houseMode = HOUSE_NONE;
    }
    else
    {
        ituRadioBoxSetChecked(radiobox, true);
        HouseSetMode(HOUSE_INDOOR);
        houseMode = HOUSE_INDOOR;
    }
    HouseModeUpdateInfoArray();
    houseModeUpdated = true;
	return true;
}

bool HouseModeOutdoorRadioBoxOnPress(ITUWidget* widget, char* param)
{
    ITURadioBox* radiobox = (ITURadioBox*) widget;

    HouseModeUpdateConfig();

    if (theConfig.house_mode == HOUSE_OUTDOOR)
    {
        ituRadioBoxSetChecked(radiobox, false);
        HouseSetMode(HOUSE_NONE);
        houseMode = HOUSE_NONE;
    }
    else
    {
        ituRadioBoxSetChecked(radiobox, true);
        HouseSetMode(HOUSE_OUTDOOR);
        houseMode = HOUSE_OUTDOOR;
    }
    HouseModeUpdateInfoArray();
    houseModeUpdated = true;
	return true;
}

bool HouseModeSleepRadioBoxOnPress(ITUWidget* widget, char* param)
{
    ITURadioBox* radiobox = (ITURadioBox*) widget;

    HouseModeUpdateConfig();

    if (theConfig.house_mode == HOUSE_SLEEP)
    {
        ituRadioBoxSetChecked(radiobox, false);
        HouseSetMode(HOUSE_NONE);
        houseMode = HOUSE_NONE;
    }
    else
    {
        ituRadioBoxSetChecked(radiobox, true);
        HouseSetMode(HOUSE_SLEEP);
        houseMode = HOUSE_SLEEP;
    }
    HouseModeUpdateInfoArray();
    houseModeUpdated = true;
	return true;
}

bool HouseLightScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = houseLightInfoCount ? (houseLightInfoCount + count - 1) / count : 1;

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

    for (; j < houseLightInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseLightInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HouseLightScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseLightInfoCount ? (houseLightInfoCount + count - 1) / count : 1;

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

    for (; j < houseLightInfoCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseLightInfoArray[j];
        sprintf(buf, "%d", info->status);
        ituTextSetString(scrolltext, buf);
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

bool HouseLightScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            char buf[4];

            if (info->status == HOUSE_OFF)
            {
                HouseExecAction(HOUSE_LIGHT, info->index, HOUSE_OPEN, NULL);
            }
            else if (info->status == HOUSE_ON)
            {
                HouseExecAction(HOUSE_LIGHT, info->index, HOUSE_CLOSE, NULL);
            }

            houseLightInfoCount = HouseGetInfoArray(HOUSE_LIGHT, houseLightInfoArray, HOUSE_MAX_LIGHT_COUNT);
            sprintf(buf, "%d", info->status);
            ituTextSetString(scrolltext, buf);
        }
    }
	return true;
}

bool HouseAirConditionerScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = houseAirConditionerInfoCount ? (houseAirConditionerInfoCount + count - 1) / count : 1;

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

    for (; j < houseAirConditionerInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseAirConditionerInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HouseAirConditionerScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseAirConditionerInfoCount ? (houseAirConditionerInfoCount + count - 1) / count : 1;

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

    for (; j < houseAirConditionerInfoCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseAirConditionerInfoArray[j];
        sprintf(buf, "%d", info->status);
        ituTextSetString(scrolltext, buf);
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

bool HouseAirConditionerScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            char buf[4];

            if (info->status == HOUSE_OFF)
            {
                HouseExecAction(HOUSE_AIRCONDITIONER, info->index, HOUSE_OPEN, NULL);
            }
            else if (info->status == HOUSE_ON)
            {
                HouseExecAction(HOUSE_AIRCONDITIONER, info->index, HOUSE_CLOSE, NULL);
            }

            houseAirConditionerInfoCount = HouseGetInfoArray(HOUSE_AIRCONDITIONER, houseAirConditionerInfoArray, HOUSE_MAX_AIRCONDITIONER_COUNT);
            sprintf(buf, "%d", info->status);
            ituTextSetString(scrolltext, buf);
        }
    }
	return true;
}

bool HouseCurtainScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = houseCurtainInfoCount ? (houseCurtainInfoCount + count - 1) / count : 1;

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

    for (; j < houseCurtainInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseCurtainInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HouseCurtainCloseScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseCurtainInfoCount ? (houseCurtainInfoCount + count - 1) / count : 1;

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

    for (; j < houseCurtainInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseAirConditionerInfoArray[j];
        ituTextSetString(scrolltext, "0");
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

bool HouseCurtainCloseScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            HouseExecAction(HOUSE_CURTAIN, info->index, HOUSE_CLOSE, NULL);
        }
    }
	return true;
}

bool HouseCurtainPauseScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseCurtainInfoCount ? (houseCurtainInfoCount + count - 1) / count : 1;

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

    for (; j < houseCurtainInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseAirConditionerInfoArray[j];
        ituTextSetString(scrolltext, "0");
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

bool HouseCurtainPauseScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            HouseExecAction(HOUSE_CURTAIN, info->index, HOUSE_STOP, NULL);
        }
    }
	return true;
}

bool HouseCurtainOpenScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseCurtainInfoCount ? (houseCurtainInfoCount + count - 1) / count : 1;

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

    for (; j < houseCurtainInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseAirConditionerInfoArray[j];
        ituTextSetString(scrolltext, "0");
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

bool HouseCurtainOpenScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            HouseExecAction(HOUSE_CURTAIN, info->index, HOUSE_OPEN, NULL);
        }
    }
	return true;
}

bool HousePlugScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = housePlugInfoCount ? (housePlugInfoCount + count - 1) / count : 1;

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

    for (; j < housePlugInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &housePlugInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HousePlugScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = housePlugInfoCount ? (housePlugInfoCount + count - 1) / count : 1;

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

    for (; j < housePlugInfoCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &housePlugInfoArray[j];
        sprintf(buf, "%d", info->status);
        ituTextSetString(scrolltext, buf);
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

bool HousePlugScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            char buf[4];

            if (info->status == HOUSE_OFF)
            {
                HouseExecAction(HOUSE_PLUG, info->index, HOUSE_OPEN, NULL);
            }
            else if (info->status == HOUSE_ON)
            {
                HouseExecAction(HOUSE_PLUG, info->index, HOUSE_CLOSE, NULL);
            }

            housePlugInfoCount = HouseGetInfoArray(HOUSE_PLUG, housePlugInfoArray, HOUSE_MAX_PLUG_COUNT);
            sprintf(buf, "%d", info->status);
            ituTextSetString(scrolltext, buf);
        }
    }
	return true;
}

bool HouseTVScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = houseTVInfoCount ? (houseTVInfoCount + count - 1) / count : 1;

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

    for (; j < houseTVInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseTVInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HouseTVScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseTVInfoCount ? (houseTVInfoCount + count - 1) / count : 1;

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

    for (; j < houseTVInfoCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseTVInfoArray[j];
        sprintf(buf, "%d", info->status);
        ituTextSetString(scrolltext, buf);
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

bool HouseTVScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            char buf[4];

            if (info->status == HOUSE_OFF)
            {
                HouseExecAction(HOUSE_TV, info->index, HOUSE_OPEN, NULL);
            }
            else if (info->status == HOUSE_ON)
            {
                HouseExecAction(HOUSE_TV, info->index, HOUSE_CLOSE, NULL);
            }

            houseTVInfoCount = HouseGetInfoArray(HOUSE_TV, houseTVInfoArray, HOUSE_MAX_TV_COUNT);
            sprintf(buf, "%d", info->status);
            ituTextSetString(scrolltext, buf);
        }
    }
	return true;
}

bool HouseModeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = houseModeInfoCount ? (houseModeInfoCount + count - 1) / count : 1;

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

    for (; j < houseModeInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseModeInfoArray[j];
        ituTextSetString(scrolltext, info->name);
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

bool HouseModeScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count;
    HouseInfo* info;

    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = houseModeInfoCount ? (houseModeInfoCount + count - 1) / count : 1;

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

    for (; j < houseModeInfoCount; j++)
    {
        char buf[4];
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &houseModeInfoArray[j];
        sprintf(buf, "%d", info->status);
        ituTextSetString(scrolltext, buf);
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

bool HouseModeScrollIconListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* scrolltext = (ITUScrollText*)ituListBoxGetFocusItem(listbox);
    if (scrolltext)
    {
        HouseInfo* info = (HouseInfo*)ituWidgetGetCustomData(scrolltext);
        if (info)
        {
            char buf[4];

            if (info->status == HOUSE_ON)
                info->status = HOUSE_OFF;
            else
                info->status = HOUSE_ON;

            sprintf(buf, "%d", info->status);
            ituTextSetString(scrolltext, buf);
            houseModeUpdated = true;
        }
    }
	return true;
}

bool HouseOnEnter(ITUWidget* widget, char* param)
{
    if (!houseModeIndoorRadioBox)
    {
        houseModeIndoorRadioBox = ituSceneFindWidget(&theScene, "houseModeIndoorRadioBox");
        assert(houseModeIndoorRadioBox);

        houseModeOutdoorRadioBox = ituSceneFindWidget(&theScene, "houseModeOutdoorRadioBox");
        assert(houseModeOutdoorRadioBox);

        houseModeSleepRadioBox = ituSceneFindWidget(&theScene, "houseModeSleepRadioBox");
        assert(houseModeSleepRadioBox);

        houseModeSettingLabelText = ituSceneFindWidget(&theScene, "houseModeSettingLabelText");
        assert(houseModeSettingLabelText);
	}

    if (houseLightInfoCount == 0)
        houseLightInfoCount = HouseGetInfoArray(HOUSE_LIGHT, houseLightInfoArray, HOUSE_MAX_LIGHT_COUNT);

    if (houseAirConditionerInfoCount == 0)
        houseAirConditionerInfoCount = HouseGetInfoArray(HOUSE_AIRCONDITIONER, houseAirConditionerInfoArray, HOUSE_MAX_AIRCONDITIONER_COUNT);

    if (houseCurtainInfoCount == 0)
        houseCurtainInfoCount = HouseGetInfoArray(HOUSE_CURTAIN, houseCurtainInfoArray, HOUSE_MAX_CURTAIN_COUNT);

    if (housePlugInfoCount == 0)
        housePlugInfoCount = HouseGetInfoArray(HOUSE_PLUG, housePlugInfoArray, HOUSE_MAX_PLUG_COUNT);

    if (houseTVInfoCount == 0)
        houseTVInfoCount = HouseGetInfoArray(HOUSE_TV, houseTVInfoArray, HOUSE_MAX_TV_COUNT);

    switch (theConfig.house_mode)
    {
    case HOUSE_NONE:
        ituRadioBoxSetChecked(houseModeIndoorRadioBox, false);
        ituRadioBoxSetChecked(houseModeOutdoorRadioBox, false);
        ituRadioBoxSetChecked(houseModeSleepRadioBox, false);
        break;

    case HOUSE_INDOOR:
        ituRadioBoxSetChecked(houseModeIndoorRadioBox, true);
        break;

    case HOUSE_OUTDOOR:
        ituRadioBoxSetChecked(houseModeOutdoorRadioBox, true);
        break;

    case HOUSE_SLEEP:
        ituRadioBoxSetChecked(houseModeSleepRadioBox, true);
        break;
    }
    houseModeUpdated = false;
    houseModeInfoCount = 0;
	return true;
}

bool HouseOnLeave(ITUWidget* widget, char* param)
{
    if (houseModeUpdated)
        ConfigSave();

    return true;
}

void HouseReset(void)
{
    houseModeSettingLabelText  = NULL;
    houseModeIndoorRadioBox  = NULL;
}
