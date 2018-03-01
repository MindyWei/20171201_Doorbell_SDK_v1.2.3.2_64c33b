#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

#define RING1 "ring1.wav"
#define RING2 "ring2.wav"
#define RING3 "ring3.wav"
#define RING4 "ring4.wav"
#define WARN1 "warn1.wav"
#define WARN2 "warn2.wav"
#define WARN3 "warn3.wav"
#define WARN4 "warn4.wav"
#define KEY1 "key1.wav"
#define KEY2 "key2.wav"
#define KEY3 "key3.wav"
#define KEY4 "key4.wav"
#define KEY5 "key5.wav"
#define KEY6 "key6.wav"
#define KEY7 "key7.wav"

static ITULayer* settingCustomLayer;
static ITULayer* settingLayer;
static ITURadioBox* settingCustomRadioBox;
static ITURadioBox* settingCustomRingRadioBox;
static ITURadioBox* settingCustomWarnRadioBox;
static ITURadioBox* settingCustomKeyRadioBox;

static ITURadioBox* settingCustomRing1RadioBox;
static ITURadioBox* settingCustomRing2RadioBox;
static ITURadioBox* settingCustomRing3RadioBox;
static ITURadioBox* settingCustomRing4RadioBox;

static ITURadioBox* settingCustomWarn1RadioBox;
static ITURadioBox* settingCustomWarn2RadioBox;
static ITURadioBox* settingCustomWarn3RadioBox;
static ITURadioBox* settingCustomWarn4RadioBox;

static ITURadioBox* settingCustomKey1RadioBox;
static ITURadioBox* settingCustomKey2RadioBox;
static ITURadioBox* settingCustomKey3RadioBox;
static ITURadioBox* settingCustomKey4RadioBox;
static ITURadioBox* settingCustomKey5RadioBox;
static ITURadioBox* settingCustomKey6RadioBox;
static ITURadioBox* settingCustomKey7RadioBox;
static ITURadioBox* settingCustomKeyNoneRadioBox;

static ITURadioBox* settingCustomActiveRadioBox;
static ITURadioBox* settingCustomRingActiveRadioBox;
static ITURadioBox* settingCustomWarnActiveRadioBox;
static ITURadioBox* settingCustomKeyActiveRadioBox;

bool SettingCustomKeyStarOnPress(ITUWidget* widget, char* param)
{
    // save to config
    if (ituRadioBoxIsChecked(settingCustomRing2RadioBox))
	{
        LinphoneSetRingSound(RING2);
	}
    else if (ituRadioBoxIsChecked(settingCustomRing3RadioBox))
	{
        LinphoneSetRingSound(RING3);
	}
    else if (ituRadioBoxIsChecked(settingCustomRing4RadioBox))
	{
        LinphoneSetRingSound(RING4);
	}
    else
	{
        LinphoneSetRingSound(RING1);
	}

    if (ituRadioBoxIsChecked(settingCustomWarn1RadioBox))
        strcpy(theConfig.warnsound, WARN1);
    else if (ituRadioBoxIsChecked(settingCustomWarn2RadioBox))
        strcpy(theConfig.warnsound, WARN2);
    else if (ituRadioBoxIsChecked(settingCustomWarn3RadioBox))
        strcpy(theConfig.warnsound, WARN3);
    else if (ituRadioBoxIsChecked(settingCustomWarn4RadioBox))
        strcpy(theConfig.warnsound, WARN4);

    if (ituRadioBoxIsChecked(settingCustomKey1RadioBox))
        strcpy(theConfig.keysound, KEY1);
    else if (ituRadioBoxIsChecked(settingCustomKey2RadioBox))
        strcpy(theConfig.keysound, KEY2);
    else if (ituRadioBoxIsChecked(settingCustomKey3RadioBox))
        strcpy(theConfig.keysound, KEY3);
    else if (ituRadioBoxIsChecked(settingCustomKey4RadioBox))
        strcpy(theConfig.keysound, KEY4);
    else if (ituRadioBoxIsChecked(settingCustomKey5RadioBox))
        strcpy(theConfig.keysound, KEY5);
    else if (ituRadioBoxIsChecked(settingCustomKey6RadioBox))
        strcpy(theConfig.keysound, KEY6);
    else if (ituRadioBoxIsChecked(settingCustomKey7RadioBox))
        strcpy(theConfig.keysound, KEY7);
    else
        strcpy(theConfig.keysound, "");

    ConfigSave();

    ituRadioBoxSetChecked(settingCustomRingRadioBox, false);
    ituRadioBoxSetChecked(settingCustomWarnRadioBox, false);
    ituRadioBoxSetChecked(settingCustomKeyRadioBox, false);
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingCustomRadioBox);
    ituWidgetSetVisible(settingCustomLayer, true);
    return true;
}

bool SettingCustomKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituRadioBoxSetChecked(settingCustomRingRadioBox, false);
    ituRadioBoxSetChecked(settingCustomWarnRadioBox, false);
    ituRadioBoxSetChecked(settingCustomKeyRadioBox, false);
	ituWidgetEnable(settingLayer);
    ituLayerGoto(settingLayer);
    ituFocusWidget(settingCustomRadioBox);
    ituWidgetSetVisible(settingCustomLayer, true);
    return true;
}

bool SettingCustomKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (*param == '4') // left
    {
        if (settingCustomActiveRadioBox == settingCustomRingRadioBox)
        {
            if (settingCustomRingActiveRadioBox == settingCustomRing2RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing1RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing3RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing2RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing4RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing3RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing1RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing4RadioBox;

            ituRadioBoxSetChecked(settingCustomRingActiveRadioBox, true);
		}
        else if (settingCustomActiveRadioBox == settingCustomWarnRadioBox)
        {
            if (settingCustomWarnActiveRadioBox == settingCustomWarn2RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn1RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn3RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn2RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn4RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn3RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn1RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn4RadioBox;

            ituRadioBoxSetChecked(settingCustomWarnActiveRadioBox, true);
		}
        else if (settingCustomActiveRadioBox == settingCustomKeyRadioBox)
        {
            if (settingCustomKeyActiveRadioBox == settingCustomKey2RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey1RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey3RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey2RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey4RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey3RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey5RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey4RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey6RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey5RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey7RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey6RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKeyNoneRadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey7RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey1RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKeyNoneRadioBox;
            ituRadioBoxSetChecked(settingCustomKeyActiveRadioBox, true);
        }
	}
    else if (*param == '6') // right
    {
        if (settingCustomActiveRadioBox == settingCustomRingRadioBox)
        {
            if (settingCustomRingActiveRadioBox == settingCustomRing1RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing2RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing2RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing3RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing3RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing4RadioBox;
            else if (settingCustomRingActiveRadioBox == settingCustomRing4RadioBox)
                settingCustomRingActiveRadioBox = settingCustomRing1RadioBox;
            ituRadioBoxSetChecked(settingCustomRingActiveRadioBox, true);
		}
        else if (settingCustomActiveRadioBox == settingCustomWarnRadioBox)
        {
            if (settingCustomWarnActiveRadioBox == settingCustomWarn1RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn2RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn2RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn3RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn3RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn4RadioBox;
            else if (settingCustomWarnActiveRadioBox == settingCustomWarn4RadioBox)
                settingCustomWarnActiveRadioBox = settingCustomWarn1RadioBox;
            ituRadioBoxSetChecked(settingCustomWarnActiveRadioBox, true);
		}
        else if (settingCustomActiveRadioBox == settingCustomKeyRadioBox)
        {
            if (settingCustomKeyActiveRadioBox == settingCustomKey1RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey2RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey2RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey3RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey3RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey4RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey4RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey5RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey5RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey6RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey6RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey7RadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKey7RadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKeyNoneRadioBox;
            else if (settingCustomKeyActiveRadioBox == settingCustomKeyNoneRadioBox)
                settingCustomKeyActiveRadioBox = settingCustomKey1RadioBox;
            ituRadioBoxSetChecked(settingCustomKeyActiveRadioBox, true);
        }
	}
    else if (*param == '2') // up
    {
        if (settingCustomActiveRadioBox == settingCustomWarnRadioBox)
            settingCustomActiveRadioBox = settingCustomRingRadioBox;
		else if (settingCustomActiveRadioBox == settingCustomKeyRadioBox)
            settingCustomActiveRadioBox = settingCustomWarnRadioBox;
        else if (settingCustomActiveRadioBox == settingCustomRingRadioBox)
            settingCustomActiveRadioBox = settingCustomKeyRadioBox;

        ituFocusWidget(settingCustomActiveRadioBox);
        ituRadioBoxSetChecked(settingCustomActiveRadioBox, true);
    }
    else if (*param == '8') // down
    {
        if (settingCustomActiveRadioBox == settingCustomRingRadioBox)
            settingCustomActiveRadioBox = settingCustomWarnRadioBox;
        else if (settingCustomActiveRadioBox == settingCustomWarnRadioBox)
            settingCustomActiveRadioBox = settingCustomKeyRadioBox;
        else if (settingCustomActiveRadioBox == settingCustomKeyRadioBox)
            settingCustomActiveRadioBox = settingCustomRingRadioBox;

        ituFocusWidget(settingCustomActiveRadioBox);
        ituRadioBoxSetChecked(settingCustomActiveRadioBox, true);
    }
    return true;
}

void SettingCustomShow(void)
{
    if (!settingCustomRing1RadioBox)
    {
        settingCustomRing1RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing1RadioBox");
        assert(settingCustomRing1RadioBox);

        settingCustomRing2RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing2RadioBox");
        assert(settingCustomRing2RadioBox);

        settingCustomRing3RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing3RadioBox");
        assert(settingCustomRing3RadioBox);

        settingCustomRing4RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing4RadioBox");
        assert(settingCustomRing4RadioBox);

        settingCustomWarn1RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn1RadioBox");
        assert(settingCustomWarn1RadioBox);

        settingCustomWarn2RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn2RadioBox");
        assert(settingCustomWarn2RadioBox);

        settingCustomWarn3RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn3RadioBox");
        assert(settingCustomWarn3RadioBox);

        settingCustomWarn4RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn4RadioBox");
        assert(settingCustomWarn4RadioBox);

        settingCustomKey1RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey1RadioBox");
        assert(settingCustomKey1RadioBox);

        settingCustomKey2RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey2RadioBox");
        assert(settingCustomKey2RadioBox);

        settingCustomKey3RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey3RadioBox");
        assert(settingCustomKey3RadioBox);

        settingCustomKey4RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey4RadioBox");
        assert(settingCustomKey4RadioBox);

        settingCustomKey5RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey5RadioBox");
        assert(settingCustomKey5RadioBox);

        settingCustomKey6RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey6RadioBox");
        assert(settingCustomKey6RadioBox);

        settingCustomKey7RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey7RadioBox");
        assert(settingCustomKey7RadioBox);

        settingCustomKeyNoneRadioBox = ituSceneFindWidget(&theScene, "settingCustomKeyNoneRadioBox");
        assert(settingCustomKeyNoneRadioBox);
	}
    // current settings
    if (strcmp(theConfig.ringsound, RING2) == 0)
        settingCustomRingActiveRadioBox = settingCustomRing2RadioBox;
    else if (strcmp(theConfig.ringsound, RING3) == 0)
        settingCustomRingActiveRadioBox = settingCustomRing3RadioBox;
    else if (strcmp(theConfig.ringsound, RING4) == 0)
        settingCustomRingActiveRadioBox = settingCustomRing4RadioBox;
    else
        settingCustomRingActiveRadioBox = settingCustomRing1RadioBox;

    ituRadioBoxSetChecked(settingCustomRingActiveRadioBox, true);

    if (strcmp(theConfig.warnsound, WARN2) == 0)
        settingCustomWarnActiveRadioBox = settingCustomWarn2RadioBox;
    else if (strcmp(theConfig.warnsound, WARN3) == 0)
        settingCustomWarnActiveRadioBox = settingCustomWarn3RadioBox;
    else if (strcmp(theConfig.warnsound, WARN4) == 0)
        settingCustomWarnActiveRadioBox = settingCustomWarn4RadioBox;
    else
        settingCustomWarnActiveRadioBox = settingCustomWarn1RadioBox;

    ituRadioBoxSetChecked(settingCustomWarnActiveRadioBox, true);

    if (theConfig.keysound[0] == '\0')
        settingCustomKeyActiveRadioBox = settingCustomKeyNoneRadioBox;
    else if (strcmp(theConfig.keysound, KEY2) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey2RadioBox;
    else if (strcmp(theConfig.keysound, KEY3) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey3RadioBox;
    else if (strcmp(theConfig.keysound, KEY4) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey4RadioBox;
    else if (strcmp(theConfig.keysound, KEY5) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey5RadioBox;
    else if (strcmp(theConfig.keysound, KEY6) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey6RadioBox;
    else if (strcmp(theConfig.keysound, KEY7) == 0)
        settingCustomKeyActiveRadioBox = settingCustomKey7RadioBox;
    else
        settingCustomKeyActiveRadioBox = settingCustomKey1RadioBox;

    ituRadioBoxSetChecked(settingCustomKeyActiveRadioBox, true);
}

bool SettingCustomOnEnter(ITUWidget* widget, char* param)
{
    if (!settingCustomLayer)
    {
        settingCustomLayer = ituSceneFindWidget(&theScene, "settingCustomLayer");
        assert(settingCustomLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingCustomRadioBox = ituSceneFindWidget(&theScene, "settingCustomRadioBox");
        assert(settingCustomRadioBox);

        settingCustomRingRadioBox = ituSceneFindWidget(&theScene, "settingCustomRingRadioBox");
        assert(settingCustomRingRadioBox);

        settingCustomWarnRadioBox = ituSceneFindWidget(&theScene, "settingCustomWarnRadioBox");
        assert(settingCustomWarnRadioBox);

        settingCustomKeyRadioBox = ituSceneFindWidget(&theScene, "settingCustomKeyRadioBox");
        assert(settingCustomKeyRadioBox);

        if (!settingCustomRing1RadioBox)
        {
            settingCustomRing1RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing1RadioBox");
            assert(settingCustomRing1RadioBox);

            settingCustomRing2RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing2RadioBox");
            assert(settingCustomRing2RadioBox);

            settingCustomRing3RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing3RadioBox");
            assert(settingCustomRing3RadioBox);

            settingCustomRing4RadioBox = ituSceneFindWidget(&theScene, "settingCustomRing4RadioBox");
            assert(settingCustomRing4RadioBox);

            settingCustomWarn1RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn1RadioBox");
            assert(settingCustomWarn1RadioBox);

            settingCustomWarn2RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn2RadioBox");
            assert(settingCustomWarn2RadioBox);

            settingCustomWarn3RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn3RadioBox");
            assert(settingCustomWarn3RadioBox);

            settingCustomWarn4RadioBox = ituSceneFindWidget(&theScene, "settingCustomWarn4RadioBox");
            assert(settingCustomWarn4RadioBox);

            settingCustomKey1RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey1RadioBox");
            assert(settingCustomKey1RadioBox);

            settingCustomKey2RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey2RadioBox");
            assert(settingCustomKey2RadioBox);

            settingCustomKey3RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey3RadioBox");
            assert(settingCustomKey3RadioBox);

            settingCustomKey4RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey4RadioBox");
            assert(settingCustomKey4RadioBox);

            settingCustomKey5RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey5RadioBox");
            assert(settingCustomKey5RadioBox);

            settingCustomKey6RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey6RadioBox");
            assert(settingCustomKey6RadioBox);

            settingCustomKey7RadioBox = ituSceneFindWidget(&theScene, "settingCustomKey7RadioBox");
            assert(settingCustomKey7RadioBox);

            settingCustomKeyNoneRadioBox = ituSceneFindWidget(&theScene, "settingCustomKeyNoneRadioBox");
            assert(settingCustomKeyNoneRadioBox);
        }
    }
	settingCustomActiveRadioBox = settingCustomRingRadioBox;
    ituRadioBoxSetChecked(settingCustomActiveRadioBox, true);
	ituFocusWidget(settingCustomActiveRadioBox);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
    return true;
}

void SettingCustomReset(void)
{
    settingCustomLayer = NULL;
	settingCustomRing1RadioBox = NULL;
}
