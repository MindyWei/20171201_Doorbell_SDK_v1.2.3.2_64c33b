#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUTextBox* guardPasswordOldTextBox;
static ITUTextBox* guardPasswordNewTextBox;
static ITUTextBox* guardPasswordConfirmTextBox;
static ITUTextBox* activeGuardPasswordTextBox;

bool GuardPasswordOldDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    activeGuardPasswordTextBox = guardPasswordOldTextBox;
    ituTextBoxBack(activeGuardPasswordTextBox);
	return true;
}

bool GuardPasswordNewDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    activeGuardPasswordTextBox = guardPasswordNewTextBox;
    ituTextBoxBack(activeGuardPasswordTextBox);
	return true;
}

bool GuardPasswordConfirmDeleteButtonOnPress(ITUWidget* widget, char* param)
{
    activeGuardPasswordTextBox = guardPasswordConfirmTextBox;
    ituTextBoxBack(activeGuardPasswordTextBox);
	return true;
}

bool GuardPasswordNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = ituTextGetString(&btn->text);
    char* str = ituTextGetString(activeGuardPasswordTextBox);
    int count = str ? strlen(str) : 0;

    if (count > activeGuardPasswordTextBox->maxLen)
        ituTextSetString(activeGuardPasswordTextBox, NULL);

    if (activeGuardPasswordTextBox != guardPasswordConfirmTextBox)
    {
        if (count == activeGuardPasswordTextBox->maxLen)
        {
            if (activeGuardPasswordTextBox == guardPasswordOldTextBox)
                activeGuardPasswordTextBox = guardPasswordNewTextBox;
            else // if (activeGuardPasswordTextBox == guardPasswordNewTextBox)
                activeGuardPasswordTextBox = guardPasswordConfirmTextBox;
        }
    }
    ituTextBoxInput(activeGuardPasswordTextBox, input);
	return true;
}

bool GuardPasswordClearButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxSetString(activeGuardPasswordTextBox, NULL);

    if (activeGuardPasswordTextBox == guardPasswordConfirmTextBox)
    {
        activeGuardPasswordTextBox = guardPasswordNewTextBox;
    }
    else if (activeGuardPasswordTextBox == guardPasswordNewTextBox)
    {
        activeGuardPasswordTextBox = guardPasswordOldTextBox;
    }
	return true;
}

bool GuardPasswordSaveButtonOnPress(ITUWidget* widget, char* param)
{
    if (activeGuardPasswordTextBox != guardPasswordConfirmTextBox)
    {
        if (activeGuardPasswordTextBox == guardPasswordOldTextBox)
            activeGuardPasswordTextBox = guardPasswordNewTextBox;
        else // if (activeGuardPasswordTextBox == guardPasswordNewTextBox)
            activeGuardPasswordTextBox = guardPasswordConfirmTextBox;
    }
    else
    {
        char* oldpass = ituTextBoxGetString(guardPasswordOldTextBox);
        char* newpass = ituTextBoxGetString(guardPasswordNewTextBox);
        char* confirmpass = ituTextBoxGetString(guardPasswordConfirmTextBox);

        if (!oldpass ||
            !newpass ||
            !confirmpass ||
            strcmp(theConfig.guard_password, oldpass) ||
            strcmp(newpass, confirmpass))
        {
		    return true; // show errorDiag
        }
        else
        {
            strcpy(theConfig.guard_password, newpass);
            ConfigSave();
        }
    }
	return false;
}

bool GuardPasswordOnEnter(ITUWidget* widget, char* param)
{
    if (!guardPasswordOldTextBox)
    {
        guardPasswordOldTextBox = ituSceneFindWidget(&theScene, "guardPasswordOldTextBox");
        assert(guardPasswordOldTextBox);

        guardPasswordNewTextBox = ituSceneFindWidget(&theScene, "guardPasswordNewTextBox");
        assert(guardPasswordNewTextBox);

        guardPasswordConfirmTextBox = ituSceneFindWidget(&theScene, "guardPasswordConfirmTextBox");
        assert(guardPasswordConfirmTextBox);
	}
    ituTextBoxSetString(guardPasswordOldTextBox, NULL);
    ituTextBoxSetString(guardPasswordNewTextBox, NULL);
	ituTextBoxSetString(guardPasswordConfirmTextBox, NULL);
    activeGuardPasswordTextBox = guardPasswordOldTextBox;
	return true;
}

void GuardPasswordReset(void)
{
    guardPasswordOldTextBox = NULL;
}
