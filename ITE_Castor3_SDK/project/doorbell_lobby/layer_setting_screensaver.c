#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingScreenSaverLayer;
static ITULayer* settingLayer;
static ITURadioBox* settingScreenSaverRadioBox;
static ITURadioBox* settingScreenSaverPictureRadioBox;
static ITURadioBox* settingScreenSaverTimeRadioBox;
static ITURadioBox* settingScreenSaverPowerRadioBox;
static ITURadioBox* settingScreenSaverAdvertisementRadioBox;
static ITURadioBox* settingScreenSaverPostRadioBox;
static ITURadioBox* settingScreenSaverNoneRadioBox;

static ITURadioBox* settingScreenSaverActiveRadioBox;

bool SettingScreenSaverKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsActive(settingScreenSaverPictureRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverPictureRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_PHOTO;
    }
    else if (ituWidgetIsActive(settingScreenSaverTimeRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverTimeRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_CLOCK;
    }
    else if (ituWidgetIsActive(settingScreenSaverPowerRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverPowerRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_BLANK;
    }
    else if (ituWidgetIsActive(settingScreenSaverAdvertisementRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverAdvertisementRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_ADV;
    }
    else if (ituWidgetIsActive(settingScreenSaverPostRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverPostRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_POST;
    }
    else if (ituWidgetIsActive(settingScreenSaverNoneRadioBox))
    {
        ituRadioBoxSetChecked(settingScreenSaverNoneRadioBox, true);
        theConfig.screensaver_type = SCREENSAVER_NONE;
    }
    ConfigSave();
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingScreenSaverRadioBox);
    ituWidgetSetVisible(settingScreenSaverLayer, true);
    return true;
}

bool SettingScreenSaverKeySharpOnPress(ITUWidget* widget, char* param)
{
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingScreenSaverRadioBox);
    ituWidgetSetVisible(settingScreenSaverLayer, true);
    return true;
}

bool SettingScreenSaverKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '2' || *param == '4') // up
    {
        if (ituWidgetIsActive(settingScreenSaverTimeRadioBox))
			settingScreenSaverActiveRadioBox = settingScreenSaverPictureRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverPowerRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverTimeRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverAdvertisementRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverPowerRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverPostRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverAdvertisementRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverNoneRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverPostRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverPictureRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverNoneRadioBox;
		
        ituFocusWidget(settingScreenSaverActiveRadioBox);
		ituRadioBoxSetChecked(settingScreenSaverActiveRadioBox, true);
    }
    else if (*param == '6' || *param == '8') // down
    {
        if (ituWidgetIsActive(settingScreenSaverPictureRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverTimeRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverTimeRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverPowerRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverPowerRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverAdvertisementRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverAdvertisementRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverPostRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverPostRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverNoneRadioBox;
        else if (ituWidgetIsActive(settingScreenSaverNoneRadioBox))
            settingScreenSaverActiveRadioBox = settingScreenSaverPictureRadioBox;

        ituFocusWidget(settingScreenSaverActiveRadioBox);
		ituRadioBoxSetChecked(settingScreenSaverActiveRadioBox, true);
    }
	return true;
}

void SettingScreenSaverShow(void)
{
	if (!settingScreenSaverPictureRadioBox)
    {
        settingScreenSaverPictureRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPictureRadioBox");
        assert(settingScreenSaverPictureRadioBox);

        settingScreenSaverTimeRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverTimeRadioBox");
        assert(settingScreenSaverTimeRadioBox);

        settingScreenSaverPowerRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPowerRadioBox");
        assert(settingScreenSaverPowerRadioBox);

        settingScreenSaverAdvertisementRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverAdvertisementRadioBox");
        assert(settingScreenSaverAdvertisementRadioBox);

        settingScreenSaverPostRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPostRadioBox");
        assert(settingScreenSaverPostRadioBox);

        settingScreenSaverNoneRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverNoneRadioBox");
        assert(settingScreenSaverNoneRadioBox);    
	}
    switch (theConfig.screensaver_type)
    {
    case SCREENSAVER_CLOCK:
		settingScreenSaverActiveRadioBox = settingScreenSaverTimeRadioBox;
        break;

    case SCREENSAVER_BLANK:
		settingScreenSaverActiveRadioBox = settingScreenSaverPowerRadioBox;
        break;

    case SCREENSAVER_PHOTO:
		settingScreenSaverActiveRadioBox = settingScreenSaverPictureRadioBox;
        break;

    case SCREENSAVER_ADV:
		settingScreenSaverActiveRadioBox = settingScreenSaverAdvertisementRadioBox;
        break;

    case SCREENSAVER_POST:
		settingScreenSaverActiveRadioBox = settingScreenSaverPostRadioBox;
        break;

    default:
		settingScreenSaverActiveRadioBox = settingScreenSaverNoneRadioBox;
        break;
    }
    ituRadioBoxSetChecked(settingScreenSaverActiveRadioBox, true);
}

bool SettingScreenSaverOnEnter(ITUWidget* widget, char* param)
{
    if (!settingScreenSaverLayer)
    {
        settingScreenSaverLayer = ituSceneFindWidget(&theScene, "settingScreenSaverLayer");
        assert(settingScreenSaverLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingScreenSaverRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverRadioBox");
        assert(settingScreenSaverRadioBox);

	    if (!settingScreenSaverPictureRadioBox)
        {
            settingScreenSaverPictureRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPictureRadioBox");
            assert(settingScreenSaverPictureRadioBox);

            settingScreenSaverTimeRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverTimeRadioBox");
            assert(settingScreenSaverTimeRadioBox);

            settingScreenSaverPowerRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPowerRadioBox");
            assert(settingScreenSaverPowerRadioBox);
    
            settingScreenSaverAdvertisementRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverAdvertisementRadioBox");
            assert(settingScreenSaverAdvertisementRadioBox);

            settingScreenSaverPostRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverPostRadioBox");
            assert(settingScreenSaverPostRadioBox);

            settingScreenSaverNoneRadioBox = ituSceneFindWidget(&theScene, "settingScreenSaverNoneRadioBox");
            assert(settingScreenSaverNoneRadioBox);    
	    }
    }

	ituFocusWidget(settingScreenSaverActiveRadioBox);

	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingScreenSaverReset(void)
{
    settingScreenSaverLayer = NULL;
	settingScreenSaverPictureRadioBox = NULL;
}
