#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUText* errorNoAdminText;

static ErrorNumber errorNumber;

void ErrorShow(ErrorNumber err)
{
    errorNumber = err;
}

bool ErrorOnEnter(ITUWidget* widget, char* param)
{
    if (!errorNoAdminText)
    {
        errorNoAdminText = ituSceneFindWidget(&theScene, "errorNoAdminText");
        assert(errorNoAdminText);
    }

    ituWidgetSetVisible(errorNoAdminText, errorNumber == ERROR_NO_ADMIN);
    SceneEnableKeypad(false);
    return true;
}

bool ErrorOnLeave(ITUWidget* widget, char* param)
{
    SceneEnableKeypad(true);
    return true;
}

void ErrorReset(void)
{
    errorNoAdminText = NULL;
}
