#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite/itp.h"
#include "scene.h"
#include "doorbell.h"

static ITUBackground* settingMiscDoorCameraBackground;
static ITUKeyboard* settingMiscDoorCameraKeyboard;
static ITUTextBox* settingMiscDoorCameraAreaTextBox;
static ITUTextBox* settingMiscDoorCameraBuildingTextBox;
static ITUTextBox* settingMiscDoorCameraUnitTextBox;
static ITUTextBox* settingMiscDoorCameraFloorTextBox;
static ITUTextBox* settingMiscDoorCameraRoomTextBox;
static ITUTextBox* settingMiscDoorCameraExtTextBox;
static ITUBackgroundButton* settingMiscDoorCameraErrorBackgroundButton;
static ITUText* settingMiscDoorCameraErrorText;

// status
static ITUTextBox* activeSettingMiscDoorCameraRoomAddrTextBox;
static ServerInfo settingMiscDoorCameraServerInfo;
static sem_t settingMiscDoorCameraServerInfoSem;

bool SettingMiscDoorCameraRoomAddrBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activeSettingMiscDoorCameraRoomAddrTextBox, NULL);

    if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraExtTextBox)
    {
        activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraRoomTextBox;
    }
    else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraRoomTextBox)
    {
        activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraFloorTextBox;
    }
    else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraFloorTextBox)
    {
        activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraUnitTextBox;
    }
    else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraUnitTextBox)
    {
        activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraBuildingTextBox;
    }
    else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraBuildingTextBox)
    {
        activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraAreaTextBox;
    }
    return true;
}

static void GetOutdoorInfoCallback(ServerInfo* info)
{
    memcpy(&settingMiscDoorCameraServerInfo ,info, sizeof(ServerInfo));
    sem_post(&settingMiscDoorCameraServerInfoSem);
}

bool SettingMiscDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    int ret, ver = -1;
    char* verStr;
    char* ip = NULL;
    char outdoor_area[4], outdoor_building[4], outdoor_unit[4], outdoor_floor[4], outdoor_room[4], outdoor_ext[4], reason[128];
    
    reason[0] = '\0';

    strcpy(outdoor_area, ituTextGetString(settingMiscDoorCameraAreaTextBox));
    strcpy(outdoor_building, ituTextGetString(settingMiscDoorCameraBuildingTextBox));
    strcpy(outdoor_unit, ituTextGetString(settingMiscDoorCameraUnitTextBox));
    strcpy(outdoor_floor, ituTextGetString(settingMiscDoorCameraFloorTextBox));
    strcpy(outdoor_room, ituTextGetString(settingMiscDoorCameraRoomTextBox));
    strcpy(outdoor_ext, ituTextGetString(settingMiscDoorCameraExtTextBox));

    if (!AddressBookIsOutdoorExist(
        outdoor_area,
        outdoor_building,
        outdoor_unit,
        outdoor_floor,
        outdoor_room,
        outdoor_ext))
    {
        sprintf(reason, "Address not exist.");
        goto error;
    }

    if (DeviceInfoIsInEngineerMode())
        ip = strdup(theConfig.outdoor_ipaddr);
    else
        ip = AddressBookGetDeviceIP(outdoor_area, outdoor_building, outdoor_unit, outdoor_floor, outdoor_room, outdoor_ext);

    if (!ip)
    {
        sprintf(reason, "IP not exist.");
        goto error;
    }

    RemoteGetOutdoorInfo(ip, GetOutdoorInfoCallback);
    ret = itpSemWaitTimeout(&settingMiscDoorCameraServerInfoSem, 15000);
    if (ret)
    {
        sprintf(reason, "Get outdoor info timeout: %d", ret);
        goto error;
    }

    verStr = AddressBookGetVersion();
    if (verStr)
    {
        ver = atoi(verStr);
        free(verStr);
    }

    if (settingMiscDoorCameraServerInfo.addressBookVersion != ver)
    {
        sprintf(reason, "DoorCamera addressbook ver %d != %d", settingMiscDoorCameraServerInfo.addressBookVersion, ver);
        goto error;
    }

    ret = RemoteSetOutdoorSetting(ip, outdoor_area, outdoor_building, outdoor_unit, outdoor_floor, outdoor_room, outdoor_ext);
    if (ret)
    {
        sprintf(reason, "Curl error: %d", ret);
        goto error;
    }

    free(ip);

    return true;

error:

    free(ip);

    ituWidgetDisable(settingMiscDoorCameraBackground);
    ituWidgetSetVisible(settingMiscDoorCameraErrorBackgroundButton, true);
    ituTextSetString(settingMiscDoorCameraErrorText, reason);
    printf("Error: %s", reason);
    activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraAreaTextBox;
    ituTextBoxSetString(settingMiscDoorCameraAreaTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraBuildingTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraUnitTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraFloorTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraRoomTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraExtTextBox, NULL);

    return false;
}

bool SettingMiscDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = param;


    char* str = ituTextGetString(activeSettingMiscDoorCameraRoomAddrTextBox);
    int count = str ? strlen(str) : 0;

    if (count >= activeSettingMiscDoorCameraRoomAddrTextBox->maxLen)
    {
        if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraAreaTextBox)
            activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraBuildingTextBox;
        else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraBuildingTextBox)
            activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraUnitTextBox;
        else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraUnitTextBox)
            activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraFloorTextBox;
        else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraFloorTextBox)
            activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraRoomTextBox;
        else if (activeSettingMiscDoorCameraRoomAddrTextBox == settingMiscDoorCameraRoomTextBox)
            activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraExtTextBox;
    }
    ituTextBoxInput(activeSettingMiscDoorCameraRoomAddrTextBox, input);
    return true;
}

bool SettingMiscDoorCameraOnEnter(ITUWidget* widget, char* param)
{
    if (!settingMiscDoorCameraBackground)
    {
        settingMiscDoorCameraBackground = ituSceneFindWidget(&theScene, "settingMiscDoorCameraBackground");
        assert(settingMiscDoorCameraBackground);

        settingMiscDoorCameraKeyboard = ituSceneFindWidget(&theScene, "settingMiscDoorCameraKeyboard");
        assert(settingMiscDoorCameraKeyboard);

        settingMiscDoorCameraAreaTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraAreaTextBox");
        assert(settingMiscDoorCameraAreaTextBox);

        settingMiscDoorCameraBuildingTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraBuildingTextBox");
        assert(settingMiscDoorCameraBuildingTextBox);

        settingMiscDoorCameraUnitTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraUnitTextBox");
        assert(settingMiscDoorCameraUnitTextBox);

        settingMiscDoorCameraFloorTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraFloorTextBox");
        assert(settingMiscDoorCameraFloorTextBox);

        settingMiscDoorCameraRoomTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraRoomTextBox");
        assert(settingMiscDoorCameraRoomTextBox);

        settingMiscDoorCameraExtTextBox = ituSceneFindWidget(&theScene, "settingMiscDoorCameraExtTextBox");
        assert(settingMiscDoorCameraExtTextBox);

        settingMiscDoorCameraErrorBackgroundButton = ituSceneFindWidget(&theScene, "settingMiscDoorCameraErrorBackgroundButton");
        assert(settingMiscDoorCameraErrorBackgroundButton);

        settingMiscDoorCameraErrorText = ituSceneFindWidget(&theScene, "settingMiscDoorCameraErrorText");
        assert(settingMiscDoorCameraErrorText);

        sem_init(&settingMiscDoorCameraServerInfoSem, 0, 0);
    }

    activeSettingMiscDoorCameraRoomAddrTextBox = settingMiscDoorCameraAreaTextBox;

    ituTextBoxSetString(settingMiscDoorCameraAreaTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraBuildingTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraUnitTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraFloorTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraRoomTextBox, NULL);
    ituTextBoxSetString(settingMiscDoorCameraExtTextBox, NULL);
    ituTextSetString(settingMiscDoorCameraErrorText, NULL);

    memset(&settingMiscDoorCameraServerInfo, 0, sizeof(ServerInfo));

    return true;
}

void SettingMiscDoorCameraReset(void)
{
    settingMiscDoorCameraBackground = NULL;
    sem_destroy(&settingMiscDoorCameraServerInfoSem);
}
