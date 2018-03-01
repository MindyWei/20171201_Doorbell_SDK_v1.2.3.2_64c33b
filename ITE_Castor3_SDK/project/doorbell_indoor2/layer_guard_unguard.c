#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUTextBox* guardUnguardTextBox;
static ITURadioBox* guardIndoorRadioBox;
static ITURadioBox* guardOutdoorRadioBox;
static ITURadioBox* guardSleepRadioBox;

bool GuardUnguardConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* password = ituTextGetString(guardUnguardTextBox);
    if (password && strcmp(password, theConfig.guard_password) == 0)
    {
        GuardSetMode(GUARD_NONE);
        ituWidgetEnable(guardIndoorRadioBox);
        ituWidgetEnable(guardOutdoorRadioBox);
        ituWidgetEnable(guardSleepRadioBox);
		return true; // goto guardMainLayer
    }
    else
    {
        ituTextBoxSetString(guardUnguardTextBox, NULL);
    }
	return false;
}

bool GuardUnguardOnEnter(ITUWidget* widget, char* param)
{
    if (!guardUnguardTextBox)
    {
        guardUnguardTextBox = ituSceneFindWidget(&theScene, "guardUnguardTextBox");
        assert(guardUnguardTextBox);

        guardIndoorRadioBox = ituSceneFindWidget(&theScene, "guardIndoorRadioBox");
        assert(guardIndoorRadioBox);

        guardOutdoorRadioBox = ituSceneFindWidget(&theScene, "guardOutdoorRadioBox");
        assert(guardOutdoorRadioBox);

        guardSleepRadioBox = ituSceneFindWidget(&theScene, "guardSleepRadioBox");
        assert(guardSleepRadioBox);
	}
	return true;
}

void GuardUnguardReset(void)
{
    guardUnguardTextBox = NULL;
}
