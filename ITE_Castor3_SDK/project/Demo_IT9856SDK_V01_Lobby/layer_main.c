#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITUSprite* networkIndicatorSprite;

bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param)
{
    int on = atoi(param);

    if (!networkIndicatorSprite)
    {
        networkIndicatorSprite = ituSceneFindWidget(&theScene, "networkIndicatorSprite");
        assert(networkIndicatorSprite);
    }

	if (on)
        ituSpriteGoto(networkIndicatorSprite, 1);
	else
        ituSpriteGoto(networkIndicatorSprite, 0);

	return true;
}

bool MainKeyManagerOnPress(ITUWidget* widget, char* param)
{
	ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
    assert(callingLayer);
    
    // call manager
    puts("call manager");
    if (CallingCall(NULL) == 0)
        ituLayerGoto(callingLayer);
    
    return true;
}

void MainReset(void)
{
	networkIndicatorSprite = NULL;
}
