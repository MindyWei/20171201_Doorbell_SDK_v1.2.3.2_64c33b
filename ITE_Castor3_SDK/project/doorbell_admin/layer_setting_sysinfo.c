#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite/itp.h"
#include "scene.h"
#include "doorbell.h"

static ITUText* settingSysInfoDeviceTypeText;
static ITUText* settingSysInfoRoomAddrText;
static ITUText* settingSysInfoIPAddrText;
static ITUText* settingSysInfoServerIPText;
static ITUText* settingSysInfoMacText;
static ITUText* settingSysInfoHWVersionText;
static ITUText* settingSysInfoSWVersionText;
static ITUText* settingSysInfoAddressBookVersionText;
static ITUText* settingSysInfoHelpTelText;

bool SettingSysInfoOnEnter(ITUWidget* widget, char* param)
{
    ITPEthernetInfo ethInfo;
    char buf[32];
    char* ptr;

    if (!settingSysInfoDeviceTypeText)
    {
        settingSysInfoDeviceTypeText = ituSceneFindWidget(&theScene, "settingSysInfoDeviceTypeText");
        assert(settingSysInfoDeviceTypeText);

        settingSysInfoRoomAddrText = ituSceneFindWidget(&theScene, "settingSysInfoRoomAddrText");
        assert(settingSysInfoRoomAddrText);

        settingSysInfoIPAddrText = ituSceneFindWidget(&theScene, "settingSysInfoIPAddrText");
        assert(settingSysInfoIPAddrText);

        settingSysInfoServerIPText = ituSceneFindWidget(&theScene, "settingSysInfoServerIPText");
        assert(settingSysInfoServerIPText);

        settingSysInfoMacText = ituSceneFindWidget(&theScene, "settingSysInfoMacText");
        assert(settingSysInfoMacText);

        settingSysInfoHWVersionText = ituSceneFindWidget(&theScene, "settingSysInfoHWVersionText");
        assert(settingSysInfoHWVersionText);

        settingSysInfoSWVersionText = ituSceneFindWidget(&theScene, "settingSysInfoSWVersionText");
        assert(settingSysInfoSWVersionText);

        settingSysInfoAddressBookVersionText = ituSceneFindWidget(&theScene, "settingSysInfoAddressBookVersionText");
        assert(settingSysInfoAddressBookVersionText);

        settingSysInfoHelpTelText = ituSceneFindWidget(&theScene, "settingSysInfoHelpTelText");
        assert(settingSysInfoHelpTelText);
    }
    ituTextSetString(settingSysInfoDeviceTypeText, StringGetDeviceType(theDeviceInfo.type));

    ptr = StringGetRoomAddress(theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);
    ituTextSetString(settingSysInfoRoomAddrText, ptr);
    free(ptr);

    ituTextSetString(settingSysInfoIPAddrText, theConfig.ipaddr);

    ituTextSetString(settingSysInfoServerIPText, theCenterInfo.ip);

    ethInfo.index = 0;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &ethInfo);

    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", 
        ethInfo.hardwareAddress[0], 
        ethInfo.hardwareAddress[1], 
        ethInfo.hardwareAddress[2], 
        ethInfo.hardwareAddress[3], 
        ethInfo.hardwareAddress[4], 
        ethInfo.hardwareAddress[5]);
    ituTextSetString(settingSysInfoMacText, buf);

    ituTextSetString(settingSysInfoHWVersionText, CFG_HW_VERSION);
    ituTextSetString(settingSysInfoSWVersionText, CFG_VERSION_MAJOR_STR "." CFG_VERSION_MINOR_STR "." CFG_VERSION_PATCH_STR "." CFG_VERSION_CUSTOM_STR "." CFG_VERSION_TWEAK_STR);

    ptr = AddressBookGetVersion();
    ituTextSetString(settingSysInfoAddressBookVersionText, ptr);
    free(ptr);

    ituTextSetString(settingSysInfoHelpTelText, theConfig.help_tel);

    return true;
}

void SettingSysInfoReset(void)
{
    settingSysInfoDeviceTypeText = NULL;
}
