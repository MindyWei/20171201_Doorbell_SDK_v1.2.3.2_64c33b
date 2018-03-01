#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* callingLayer;
static ITULayer* calledLayer;
static ITULayer* openDoorLayer;
static ITULayer* passwordLayer;
static ITUBackground* userRoomBackground;
static ITUTextBox* userAreaTextBox;
static ITUTextBox* userBuildingTextBox;
static ITUTextBox* userUnitTextBox;
static ITUTextBox* userFloorTextBox;
static ITUTextBox* userRoomTextBox;
static ITUTextBox* userPasswordTextBox;

static ITUTextBox* userActiveTextBox;
static bool userEnteringPassword;

bool UserCallIncomingOnCustom(ITUWidget* widget, char* param)
{
	ituLayerGoto(calledLayer);
	return true;
}

bool UserKeyStarOnPress(ITUWidget* widget, char* param)
{
    char* str = ituTextGetString(userActiveTextBox);
    int count = str ? strlen(str) : 0;

    LinphoneEnableKeySound();
    LinphonePlayKeySound();
    LinphoneDisableKeySound();

    if (!ituWidgetIsVisible(userPasswordTextBox) && count == 0)
    {
        if ((theDeviceInfo.type == DEVICE_WALL && userActiveTextBox == userBuildingTextBox) ||
            (theDeviceInfo.type == DEVICE_BUILDING && userActiveTextBox == userUnitTextBox) ||
            (theDeviceInfo.type == DEVICE_ENTRANCE && userActiveTextBox == userFloorTextBox))
        {
            ituWidgetSetVisible(userRoomBackground, false);
            ituWidgetSetVisible(userPasswordTextBox, true);
            userActiveTextBox = userPasswordTextBox;
            ituTextBoxInput(userActiveTextBox, "*");
        }
    }
    else if (ituWidgetIsVisible(userPasswordTextBox))
    {
        assert(count >= 1);

        puts(str);

        if (userEnteringPassword)
        {
            userEnteringPassword = false;

            if (strcmp(str, "0000") == 0)
            {
                ituLayerGoto(passwordLayer);
            }
            else
            {
                // password incorrect
                ituWidgetSetVisible(userRoomBackground, true);
                ituWidgetSetVisible(userPasswordTextBox, false);
                ituTextBoxSetString(userPasswordTextBox, NULL);

                if (theDeviceInfo.type == DEVICE_WALL)
                {
                    userActiveTextBox = userBuildingTextBox;
                }
                else if (theDeviceInfo.type == DEVICE_BUILDING)
                {
                    userActiveTextBox = userUnitTextBox;
                }
                else
                {
                    userActiveTextBox = userFloorTextBox;
                }
            }
        }
        else if (count == 1 && str[0] == '*' && param[0] == '*')
        {
            ituTextBoxSetString(userPasswordTextBox, NULL);
            userEnteringPassword = true;
        }
        else if (count >= 2)
        {
            // is local open door password
            if (strcmp(&str[1], theConfig.door_password) == 0)
            {
                // open the door
                time_t t = SnapshotTake();
                SceneOpenDoor(false);
                PeripheralOpenDoor();
                EventWriteOpenDoorLog(EVENT_OPENDOOR_PASSWORD, &theDeviceInfo, true, NULL, &t);
				ituLayerGoto(openDoorLayer);
            }
            else
            {
                // password incorrect
                ituWidgetSetVisible(userRoomBackground, true);
                ituWidgetSetVisible(userPasswordTextBox, false);
                ituTextBoxSetString(userPasswordTextBox, NULL);

                if (theDeviceInfo.type == DEVICE_WALL)
                {
                    userActiveTextBox = userBuildingTextBox;
                }
                else if (theDeviceInfo.type == DEVICE_BUILDING)
                {
                    userActiveTextBox = userUnitTextBox;
                }
                else
                {
                    userActiveTextBox = userFloorTextBox;
                }
                EventWriteOpenDoorLog(EVENT_OPENDOOR_PASSWORD, &theDeviceInfo, false, NULL, NULL);
            }
        }
        else
        {
            ituTextBoxInput(userPasswordTextBox, "*");
        }
    }
    else
    {
        assert(ituWidgetIsVisible(userRoomBackground));

        if (userActiveTextBox == userRoomTextBox && count >= userActiveTextBox->maxLen)
        {
            DeviceInfo info;

            memset(&info, 0, sizeof(DeviceInfo));

            strcpy(info.area, ituTextBoxGetString(userAreaTextBox));
            strcpy(info.building, ituTextBoxGetString(userBuildingTextBox));
            strcpy(info.unit, ituTextBoxGetString(userUnitTextBox));
            strcpy(info.floor, ituTextBoxGetString(userFloorTextBox));
            strcpy(info.room, ituTextBoxGetString(userRoomTextBox));

            if (CallingCall(&info))
            {
                 // calling fail
                 LinphonePlayHintSound(HINT_SOUND_ROOM_NOT_EXIST);

                ituTextBoxSetString(userAreaTextBox, NULL);
                ituTextBoxSetString(userBuildingTextBox, NULL);
                ituTextBoxSetString(userUnitTextBox, NULL);
                ituTextBoxSetString(userFloorTextBox, NULL);
                ituTextBoxSetString(userRoomTextBox, NULL);
                ituTextBoxSetString(userPasswordTextBox, NULL);

                if (theDeviceInfo.type == DEVICE_WALL)
                {
                    ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
                    userActiveTextBox = userBuildingTextBox;
                }
                else if (theDeviceInfo.type == DEVICE_BUILDING)
                {
                    ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
                    ituTextBoxSetString(userBuildingTextBox, theDeviceInfo.building);
                    userActiveTextBox = userUnitTextBox;
                }
                else
                {
                    ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
                    ituTextBoxSetString(userBuildingTextBox, theDeviceInfo.building);
                    ituTextBoxSetString(userUnitTextBox, theDeviceInfo.unit);
                    userActiveTextBox = userFloorTextBox;
                }

                ituTextBoxSetString(userActiveTextBox, NULL);
                ituWidgetSetActive(userActiveTextBox, true);
            }
            else
            {
                ituLayerGoto(callingLayer);
            }
        }
    }
	return true;
}

bool UserKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxSetString(userActiveTextBox, NULL);

    if (ituWidgetIsVisible(userPasswordTextBox))
    {
        ituWidgetSetVisible(userRoomBackground, true);
        ituWidgetSetVisible(userPasswordTextBox, false);

        if (theDeviceInfo.type == DEVICE_WALL)
        {
            userActiveTextBox = userBuildingTextBox;
        }
        else if (theDeviceInfo.type == DEVICE_BUILDING)
        {
            userActiveTextBox = userUnitTextBox;
        }
        else
        {
            userActiveTextBox = userFloorTextBox;
        }
    }
    else
    {
        if (userActiveTextBox == userRoomTextBox)
        {
            userActiveTextBox = userFloorTextBox;
        }
        else if (theDeviceInfo.type != DEVICE_ENTRANCE)
        {
            if (userActiveTextBox == userFloorTextBox)
            {
                userActiveTextBox = userUnitTextBox;
            }
            else if (theDeviceInfo.type == DEVICE_WALL)
            {
                if (userActiveTextBox == userUnitTextBox)
                {
                    userActiveTextBox = userBuildingTextBox;
                }
            }
        }
    }
    LinphoneEnableKeySound();
    LinphonePlayKeySound();
    LinphoneDisableKeySound();
    return true;
}

bool UserKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(userPasswordTextBox))
    {
        ituTextBoxInput(userActiveTextBox, param);
        LinphoneEnableKeySound();
        LinphonePlayKeySound();
        LinphoneDisableKeySound();
    }
    else
    {
        char* str = ituTextGetString(userActiveTextBox);
        int count = str ? strlen(str) : 0;

        if (count >= userActiveTextBox->maxLen)
        {
            if (userActiveTextBox == userBuildingTextBox)
                userActiveTextBox = userUnitTextBox;
            else if (userActiveTextBox == userUnitTextBox)
                userActiveTextBox = userFloorTextBox;
            else if (userActiveTextBox == userFloorTextBox)
                userActiveTextBox = userRoomTextBox;
        }
        ituTextBoxInput(userActiveTextBox, param);
        LinphonePlayNumberKeySound(param[0]);
    }
    return true;
}

bool UserOnEnter(ITUWidget* widget, char* param)
{
    if (!userRoomBackground)
    {
        userRoomBackground = ituSceneFindWidget(&theScene, "userRoomBackground");
        assert(userRoomBackground);

        userAreaTextBox = ituSceneFindWidget(&theScene, "userAreaTextBox");
        assert(userAreaTextBox);

        userBuildingTextBox = ituSceneFindWidget(&theScene, "userBuildingTextBox");
        assert(userBuildingTextBox);

        userUnitTextBox = ituSceneFindWidget(&theScene, "userUnitTextBox");
        assert(userUnitTextBox);

        userFloorTextBox = ituSceneFindWidget(&theScene, "userFloorTextBox");
        assert(userFloorTextBox);

        userRoomTextBox = ituSceneFindWidget(&theScene, "userRoomTextBox");
        assert(userRoomTextBox);

        userPasswordTextBox = ituSceneFindWidget(&theScene, "userPasswordTextBox");
        assert(userPasswordTextBox);

        passwordLayer = ituSceneFindWidget(&theScene, "passwordLayer");
        assert(passwordLayer);

        openDoorLayer = ituSceneFindWidget(&theScene, "openDoorLayer");
        assert(openDoorLayer);

        callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
        assert(callingLayer);

        calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
        assert(calledLayer);

		if (NetworkIsReady())
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
		else
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");    
    }

    ituTextBoxSetString(userAreaTextBox, NULL);
    ituTextBoxSetString(userBuildingTextBox, NULL);
    ituTextBoxSetString(userUnitTextBox, NULL);
    ituTextBoxSetString(userFloorTextBox, NULL);
    ituTextBoxSetString(userRoomTextBox, NULL);
    ituTextBoxSetString(userPasswordTextBox, NULL);

    if (theDeviceInfo.type == DEVICE_WALL)
    {
        ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
        userActiveTextBox = userBuildingTextBox;
    }
    else if (theDeviceInfo.type == DEVICE_BUILDING)
    {
        ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
        ituTextBoxSetString(userBuildingTextBox, theDeviceInfo.building);
        userActiveTextBox = userUnitTextBox;
    }
    else
    {
        ituTextBoxSetString(userAreaTextBox, theDeviceInfo.area);
        ituTextBoxSetString(userBuildingTextBox, theDeviceInfo.building);
        ituTextBoxSetString(userUnitTextBox, theDeviceInfo.unit);
        userActiveTextBox = userFloorTextBox;
    }

    ituTextBoxSetString(userActiveTextBox, NULL);
    ituWidgetSetActive(userActiveTextBox, true);
    LinphoneDisableKeySound();
    userEnteringPassword = false;
	return true;
}

bool UserOnLeave(ITUWidget* widget, char* param)
{
    LinphoneEnableKeySound();
    return true;
}

void UserReset(void)
{
    userRoomBackground = NULL;
}
