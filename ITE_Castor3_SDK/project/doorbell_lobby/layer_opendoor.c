#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define OPENDOOR_DELAY (1 * 1000 / 33)  // 1 secs

static ITULayer* userLayer;

static void OpenDoorDelayCommand(int arg)
{
	ituLayerGoto(userLayer);
}

bool OpenDoorOnEnter(ITUWidget* widget, char* param)
{
    if (!userLayer)
    {
        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);
    }
    ituSceneExecuteCommand(&theScene, OPENDOOR_DELAY, OpenDoorDelayCommand, 0);
    LinphonePlayHintSound(HINT_SOUND_OPENDOOR);

    return true;
}

void OpenDoorReset(void)
{
    userLayer = NULL;
}
