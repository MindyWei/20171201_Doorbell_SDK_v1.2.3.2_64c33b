#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "scene.h"
#include "doorbell.h"

#define ENG "eng"
#define CHS "chs"
#define CHT "cht"

static ITULayer* settingLayer;
static ITULayer* settingSystemLayer;
static ITULayer* userLayer;
static ITURadioBox* settingSystemRadioBox;
static ITUButton* settingSystemRebootButton;
static ITUButton* settingSystemLangChtButton;
static ITUButton* settingSystemResetButton;
static ITUButton* settingSystemLangChsButton;
static ITUButton* settingSystemLangEngButton;
static ITURadioBox* settingSystemBrightnessRadioBox;
static ITUSprite* settingSystemBrightnessSprite;
static ITUBackground* settingSystemRebootBackground;

static int settingSystemMaxBrightness;

bool SettingSystemKeyStarOnPress(ITUWidget* widget, char* param)
{
    bool reloadScene = false;

    if (ituWidgetIsActive(settingSystemRebootButton))
    {
        if (ituWidgetIsVisible(settingSystemRebootBackground))
        {
        #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
            sleep(3);
            ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
        #endif
            itp_codec_standby();
            exit(0);
        }
        else
        {
            ituWidgetSetVisible(settingSystemRebootBackground, true);
        }
    }
    else if (ituWidgetIsActive(settingSystemLangChtButton))
    {
        if (strcmp(theConfig.lang, CHT))
        {
            strcpy(theConfig.lang, CHT);
            strcpy(theConfig.itufile, "doorbell_lobby_cht.itu");
            reloadScene = true;
            ConfigSave();
        }
    }
    else if (ituWidgetIsActive(settingSystemResetButton))
    {
        if (ituWidgetIsVisible(settingSystemRebootBackground))
        {
            SceneQuit(QUIT_RESET_FACTORY);
        }
        else
        {
            ituWidgetSetVisible(settingSystemRebootBackground, true);
        }
    }
    else if (ituWidgetIsActive(settingSystemLangChsButton))
    {
        if (strcmp(theConfig.lang, CHS))
        {
            strcpy(theConfig.lang, CHS);
            strcpy(theConfig.itufile, "doorbell_lobby_chs.itu");
            reloadScene = true;
            ConfigSave();
        }
    }
    else if (ituWidgetIsActive(settingSystemLangEngButton))
    {
        if (strcmp(theConfig.lang, ENG))
        {
            strcpy(theConfig.lang, ENG);
            strcpy(theConfig.itufile, "doorbell_lobby.itu");
            reloadScene = true;
            ConfigSave();
        }
    }
    else if (ituWidgetIsActive(settingSystemBrightnessRadioBox))
    {
        theConfig.brightness = settingSystemBrightnessSprite->frame;
        ConfigSave();

	    ituWidgetEnable(settingLayer);
        ituLayerGoto(settingLayer);
        ituFocusWidget(settingSystemRadioBox);
        ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, false);
        ituWidgetSetVisible(settingSystemLayer, true);
    }
    if (reloadScene)
    {
        SceneLoad();
		ituLayerGoto(userLayer);
    }
    return true;
}

bool SettingSystemKeySharpOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingSystemRebootBackground))
    {
        ituWidgetSetVisible(settingSystemRebootBackground, false);
    }
    else
    {
        ituSpriteGoto(settingSystemBrightnessSprite, theConfig.brightness);
        ScreenSetBrightness(theConfig.brightness);

	    ituWidgetEnable(settingLayer);
        ituLayerGoto(settingLayer);
        ituFocusWidget(settingSystemRadioBox);
        ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, false);
        ituWidgetSetVisible(settingSystemLayer, true);
    }
    return true;
}

bool SettingSystemKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '2' || *param == '4') // left, up
    {
        if (ituWidgetIsActive(settingSystemRebootButton))
        {
            ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, true);
            ituFocusWidget(settingSystemBrightnessRadioBox);
        }
        else if (ituWidgetIsActive(settingSystemLangEngButton))
        {
            ituFocusWidget(settingSystemLangChsButton);
        }
        else if (ituWidgetIsActive(settingSystemLangChsButton))
        {
            ituFocusWidget(settingSystemResetButton);
        }
        else if (ituWidgetIsActive(settingSystemResetButton))
        {
            ituFocusWidget(settingSystemLangChtButton);
        }
        else if (ituWidgetIsActive(settingSystemLangChtButton))
        {
            ituFocusWidget(settingSystemRebootButton);
        }
        else if (ituWidgetIsActive(settingSystemBrightnessRadioBox))
        {
            if (*param == '4')
            {
                int frame = settingSystemBrightnessSprite->frame - 1;

                if (frame < 0)
                    frame = 0;

                ituSpriteGoto(settingSystemBrightnessSprite, frame);
                ScreenSetBrightness(settingSystemBrightnessSprite->frame);
            }
            else
            {
                ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, false);
                ituFocusWidget(settingSystemLangEngButton);
            }
        }
    }
    else if (*param == '6' || *param == '8') // right, down
    {
        if (ituWidgetIsActive(settingSystemRebootButton))
        {
            ituFocusWidget(settingSystemLangChtButton);
        }
        else if (ituWidgetIsActive(settingSystemLangChtButton))
        {
            ituFocusWidget(settingSystemResetButton);
        }
        else if (ituWidgetIsActive(settingSystemResetButton))
        {
            ituFocusWidget(settingSystemLangChsButton);
        }
        else if (ituWidgetIsActive(settingSystemLangChsButton))
        {
            ituFocusWidget(settingSystemLangEngButton);
        }
        else if (ituWidgetIsActive(settingSystemLangEngButton))
        {
            ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, true);
            ituFocusWidget(settingSystemBrightnessRadioBox);
        }
        else if (ituWidgetIsActive(settingSystemBrightnessRadioBox))
        {
            if (*param == '6')
            {
                int frame = settingSystemBrightnessSprite->frame + 1;

                if (frame > settingSystemMaxBrightness)
                    frame = settingSystemMaxBrightness;

                ituSpriteGoto(settingSystemBrightnessSprite, frame);
                ScreenSetBrightness(settingSystemBrightnessSprite->frame);
            }
            else
            {
                ituRadioBoxSetChecked(settingSystemBrightnessRadioBox, false);
                ituFocusWidget(settingSystemRebootButton);
            }
        }
    }
    return true;
}

void SettingSystemShow(void)
{
    if (!settingSystemBrightnessSprite)
    {
        settingSystemBrightnessSprite = ituSceneFindWidget(&theScene, "settingSystemBrightnessSprite");
        assert(settingSystemBrightnessSprite);

        settingSystemRebootBackground = ituSceneFindWidget(&theScene, "settingSystemRebootBackground");
        assert(settingSystemRebootBackground);

        settingSystemMaxBrightness = itcTreeGetChildCount(settingSystemBrightnessSprite) - 1;
	}
    ituSpriteGoto(settingSystemBrightnessSprite, theConfig.brightness);
    ituWidgetSetVisible(settingSystemRebootBackground, false);
}

bool SettingSystemOnEnter(ITUWidget* widget, char* param)
{
    if (!settingSystemLayer)
    {
        settingSystemLayer = ituSceneFindWidget(&theScene, "settingSystemLayer");
        assert(settingSystemLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);

        settingSystemRadioBox = ituSceneFindWidget(&theScene, "settingSystemRadioBox");
        assert(settingSystemRadioBox);

        settingSystemRebootButton = ituSceneFindWidget(&theScene, "settingSystemRebootButton");
        assert(settingSystemRebootButton);

        settingSystemLangChtButton = ituSceneFindWidget(&theScene, "settingSystemLangChtButton");
        assert(settingSystemLangChtButton);

        settingSystemResetButton = ituSceneFindWidget(&theScene, "settingSystemResetButton");
        assert(settingSystemResetButton);

        settingSystemLangChsButton = ituSceneFindWidget(&theScene, "settingSystemLangChsButton");
        assert(settingSystemLangChsButton);

        settingSystemLangEngButton = ituSceneFindWidget(&theScene, "settingSystemLangEngButton");
        assert(settingSystemLangEngButton);

        settingSystemBrightnessRadioBox = ituSceneFindWidget(&theScene, "settingSystemBrightnessRadioBox");
        assert(settingSystemBrightnessRadioBox);

        settingSystemBrightnessSprite = ituSceneFindWidget(&theScene, "settingSystemBrightnessSprite");
        assert(settingSystemBrightnessSprite);

        settingSystemRebootBackground = ituSceneFindWidget(&theScene, "settingSystemRebootBackground");
        assert(settingSystemRebootBackground);
    }
    ituFocusWidget(settingSystemRebootButton);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
    ituWidgetSetVisible(settingSystemRebootBackground, false);

	return true;
}

void SettingSystemReset(void)
{
    settingSystemLayer = NULL;
    settingSystemBrightnessSprite = NULL;
}
