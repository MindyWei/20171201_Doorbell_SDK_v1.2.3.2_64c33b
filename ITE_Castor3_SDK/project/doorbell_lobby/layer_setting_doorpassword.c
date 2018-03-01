#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingDoorPasswordLayer;
static ITULayer* settingLayer;
static ITURadioBox* settingDoorPasswordRadioBox;
static ITURadioBox* settingDoorPasswordNewRadioBox;
static ITUTextBox* settingDoorPasswordNewTextBox;
static ITURadioBox* settingDoorPasswordConfirmRadioBox;
static ITUTextBox* settingDoorPasswordConfirmTextBox;

static ITURadioBox* settingDoorPasswordActiveRadioBox;
static ITUTextBox* settingDoorPasswordActiveTextBox;

bool SettingDoorPasswordKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (settingDoorPasswordActiveRadioBox == settingDoorPasswordNewRadioBox)
    {
        settingDoorPasswordActiveRadioBox = settingDoorPasswordConfirmRadioBox;
        settingDoorPasswordActiveTextBox = settingDoorPasswordConfirmTextBox;
        ituRadioBoxSetChecked(settingDoorPasswordConfirmRadioBox, true);
        ituFocusWidget(settingDoorPasswordConfirmTextBox);
    }
	else
	{
        char* newpass = ituTextBoxGetString(settingDoorPasswordNewTextBox);
        char* confirmpass = ituTextBoxGetString(settingDoorPasswordConfirmTextBox);

        if (!newpass ||
            !confirmpass ||
            strcmp(newpass, confirmpass) )
        {
            // password incorrect
            ituTextBoxSetString(settingDoorPasswordNewTextBox, NULL);
            ituTextBoxSetString(settingDoorPasswordConfirmTextBox, NULL);
            settingDoorPasswordActiveRadioBox = settingDoorPasswordNewRadioBox;
            settingDoorPasswordActiveTextBox = settingDoorPasswordNewTextBox;
            ituRadioBoxSetChecked(settingDoorPasswordNewRadioBox, true);
            ituFocusWidget(settingDoorPasswordNewTextBox);
        }
        else
        {
            strcpy(theConfig.door_password, newpass);
            ConfigSave();
            ituRadioBoxSetChecked(settingDoorPasswordActiveRadioBox, false);

			ituWidgetEnable(settingLayer);
			ituLayerGoto(settingLayer);
            ituFocusWidget(settingDoorPasswordRadioBox);
            ituWidgetSetVisible(settingDoorPasswordLayer, true);
        }
    }
	return true;
}

bool SettingDoorPasswordKeySharpOnPress(ITUWidget* widget, char* param)
{
    if (settingDoorPasswordActiveRadioBox == settingDoorPasswordNewRadioBox)
    {
		if(ituTextBoxGetString(settingDoorPasswordNewTextBox))
		{
            ituTextBoxSetString(settingDoorPasswordNewTextBox, NULL);
		}
		else
		{       
            ituRadioBoxSetChecked(settingDoorPasswordNewRadioBox, false);
			ituWidgetEnable(settingLayer);
            ituLayerGoto(settingLayer);
            ituFocusWidget(settingDoorPasswordRadioBox);
            ituWidgetSetVisible(settingDoorPasswordLayer, true);
        }
	}
    else //settingDoorPasswordActiveRadioBox == settingDoorPasswordConfirmRadioBox
    {
		if(ituTextBoxGetString(settingDoorPasswordConfirmTextBox))
		{
            ituTextBoxSetString(settingDoorPasswordConfirmTextBox, NULL);
		}
		else
		{       
            settingDoorPasswordActiveRadioBox = settingDoorPasswordNewRadioBox;
            settingDoorPasswordActiveTextBox = settingDoorPasswordNewTextBox;
            ituRadioBoxSetChecked(settingDoorPasswordNewRadioBox, true);
            ituTextBoxSetString(settingDoorPasswordNewTextBox, NULL);
            ituFocusWidget(settingDoorPasswordNewTextBox);
        }
	}
	return true;
}

bool SettingDoorPasswordKeyNumberOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxInput(settingDoorPasswordActiveTextBox, param);
	return true;
}

bool SettingDoorPasswordOnEnter(ITUWidget* widget, char* param)
{
    if (!settingLayer)
    {
        settingDoorPasswordLayer = ituSceneFindWidget(&theScene, "settingDoorPasswordLayer");
        assert(settingDoorPasswordLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingDoorPasswordRadioBox = ituSceneFindWidget(&theScene, "settingDoorPasswordRadioBox");
        assert(settingDoorPasswordRadioBox);

        settingDoorPasswordNewRadioBox = ituSceneFindWidget(&theScene, "settingDoorPasswordNewRadioBox");
        assert(settingDoorPasswordNewRadioBox);

        settingDoorPasswordNewTextBox = ituSceneFindWidget(&theScene, "settingDoorPasswordNewTextBox");
        assert(settingDoorPasswordNewTextBox);

        settingDoorPasswordConfirmRadioBox = ituSceneFindWidget(&theScene, "settingDoorPasswordConfirmRadioBox");
        assert(settingDoorPasswordConfirmRadioBox);

        settingDoorPasswordConfirmTextBox = ituSceneFindWidget(&theScene, "settingDoorPasswordConfirmTextBox");
        assert(settingDoorPasswordConfirmTextBox);
    }
    settingDoorPasswordActiveRadioBox = settingDoorPasswordNewRadioBox;
    settingDoorPasswordActiveTextBox = settingDoorPasswordNewTextBox;
    ituFocusWidget(settingDoorPasswordNewTextBox);
    ituRadioBoxSetChecked(settingDoorPasswordNewRadioBox, true);
    ituTextBoxSetString(settingDoorPasswordNewTextBox, NULL);
    ituTextBoxSetString(settingDoorPasswordConfirmTextBox, NULL);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingDoorPasswordReset(void)
{
    settingDoorPasswordLayer = NULL;
}
