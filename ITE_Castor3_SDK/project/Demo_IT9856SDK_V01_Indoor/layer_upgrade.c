#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* mainMenuLayer;

bool UpgradeOnTimer(ITUWidget* widget, char* param)
{
    if (UpgradeIsFinished())
    {
        SceneQuit(QUIT_NONE);
        NetworkSntpUpdate();
        ituLayerGoto(mainMenuLayer);
    }
	return true;
}

bool UpgradeOnEnter(ITUWidget* widget, char* param)
{
    if (!mainMenuLayer)
    {
        mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);
    }    
    UpgradeProcess(SceneGetQuitValue());
	return true;
}

void UpgradeReset(void)
{
    mainMenuLayer = NULL;
}
