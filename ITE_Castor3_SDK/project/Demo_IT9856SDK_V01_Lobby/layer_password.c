#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* userLayer;
static ITULayer* settingLayer;
static ITULayer* settingDataSyncLayer;
static ITUTextBox* passwordTextBox;
static ITURadioBox* settingDataSyncRadioBox;

bool PasswordKeyStarOnPress(ITUWidget* widget, char* param)
{
    char* str = ituTextGetString(passwordTextBox);

    if (!str || strcmp(str, theConfig.engineer_password))
    {
        // password incorrect
        ituTextBoxSetString(passwordTextBox, NULL);
    }
    else
    {
	    ituWidgetEnable(settingLayer);
		ituLayerGoto(settingLayer);
    }
    return true;
}

bool PasswordKeySharpOnPress(ITUWidget* widget, char* param)
{
    ituLayerGoto(userLayer);
    return true;
}

bool PasswordKeyNumberOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxInput(passwordTextBox, param);
    return true;
}

bool PasswordOnEnter(ITUWidget* widget, char* param)
{
    if (!passwordTextBox)
    {
        passwordTextBox = ituSceneFindWidget(&theScene, "passwordTextBox");
        assert(passwordTextBox);

        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingDataSyncLayer = ituSceneFindWidget(&theScene, "settingDataSyncLayer");
        assert(settingDataSyncLayer);

        settingDataSyncLayer = ituSceneFindWidget(&theScene, "settingDataSyncLayer");
        assert(settingDataSyncLayer);

        settingDataSyncRadioBox = ituSceneFindWidget(&theScene, "settingDataSyncRadioBox");
        assert(settingDataSyncRadioBox);
	}
    ituTextBoxSetString(passwordTextBox, NULL);
    ituWidgetSetActive(passwordTextBox, true);
	return true;
}

void PasswordReset(void)
{
    passwordTextBox = NULL;
}
