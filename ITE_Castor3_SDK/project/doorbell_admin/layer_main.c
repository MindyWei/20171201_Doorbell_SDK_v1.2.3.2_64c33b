#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* calledLayer;

bool MainNotifyCallIncomingOnCustom(ITUWidget* widget, char* param)
{
	char addr[16];
	int id=0, video=0;

    if (!calledLayer)
    {
        calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
        assert(calledLayer);
    }
	sscanf(param, "%s %d %d", addr, &id, &video);
	CalledIncomingShow(id, addr, video);
	ituLayerGoto(calledLayer);

	return true;
}

void MainReset(void)
{
    calledLayer = NULL;
}

