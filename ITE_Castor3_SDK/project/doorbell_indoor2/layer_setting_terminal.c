#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define MAX_INTERCOM_COUNT 99

static ITUTextBox* settingTerminalAreaTextBox;
static ITUTextBox* settingTerminalBuildingTextBox;
static ITUTextBox* settingTerminalUnitTextBox;
static ITUTextBox* settingTerminalFloorTextBox;
static ITUTextBox* settingTerminalRoomTextBox;
static ITUTextBox* settingTerminalExtTextBox;
static ITUBackground* settingTerminalBackground;
static ITUBackgroundButton* settingTerminalErrorBackgroundButton;

// status
static ITUTextBox* activeSettingTerminalTextBox;

bool SettingTerminalBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activeSettingTerminalTextBox, NULL);

    if (activeSettingTerminalTextBox == settingTerminalExtTextBox)
    {
        activeSettingTerminalTextBox = settingTerminalRoomTextBox;
    }
    else if (activeSettingTerminalTextBox == settingTerminalRoomTextBox)
    {
        activeSettingTerminalTextBox = settingTerminalFloorTextBox;
    }
    else if (activeSettingTerminalTextBox == settingTerminalFloorTextBox)
    {
        activeSettingTerminalTextBox = settingTerminalUnitTextBox;
    }
    else if (activeSettingTerminalTextBox == settingTerminalUnitTextBox)
    {
        activeSettingTerminalTextBox = settingTerminalBuildingTextBox;
    }
    else if (activeSettingTerminalTextBox == settingTerminalBuildingTextBox)
    {
        activeSettingTerminalTextBox = settingTerminalAreaTextBox;
    }
    return true;
}

bool SettingTerminalConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* area = ituTextBoxGetString(settingTerminalAreaTextBox);
    char* building = ituTextBoxGetString(settingTerminalBuildingTextBox);
    char* unit = ituTextBoxGetString(settingTerminalUnitTextBox);
    char* floor = ituTextBoxGetString(settingTerminalFloorTextBox);
    char* room = ituTextBoxGetString(settingTerminalRoomTextBox);
    char* ext = ituTextBoxGetString(settingTerminalExtTextBox);

    if (area && building && unit && floor && room && ext)
    {
        char* buf = AddressBookGetDeviceIP(area, building, unit, floor, room, ext);
        if (buf)
        {
            strcpy(theConfig.area, area);
            strcpy(theConfig.building, building);
            strcpy(theConfig.unit, unit);
            strcpy(theConfig.floor, floor);
            strcpy(theConfig.room, room);
            strcpy(theConfig.ext, ext);

            ConfigSavePrivate();

            // reset network
            SceneQuit(QUIT_RESET_NETWORK);

            free(buf);
        }
        else
        {
            ituWidgetDisable(settingTerminalBackground);
            ituWidgetSetVisible(settingTerminalErrorBackgroundButton, true);
        }
    }
    else if (!area && !building && !unit && !floor && !room && !ext)
    {
        theConfig.area[0] = '\0';
        theConfig.building[0] = '\0';
        theConfig.unit[0] = '\0';
        theConfig.floor[0] = '\0';
        theConfig.room[0] = '\0';
        theConfig.ext[0] = '\0';

        ConfigSavePrivate();

        // reset network
        SceneQuit(QUIT_RESET_NETWORK);
    }
    return true;
}

bool SettingTerminalCancelButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxSetString(settingTerminalAreaTextBox, theConfig.area);
    ituTextBoxSetString(settingTerminalBuildingTextBox, theConfig.building);
    ituTextBoxSetString(settingTerminalUnitTextBox, theConfig.unit);
    ituTextBoxSetString(settingTerminalFloorTextBox, theConfig.floor);
    ituTextBoxSetString(settingTerminalRoomTextBox, theConfig.room);
    ituTextBoxSetString(settingTerminalExtTextBox, theConfig.ext);

    if (theConfig.ext[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalExtTextBox;
    else if (theConfig.room[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalRoomTextBox;
    else if (theConfig.floor[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalFloorTextBox;
    else if (theConfig.unit[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalUnitTextBox;
    else if (theConfig.building[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalBuildingTextBox;
    else
        activeSettingTerminalTextBox = settingTerminalAreaTextBox;

    return true;
}

bool SettingTerminalNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = param;
    char* str = ituTextGetString(activeSettingTerminalTextBox);
    int count = str ? strlen(str) : 0;

    if (count >= activeSettingTerminalTextBox->maxLen)
    {
        if (activeSettingTerminalTextBox == settingTerminalAreaTextBox)
            activeSettingTerminalTextBox = settingTerminalBuildingTextBox;
        else if (activeSettingTerminalTextBox == settingTerminalBuildingTextBox)
            activeSettingTerminalTextBox = settingTerminalUnitTextBox;
        else if (activeSettingTerminalTextBox == settingTerminalUnitTextBox)
            activeSettingTerminalTextBox = settingTerminalFloorTextBox;
        else if (activeSettingTerminalTextBox == settingTerminalFloorTextBox)
            activeSettingTerminalTextBox = settingTerminalRoomTextBox;
        else if (activeSettingTerminalTextBox == settingTerminalRoomTextBox)
            activeSettingTerminalTextBox = settingTerminalExtTextBox;
    }
    ituTextBoxInput(activeSettingTerminalTextBox, input);
    return true;
}

bool SettingTerminalOnEnter(ITUWidget* widget, char* param)
{
    if (!settingTerminalAreaTextBox)
    {
        settingTerminalAreaTextBox = ituSceneFindWidget(&theScene, "settingTerminalAreaTextBox");
        assert(settingTerminalAreaTextBox);

        settingTerminalBuildingTextBox = ituSceneFindWidget(&theScene, "settingTerminalBuildingTextBox");
        assert(settingTerminalBuildingTextBox);

        settingTerminalUnitTextBox = ituSceneFindWidget(&theScene, "settingTerminalUnitTextBox");
        assert(settingTerminalUnitTextBox);

        settingTerminalFloorTextBox = ituSceneFindWidget(&theScene, "settingTerminalFloorTextBox");
        assert(settingTerminalFloorTextBox);

        settingTerminalRoomTextBox = ituSceneFindWidget(&theScene, "settingTerminalRoomTextBox");
        assert(settingTerminalRoomTextBox);

        settingTerminalExtTextBox = ituSceneFindWidget(&theScene, "settingTerminalExtTextBox");
        assert(settingTerminalExtTextBox);

        settingTerminalBackground = ituSceneFindWidget(&theScene, "settingTerminalBackground");
        assert(settingTerminalBackground);

        settingTerminalErrorBackgroundButton = ituSceneFindWidget(&theScene, "settingTerminalErrorBackgroundButton");
        assert(settingTerminalErrorBackgroundButton);
    }

    ituTextBoxSetString(settingTerminalAreaTextBox, theConfig.area);
    ituTextBoxSetString(settingTerminalBuildingTextBox, theConfig.building);
    ituTextBoxSetString(settingTerminalUnitTextBox, theConfig.unit);
    ituTextBoxSetString(settingTerminalFloorTextBox, theConfig.floor);
    ituTextBoxSetString(settingTerminalRoomTextBox, theConfig.room);
    ituTextBoxSetString(settingTerminalExtTextBox, theConfig.ext);

    if (theConfig.ext[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalExtTextBox;
    else if (theConfig.room[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalRoomTextBox;
    else if (theConfig.floor[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalFloorTextBox;
    else if (theConfig.unit[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalUnitTextBox;
    else if (theConfig.building[0] != '\0')
        activeSettingTerminalTextBox = settingTerminalBuildingTextBox;
    else
        activeSettingTerminalTextBox = settingTerminalAreaTextBox;

    return true;
}

void SettingTerminalReset(void)
{
    settingTerminalAreaTextBox = NULL;
}
