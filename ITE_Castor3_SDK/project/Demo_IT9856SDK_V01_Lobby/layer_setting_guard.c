#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingGuardLayer;
static ITURadioBox* settingGuardRadioBox;
static ITURadioBox* settingGuardYesRadioBox;
static ITURadioBox* settingGuardNoRadioBox;
static ITURadioBox* settingGuardTimeRadioBox;
static ITUTextBox* settingGuardTimeTextBox;

static ITURadioBox* settingGuardActiveRadioBox;

bool SettingGuardKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (settingGuardActiveRadioBox == settingGuardYesRadioBox || settingGuardActiveRadioBox == settingGuardNoRadioBox)
    {
        settingGuardActiveRadioBox = settingGuardTimeRadioBox;
        ituRadioBoxSetChecked(settingGuardActiveRadioBox, true);
        ituFocusWidget(settingGuardActiveRadioBox);
        ituTextBoxSetString(settingGuardTimeTextBox, NULL);
    }
    else
    {
        char* str = ituTextBoxGetString(settingGuardTimeTextBox);
        if (str)
        {
            int value = atoi(str);
            if (value >= 15 && value <= 120)
            {
                if (ituRadioBoxIsChecked(settingGuardYesRadioBox))
                {
                    theConfig.guard_sensors[GUARD_DOOR] = '1';
                }
                else
                {
                    theConfig.guard_sensors[GUARD_DOOR] = '0';
                }
                theConfig.guard_time = value;
                GuardSetMode();
                ConfigSave();
                ituRadioBoxSetChecked(settingGuardTimeRadioBox, false);

	            ituWidgetEnable(settingLayer);
			    ituLayerGoto(settingLayer);
                ituFocusWidget(settingGuardRadioBox);
                ituWidgetSetVisible(settingGuardLayer, true);
            }
		    else
                ituTextBoxSetString(settingGuardTimeTextBox, NULL);
        }
    }
    return true;
}

bool SettingGuardKeySharpOnPress(ITUWidget* widget, char* param)
{
    if (settingGuardActiveRadioBox == settingGuardYesRadioBox || settingGuardActiveRadioBox == settingGuardNoRadioBox)
    {
        ituRadioBoxSetChecked(settingGuardTimeRadioBox, false);
 	    ituWidgetEnable(settingLayer);
        ituLayerGoto(settingLayer);
        ituFocusWidget(settingGuardRadioBox);
        ituWidgetSetVisible(settingGuardLayer, true);
    }
    else
    {
        if(ituTextBoxGetString(settingGuardTimeTextBox))
        {
            ituTextBoxSetString(settingGuardTimeTextBox, NULL);
        }
	    else
	    {       
            ituRadioBoxSetChecked(settingGuardTimeRadioBox, false);
 	        ituWidgetEnable(settingLayer);
            ituLayerGoto(settingLayer);
            ituFocusWidget(settingGuardRadioBox);
            ituWidgetSetVisible(settingGuardLayer, true);
	    }
    }
    return true;
}

bool SettingGuardKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (settingGuardActiveRadioBox == settingGuardYesRadioBox)
    {
        if (*param == '6' || *param == '8') // right, down
        {
            settingGuardActiveRadioBox = settingGuardNoRadioBox;
            ituFocusWidget(settingGuardActiveRadioBox);
            ituRadioBoxSetChecked(settingGuardActiveRadioBox, true);
        }
    }
    else if (settingGuardActiveRadioBox == settingGuardNoRadioBox)
    {
        if (*param == '2' || *param == '4') // left, up
        {
            settingGuardActiveRadioBox = settingGuardYesRadioBox;
            ituFocusWidget(settingGuardActiveRadioBox);
            ituRadioBoxSetChecked(settingGuardActiveRadioBox, true);
        }
    }
    else if (settingGuardActiveRadioBox == settingGuardTimeRadioBox)
    {
        ituTextBoxInput(settingGuardTimeTextBox, param);
    }
	return true;
}

void SettingGuardShow(void)
{
    char buf[8];

    if (!settingGuardTimeTextBox)
    {
	    settingGuardTimeTextBox = ituSceneFindWidget(&theScene, "settingGuardTimeTextBox");
        assert(settingGuardTimeTextBox);

        settingGuardYesRadioBox = ituSceneFindWidget(&theScene, "settingGuardYesRadioBox");
        assert(settingGuardYesRadioBox);

        settingGuardNoRadioBox = ituSceneFindWidget(&theScene, "settingGuardNoRadioBox");
        assert(settingGuardNoRadioBox);
	}

    if (theConfig.guard_sensors[GUARD_DOOR] == '1')
        ituRadioBoxSetChecked(settingGuardYesRadioBox, true);
    else
        ituRadioBoxSetChecked(settingGuardNoRadioBox, true);

    sprintf(buf, "%d", theConfig.guard_time);
    ituTextBoxSetString(settingGuardTimeTextBox, buf);
}

bool SettingGuardOnEnter(ITUWidget* widget, char* param)
{
    if (!settingGuardLayer)
    {
        settingGuardLayer = ituSceneFindWidget(&theScene, "settingGuardLayer");
        assert(settingGuardLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingGuardRadioBox = ituSceneFindWidget(&theScene, "settingGuardRadioBox");
        assert(settingGuardRadioBox);

        settingGuardTimeRadioBox = ituSceneFindWidget(&theScene, "settingGuardTimeRadioBox");
        assert(settingGuardTimeRadioBox);

        if (!settingGuardTimeTextBox)
        {
	        settingGuardTimeTextBox = ituSceneFindWidget(&theScene, "settingGuardTimeTextBox");
            assert(settingGuardTimeTextBox);

            settingGuardYesRadioBox = ituSceneFindWidget(&theScene, "settingGuardYesRadioBox");
            assert(settingGuardYesRadioBox);

            settingGuardNoRadioBox = ituSceneFindWidget(&theScene, "settingGuardNoRadioBox");
            assert(settingGuardNoRadioBox);
	    }
    }

    if (theConfig.guard_sensors[GUARD_DOOR] == '1')
        settingGuardActiveRadioBox = settingGuardYesRadioBox;
    else
        settingGuardActiveRadioBox = settingGuardNoRadioBox;

    ituFocusWidget(settingGuardActiveRadioBox);

	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingGuardReset(void)
{
    settingGuardLayer = NULL;
	settingGuardTimeTextBox = NULL;
}
