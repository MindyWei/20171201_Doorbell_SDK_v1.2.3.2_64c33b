#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingLocalPasswordLayer;
static ITURadioBox* settingLocalPasswordRadioBox;
static ITURadioBox* settingLocalPasswordOldRadioBox;
static ITUTextBox* settingLocalPasswordOldTextBox;
static ITURadioBox* settingLocalPasswordNewRadioBox;
static ITUTextBox* settingLocalPasswordNewTextBox;
static ITURadioBox* settingLocalPasswordConfirmRadioBox;
static ITUTextBox* settingLocalPasswordConfirmTextBox;

static ITURadioBox* settingLocalPasswordActiveRadioBox;
static ITUTextBox* settingLocalPasswordActiveTextBox;

bool SettingLocalPasswordKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (settingLocalPasswordActiveRadioBox == settingLocalPasswordOldRadioBox)
    {
        settingLocalPasswordActiveRadioBox = settingLocalPasswordNewRadioBox;
        settingLocalPasswordActiveTextBox = settingLocalPasswordNewTextBox;
        ituRadioBoxSetChecked(settingLocalPasswordNewRadioBox, true);
        ituFocusWidget(settingLocalPasswordNewTextBox);
    }
    else if (settingLocalPasswordActiveRadioBox == settingLocalPasswordNewRadioBox)
    {
        settingLocalPasswordActiveRadioBox = settingLocalPasswordConfirmRadioBox;
        settingLocalPasswordActiveTextBox = settingLocalPasswordConfirmTextBox;
        ituRadioBoxSetChecked(settingLocalPasswordConfirmRadioBox, true);
        ituFocusWidget(settingLocalPasswordConfirmTextBox);
    }
    else
    {
        char* oldpass = ituTextBoxGetString(settingLocalPasswordOldTextBox);
        char* newpass = ituTextBoxGetString(settingLocalPasswordNewTextBox);
        char* confirmpass = ituTextBoxGetString(settingLocalPasswordConfirmTextBox);

        if (!oldpass ||
            !newpass ||
            !confirmpass ||
            strcmp(oldpass, theConfig.engineer_password) ||
            strcmp(newpass, confirmpass))
        {
            // password incorrect
            ituTextBoxSetString(settingLocalPasswordOldTextBox, NULL);
            ituTextBoxSetString(settingLocalPasswordNewTextBox, NULL);
            ituTextBoxSetString(settingLocalPasswordConfirmTextBox, NULL);
            settingLocalPasswordActiveRadioBox = settingLocalPasswordOldRadioBox;
            settingLocalPasswordActiveTextBox = settingLocalPasswordOldTextBox;
            ituRadioBoxSetChecked(settingLocalPasswordOldRadioBox, true);
            ituFocusWidget(settingLocalPasswordOldTextBox);
        }
        else
        {
            strcpy(theConfig.engineer_password, newpass);
            ConfigSave();
            ituRadioBoxSetChecked(settingLocalPasswordActiveRadioBox, false);

			ituWidgetEnable(settingLayer);
			ituLayerGoto(settingLayer);
            ituFocusWidget(settingLocalPasswordRadioBox);
            ituWidgetSetVisible(settingLocalPasswordLayer, true);
        }
    }
    return true;
}

bool SettingLocalPasswordKeySharpOnPress(ITUWidget* widget, char* param)
{
    if (settingLocalPasswordActiveRadioBox == settingLocalPasswordOldRadioBox)
    {
		if(ituTextBoxGetString(settingLocalPasswordOldTextBox))
		{
            ituTextBoxSetString(settingLocalPasswordOldTextBox, NULL);
		}
		else
		{
            ituRadioBoxSetChecked(settingLocalPasswordOldRadioBox, false);
			ituWidgetEnable(settingLayer);
            ituLayerGoto(settingLayer);
            ituFocusWidget(settingLocalPasswordRadioBox);
            ituWidgetSetVisible(settingLocalPasswordLayer, true);
		}
    }
    else if (settingLocalPasswordActiveRadioBox == settingLocalPasswordNewRadioBox)
    {
		if(ituTextBoxGetString(settingLocalPasswordNewTextBox))
		{
            ituTextBoxSetString(settingLocalPasswordNewTextBox, NULL);
		}
		else
		{       
            settingLocalPasswordActiveRadioBox = settingLocalPasswordOldRadioBox;
            settingLocalPasswordActiveTextBox = settingLocalPasswordOldTextBox;
            ituRadioBoxSetChecked(settingLocalPasswordOldRadioBox, true);
            ituTextBoxSetString(settingLocalPasswordOldTextBox, NULL);
            ituFocusWidget(settingLocalPasswordOldTextBox);
        }
	}
    else //settingLocalPasswordActiveRadioBox == settingLocalPasswordConfirmRadioBox
    {
		if(ituTextBoxGetString(settingLocalPasswordConfirmTextBox))
		{
            ituTextBoxSetString(settingLocalPasswordConfirmTextBox, NULL);
		}
		else
		{       
            settingLocalPasswordActiveRadioBox = settingLocalPasswordNewRadioBox;
            settingLocalPasswordActiveTextBox = settingLocalPasswordNewTextBox;
            ituRadioBoxSetChecked(settingLocalPasswordNewRadioBox, true);
            ituTextBoxSetString(settingLocalPasswordNewTextBox, NULL);
            ituFocusWidget(settingLocalPasswordNewTextBox);
        }
	}
	return true;
}

bool SettingLocalPasswordKeyNumberOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxInput(settingLocalPasswordActiveTextBox, param);
	return true;
}

bool SettingLocalPasswordOnEnter(ITUWidget* widget, char* param)
{
    if (!settingLayer)
    {
        settingLocalPasswordLayer = ituSceneFindWidget(&theScene, "settingLocalPasswordLayer");
        assert(settingLocalPasswordLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingLocalPasswordRadioBox = ituSceneFindWidget(&theScene, "settingLocalPasswordRadioBox");
        assert(settingLocalPasswordRadioBox);

        settingLocalPasswordOldRadioBox = ituSceneFindWidget(&theScene, "settingLocalPasswordOldRadioBox");
        assert(settingLocalPasswordOldRadioBox);

        settingLocalPasswordOldTextBox = ituSceneFindWidget(&theScene, "settingLocalPasswordOldTextBox");
        assert(settingLocalPasswordOldTextBox);

        settingLocalPasswordNewRadioBox = ituSceneFindWidget(&theScene, "settingLocalPasswordNewRadioBox");
        assert(settingLocalPasswordNewRadioBox);

        settingLocalPasswordNewTextBox = ituSceneFindWidget(&theScene, "settingLocalPasswordNewTextBox");
        assert(settingLocalPasswordNewTextBox);

        settingLocalPasswordConfirmRadioBox = ituSceneFindWidget(&theScene, "settingLocalPasswordConfirmRadioBox");
        assert(settingLocalPasswordConfirmRadioBox);

        settingLocalPasswordConfirmTextBox = ituSceneFindWidget(&theScene, "settingLocalPasswordConfirmTextBox");
        assert(settingLocalPasswordConfirmTextBox);
    }
    settingLocalPasswordActiveRadioBox = settingLocalPasswordOldRadioBox;
    settingLocalPasswordActiveTextBox = settingLocalPasswordOldTextBox;
    ituFocusWidget(settingLocalPasswordOldTextBox);
    ituRadioBoxSetChecked(settingLocalPasswordOldRadioBox, true);
    ituTextBoxSetString(settingLocalPasswordOldTextBox, NULL);
    ituTextBoxSetString(settingLocalPasswordNewTextBox, NULL);
    ituTextBoxSetString(settingLocalPasswordConfirmTextBox, NULL);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingLocalPasswordReset(void)
{
    settingLocalPasswordLayer = NULL;
}
