#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingInfoLayer;
static ITURadioBox* settingInfoRadioBox;
static ITUText* settingInfoTypeText;
static ITUText* settingInfoLocalIPText;
static ITUText* settingInfoManagerIPText;
static ITUText* settingInfoServerIPText;
static ITUText* settingInfoMacText;
static ITUText* settingInfoHWVersionText;
static ITUText* settingInfoFWVersionText;
static ITUText* settingInfoAddressBookVersionText;
static ITUText* settingInfoHelpTelText;

void SettingInfoShow(void)
{
    ITPEthernetInfo ethInfo;
    char buf[32];
    char* ptr;

    if (!settingInfoLayer)
    {   
        settingInfoLayer = ituSceneFindWidget(&theScene, "settingInfoLayer");
        assert(settingInfoLayer);    

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);    

        settingInfoRadioBox = ituSceneFindWidget(&theScene, "settingInfoRadioBox");
        assert(settingInfoRadioBox);    

        settingInfoTypeText = ituSceneFindWidget(&theScene, "settingInfoTypeText");
        assert(settingInfoTypeText);    

        settingInfoLocalIPText = ituSceneFindWidget(&theScene, "settingInfoLocalIPText");
        assert(settingInfoLocalIPText);
    
        settingInfoManagerIPText = ituSceneFindWidget(&theScene, "settingInfoManagerIPText");
        assert(settingInfoManagerIPText);    

        settingInfoServerIPText = ituSceneFindWidget(&theScene, "settingInfoServerIPText");
        assert(settingInfoServerIPText);
    
        settingInfoMacText = ituSceneFindWidget(&theScene, "settingInfoMacText");
        assert(settingInfoMacText);    

        settingInfoHWVersionText = ituSceneFindWidget(&theScene, "settingInfoHWVersionText");
        assert(settingInfoHWVersionText);
    
        settingInfoFWVersionText = ituSceneFindWidget(&theScene, "settingInfoFWVersionText");
        assert(settingInfoFWVersionText);    

        settingInfoAddressBookVersionText = ituSceneFindWidget(&theScene, "settingInfoAddressBookVersionText");
        assert(settingInfoAddressBookVersionText);    

        settingInfoHelpTelText = ituSceneFindWidget(&theScene, "settingInfoHelpTelText");
        assert(settingInfoHelpTelText);
    }

    ituTextSetString(settingInfoTypeText, theDeviceInfo.alias);
    ituTextSetString(settingInfoLocalIPText, theConfig.ipaddr);
    ituTextSetString(settingInfoManagerIPText, theCenterInfo.ip);
    ituTextSetString(settingInfoServerIPText, theCenterInfo.ip);

    ethInfo.index = 0;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &ethInfo);

    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", 
        ethInfo.hardwareAddress[0], 
        ethInfo.hardwareAddress[1], 
        ethInfo.hardwareAddress[2], 
        ethInfo.hardwareAddress[3], 
        ethInfo.hardwareAddress[4], 
        ethInfo.hardwareAddress[5]);
    ituTextSetString(settingInfoMacText, buf);

    ituTextSetString(settingInfoHWVersionText, CFG_HW_VERSION);
    ituTextSetString(settingInfoFWVersionText, CFG_VERSION_MAJOR_STR "." CFG_VERSION_MINOR_STR "." CFG_VERSION_PATCH_STR "." CFG_VERSION_CUSTOM_STR "." CFG_VERSION_TWEAK_STR);

    ptr = AddressBookGetVersion();
    ituTextSetString(settingInfoAddressBookVersionText, ptr);
    free(ptr);

    ituTextSetString(settingInfoHelpTelText, theConfig.help_tel);
}

void SettingInfoReset(void)
{
    settingInfoLayer = NULL;
}
