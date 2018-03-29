#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITURadioBox* settingMiscClearRadioBox;
static ITURadioBox* settingMiscUpgradeRadioBox;
static ITURadioBox* settingMiscResetRadioBox;
//static ITURadioBox* settingMiscExportAddressbookRadioBox;

bool SettingMiscConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingMiscClearRadioBox))
    {
        SceneQuit(QUIT_RESET_TEMP_DRIVE);
    }
    else if (ituRadioBoxIsChecked(settingMiscUpgradeRadioBox))
    {
        if (theCenterInfo.ip[0] == '\0')
        {
            UpgradeSetUrl(NULL);
        }
        else
        {
            ServerInfo* serverInfo = RemoteGetServerInfo();
            UpgradeSetUrl(serverInfo ? serverInfo->firmwareUrl : NULL);
        }
        SceneQuit(QUIT_UPGRADE_FIRMWARE);
    }
    else if (ituRadioBoxIsChecked(settingMiscResetRadioBox))
    {
        SceneQuit(QUIT_RESET_FACTORY);
    }
    //else if (ituRadioBoxIsChecked(settingMiscExportAddressbookRadioBox))
    //{
    //    AddressBookExport();
    //}
    return true;
}

bool SettingMiscOnEnter(ITUWidget* widget, char* param)
{
    if (!settingMiscClearRadioBox)
    {
        settingMiscClearRadioBox = ituSceneFindWidget(&theScene, "settingMiscClearRadioBox");
        assert(settingMiscClearRadioBox);

        settingMiscUpgradeRadioBox = ituSceneFindWidget(&theScene, "settingMiscUpgradeRadioBox");
        assert(settingMiscUpgradeRadioBox);

        settingMiscResetRadioBox = ituSceneFindWidget(&theScene, "settingMiscResetRadioBox");
        assert(settingMiscResetRadioBox);

        //settingMiscExportAddressbookRadioBox = ituSceneFindWidget(&theScene, "settingMiscExportAddressbookRadioBox");
        //assert(settingMiscExportAddressbookRadioBox);
    }
    return true;
}

bool SettingMiscOnLeave(ITUWidget* widget, char* param)
{
    return true;
}

void SettingMiscReset(void)
{
    settingMiscClearRadioBox = NULL;
}
