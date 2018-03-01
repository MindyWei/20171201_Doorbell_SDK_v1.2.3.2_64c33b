#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer* guardMainLayer;
static ITURadioBox* guardIndoorRadioBox;
static ITURadioBox* guardOutdoorRadioBox;
static ITURadioBox* guardSleepRadioBox;
static ITURadioBox* guardUnguardRadioBox;

static GuardMode guardModeOld;

bool GuardMainOnEnter(ITUWidget* widget, char* param)
{
    if (!guardMainLayer)
    {
        guardMainLayer = ituSceneFindWidget(&theScene, "guardMainLayer");
        assert(guardMainLayer);

        guardUnguardRadioBox = ituSceneFindWidget(&theScene, "guardUnguardRadioBox");
        assert(guardUnguardRadioBox);

        guardIndoorRadioBox = ituSceneFindWidget(&theScene, "guardIndoorRadioBox");
        assert(guardIndoorRadioBox);

        guardOutdoorRadioBox = ituSceneFindWidget(&theScene, "guardOutdoorRadioBox");
        assert(guardOutdoorRadioBox);

        guardSleepRadioBox = ituSceneFindWidget(&theScene, "guardSleepRadioBox");
        assert(guardSleepRadioBox);
	}
    ituRadioBoxSetChecked(guardUnguardRadioBox, false);
    ituRadioBoxSetChecked(guardIndoorRadioBox, false);
    ituRadioBoxSetChecked(guardOutdoorRadioBox, false);
    ituRadioBoxSetChecked(guardSleepRadioBox, false);

    // current settings
    switch (theConfig.guard_mode)
    {
    case GUARD_NONE:
        //ituRadioBoxSetChecked(guardUnguardRadioBox, true);
        break;

    case GUARD_INDOOR:
        ituRadioBoxSetChecked(guardIndoorRadioBox, true);
        break;

    case GUARD_OUTDOOR:
        ituRadioBoxSetChecked(guardOutdoorRadioBox, true);
        break;

    case GUARD_SLEEP:
        ituRadioBoxSetChecked(guardSleepRadioBox, true);
        break;
    }
    guardModeOld = theConfig.guard_mode;


    if (GuardIsAlarming())
    {
        switch (theConfig.guard_mode)
        {
        case GUARD_INDOOR:
            ituWidgetDisable(guardOutdoorRadioBox);
            ituWidgetDisable(guardSleepRadioBox);
            break;

        case GUARD_OUTDOOR:
            ituWidgetDisable(guardIndoorRadioBox);
            ituWidgetDisable(guardSleepRadioBox);
            break;

        case GUARD_SLEEP:
            ituWidgetDisable(guardIndoorRadioBox);
            ituWidgetDisable(guardOutdoorRadioBox);
            break;
        }
    }
	return true;
}

bool GuardMainOnLeave(ITUWidget* widget, char* param)
{
    if (guardModeOld != theConfig.guard_mode)
        ConfigSave();
	return true;
}

void GuardMainReset(void)
{
    guardMainLayer = NULL;
}
