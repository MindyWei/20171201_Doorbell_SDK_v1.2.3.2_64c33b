#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"


static ITULayer* userLayer;
static ITULayer* settingUserModeLayer;
static ITULayer* settingDataSyncLayer;
static ITULayer* settingLocalPasswordLayer;
static ITULayer* settingDoorPasswordLayer;
static ITULayer* settingGuardLayer;
static ITULayer* settingTimeLayer;
static ITULayer* settingInfoLayer;
static ITULayer* settingLocalPositionLayer;
static ITULayer* settingScreenSaverLayer;
static ITULayer* settingAudioVolumeLayer;
static ITULayer* settingCustomLayer;
static ITULayer* settingSystemLayer;
static ITUBackground* settingFirstBackground;
static ITURadioBox* settingUserModeRadioBox;
static ITURadioBox* settingDataSyncRadioBox;
static ITURadioBox* settingLocalPasswordRadioBox;
static ITURadioBox* settingDoorPasswordRadioBox;
static ITURadioBox* settingGuardRadioBox;
static ITURadioBox* settingTimeRadioBox;
static ITUBackground* settingSecondBackground;
static ITURadioBox* settingInfoRadioBox;
static ITURadioBox* settingLocalPositionRadioBox;
static ITURadioBox* settingScreenSaverRadioBox;
static ITURadioBox* settingAudioVolumeRadioBox;
static ITURadioBox* settingCustomRadioBox;
static ITURadioBox* settingSystemRadioBox;
static ITUText* settingPageText;

bool SettingKeyStarOnPress(ITUWidget* widget, char* param)
{
	if(ituWidgetIsActive(settingUserModeRadioBox))
	{
        ituLayerGoto(userLayer);
	}
    else if(ituWidgetIsActive(settingDataSyncRadioBox))
	{
		ituWidgetEnable(settingDataSyncLayer);
        ituLayerGoto(settingDataSyncLayer);
	}
	else if(ituWidgetIsActive(settingLocalPasswordRadioBox))
	{
		ituWidgetEnable(settingLocalPasswordLayer);
        ituLayerGoto(settingLocalPasswordLayer);
	}
	else if(ituWidgetIsActive(settingDoorPasswordRadioBox))
	{
		ituWidgetEnable(settingDoorPasswordLayer);
        ituLayerGoto(settingDoorPasswordLayer);
	}
	else if(ituWidgetIsActive(settingGuardRadioBox))
	{
		ituWidgetEnable(settingGuardLayer);
        ituLayerGoto(settingGuardLayer);
	}
	else if(ituWidgetIsActive(settingTimeRadioBox))
	{
		ituWidgetEnable(settingTimeLayer);
        ituLayerGoto(settingTimeLayer);
	}
	else if(ituWidgetIsActive(settingLocalPositionRadioBox))
	{
		ituWidgetEnable(settingLocalPositionLayer);
        ituLayerGoto(settingLocalPositionLayer);
	}
	else if(ituWidgetIsActive(settingScreenSaverRadioBox))
	{
		ituWidgetEnable(settingScreenSaverLayer);
        ituLayerGoto(settingScreenSaverLayer);
	}
	else if(ituWidgetIsActive(settingAudioVolumeRadioBox))
	{
		ituWidgetEnable(settingAudioVolumeLayer);
        ituLayerGoto(settingAudioVolumeLayer);
	}
	else if(ituWidgetIsActive(settingCustomRadioBox))
	{
		ituWidgetEnable(settingCustomLayer);
        ituLayerGoto(settingCustomLayer);
	}
	else if(ituWidgetIsActive(settingSystemRadioBox))
	{
		ituWidgetEnable(settingSystemLayer);
        ituLayerGoto(settingSystemLayer);
	}
    return true;
}

bool SettingKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituWidgetSetVisible(settingDataSyncLayer, false);
    ituWidgetSetVisible(settingLocalPasswordLayer, false);
    ituWidgetSetVisible(settingDoorPasswordLayer, false);
    ituWidgetSetVisible(settingGuardLayer, false);
    ituWidgetSetVisible(settingTimeLayer, false);
    ituWidgetSetVisible(settingInfoLayer, false);
    ituWidgetSetVisible(settingLocalPositionLayer, false);
    ituWidgetSetVisible(settingScreenSaverLayer, false);
    ituWidgetSetVisible(settingAudioVolumeLayer, false);
    ituWidgetSetVisible(settingCustomLayer, false);
    ituWidgetSetVisible(settingSystemLayer, false);

    ituWidgetSetVisible(settingFirstBackground, true);
    ituWidgetSetVisible(settingSecondBackground, false);
    ituTextSetString(settingPageText, "1 / 2");

    ituFocusWidget(settingUserModeRadioBox);
    ituRadioBoxSetChecked(settingUserModeRadioBox, true);
    ituWidgetSetVisible(settingUserModeLayer, true);
    return true;
}

bool SettingKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '2' || *param == '8' || *param == '4' || *param == '6') // clear all the child show first
	{
        ituWidgetSetVisible(settingUserModeLayer, false);
        ituWidgetSetVisible(settingDataSyncLayer, false);
        ituWidgetSetVisible(settingLocalPasswordLayer, false);
        ituWidgetSetVisible(settingDoorPasswordLayer, false);
        ituWidgetSetVisible(settingGuardLayer, false);
        ituWidgetSetVisible(settingTimeLayer, false);
        ituWidgetSetVisible(settingInfoLayer, false);
        ituWidgetSetVisible(settingLocalPositionLayer, false);
        ituWidgetSetVisible(settingScreenSaverLayer, false);
        ituWidgetSetVisible(settingAudioVolumeLayer, false);
        ituWidgetSetVisible(settingCustomLayer, false);
        ituWidgetSetVisible(settingSystemLayer, false);
	}

    if (*param == '2' || *param == '4') // up, left
    {
		if (ituWidgetIsVisible(settingFirstBackground))
		{
            if (ituWidgetIsActive(settingDataSyncRadioBox))
            {
                ituFocusWidget(settingUserModeRadioBox);
                ituRadioBoxSetChecked(settingUserModeRadioBox, true);
                ituWidgetSetVisible(settingUserModeLayer, true);

            }
            else if (ituWidgetIsActive(settingLocalPasswordRadioBox))
            {
                ituFocusWidget(settingDataSyncRadioBox);
                ituRadioBoxSetChecked(settingDataSyncRadioBox, true);
                ituWidgetSetVisible(settingDataSyncLayer, true);
            }
            else if (ituWidgetIsActive(settingDoorPasswordRadioBox))
            {
                ituFocusWidget(settingLocalPasswordRadioBox);
                ituRadioBoxSetChecked(settingLocalPasswordRadioBox, true);
                ituWidgetSetVisible(settingLocalPasswordLayer, true);
            }
            else if (ituWidgetIsActive(settingGuardRadioBox))
            {
                ituFocusWidget(settingDoorPasswordRadioBox);
                ituRadioBoxSetChecked(settingDoorPasswordRadioBox, true);
                ituWidgetSetVisible(settingDoorPasswordLayer, true);
            }
            else if (ituWidgetIsActive(settingTimeRadioBox))
            {
                SettingGuardShow();
                ituFocusWidget(settingGuardRadioBox);
                ituRadioBoxSetChecked(settingGuardRadioBox, true);
                ituWidgetSetVisible(settingGuardLayer, true);
            }
			else
			{
				ituWidgetSetVisible(settingFirstBackground, false);
                ituWidgetSetVisible(settingSecondBackground, true);
				ituTextSetString(settingPageText, "2 / 2");

                SettingSystemShow();
                ituFocusWidget(settingSystemRadioBox);
                ituRadioBoxSetChecked(settingSystemRadioBox, true);
                ituWidgetSetVisible(settingSystemLayer, true);
			}
		}
        else
        {
	        if (ituWidgetIsActive(settingInfoRadioBox))
            {
                ituWidgetSetVisible(settingFirstBackground, true);
                ituWidgetSetVisible(settingSecondBackground, false);
				ituTextSetString(settingPageText, "1 / 2");

	            SettingTimeShow();
                ituFocusWidget(settingTimeRadioBox);
                ituRadioBoxSetChecked(settingTimeRadioBox, true);
                ituWidgetSetVisible(settingTimeLayer, true);
            }
            else if (ituWidgetIsActive(settingLocalPositionRadioBox))
            {
                SettingInfoShow();
                ituFocusWidget(settingInfoRadioBox);
                ituRadioBoxSetChecked(settingInfoRadioBox, true);
                ituWidgetSetVisible(settingInfoLayer, true);
            }
            else if (ituWidgetIsActive(settingScreenSaverRadioBox))
            {
	            SettingLocalPositionShow();
                ituFocusWidget(settingLocalPositionRadioBox);
                ituRadioBoxSetChecked(settingLocalPositionRadioBox, true);
                ituWidgetSetVisible(settingLocalPositionLayer, true);
            }
            else if (ituWidgetIsActive(settingAudioVolumeRadioBox))
            {
	            SettingScreenSaverShow();
                ituFocusWidget(settingScreenSaverRadioBox);
                ituRadioBoxSetChecked(settingScreenSaverRadioBox, true);
                ituWidgetSetVisible(settingScreenSaverLayer, true);
           }
            else if (ituWidgetIsActive(settingCustomRadioBox))
            {
                SettingAudioVolumeShow();
                ituFocusWidget(settingAudioVolumeRadioBox);
                ituRadioBoxSetChecked(settingAudioVolumeRadioBox, true);
                ituWidgetSetVisible(settingAudioVolumeLayer, true);
            } 
            else if (ituWidgetIsActive(settingSystemRadioBox))
            {
	            SettingCustomShow();
                ituFocusWidget(settingCustomRadioBox);
                ituRadioBoxSetChecked(settingCustomRadioBox, true);
                ituWidgetSetVisible(settingCustomLayer, true);
            }
		}
	}
    else if (*param == '8' || *param == '6') // down, right
	{
        if (ituWidgetIsVisible(settingFirstBackground))
		{
            if (ituWidgetIsActive(settingUserModeRadioBox))
            {
                ituFocusWidget(settingDataSyncRadioBox);
                ituRadioBoxSetChecked(settingDataSyncRadioBox, true);
                ituWidgetSetVisible(settingDataSyncLayer, true);
            }
            else if (ituWidgetIsActive(settingDataSyncRadioBox))
            {
                ituFocusWidget(settingLocalPasswordRadioBox);
                ituRadioBoxSetChecked(settingLocalPasswordRadioBox, true);
                ituWidgetSetVisible(settingLocalPasswordLayer, true);
            }
            else if (ituWidgetIsActive(settingLocalPasswordRadioBox))
            {
                ituFocusWidget(settingDoorPasswordRadioBox);
                ituRadioBoxSetChecked(settingDoorPasswordRadioBox, true);
                ituWidgetSetVisible(settingDoorPasswordLayer, true);
            }
            else if (ituWidgetIsActive(settingDoorPasswordRadioBox))
            {
                SettingGuardShow();
                ituFocusWidget(settingGuardRadioBox);
                ituRadioBoxSetChecked(settingGuardRadioBox, true);
                ituWidgetSetVisible(settingGuardLayer, true);
            }
            else if (ituWidgetIsActive(settingGuardRadioBox))
            {
                SettingTimeShow();
                ituFocusWidget(settingTimeRadioBox);
                ituRadioBoxSetChecked(settingTimeRadioBox, true);
                ituWidgetSetVisible(settingTimeLayer, true);
            }
            else if (ituWidgetIsActive(settingTimeRadioBox))
            {
                ituWidgetSetVisible(settingFirstBackground, false);
                ituWidgetSetVisible(settingSecondBackground, true);
				ituTextSetString(settingPageText, "2 / 2");

                SettingInfoShow();
                ituFocusWidget(settingInfoRadioBox);
                ituRadioBoxSetChecked(settingInfoRadioBox, true);
                ituWidgetSetVisible(settingInfoLayer, true);
            }
        }
        else
		{
            if (ituWidgetIsActive(settingInfoRadioBox))
            {
                SettingLocalPositionShow();
                ituFocusWidget(settingLocalPositionRadioBox);
                ituRadioBoxSetChecked(settingLocalPositionRadioBox, true);
                ituWidgetSetVisible(settingLocalPositionLayer, true);
            }
            else if (ituWidgetIsActive(settingLocalPositionRadioBox))
            {
                SettingScreenSaverShow();
                ituFocusWidget(settingScreenSaverRadioBox);
                ituRadioBoxSetChecked(settingScreenSaverRadioBox, true);
                ituWidgetSetVisible(settingScreenSaverLayer, true);
            }
            else if (ituWidgetIsActive(settingScreenSaverRadioBox))
            {
                SettingAudioVolumeShow();
                ituFocusWidget(settingAudioVolumeRadioBox);
                ituRadioBoxSetChecked(settingAudioVolumeRadioBox, true);
                ituWidgetSetVisible(settingAudioVolumeLayer, true);
            }
            else if (ituWidgetIsActive(settingAudioVolumeRadioBox))
            {
                SettingCustomShow();
                ituFocusWidget(settingCustomRadioBox);
                ituRadioBoxSetChecked(settingCustomRadioBox, true);
                ituWidgetSetVisible(settingCustomLayer, true);
            }
            else if (ituWidgetIsActive(settingCustomRadioBox))
            {
                SettingSystemShow();
                ituFocusWidget(settingSystemRadioBox);
                ituRadioBoxSetChecked(settingSystemRadioBox, true);
                ituWidgetSetVisible(settingSystemLayer, true);
            }
			else
			{
                ituWidgetSetVisible(settingFirstBackground, true);
                ituWidgetSetVisible(settingSecondBackground, false);
				ituTextSetString(settingPageText, "1 / 2");

                ituFocusWidget(settingUserModeRadioBox);
                ituRadioBoxSetChecked(settingUserModeRadioBox, true);
                ituWidgetSetVisible(settingUserModeLayer, true);
			}
        }
	}
	return true;
}

bool SettingOnEnter(ITUWidget* widget, char* param)
{
    if (!userLayer)
    {
        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);

        settingUserModeLayer = ituSceneFindWidget(&theScene, "settingUserModeLayer");
        assert(settingUserModeLayer);

        settingDataSyncLayer = ituSceneFindWidget(&theScene, "settingDataSyncLayer");
        assert(settingDataSyncLayer);

        settingLocalPasswordLayer = ituSceneFindWidget(&theScene, "settingLocalPasswordLayer");
        assert(settingLocalPasswordLayer);

        settingDoorPasswordLayer = ituSceneFindWidget(&theScene, "settingDoorPasswordLayer");
        assert(settingDoorPasswordLayer);

        settingGuardLayer = ituSceneFindWidget(&theScene, "settingGuardLayer");
        assert(settingGuardLayer);

        settingTimeLayer = ituSceneFindWidget(&theScene, "settingTimeLayer");
        assert(settingTimeLayer);

        settingInfoLayer = ituSceneFindWidget(&theScene, "settingInfoLayer");
        assert(settingInfoLayer);

        settingLocalPositionLayer = ituSceneFindWidget(&theScene, "settingLocalPositionLayer");
        assert(settingLocalPositionLayer);

        settingScreenSaverLayer = ituSceneFindWidget(&theScene, "settingScreenSaverLayer");
        assert(settingScreenSaverLayer);

        settingAudioVolumeLayer = ituSceneFindWidget(&theScene, "settingAudioVolumeLayer");
        assert(settingAudioVolumeLayer);

        settingCustomLayer = ituSceneFindWidget(&theScene, "settingCustomLayer");
        assert(settingCustomLayer);

        settingSystemLayer = ituSceneFindWidget(&theScene, "settingSystemLayer");
        assert(settingSystemLayer);

        settingFirstBackground = ituSceneFindWidget(&theScene, "settingFirstBackground");
        assert(settingFirstBackground);

        settingSecondBackground = ituSceneFindWidget(&theScene, "settingSecondBackground");
        assert(settingSecondBackground);

        settingUserModeRadioBox = ituSceneFindWidget(&theScene, "settingUserModeRadioBox");
        assert(settingUserModeRadioBox);

        settingDataSyncRadioBox = ituSceneFindWidget(&theScene, "settingDataSyncRadioBox");
        assert(settingDataSyncRadioBox);

        settingLocalPasswordRadioBox = ituSceneFindWidget(&theScene, "settingLocalPasswordRadioBox");
        assert(settingLocalPasswordRadioBox);

        settingDoorPasswordRadioBox = ituSceneFindWidget(&theScene, "settingDoorPasswordRadioBox");
        assert(settingDoorPasswordRadioBox);

        settingGuardRadioBox = ituSceneFindWidget(&theScene, "settingGuardRadioBox");
        assert(settingGuardRadioBox);

        settingTimeRadioBox = ituSceneFindWidget(&theScene, "settingTimeRadioBox");
        assert(settingTimeRadioBox);

        settingInfoRadioBox = ituSceneFindWidget(&theScene, "settingInfoRadioBox");
        assert(settingInfoRadioBox);

        settingLocalPositionRadioBox = ituSceneFindWidget(&theScene, "settingLocalPositionRadioBox");
        assert(settingLocalPositionRadioBox);

        settingScreenSaverRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverRadioBox");
        assert(settingScreenSaverRadioBox);

        settingAudioVolumeRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeRadioBox");
        assert(settingAudioVolumeRadioBox);

        settingCustomRadioBox = ituSceneFindWidget(&theScene, "settingCustomRadioBox");
        assert(settingCustomRadioBox);

        settingSystemRadioBox = ituSceneFindWidget(&theScene, "settingSystemRadioBox");
        assert(settingSystemRadioBox);

        settingPageText = ituSceneFindWidget(&theScene, "settingPageText");
        assert(settingPageText);
    }

	//disable all setting child to avoid receive event
    ituWidgetDisable(settingDataSyncLayer);
    ituWidgetDisable(settingLocalPasswordLayer);
    ituWidgetDisable(settingDoorPasswordLayer);
    ituWidgetDisable(settingGuardLayer);
    ituWidgetDisable(settingTimeLayer);
    ituWidgetDisable(settingLocalPositionLayer);
    ituWidgetDisable(settingScreenSaverLayer);
    ituWidgetDisable(settingAudioVolumeLayer);
    ituWidgetDisable(settingCustomLayer);
    ituWidgetDisable(settingSystemLayer);

    if (strcmp(param, "passwordLayer") == 0)
    {
        // reset to defaut page
        ituWidgetSetVisible(settingDataSyncLayer, false);
        ituWidgetSetVisible(settingLocalPasswordLayer, false);
        ituWidgetSetVisible(settingDoorPasswordLayer, false);
        ituWidgetSetVisible(settingGuardLayer, false);
        ituWidgetSetVisible(settingTimeLayer, false);
        ituWidgetSetVisible(settingInfoLayer, false);
        ituWidgetSetVisible(settingLocalPositionLayer, false);
        ituWidgetSetVisible(settingScreenSaverLayer, false);
        ituWidgetSetVisible(settingAudioVolumeLayer, false);
        ituWidgetSetVisible(settingCustomLayer, false);
        ituWidgetSetVisible(settingSystemLayer, false);

        ituWidgetSetVisible(settingFirstBackground, true);
        ituWidgetSetVisible(settingSecondBackground, false);
        ituTextSetString(settingPageText, "1 / 2");

        ituFocusWidget(settingDataSyncRadioBox);
        ituRadioBoxSetChecked(settingDataSyncRadioBox, true);
        ituWidgetSetVisible(settingDataSyncLayer, true);
    }
	return true;
}

void SettingReset(void)
{
    userLayer = NULL;
}