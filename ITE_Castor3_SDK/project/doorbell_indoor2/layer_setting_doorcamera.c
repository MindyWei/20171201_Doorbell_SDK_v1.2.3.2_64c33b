#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define MAX_DOORCAMERA_COUNT 99

static ITUText* settingDoorCameraTypeText;
static ITUText* settingDoorCameraRoomAddrText;
static ITUText* settingDoorCameraIPAddrText;
static ITUText* settingDoorCameraAddressBookVersionText;
static ITUText* settingDoorCameraSWVersionText;
static ITUTextBox* settingDoorCameraRingVolTextBox;
static ITUTextBox* settingDoorCameraPlayVolTextBox;
static ITUTextBox* settingDoorCameraRecVolTextBox;
static ITUTextBox* settingDoorCameraDialTTextBox;
static ITUTextBox* settingDoorCameraCallTTextBox;
static ITUTextBox* settingDoorCameraMemoTTextBox;
static ITUKeyboard* settingDoorCameraKeyboard;

// status
static ServerInfo settingDoorCameraServerInfo;
static DeviceInfo settingDoorCameraDeviceInfoArray[MAX_DOORCAMERA_COUNT], settingDoorCameraDeviceInfoDefault, *settingDoorCameraDeviceInfo;
static int settingDoorCameraInfoIndex;
static int settingDoorCameraInfoCount;
static bool settingDoorCameraInfoFilled;
static ITUTextBox* activeSettingDoorCameraRoomAddrTextBox;

static void GetOutdoorInfoCallback(ServerInfo* info)
{
    memcpy(&settingDoorCameraServerInfo ,info, sizeof(ServerInfo));
    settingDoorCameraInfoFilled = true;
}

bool SettingDoorCameraPrevButtonOnPress(ITUWidget* widget, char* param)
{
    char* str;

    if (settingDoorCameraInfoIndex <= 0)
        return false;

    settingDoorCameraInfoIndex--;

    settingDoorCameraDeviceInfo = &settingDoorCameraDeviceInfoArray[settingDoorCameraInfoIndex];

    memset(&settingDoorCameraServerInfo, 0, sizeof(ServerInfo));

    settingDoorCameraInfoFilled = false;
    RemoteGetOutdoorInfo(settingDoorCameraDeviceInfo->ip, GetOutdoorInfoCallback);

    str = (char*)StringGetDeviceType(settingDoorCameraDeviceInfo->type);
    ituTextSetString(settingDoorCameraTypeText, str);

    str = StringGetRoomAddress(settingDoorCameraDeviceInfo->area, settingDoorCameraDeviceInfo->building, settingDoorCameraDeviceInfo->unit, settingDoorCameraDeviceInfo->floor, settingDoorCameraDeviceInfo->room, settingDoorCameraDeviceInfo->ext);
    ituTextSetString(settingDoorCameraRoomAddrText, str);
    free(str);

    ituTextSetString(settingDoorCameraIPAddrText, settingDoorCameraDeviceInfo->ip);

    ituTextSetString(settingDoorCameraAddressBookVersionText, NULL);
    ituTextSetString(settingDoorCameraSWVersionText, NULL);
    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraMemoTTextBox, NULL);

    return true;
}

bool SettingDoorCameraNextButtonOnPress(ITUWidget* widget, char* param)
{
    char* str;

    if (settingDoorCameraInfoIndex >= settingDoorCameraInfoCount - 1)
        return false;

    settingDoorCameraInfoIndex++;

    settingDoorCameraDeviceInfo = &settingDoorCameraDeviceInfoArray[settingDoorCameraInfoIndex];

    memset(&settingDoorCameraServerInfo, 0, sizeof(ServerInfo));

    settingDoorCameraInfoFilled = false;
    RemoteGetOutdoorInfo(settingDoorCameraDeviceInfo->ip, GetOutdoorInfoCallback);

    str = (char*)StringGetDeviceType(settingDoorCameraDeviceInfo->type);
    ituTextSetString(settingDoorCameraTypeText, str);

    str = StringGetRoomAddress(settingDoorCameraDeviceInfo->area, settingDoorCameraDeviceInfo->building, settingDoorCameraDeviceInfo->unit, settingDoorCameraDeviceInfo->floor, settingDoorCameraDeviceInfo->room, settingDoorCameraDeviceInfo->ext);
    ituTextSetString(settingDoorCameraRoomAddrText, str);
    free(str);

    ituTextSetString(settingDoorCameraIPAddrText, settingDoorCameraDeviceInfo->ip);

    ituTextSetString(settingDoorCameraAddressBookVersionText, NULL);
    ituTextSetString(settingDoorCameraSWVersionText, NULL);
    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraMemoTTextBox, NULL);

    return true;
}

bool SettingDoorCameraConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* ringVolStr = ituTextBoxGetString(settingDoorCameraRingVolTextBox);
    char* playVolStr = ituTextBoxGetString(settingDoorCameraPlayVolTextBox);
    char* recVolStr = ituTextBoxGetString(settingDoorCameraRecVolTextBox);
    char* dialTimeStr = ituTextBoxGetString(settingDoorCameraDialTTextBox);
    char* callingTimeStr = ituTextBoxGetString(settingDoorCameraCallTTextBox);
    char* voiceMemoTimeStr = ituTextBoxGetString(settingDoorCameraMemoTTextBox);
    int ringVol = ringVolStr ? atoi(ringVolStr) : -1;
    int playVol = playVolStr ? atoi(playVolStr) : -1;
    int recVol = recVolStr ? atoi(recVolStr) : -1;
    int dialTime = dialTimeStr ? atoi(dialTimeStr) : -1;
    int callingTime = callingTimeStr ? atoi(callingTimeStr) : -1;
    int voiceMemoTime = voiceMemoTimeStr ? atoi(voiceMemoTimeStr) : -1;

    if (settingDoorCameraDeviceInfo)
    {
        bool pass = false;

        if (ringVol >= 0 && ringVol <= 10 &&
            playVol >= 0 && playVol <= 10 &&
            recVol >= 0 && recVol <= 10 &&
            dialTime >= 30 && dialTime <= 60 &&
            voiceMemoTime >= 5 && voiceMemoTime <= 60 &&
            callingTime >= 15 && callingTime <= 120)
        {
            ringVol = ringVol * 100 / 10;
            playVol = playVol * 100 / 10;
            recVol = recVol * 100 / 10;

            RemoteSetOutdoorConfig(settingDoorCameraDeviceInfo->ip, ringVol, playVol, recVol, dialTime, voiceMemoTime, callingTime);
            settingDoorCameraServerInfo.ringLevel = ringVol;
            settingDoorCameraServerInfo.playLevel = playVol;
            settingDoorCameraServerInfo.recLevel = recVol;
            settingDoorCameraServerInfo.dialTime = dialTime;
            settingDoorCameraServerInfo.voiceMemoTime = voiceMemoTime;
            settingDoorCameraServerInfo.callingTime = callingTime;
        }
        else
        {
            if (ringVol < 0 || ringVol > 10)
                ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);

            if (playVol < 0 || playVol > 10)
                ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);

            if (recVol < 0 || recVol > 10)
                ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);

            if (dialTime < 30 || dialTime > 60)
                ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);

            if (voiceMemoTime < 5 || voiceMemoTime > 60)
                ituTextBoxSetString(settingDoorCameraMemoTTextBox, NULL);

            if (callingTime < 15 || callingTime > 120)
                ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
        }
    }
    return true;
}

bool SettingDoorCameraCancelButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[32];
    int value;

    if (settingDoorCameraDeviceInfo == NULL)
        return false;

    value = settingDoorCameraServerInfo.ringLevel * 10 / 100;
    sprintf(buf, "%d", value);
    ituTextBoxSetString(settingDoorCameraRingVolTextBox, buf);

    value = settingDoorCameraServerInfo.playLevel * 10 / 100;
    sprintf(buf, "%d", value);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, buf);

    value = settingDoorCameraServerInfo.recLevel * 10 / 100;
    sprintf(buf, "%d", value);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, buf);

    sprintf(buf, "%d", settingDoorCameraServerInfo.dialTime);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, buf);

    sprintf(buf, "%d", settingDoorCameraServerInfo.callingTime);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, buf);

    sprintf(buf, "%d", settingDoorCameraServerInfo.voiceMemoTime);
    ituTextBoxSetString(settingDoorCameraMemoTTextBox, buf);

    return true;
}

bool SettingDoorCameraNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = param;

    if (settingDoorCameraDeviceInfo == NULL)
        return false;

    ituTextBoxInput((ITUTextBox*)settingDoorCameraKeyboard->target, input);

    return true;
}

bool SettingDoorCameraOnTimer(ITUWidget* widget, char* param)
{
    if (settingDoorCameraInfoFilled)
    {
        int value;
        char buf[32];
        settingDoorCameraInfoFilled = false;

        sprintf(buf, "%d", settingDoorCameraServerInfo.addressBookVersion);
        ituTextSetString(settingDoorCameraAddressBookVersionText, buf);

        ituTextSetString(settingDoorCameraAddressBookVersionText, settingDoorCameraServerInfo.firmwareVersion);

        value = settingDoorCameraServerInfo.ringLevel * 10 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRingVolTextBox, buf);

        value = settingDoorCameraServerInfo.playLevel * 10 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraPlayVolTextBox, buf);

        value = settingDoorCameraServerInfo.recLevel * 10 / 100;
        sprintf(buf, "%d", value);
        ituTextBoxSetString(settingDoorCameraRecVolTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraServerInfo.dialTime);
        ituTextBoxSetString(settingDoorCameraDialTTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraServerInfo.callingTime);
        ituTextBoxSetString(settingDoorCameraCallTTextBox, buf);

        sprintf(buf, "%d", settingDoorCameraServerInfo.voiceMemoTime);
        ituTextBoxSetString(settingDoorCameraMemoTTextBox, buf);
    }
    return true;
}

bool SettingDoorCameraOnEnter(ITUWidget* widget, char* param)
{
    if (!settingDoorCameraTypeText)
    {
        settingDoorCameraTypeText = ituSceneFindWidget(&theScene, "settingDoorCameraTypeText");
        assert(settingDoorCameraTypeText);

        settingDoorCameraRoomAddrText = ituSceneFindWidget(&theScene, "settingDoorCameraRoomAddrText");
        assert(settingDoorCameraRoomAddrText);

        settingDoorCameraIPAddrText = ituSceneFindWidget(&theScene, "settingDoorCameraIPAddrText");
        assert(settingDoorCameraIPAddrText);

        settingDoorCameraAddressBookVersionText = ituSceneFindWidget(&theScene, "settingDoorCameraAddressBookVersionText");
        assert(settingDoorCameraAddressBookVersionText);

        settingDoorCameraSWVersionText = ituSceneFindWidget(&theScene, "settingDoorCameraSWVersionText");
        assert(settingDoorCameraSWVersionText);

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
       
        settingDoorCameraMemoTTextBox = ituSceneFindWidget(&theScene, "settingDoorCameraMemoTTextBox");
        assert(settingDoorCameraMemoTTextBox);

        settingDoorCameraKeyboard = ituSceneFindWidget(&theScene, "settingDoorCameraKeyboard");
        assert(settingDoorCameraKeyboard);
    }

    memset(&settingDoorCameraServerInfo, 0, sizeof(ServerInfo));
    settingDoorCameraInfoIndex = -1;
    settingDoorCameraInfoFilled = false;
    settingDoorCameraDeviceInfo = NULL;

    ituTextSetString(settingDoorCameraTypeText, NULL);
    ituTextSetString(settingDoorCameraRoomAddrText, NULL);
    ituTextSetString(settingDoorCameraIPAddrText, NULL);
    ituTextSetString(settingDoorCameraAddressBookVersionText, NULL);
    ituTextSetString(settingDoorCameraSWVersionText, NULL);
    ituTextBoxSetString(settingDoorCameraRingVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraPlayVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraRecVolTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraDialTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraCallTTextBox, NULL);
    ituTextBoxSetString(settingDoorCameraMemoTTextBox, NULL);

    settingDoorCameraInfoCount = AddressBookGetOutdoorInfoArray(settingDoorCameraDeviceInfoArray, MAX_DOORCAMERA_COUNT,
        theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room);
    if (settingDoorCameraInfoCount > 0)
    {
        char* str;

        settingDoorCameraInfoIndex = 0;
        settingDoorCameraDeviceInfo = &settingDoorCameraDeviceInfoArray[settingDoorCameraInfoIndex];
        RemoteGetOutdoorInfo(settingDoorCameraDeviceInfo->ip, GetOutdoorInfoCallback);

        str = (char*)StringGetDeviceType(settingDoorCameraDeviceInfo->type);
        ituTextSetString(settingDoorCameraTypeText, str);

        str = StringGetRoomAddress(settingDoorCameraDeviceInfo->area, settingDoorCameraDeviceInfo->building, settingDoorCameraDeviceInfo->unit, settingDoorCameraDeviceInfo->floor, settingDoorCameraDeviceInfo->room, settingDoorCameraDeviceInfo->ext);
        ituTextSetString(settingDoorCameraRoomAddrText, str);
        free(str);

        ituTextSetString(settingDoorCameraIPAddrText, settingDoorCameraDeviceInfo->ip);
    }

    return true;
}

bool SettingDoorCameraOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void SettingDoorCameraReset(void)
{
    settingDoorCameraTypeText = NULL;
}
