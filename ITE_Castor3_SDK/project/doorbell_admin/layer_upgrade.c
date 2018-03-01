#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* mainLayer;

bool UpgradeOnTimer(ITUWidget* widget, char* param)
{
    if (UpgradeIsFinished())
    {
        SceneQuit(QUIT_NONE);
        NetworkSntpUpdate();
        ituLayerGoto(mainLayer);
    }
	return true;
}

bool UpgradeOnEnter(ITUWidget* widget, char* param)
{
    if (!mainLayer)
    {
        mainLayer = ituSceneFindWidget(&theScene, "mainLayer");
        assert(mainLayer);
    }    
    UpgradeProcess(SceneGetQuitValue());
	return true;
}

void UpgradeReset(void)
{
    mainLayer = NULL;
}
