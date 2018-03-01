#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"

// scene
extern ITUScene theScene;
extern bool networkReady;

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
	{
        ituSpriteGoto(networkIndicatorSprite, 1);
		networkReady = true;
	}	
	else
	{
        ituSpriteGoto(networkIndicatorSprite, 0);
		networkReady = false;
	}

	return true;
}

void MainReset(void)
{
	networkIndicatorSprite = NULL;
}
