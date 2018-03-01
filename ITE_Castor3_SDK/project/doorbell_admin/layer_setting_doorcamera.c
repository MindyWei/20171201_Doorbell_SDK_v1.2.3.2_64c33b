#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define MAX_INTERCOM_COUNT 99

static ITUKeyboard* settingDoorCameraKeyboard;
static ITUBackground* settingDoorCameraRoomAddrBackground;
static ITUTextBox* settingDoorCameraAreaTextBox;
static ITUTextBox* settingDoorCameraBuildingTextBox;
static ITUTextBox* settingDoorCameraUnitTextBox;
static ITUTextBox* settingDoorCameraFloorTextBox;
static ITUTextBox* settingDoorCameraRoomTextBox;
static ITUTextBox* settingDoorCameraExtTextBox;
static ITUBackground* settingDoorCameraAliasBackground;
static ITUText* settingDoorCameraAliasText;
static ITUTextBox* settingDoorCameraRingVolTextBox;
static ITUTextBox* settingDoorCameraPlayVolTextBox;
static ITUTextBox* settingDoorCameraRecVolTextBox;
static ITUTextBox* settingDoorCameraDialTTextBox;
static ITUTextBox* settingDoorCameraCallTTextBox;
static ITURadioBox* settingDoorCameraGuardYesRadioBox;
static ITURadioBox* settingDoorCameraGuardNoRadioBox;
static ITUTextBox* settingDoorCameraGuardTTextBox;
static ITUBackground* settingDoorCameraMainBackground;
static ITUBackground* settingDoorCameraTimeBackground;
static ITUBackground* settingDoorCameraGuardBackground;
static ITUButton* settingDoorCameraPrevButton;
static ITUButton* settingDoorCameraNextButton;

// status
static ServerInfo settingDoorCameraIntercomServerInfo;
static DeviceInfo settingDoorCameraIntercomDeviceInfoArray[MAX_INTERCOM_COUNT], settingDoorCameraIntercomDeviceInfoDefault, *settingDoorCameraIntercomDeviceInfo;
static int settingDoorCameraIntercomInfoIndex;
static int settingDoorCameraIntercomInfoCount;
static bool settingDoorCameraIntercomInfoFilled;
static ITUTextBox* activeSettingDoorCameraRoomAddrTextBox;

bool SettingDoorCameraRoomAddrBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activeSettingDoorCameraRoomAddrTextBox, NULL);

    if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraExtTextBox)
    {
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraRoomTextBox;
    }
    else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraRoomTextBox)
    {
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraFloorTextBox;
    }
    else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraFloorTextBox)
    {
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraUnitTextBox;
    }
    else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraUnitTextBox)
    {
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraBuildingTextBox;
    }
    else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraBuildingTextBox)
    {
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraAreaTextBox;
    }
    return true;
}

static void GetIntercomInfoCallback(ServerInfo* info)
{
    memcpy(&settingDoorCameraIntercomServerInfo ,info, sizeof(ServerInfo));
    settingDoorCameraIntercomInfoFilled = true;
}

bool SettingDoorCameraPrevButtonOnPress(ITUWidget* widget, char* param)
{
    if (settingDoorCameraIntercomInfoIndex <= 0)
        return false;

    settingDoorCameraIntercomInfoIndex--;

    settingDoorCameraIntercomDeviceInfo = &settingDoorCameraIntercomDeviceInfoArray[settingDoorCameraIntercomInfoIndex];

    memset(&settingDoorCameraIntercomServerInfo, 0, sizeof(ServerInfo));

    settingDoorCameraIntercomInfoFilled = false;
    RemoteGetIntercomInfo(settingDoorCameraIntercomDeviceInfo->ip, GetIntercomInfoCallback);

    ituTextSetString(settingDoorCameraAliasText, settingDoorCameraIntercomDeviceInfo->alias);

    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituRadioBoxSetChecked(settingDoorCameraGuardYesRadioBox, false);
    ituRadioBoxSetChecked(settingDoorCameraGuardNoRadioBox, false);
    ituTextBoxSetString(settingDoorCameraGuardTTextBox, NULL);

    return true;
}

bool SettingDoorCameraNextButtonOnPress(ITUWidget* widget, char* param)
{
    if (settingDoorCameraIntercomInfoIndex >= settingDoorCameraIntercomInfoCount - 1)
        return false;

    settingDoorCameraIntercomInfoIndex++;

    settingDoorCameraIntercomDeviceInfo = &settingDoorCameraIntercomDeviceInfoArray[settingDoorCameraIntercomInfoIndex];

    memset(&settingDoorCameraIntercomServerInfo, 0, sizeof(ServerInfo));

    settingDoorCameraIntercomInfoFilled = false;
    RemoteGetIntercomInfo(settingDoorCameraIntercomDeviceInfo->ip, GetIntercomInfoCallback);

    ituTextSetString(settingDoorCameraAliasText, settingDoorCameraIntercomDeviceInfo->alias);

    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituRadioBoxSetChecked(settingDoorCameraGuardYesRadioBox, false);
    ituRadioBoxSetChecked(settingDoorCameraGuardNoRadioBox, false);
    ituTextBoxSetString(settingDoorCameraGuardTTextBox, NULL);

    return true;
}

bool SettingDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingDoorCameraMainBackground))
    {
        if (ituWidgetIsVisible(settingDoorCameraRoomAddrBackground) && settingDoorCameraIntercomDeviceInfo)
        {
            int ret, ver = -1;
            char* verStr;
            char intercom_area[4], intercom_building[4], intercom_unit[4], intercom_floor[4], intercom_room[4], intercom_ext[4], reason[128];
            
            reason[0] = '\0';

            strcpy(intercom_area, ituTextGetString(settingDoorCameraAreaTextBox));
            strcpy(intercom_building, ituTextGetString(settingDoorCameraBuildingTextBox));
            strcpy(intercom_unit, ituTextGetString(settingDoorCameraUnitTextBox));
            strcpy(intercom_floor, ituTextGetString(settingDoorCameraFloorTextBox));
            strcpy(intercom_room, ituTextGetString(settingDoorCameraRoomTextBox));
            strcpy(intercom_ext, ituTextGetString(settingDoorCameraExtTextBox));

            if (!AddressBookIsIntercomExist(
                intercom_area,
                intercom_building,
                intercom_unit,
                intercom_floor,
                intercom_room,
                intercom_ext))
            {
                sprintf(reason, "Address not exist.");
                goto error;
            }

            verStr = AddressBookGetVersion();
            if (verStr)
            {
                ver = atoi(verStr);
                free(verStr);
            }

            if (settingDoorCameraIntercomServerInfo.addressBookVersion != ver)
            {
                sprintf(reason, "Intercom addressbook ver %d != %d", settingDoorCameraIntercomServerInfo.addressBookVersion, ver);
                goto error;
            }

            ret = RemoteSetIntercomSetting(settingDoorCameraIntercomDeviceInfo->ip, intercom_area, intercom_building, intercom_unit, intercom_floor, intercom_room, intercom_ext);
            if (ret)
            {
                sprintf(reason, "Curl error: %d", ret);
                goto error;
            }
        error:
            if (reason[0] != '\0')
            {
                printf("Error: %s", reason);
                ituTextBoxSetString(settingDoorCameraAreaTextBox, NULL);
                ituTextBoxSetString(settingDoorCameraBuildingTextBox, NULL);
                ituTextBoxSetString(settingDoorCameraUnitTextBox, NULL);
                ituTextBoxSetString(settingDoorCameraFloorTextBox, NULL);
                ituTextBoxSetString(settingDoorCameraRoomTextBox, NULL);
                ituTextBoxSetString(settingDoorCameraExtTextBox, NULL);
            }
            return true;
        }
    }
    else if (ituWidgetIsVisible(settingDoorCameraTimeBackground))
    {
        char* ringVolStr = ituTextBoxGetString(settingDoorCameraRingVolTextBox);
        char* playVolStr = ituTextBoxGetString(settingDoorCameraPlayVolTextBox);
        char* recVolStr = ituTextBoxGetString(settingDoorCameraRecVolTextBox);
        char* dialTimeStr = ituTextBoxGetString(settingDoorCameraDialTTextBox);
        char* callingTimeStr = ituTextBoxGetString(settingDoorCameraCallTTextBox);
        int ringVol = ringVolStr ? atoi(ringVolStr) : -1;
        int playVol = playVolStr ? atoi(playVolStr) : -1;
        int recVol = recVolStr ? atoi(recVolStr) : -1;
        int dialTime = dialTimeStr ? atoi(dialTimeStr) : -1;
        int callingTime = callingTimeStr ? atoi(callingTimeStr) : -1;

        if (settingDoorCameraIntercomDeviceInfo)
        {
            bool pass = false;

            if (ringVol >= 0 && ringVol <= 5 &&
                playVol >= 0 && playVol <= 5 &&
                recVol >= 0 && recVol <= 5 &&
                dialTime >= 15 && dialTime <= 120 &&
                callingTime >= 15 && callingTime <= 120)
            {
                ringVol = ringVol * 100 / 5;
                playVol = playVol * 100 / 5;
                recVol = recVol * 100 / 5;

                RemoteSetIntercomConfig(settingDoorCameraIntercomDeviceInfo->ip, ringVol, playVol, recVol, dialTime, callingTime, settingDoorCameraIntercomServerInfo.guardTime);
                settingDoorCameraIntercomServerInfo.ringLevel = ringVol;
                settingDoorCameraIntercomServerInfo.playLevel = playVol;
                settingDoorCameraIntercomServerInfo.recLevel = recVol;
                settingDoorCameraIntercomServerInfo.dialTime = dialTime;
                settingDoorCameraIntercomServerInfo.callingTime = callingTime;
            }
            else
            {
                if (ringVol < 0 || ringVol > 5)
                    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);

                if (playVol < 0 || playVol > 5)
                    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);

                if (recVol < 0 || recVol > 5)
                    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);

                if (dialTime < 15 || dialTime > 120)
                    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);

                if (callingTime < 15 || callingTime > 120)
                    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
            }
        }
    }
    else if (ituWidgetIsVisible(settingDoorCameraGuardBackground))
    {
        char* guardTimeStr = ituTextBoxGetString(settingDoorCameraGuardTTextBox);
        int guardTime = guardTimeStr ? atoi(guardTimeStr) : -1;

        if (settingDoorCameraIntercomDeviceInfo)
        {
            if (ituRadioBoxIsChecked(settingDoorCameraGuardNoRadioBox))
            {
                guardTime = 0;
                RemoteSetIntercomConfig(settingDoorCameraIntercomDeviceInfo->ip, 
                    settingDoorCameraIntercomServerInfo.ringLevel, 
                    settingDoorCameraIntercomServerInfo.playLevel, 
                    settingDoorCameraIntercomServerInfo.recLevel, 
                    settingDoorCameraIntercomServerInfo.dialTime, 
                    settingDoorCameraIntercomServerInfo.callingTime, 
                    guardTime);
                settingDoorCameraIntercomServerInfo.guardTime = guardTime;
            }
            else if (guardTime >= 15 && guardTime <= 120)
            {
                RemoteSetIntercomConfig(settingDoorCameraIntercomDeviceInfo->ip, 
                    settingDoorCameraIntercomServerInfo.ringLevel, 
                    settingDoorCameraIntercomServerInfo.playLevel, 
                    settingDoorCameraIntercomServerInfo.recLevel, 
                    settingDoorCameraIntercomServerInfo.dialTime, 
                    settingDoorCameraIntercomServerInfo.callingTime, 
                    guardTime);
                settingDoorCameraIntercomServerInfo.guardTime = guardTime;
            }
            else
            {
                ituTextBoxSetString(settingDoorCameraGuardTTextBox, NULL);
            }
        }
    }
    return true;
}

bool SettingDoorCameraCancelButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[32];
    int value;

    if (settingDoorCameraIntercomDeviceInfo == NULL)
        return false;

    if (ituWidgetIsVisible(settingDoorCameraMainBackground))
    {
        if (ituWidgetIsVisible(settingDoorCameraRoomAddrBackground))
        {
            ituTextBoxSetString(settingDoorCameraAreaTextBox, settingDoorCameraIntercomDeviceInfo->area);
            ituTextBoxSetString(settingDoorCameraBuildingTextBox, settingDoorCameraIntercomDeviceInfo->building);
            ituTextBoxSetString(settingDoorCameraUnitTextBox, settingDoorCameraIntercomDeviceInfo->unit);
            ituTextBoxSetString(settingDoorCameraFloorTextBox, settingDoorCameraIntercomDeviceInfo->floor);
            ituTextBoxSetString(settingDoorCameraRoomTextBox, settingDoorCameraIntercomDeviceInfo->room);
            ituTextBoxSetString(settingDoorCameraExtTextBox, settingDoorCameraIntercomDeviceInfo->ext);
        }
    }
    else if (ituWidgetIsVisible(settingDoorCameraTimeBackground))
    {
        value = settingDoorCameraIntercomServerInfo.ringLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRingVolTextBox, buf);

        value = settingDoorCameraIntercomServerInfo.playLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraPlayVolTextBox, buf);

        value = settingDoorCameraIntercomServerInfo.recLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRecVolTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.dialTime);
        ituTextBoxSetString(settingDoorCameraDialTTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.callingTime);
        ituTextBoxSetString(settingDoorCameraCallTTextBox, buf);
    }
    else if (ituWidgetIsVisible(settingDoorCameraGuardBackground))
    {
        if (settingDoorCameraIntercomServerInfo.guardTime > 0)
        {
            ituRadioBoxSetChecked(settingDoorCameraGuardYesRadioBox, true);

            sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.guardTime);
            ituTextBoxSetString(settingDoorCameraGuardTTextBox, buf);
        }
        else
        {
            ituRadioBoxSetChecked(settingDoorCameraGuardNoRadioBox, true);
        }
    }
    return true;
}

bool SettingDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = param;

    if (settingDoorCameraIntercomDeviceInfo == NULL)
        return false;

    if (ituWidgetIsVisible(settingDoorCameraMainBackground))
    {
        if (ituWidgetIsVisible(settingDoorCameraRoomAddrBackground))
        {
            char* str = ituTextGetString(activeSettingDoorCameraRoomAddrTextBox);
            int count = str ? strlen(str) : 0;

            if (count >= activeSettingDoorCameraRoomAddrTextBox->maxLen)
            {
                if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraAreaTextBox)
                    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraBuildingTextBox;
                else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraBuildingTextBox)
                    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraUnitTextBox;
                else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraUnitTextBox)
                    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraFloorTextBox;
                else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraFloorTextBox)
                    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraRoomTextBox;
                else if (activeSettingDoorCameraRoomAddrTextBox == settingDoorCameraRoomTextBox)
                    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraExtTextBox;
            }
            ituTextBoxInput(activeSettingDoorCameraRoomAddrTextBox, input);
        }
    }
    else if (ituWidgetIsVisible(settingDoorCameraTimeBackground))
    {
        ituTextBoxInput((ITUTextBox*)settingDoorCameraKeyboard->target, input);
    }
    else if (ituWidgetIsVisible(settingDoorCameraGuardBackground))
    {
        ituTextBoxInput(settingDoorCameraGuardTTextBox, input);
    }
    return true;
}

bool SettingDoorCameraOnTimer(ITUWidget* widget, char* param)
{
    if (settingDoorCameraIntercomInfoFilled)
    {
        int value;
        char buf[32];
        settingDoorCameraIntercomInfoFilled = false;

        value = settingDoorCameraIntercomServerInfo.ringLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRingVolTextBox, buf);

        value = settingDoorCameraIntercomServerInfo.playLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraPlayVolTextBox, buf);

        value = settingDoorCameraIntercomServerInfo.recLevel * 5 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRecVolTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.dialTime);
        ituTextBoxSetString(settingDoorCameraDialTTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.callingTime);
        ituTextBoxSetString(settingDoorCameraCallTTextBox, buf);

        if (settingDoorCameraIntercomServerInfo.guardTime > 0)
        {
            ituRadioBoxSetChecked(settingDoorCameraGuardYesRadioBox, true);

            sprintf(buf, "%d", settingDoorCameraIntercomServerInfo.guardTime);
            ituTextBoxSetString(settingDoorCameraGuardTTextBox, buf);
        }
        else
        {
            ituRadioBoxSetChecked(settingDoorCameraGuardNoRadioBox, true);
        }
    }
    return true;
}

bool SettingDoorCameraOnEnter(ITUWidget* widget, char* param)
{
    if (!settingDoorCameraKeyboard)
    {
        settingDoorCameraKeyboard = ituSceneFindWidget(&theScene, "settingDoorCameraKeyboard");
        assert(settingDoorCameraKeyboard);

        settingDoorCameraRoomAddrBackground = ituSceneFindWidget(&theScene, "settingDoorCameraRoomAddrBackground");
        assert(settingDoorCameraRoomAddrBackground);

        settingDoorCameraAreaTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraAreaTextBox");
        assert(settingDoorCameraAreaTextBox);

        settingDoorCameraBuildingTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraBuildingTextBox");
        assert(settingDoorCameraBuildingTextBox);

        settingDoorCameraUnitTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraUnitTextBox");
        assert(settingDoorCameraUnitTextBox);

        settingDoorCameraFloorTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraFloorTextBox");
        assert(settingDoorCameraFloorTextBox);

        settingDoorCameraRoomTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraRoomTextBox");
        assert(settingDoorCameraRoomTextBox);

        settingDoorCameraExtTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraExtTextBox");
        assert(settingDoorCameraExtTextBox);

        settingDoorCameraAliasBackground = ituSceneFindWidget(&theScene, "settingDoorCameraAliasBackground");
        assert(settingDoorCameraAliasBackground);

        settingDoorCameraAliasText = ituSceneFindWidget(&theScene, "settingDoorCameraAliasText");
        assert(settingDoorCameraAliasText);

        settingDoorCameraRingVolTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraRingVolTextBox");
        assert(settingDoorCameraRingVolTextBox);

        settingDoorCameraPlayVolTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraPlayVolTextBox");
        assert(settingDoorCameraPlayVolTextBox);

        settingDoorCameraRecVolTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraRecVolTextBox");
        assert(settingDoorCameraRecVolTextBox);

        settingDoorCameraDialTTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraDialTTextBox");
        assert(settingDoorCameraDialTTextBox);

        settingDoorCameraCallTTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraCallTTextBox");
        assert(settingDoorCameraCallTTextBox);
        
        settingDoorCameraGuardYesRadioBox = ituSceneFindWidget(&theScene, "settingDoorCameraGuardYesRadioBox");
        assert(settingDoorCameraGuardYesRadioBox);        
        
        settingDoorCameraGuardNoRadioBox = ituSceneFindWidget(&theScene, "settingDoorCameraGuardNoRadioBox");
        assert(settingDoorCameraGuardNoRadioBox);        
        
        settingDoorCameraGuardTTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraGuardTTextBox");
        assert(settingDoorCameraGuardTTextBox);                        

        settingDoorCameraMainBackground = ituSceneFindWidget(&theScene, "settingDoorCameraMainBackground");
        assert(settingDoorCameraMainBackground);                        

        settingDoorCameraTimeBackground = ituSceneFindWidget(&theScene, "settingDoorCameraTimeBackground");
        assert(settingDoorCameraTimeBackground);                        

        settingDoorCameraGuardBackground = ituSceneFindWidget(&theScene, "settingDoorCameraGuardBackground");
        assert(settingDoorCameraGuardBackground);                        

        settingDoorCameraPrevButton = ituSceneFindWidget(&theScene, "settingDoorCameraPrevButton");
        assert(settingDoorCameraPrevButton);                        

        settingDoorCameraNextButton = ituSceneFindWidget(&theScene, "settingDoorCameraNextButton");
        assert(settingDoorCameraNextButton);                        
    }

    memset(&settingDoorCameraIntercomServerInfo, 0, sizeof(ServerInfo));
    settingDoorCameraIntercomInfoIndex = -1;
    settingDoorCameraIntercomInfoFilled = false;
    settingDoorCameraIntercomDeviceInfo = NULL;
    activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraAreaTextBox;

    ituTextBoxSetString(settingDoorCameraAreaTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraBuildingTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraUnitTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraFloorTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRoomTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraExtTextBox, NULL);

    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituRadioBoxSetChecked(settingDoorCameraGuardYesRadioBox, false);
    ituRadioBoxSetChecked(settingDoorCameraGuardNoRadioBox, false);
    ituTextBoxSetString(settingDoorCameraGuardTTextBox, NULL);

    if (theConfig.area[0] == '\0' && theConfig.building[0] == '\0' && theConfig.unit[0] == '\0' && theConfig.floor[0] == '\0' && theConfig.room[0] == '\0' && theConfig.ext[0] == '\0')
    {
        ituWidgetSetVisible(settingDoorCameraPrevButton, false);
        ituWidgetSetVisible(settingDoorCameraNextButton, false);
        ituWidgetSetVisible(settingDoorCameraRoomAddrBackground, true);
        ituWidgetSetVisible(settingDoorCameraAliasBackground, false);

        AddressBookGetDeviceInfo(&settingDoorCameraIntercomDeviceInfoDefault, theConfig.intercom_ipaddr);
        settingDoorCameraIntercomDeviceInfo = &settingDoorCameraIntercomDeviceInfoDefault;
        RemoteGetIntercomInfo(settingDoorCameraIntercomDeviceInfo->ip, GetIntercomInfoCallback);

        ituTextBoxSetString(settingDoorCameraAreaTextBox, settingDoorCameraIntercomDeviceInfo->area);
        ituTextBoxSetString(settingDoorCameraBuildingTextBox, settingDoorCameraIntercomDeviceInfo->building);
        ituTextBoxSetString(settingDoorCameraUnitTextBox, settingDoorCameraIntercomDeviceInfo->unit);
        ituTextBoxSetString(settingDoorCameraFloorTextBox, settingDoorCameraIntercomDeviceInfo->floor);
        ituTextBoxSetString(settingDoorCameraRoomTextBox, settingDoorCameraIntercomDeviceInfo->room);
        ituTextBoxSetString(settingDoorCameraExtTextBox, settingDoorCameraIntercomDeviceInfo->ext);
        activeSettingDoorCameraRoomAddrTextBox = settingDoorCameraExtTextBox;
    }
    else
    {
        ituWidgetSetVisible(settingDoorCameraPrevButton, true);
        ituWidgetSetVisible(settingDoorCameraNextButton, true);
        ituWidgetSetVisible(settingDoorCameraRoomAddrBackground, false);
        ituWidgetSetVisible(settingDoorCameraAliasBackground, true);

        settingDoorCameraIntercomInfoCount = AddressBookGetIntercomInfoArray(settingDoorCameraIntercomDeviceInfoArray, MAX_INTERCOM_COUNT);
        if (settingDoorCameraIntercomInfoCount > 0)
        {
            settingDoorCameraIntercomInfoIndex = 0;
            settingDoorCameraIntercomDeviceInfo = &settingDoorCameraIntercomDeviceInfoArray[settingDoorCameraIntercomInfoIndex];
            RemoteGetIntercomInfo(settingDoorCameraIntercomDeviceInfo->ip, GetIntercomInfoCallback);

            ituTextSetString(settingDoorCameraAliasText, settingDoorCameraIntercomDeviceInfo->alias);
        }
    }
    return true;
}

bool SettingDoorCameraOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void SettingDoorCameraReset(void)
{
    settingDoorCameraKeyboard = NULL;
}
