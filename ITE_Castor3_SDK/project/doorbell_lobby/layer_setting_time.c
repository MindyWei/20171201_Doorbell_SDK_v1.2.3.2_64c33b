#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingTimeLayer;
static ITURadioBox* settingTimeRadioBox;
static ITURadioBox* settingVideoTimeRadioBox;
static ITUTextBox* settingVideoTimeTextBox;
static ITURadioBox* settingVideoTimeDialRadioBox;
static ITUTextBox* settingVideoTimeDialTextBox;
static ITURadioBox* settingVideoTimeCallRadioBox;
static ITUTextBox* settingVideoTimeCallTextBox;
static ITURadioBox* settingDoorDelayRadioBox;
static ITUTextBox* settingDoorDelayTextBox;

static ITURadioBox* settingTimeActiveRadioBox;
static ITUTextBox* settingTimeActiveTextBox;

bool SettingTimeKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (settingTimeActiveRadioBox == settingVideoTimeRadioBox)
    {
        char* str = ituTextBoxGetString(settingTimeActiveTextBox);
        if (str)
        {
            int value = atoi(str);
            if (value >= 5 && value <= 60)
            {
                theConfig.voicememo_time = value;
                ConfigSave();

                settingTimeActiveRadioBox = settingVideoTimeDialRadioBox;
                settingTimeActiveTextBox = settingVideoTimeDialTextBox;
                ituRadioBoxSetChecked(settingTimeActiveRadioBox, true);
                ituFocusWidget(settingTimeActiveTextBox);
            }
            ituTextBoxSetString(settingTimeActiveTextBox, NULL);
        }
    }
    else if (settingTimeActiveRadioBox == settingVideoTimeDialRadioBox)
    {
        char* str = ituTextBoxGetString(settingTimeActiveTextBox);
        if (str)
        {
            int value = atoi(str);
            if (value >= 30 && value <= 60)
            {
                theConfig.dial_time = value;
                ConfigSave();

                settingTimeActiveRadioBox = settingVideoTimeCallRadioBox;
                settingTimeActiveTextBox = settingVideoTimeCallTextBox;
                ituRadioBoxSetChecked(settingTimeActiveRadioBox, true);
                ituFocusWidget(settingTimeActiveTextBox);
            }
            ituTextBoxSetString(settingTimeActiveTextBox, NULL);
        }
    }
    else if (settingTimeActiveRadioBox == settingVideoTimeCallRadioBox)
    {
        char* str = ituTextBoxGetString(settingTimeActiveTextBox);
        if (str)
        {
            int value = atoi(str);
            if (value >= 15 && value <= 120)
            {
                theConfig.calling_time = value;
                ConfigSave();

                settingTimeActiveRadioBox = settingDoorDelayRadioBox;
                settingTimeActiveTextBox = settingDoorDelayTextBox;
                ituRadioBoxSetChecked(settingTimeActiveRadioBox, true);
                ituFocusWidget(settingTimeActiveTextBox);
            }
            ituTextBoxSetString(settingTimeActiveTextBox, NULL);
        }
    }
    else if (settingTimeActiveRadioBox == settingDoorDelayRadioBox)
    {
        char* str = ituTextBoxGetString(settingTimeActiveTextBox);
        if (str)
        {
            int value = atoi(str);
            if (value >= 1 && value <= 30)
            {
                theConfig.door_delay = value;
                ConfigSave();
                ituRadioBoxSetChecked(settingTimeActiveRadioBox, false);

	            ituWidgetEnable(settingLayer);
			    ituLayerGoto(settingLayer);
                ituFocusWidget(settingTimeRadioBox);
                ituWidgetSetVisible(settingTimeLayer, true);
            }
        }
    }
    return true;
}

bool SettingTimeKeySharpOnPress(ITUWidget* widget, char* param)
{
    if(ituTextBoxGetString(settingTimeActiveTextBox))
    {
        ituTextBoxSetString(settingTimeActiveTextBox, NULL);
    }
    else
    {       
        ituRadioBoxSetChecked(settingVideoTimeRadioBox, false);
        ituRadioBoxSetChecked(settingVideoTimeDialRadioBox, false);
        ituRadioBoxSetChecked(settingVideoTimeCallRadioBox, false);
        ituRadioBoxSetChecked(settingDoorDelayRadioBox, false);
        ituWidgetEnable(settingLayer);
        ituLayerGoto(settingLayer);
        ituFocusWidget(settingTimeRadioBox);
        ituWidgetSetVisible(settingTimeLayer, true);
    }
    return true;
}

bool SettingTimeKeyNumberOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxInput(settingTimeActiveTextBox, param);
	return true;
}

void SettingTimeShow(void)
{
    char buf[8];
    if (!settingVideoTimeTextBox)
    {
        settingVideoTimeTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeTextBox");
        assert(settingVideoTimeTextBox);

        settingVideoTimeDialTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeDialTextBox");
        assert(settingVideoTimeDialTextBox);

        settingVideoTimeCallTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeCallTextBox");
        assert(settingVideoTimeCallTextBox);    

        settingDoorDelayTextBox = ituSceneFindWidget(&theScene, "settingDoorDelayTextBox");
        assert(settingDoorDelayTextBox);    
    }
    sprintf(buf, "%d", theConfig.voicememo_time);
    ituTextBoxSetString(settingVideoTimeTextBox, buf);

    sprintf(buf, "%d", theConfig.dial_time);
    ituTextBoxSetString(settingVideoTimeDialTextBox, buf);

    sprintf(buf, "%d", theConfig.calling_time);
    ituTextBoxSetString(settingVideoTimeCallTextBox, buf);

    sprintf(buf, "%d", theConfig.door_delay);
    ituTextBoxSetString(settingDoorDelayTextBox, buf);
}

bool SettingTimeOnEnter(ITUWidget* widget, char* param)
{
    if (!settingTimeLayer)
    {
        settingTimeLayer = ituSceneFindWidget(&theScene, "settingTimeLayer");
        assert(settingTimeLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingTimeRadioBox = ituSceneFindWidget(&theScene, "settingTimeRadioBox");
        assert(settingTimeRadioBox);

        settingVideoTimeRadioBox = ituSceneFindWidget(&theScene, "settingVideoTimeRadioBox");
        assert(settingVideoTimeRadioBox);

        settingVideoTimeDialRadioBox = ituSceneFindWidget(&theScene, "settingVideoTimeDialRadioBox");
        assert(settingVideoTimeDialRadioBox);

        settingVideoTimeCallRadioBox = ituSceneFindWidget(&theScene, "settingVideoTimeCallRadioBox");
        assert(settingVideoTimeCallRadioBox);

        settingDoorDelayRadioBox = ituSceneFindWidget(&theScene, "settingDoorDelayRadioBox");
        assert(settingDoorDelayRadioBox);

        if (!settingVideoTimeTextBox)
        {
            settingVideoTimeTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeTextBox");
            assert(settingVideoTimeTextBox);

            settingVideoTimeDialTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeDialTextBox");
            assert(settingVideoTimeDialTextBox);

            settingVideoTimeCallTextBox = ituSceneFindWidget(&theScene, "settingVideoTimeCallTextBox");
            assert(settingVideoTimeCallTextBox);

            settingDoorDelayTextBox = ituSceneFindWidget(&theScene, "settingDoorDelayTextBox");
            assert(settingDoorDelayTextBox);
        }
    }
    settingTimeActiveRadioBox = settingVideoTimeRadioBox;
    settingTimeActiveTextBox = settingVideoTimeTextBox;

    ituTextBoxSetString(settingVideoTimeTextBox, NULL);
    ituFocusWidget(settingVideoTimeRadioBox);
    ituRadioBoxSetChecked(settingVideoTimeRadioBox, true);

	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingTimeReset(void)
{
    settingTimeLayer = NULL;
    settingVideoTimeTextBox = NULL;
}