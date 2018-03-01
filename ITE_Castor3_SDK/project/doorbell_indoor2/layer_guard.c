#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* guardMainLayer;

bool GuardIndoorRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (!guardMainLayer)
    {
        guardMainLayer = ituSceneFindWidget(&theScene, "guardMainLayer");
        assert(guardMainLayer);
    }

    if (theConfig.guard_mode != GUARD_INDOOR)
    {
        if (!GuardIsAlarming())
            GuardSetMode(GUARD_INDOOR);

        ituLayerGoto(guardMainLayer);
		return true; // goto guardCheckLayer
    }
	return true; // goto guardCheckLayer

}

bool GuardOutdoorRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (!guardMainLayer)
    {
        guardMainLayer = ituSceneFindWidget(&theScene, "guardMainLayer");
        assert(guardMainLayer);
    }

    if (theConfig.guard_mode != GUARD_OUTDOOR)
    {
        if (!GuardIsAlarming())
            GuardSetMode(GUARD_OUTDOOR);

        ituLayerGoto(guardMainLayer);
		return true; // goto guardCheckLayer
    }

	return true; // goto guardCheckLayer

}

bool GuardSleepRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (!guardMainLayer)
    {
        guardMainLayer = ituSceneFindWidget(&theScene, "guardMainLayer");
        assert(guardMainLayer);
    }

    if (theConfig.guard_mode != GUARD_SLEEP)
    {
        if (!GuardIsAlarming())
            GuardSetMode(GUARD_SLEEP);

        ituLayerGoto(guardMainLayer);
		return true; // goto guardCheckLayer
	}

	return true; // goto guardCheckLayer
}

void GuardReset(void)
{
    guardMainLayer = NULL;
}
