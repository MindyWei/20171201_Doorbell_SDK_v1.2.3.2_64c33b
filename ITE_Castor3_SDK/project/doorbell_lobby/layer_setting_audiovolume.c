#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingAudioVolumeLayer;
static ITULayer* settingLayer;
static ITURadioBox* settingAudioVolumeRadioBox;
static ITURadioBox* settingAudioVolumeRingRadioBox;
static ITUSprite* settingAudioVolumeRingSprite;

static ITURadioBox* settingAudioVolumeKeyRadioBox;
static ITUSprite* settingAudioVolumeKeySprite;

static ITURadioBox* settingAudioVolumeVoiceRadioBox;
static ITUSprite* settingAudioVolumeVoiceSprite;

static ITURadioBox* settingAudioVolumeMicRadioBox;
static ITUSprite* settingAudioVolumeMicSprite;

static ITURadioBox* settingAudioVolumeActiveRadioBox;

static int settingAudioVolumeRingMaxVol;
static int settingAudioVolumeRingLevel;

static int settingAudioVolumeKeyMaxVol;
static int settingAudioVolumeKeyLevel;

static int settingAudioVolumeVoiceMaxVol;
static int settingAudioVolumeVoiceLevel;

static int settingAudioVolumeMicMaxVol;
static int settingAudioVolumeMicLevel;

bool SettingAudioVolumeKeyStarOnPress(ITUWidget* widget, char* param)
{
    LinphoneSetRingLevel(settingAudioVolumeRingLevel);
    LinphoneSetKeyLevel(settingAudioVolumeKeyLevel);
    LinphoneSetVoiceLevel(settingAudioVolumeVoiceLevel);
    LinphoneSetMicLevel(settingAudioVolumeMicLevel);
    ConfigSave();

    ituRadioBoxSetChecked(settingAudioVolumeRingRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeKeyRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeVoiceRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeMicRadioBox, false);
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingAudioVolumeRadioBox);
    ituWidgetSetVisible(settingAudioVolumeLayer, true);
    return true;
}

bool SettingAudioVolumeKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituRadioBoxSetChecked(settingAudioVolumeRingRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeKeyRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeVoiceRadioBox, false);
    ituRadioBoxSetChecked(settingAudioVolumeMicRadioBox, false);
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingAudioVolumeRadioBox);
    ituWidgetSetVisible(settingAudioVolumeLayer, true);
    return true;
}

bool SettingAudioVolumeKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '4') // left
    {
        if (settingAudioVolumeActiveRadioBox == settingAudioVolumeRingRadioBox)
        {
            if (settingAudioVolumeRingLevel <= 0)
                return true;

            settingAudioVolumeRingLevel -= 100 / settingAudioVolumeRingMaxVol;
            if (settingAudioVolumeRingLevel < 0)
                settingAudioVolumeRingLevel = 0;

            printf("ring vol-: %d%% %d\n", settingAudioVolumeRingLevel, settingAudioVolumeRingLevel * settingAudioVolumeRingMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeRingSprite, settingAudioVolumeRingLevel * settingAudioVolumeRingMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeKeyRadioBox)
        {
            if (settingAudioVolumeKeyLevel <= 0)
                return true;

            settingAudioVolumeKeyLevel -= 100 / settingAudioVolumeKeyMaxVol;
            if (settingAudioVolumeKeyLevel < 0)
                    settingAudioVolumeKeyLevel = 0;

            printf("key vol-: %d%% %d\n", settingAudioVolumeKeyLevel, settingAudioVolumeKeyLevel * settingAudioVolumeKeyMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeKeySprite, settingAudioVolumeKeyLevel * settingAudioVolumeKeyMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeVoiceRadioBox)
        {
            if (settingAudioVolumeVoiceLevel <= 0)
                return true;

            settingAudioVolumeVoiceLevel -= 100 / settingAudioVolumeVoiceMaxVol;
            if (settingAudioVolumeVoiceLevel < 0)
                    settingAudioVolumeVoiceLevel = 0;

            printf("voice vol-: %d%% %d\n", settingAudioVolumeVoiceLevel, settingAudioVolumeVoiceLevel * settingAudioVolumeVoiceMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeVoiceSprite, settingAudioVolumeVoiceLevel * settingAudioVolumeVoiceMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeMicRadioBox)
        {
            if (settingAudioVolumeMicLevel <= 0)
                return true;

            settingAudioVolumeMicLevel -= 100 / settingAudioVolumeMicMaxVol;
            if (settingAudioVolumeMicLevel < 0)
                    settingAudioVolumeMicLevel = 0;

            printf("mic vol-: %d%% %d\n", settingAudioVolumeMicLevel, settingAudioVolumeMicLevel * settingAudioVolumeMicMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeMicSprite, settingAudioVolumeMicLevel * settingAudioVolumeMicMaxVol / 100);
        }
    }
    else if (*param == '6') // right
    {
        if (settingAudioVolumeActiveRadioBox == settingAudioVolumeRingRadioBox)
        {
            if (settingAudioVolumeRingLevel >= 100)
                return true;

            settingAudioVolumeRingLevel += 100 / settingAudioVolumeRingMaxVol;
            if (settingAudioVolumeRingLevel > 100)
                settingAudioVolumeRingLevel = 100;

            printf("ring vol+: %d%% %d\n", settingAudioVolumeRingLevel, settingAudioVolumeRingLevel * settingAudioVolumeRingMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeRingSprite, settingAudioVolumeRingLevel * settingAudioVolumeRingMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeKeyRadioBox)
        {
            if (settingAudioVolumeKeyLevel >= 100)
                return true;

            settingAudioVolumeKeyLevel += 100 / settingAudioVolumeKeyMaxVol;
            if (settingAudioVolumeKeyLevel > 100)
                settingAudioVolumeKeyLevel = 100;

            printf("key vol+: %d%% %d\n", settingAudioVolumeKeyLevel, settingAudioVolumeKeyLevel * settingAudioVolumeKeyMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeKeySprite, settingAudioVolumeKeyLevel * settingAudioVolumeKeyMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeVoiceRadioBox)
        {
            if (settingAudioVolumeVoiceLevel >= 100)
                return true;

            settingAudioVolumeVoiceLevel += 100 / settingAudioVolumeVoiceMaxVol;
            if (settingAudioVolumeVoiceLevel > 100)
                settingAudioVolumeVoiceLevel = 100;

            printf("voice vol+: %d%% %d\n", settingAudioVolumeVoiceLevel, settingAudioVolumeVoiceLevel * settingAudioVolumeVoiceMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeVoiceSprite, settingAudioVolumeVoiceLevel * settingAudioVolumeVoiceMaxVol / 100);
        }
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeMicRadioBox)
        {
            if (settingAudioVolumeMicLevel >= 100)
                return true;

            settingAudioVolumeMicLevel += 100 / settingAudioVolumeMicMaxVol;
            if (settingAudioVolumeMicLevel > 100)
                settingAudioVolumeMicLevel = 100;

            printf("mic vol+: %d%% %d\n", settingAudioVolumeMicLevel, settingAudioVolumeMicLevel * settingAudioVolumeMicMaxVol / 100);
            ituSpriteGoto(settingAudioVolumeMicSprite, settingAudioVolumeMicLevel * settingAudioVolumeMicMaxVol / 100);
        }
    }
    else if (*param == '2') // up
    {
        if (settingAudioVolumeActiveRadioBox == settingAudioVolumeKeyRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeRingRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeVoiceRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeKeyRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeMicRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeVoiceRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeRingRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeMicRadioBox;

        ituRadioBoxSetChecked(settingAudioVolumeActiveRadioBox, true);
        ituFocusWidget(settingAudioVolumeActiveRadioBox);
	}
    else if (*param == '8') // down
    {
        if (settingAudioVolumeActiveRadioBox == settingAudioVolumeRingRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeKeyRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeKeyRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeVoiceRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeVoiceRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeMicRadioBox;
        else if (settingAudioVolumeActiveRadioBox == settingAudioVolumeMicRadioBox)
            settingAudioVolumeActiveRadioBox = settingAudioVolumeRingRadioBox;

        ituRadioBoxSetChecked(settingAudioVolumeActiveRadioBox, true);
        ituFocusWidget(settingAudioVolumeActiveRadioBox);
    }
	return true;
}

void SettingAudioVolumeShow(void)
{
    if (!settingAudioVolumeRingSprite)
    {
        settingAudioVolumeRingSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeRingSprite");
        assert(settingAudioVolumeRingSprite);

        settingAudioVolumeKeySprite = ituSceneFindWidget(&theScene, "settingAudioVolumeKeySprite");
        assert(settingAudioVolumeKeySprite);

        settingAudioVolumeVoiceSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeVoiceSprite");
        assert(settingAudioVolumeVoiceSprite);

        settingAudioVolumeMicSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeMicSprite");
        assert(settingAudioVolumeMicSprite);

        // volumes
        settingAudioVolumeRingMaxVol = itcTreeGetChildCount(settingAudioVolumeRingSprite) - 1;
        settingAudioVolumeKeyMaxVol = itcTreeGetChildCount(settingAudioVolumeKeySprite) - 1;
        settingAudioVolumeVoiceMaxVol = itcTreeGetChildCount(settingAudioVolumeVoiceSprite) - 1;
        settingAudioVolumeMicMaxVol = itcTreeGetChildCount(settingAudioVolumeMicSprite) - 1;
	}
    // volumes
    settingAudioVolumeRingLevel = LinphoneGetRingLevel();
    ituSpriteGoto(settingAudioVolumeRingSprite, settingAudioVolumeRingLevel * settingAudioVolumeRingMaxVol / 100);

    settingAudioVolumeKeyLevel = LinphoneGetKeyLevel();
    ituSpriteGoto(settingAudioVolumeKeySprite, settingAudioVolumeKeyLevel * settingAudioVolumeKeyMaxVol / 100);

    settingAudioVolumeVoiceLevel = LinphoneGetVoiceLevel();
    ituSpriteGoto(settingAudioVolumeVoiceSprite, settingAudioVolumeVoiceLevel * settingAudioVolumeVoiceMaxVol / 100);

    settingAudioVolumeMicLevel = LinphoneGetMicLevel();
    ituSpriteGoto(settingAudioVolumeMicSprite, settingAudioVolumeMicLevel * settingAudioVolumeMicMaxVol / 100);
}

bool SettingAudioVolumeOnEnter(ITUWidget* widget, char* param)
{
    if (!settingAudioVolumeLayer)
    {
        settingAudioVolumeLayer = ituSceneFindWidget(&theScene, "settingAudioVolumeLayer");
        assert(settingAudioVolumeLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingAudioVolumeRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeRadioBox");
        assert(settingAudioVolumeRadioBox);

        settingAudioVolumeRingRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeRingRadioBox");
        assert(settingAudioVolumeRingRadioBox);

        settingAudioVolumeKeyRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeKeyRadioBox");
        assert(settingAudioVolumeKeyRadioBox);

        settingAudioVolumeVoiceRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeVoiceRadioBox");
        assert(settingAudioVolumeVoiceRadioBox);

        settingAudioVolumeMicRadioBox = ituSceneFindWidget(&theScene, "settingAudioVolumeMicRadioBox");
        assert(settingAudioVolumeMicRadioBox);

        if (!settingAudioVolumeRingSprite)
        {
            settingAudioVolumeRingSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeRingSprite");
            assert(settingAudioVolumeRingSprite);

            settingAudioVolumeKeySprite = ituSceneFindWidget(&theScene, "settingAudioVolumeKeySprite");
            assert(settingAudioVolumeKeySprite);

            settingAudioVolumeVoiceSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeVoiceSprite");
            assert(settingAudioVolumeVoiceSprite);

            settingAudioVolumeMicSprite = ituSceneFindWidget(&theScene, "settingAudioVolumeMicSprite");
            assert(settingAudioVolumeMicSprite);

            // volumes
            settingAudioVolumeRingMaxVol = itcTreeGetChildCount(settingAudioVolumeRingSprite) - 1;
            settingAudioVolumeKeyMaxVol = itcTreeGetChildCount(settingAudioVolumeKeySprite) - 1;
            settingAudioVolumeVoiceMaxVol = itcTreeGetChildCount(settingAudioVolumeVoiceSprite) - 1;
            settingAudioVolumeMicMaxVol = itcTreeGetChildCount(settingAudioVolumeMicSprite) - 1;
	    }
    }

    settingAudioVolumeActiveRadioBox = settingAudioVolumeRingRadioBox;
    ituFocusWidget(settingAudioVolumeActiveRadioBox);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingAudioVolumeReset(void)
{
    settingAudioVolumeLayer = NULL;
	settingAudioVolumeRingSprite = NULL;
}
