#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUTextBox* guardUserPasswordTextBox;

bool GuardUserConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char* password;
    password = ituTextGetString(guardUserPasswordTextBox);
    if (password && strcmp(theConfig.guard_password, password) == 0)
	    return true; //goto guardUserSettingLayer

    ituTextBoxSetString(guardUserPasswordTextBox, NULL);
	return false;
}

bool GuardUserOnEnter(ITUWidget* widget, char* param)
{
    if (!guardUserPasswordTextBox)
    {
        guardUserPasswordTextBox = ituSceneFindWidget(&theScene, "guardUserPasswordTextBox");
        assert(guardUserPasswordTextBox);
	}
    ituTextBoxSetString(guardUserPasswordTextBox, NULL);
	return true;
}

void GuardUserReset(void)
{
    guardUserPasswordTextBox = NULL;
}
