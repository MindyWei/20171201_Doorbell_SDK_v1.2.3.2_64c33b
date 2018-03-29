#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* userLayer;

bool UpgradeOnTimer(ITUWidget* widget, char* param)
{
    if (UpgradeIsFinished())
    {
        SceneQuit(QUIT_NONE);
        NetworkSntpUpdate();
        ituLayerGoto(userLayer);
    }
    return true;
}

bool UpgradeOnEnter(ITUWidget* widget, char* param)
{
    if (!userLayer)
    {
        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);
    }
    UpgradeProcess(SceneGetQuitValue());
	return true;
}

void UpgradeReset(void)
{
    userLayer = NULL;
}
