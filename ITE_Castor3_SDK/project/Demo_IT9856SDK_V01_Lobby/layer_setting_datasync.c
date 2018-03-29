#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingDataSyncLayer;
static ITURadioBox* settingDataSyncRadioBox;
static ITUButton* settingDataSyncAddressBookUpgradeButton;
static ITUButton* settingDataSyncScreensaverUpgradeButton;
static ITUButton* settingDataSyncFirmwareUpgradeButton;
static ITUButton* settingDataSyncCardListUpgradeButton;
static ITUButton* settingDataSyncSettingUpgradeButton;
static ITUButton* settingDataSyncAdvertisementUpgradeButton;
static ITUButton* settingDataSyncExportAddressBookButton;

bool SettingDataSyncKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsActive(settingDataSyncAddressBookUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        if (serverInfo)
        {
            UpgradeSetUrl(serverInfo->addressBookUrl);
            SceneQuit(QUIT_UPGRADE_RESOURCE);
        }
    }
    else if (ituWidgetIsActive(settingDataSyncScreensaverUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        if (serverInfo)
        {
            UpgradeSetUrl(serverInfo->screensaverUrl);
            SceneQuit(QUIT_UPGRADE_RESOURCE);
        }
    }
    else if (ituWidgetIsActive(settingDataSyncFirmwareUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        UpgradeSetUrl(serverInfo ? serverInfo->firmwareUrl : NULL);
        SceneQuit(QUIT_UPGRADE_FIRMWARE);
    }
    else if (ituWidgetIsActive(settingDataSyncCardListUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        if (serverInfo)
        {
            UpgradeSetUrl(serverInfo->cardListUrl);
            SceneQuit(QUIT_UPGRADE_RESOURCE);
        }
    }
    else if (ituWidgetIsActive(settingDataSyncSettingUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        if (serverInfo)
        {
            UpgradeSetUrl(serverInfo->settingUrl);
            SceneQuit(QUIT_UPGRADE_RESOURCE);
        }
    }
    else if (ituWidgetIsActive(settingDataSyncAdvertisementUpgradeButton))
    {
        ServerInfo* serverInfo = RemoteGetServerInfo();
        if (serverInfo)
        {
            UpgradeSetUrl(serverInfo->advertisementUrl);
            SceneQuit(QUIT_UPGRADE_RESOURCE);
        }
    }
    else if (ituWidgetIsActive(settingDataSyncExportAddressBookButton))
    {
        AddressBookExport();
	    ituWidgetEnable(settingLayer);
        ituLayerGoto(settingLayer);
        ituFocusWidget(settingDataSyncRadioBox);
        ituWidgetSetVisible(settingDataSyncLayer, true);
    }
    return true;
}

bool SettingDataSyncKeySharpOnPress(ITUWidget* widget, char* param)
{
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingDataSyncRadioBox);
    ituWidgetSetVisible(settingDataSyncLayer, true);
    return true;
}

bool SettingDataSyncKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '2' || *param == '4') // left, up
    {
        if (ituWidgetIsActive(settingDataSyncAddressBookUpgradeButton))
        {
            ituFocusWidget(settingDataSyncExportAddressBookButton);
        }
        else if (ituWidgetIsActive(settingDataSyncExportAddressBookButton))
        {
            ituFocusWidget(settingDataSyncAdvertisementUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncAdvertisementUpgradeButton))
        {
            ituFocusWidget(settingDataSyncSettingUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncSettingUpgradeButton))
        {
            ituFocusWidget(settingDataSyncCardListUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncCardListUpgradeButton))
        {
            ituFocusWidget(settingDataSyncFirmwareUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncFirmwareUpgradeButton))
        {
            ituFocusWidget(settingDataSyncScreensaverUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncScreensaverUpgradeButton))
        {
            ituFocusWidget(settingDataSyncAddressBookUpgradeButton);
        }
    }
    else if (*param == '6' || *param == '8') // right, down
    {
        if (ituWidgetIsActive(settingDataSyncAddressBookUpgradeButton))
        {
            ituFocusWidget(settingDataSyncScreensaverUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncScreensaverUpgradeButton))
        {
            ituFocusWidget(settingDataSyncFirmwareUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncFirmwareUpgradeButton))
        {
            ituFocusWidget(settingDataSyncCardListUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncCardListUpgradeButton))
        {
            ituFocusWidget(settingDataSyncSettingUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncSettingUpgradeButton))
        {
            ituFocusWidget(settingDataSyncAdvertisementUpgradeButton);
        }
        else if (ituWidgetIsActive(settingDataSyncAdvertisementUpgradeButton))
        {
            ituFocusWidget(settingDataSyncExportAddressBookButton);
        }
        else if (ituWidgetIsActive(settingDataSyncExportAddressBookButton))
        {
            ituFocusWidget(settingDataSyncAddressBookUpgradeButton);
        }
    }
    return true;
}

bool SettingDataSyncOnEnter(ITUWidget* widget, char* param)
{
    if (!settingDataSyncLayer)
    {
        settingDataSyncLayer = ituSceneFindWidget(&theScene, "settingDataSyncLayer");
        assert(settingDataSyncLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingDataSyncRadioBox = ituSceneFindWidget(&theScene, "settingDataSyncRadioBox");
        assert(settingDataSyncRadioBox);

        settingDataSyncAddressBookUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncAddressBookUpgradeButton");
        assert(settingDataSyncAddressBookUpgradeButton);

        settingDataSyncScreensaverUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncScreensaverUpgradeButton");
        assert(settingDataSyncScreensaverUpgradeButton);

        settingDataSyncFirmwareUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncFirmwareUpgradeButton");
        assert(settingDataSyncFirmwareUpgradeButton);

        settingDataSyncCardListUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncCardListUpgradeButton");
        assert(settingDataSyncCardListUpgradeButton);

        settingDataSyncSettingUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncSettingUpgradeButton");
        assert(settingDataSyncSettingUpgradeButton);

        settingDataSyncAdvertisementUpgradeButton = ituSceneFindWidget(&theScene, "settingDataSyncAdvertisementUpgradeButton");
        assert(settingDataSyncAdvertisementUpgradeButton);

        settingDataSyncExportAddressBookButton = ituSceneFindWidget(&theScene, "settingDataSyncExportAddressBookButton");
        assert(settingDataSyncExportAddressBookButton);
    }
    ituFocusWidget(settingDataSyncAddressBookUpgradeButton);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);

	return true;
}

void SettingDataSyncReset(void)
{
    settingDataSyncLayer = NULL;
}
