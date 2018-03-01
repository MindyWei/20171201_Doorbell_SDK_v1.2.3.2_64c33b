#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "config.h"

static ITUBackgroundButton* mainMenuBackgroundButton;


bool MainMenuOnTimer(ITUWidget* widget, char* param)
{

}

bool MainMenuOnEnter(ITUWidget* widget, char* param)
{
    if (!mainMenuBackgroundButton)
    {
        ITUWidget* widget;

        mainMenuBackgroundButton = ituSceneFindWidget(&theScene, "mainMenuBackgroundButton");
        assert(mainMenuBackgroundButton);

        widget = (ITUWidget*)mainMenuBackgroundButton;        
    }

    return true;
}

void MainMenuReset(void)
{
    mainMenuBackgroundButton = NULL;
}