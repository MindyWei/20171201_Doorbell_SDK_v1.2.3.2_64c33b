#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITURadioBox* settingSensorEmergencyOffRadioBox;
static ITURadioBox* settingSensorEmergencyOnRadioBox;
static ITURadioBox* settingSensorInfraredOffRadioBox;
static ITURadioBox* settingSensorInfraredOnRadioBox;
static ITURadioBox* settingSensorDoorOffRadioBox;
static ITURadioBox* settingSensorDoorOnRadioBox;
static ITURadioBox* settingSensorWindowOffRadioBox;
static ITURadioBox* settingSensorWindowOnRadioBox;
static ITURadioBox* settingSensorSmokeOffRadioBox;
static ITURadioBox* settingSensorSmokeOnRadioBox;
static ITURadioBox* settingSensorGasOffRadioBox;
static ITURadioBox* settingSensorGasOnRadioBox;
static ITURadioBox* settingSensorAreaOffRadioBox;
static ITURadioBox* settingSensorAreaOnRadioBox;
static ITURadioBox* settingSensorRobOffRadioBox;
static ITURadioBox* settingSensorRobOnRadioBox;

static settingSensorChanged;

bool SettingSensorOffRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.guard_sensor_initvalues[GUARD_EMERGENCY] = ituRadioBoxIsChecked(settingSensorEmergencyOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_INFRARED] = ituRadioBoxIsChecked(settingSensorInfraredOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_DOOR] = ituRadioBoxIsChecked(settingSensorDoorOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_WINDOW] = ituRadioBoxIsChecked(settingSensorWindowOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_SMOKE] = ituRadioBoxIsChecked(settingSensorSmokeOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_GAS] = ituRadioBoxIsChecked(settingSensorGasOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_AREA] = ituRadioBoxIsChecked(settingSensorAreaOffRadioBox) ? '0' : '1';
    theConfig.guard_sensor_initvalues[GUARD_ROB] = ituRadioBoxIsChecked(settingSensorRobOffRadioBox) ? '0' : '1';

    settingSensorChanged = true;

	return true;
}

bool SettingSensorOnRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.guard_sensor_initvalues[GUARD_EMERGENCY] = ituRadioBoxIsChecked(settingSensorEmergencyOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_INFRARED] = ituRadioBoxIsChecked(settingSensorInfraredOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_DOOR] = ituRadioBoxIsChecked(settingSensorDoorOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_WINDOW] = ituRadioBoxIsChecked(settingSensorWindowOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_SMOKE] = ituRadioBoxIsChecked(settingSensorSmokeOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_GAS] = ituRadioBoxIsChecked(settingSensorGasOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_AREA] = ituRadioBoxIsChecked(settingSensorAreaOnRadioBox) ? '1' : '0';
    theConfig.guard_sensor_initvalues[GUARD_ROB] = ituRadioBoxIsChecked(settingSensorRobOnRadioBox) ? '1' : '0';

    settingSensorChanged = true;

	return true;
}

bool SettingSensorOnEnter(ITUWidget* widget, char* param)
{
    if (!settingSensorEmergencyOffRadioBox)
    {
        settingSensorEmergencyOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorEmergencyOffRadioBox");
        assert(settingSensorEmergencyOffRadioBox);

        settingSensorEmergencyOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorEmergencyOnRadioBox");
        assert(settingSensorEmergencyOnRadioBox);

        settingSensorInfraredOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorInfraredOffRadioBox");
        assert(settingSensorInfraredOffRadioBox);

        settingSensorInfraredOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorInfraredOnRadioBox");
        assert(settingSensorInfraredOnRadioBox);

        settingSensorDoorOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorDoorOffRadioBox");
        assert(settingSensorDoorOffRadioBox);

        settingSensorDoorOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorDoorOnRadioBox");
        assert(settingSensorDoorOnRadioBox);

        settingSensorWindowOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorWindowOffRadioBox");
        assert(settingSensorWindowOffRadioBox);

        settingSensorWindowOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorWindowOnRadioBox");
        assert(settingSensorWindowOnRadioBox);

        settingSensorSmokeOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorSmokeOffRadioBox");
        assert(settingSensorSmokeOffRadioBox);

        settingSensorSmokeOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorSmokeOnRadioBox");
        assert(settingSensorSmokeOnRadioBox);

        settingSensorGasOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorGasOffRadioBox");
        assert(settingSensorGasOffRadioBox);

        settingSensorGasOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorGasOnRadioBox");
        assert(settingSensorGasOnRadioBox);

        settingSensorAreaOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorAreaOffRadioBox");
        assert(settingSensorAreaOffRadioBox);

        settingSensorAreaOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorAreaOnRadioBox");
        assert(settingSensorAreaOnRadioBox);

        settingSensorRobOffRadioBox = ituSceneFindWidget(&theScene, "settingSensorRobOffRadioBox");
        assert(settingSensorRobOffRadioBox);

        settingSensorRobOnRadioBox = ituSceneFindWidget(&theScene, "settingSensorRobOnRadioBox");
        assert(settingSensorRobOnRadioBox);
    }

    if (theConfig.guard_sensor_initvalues[GUARD_EMERGENCY] == '1')
        ituRadioBoxSetChecked(settingSensorEmergencyOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorEmergencyOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_INFRARED] == '1')
        ituRadioBoxSetChecked(settingSensorInfraredOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorInfraredOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_DOOR] == '1')
        ituRadioBoxSetChecked(settingSensorDoorOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorDoorOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_WINDOW] == '1')
        ituRadioBoxSetChecked(settingSensorWindowOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorWindowOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_SMOKE] == '1')
        ituRadioBoxSetChecked(settingSensorSmokeOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorSmokeOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_GAS] == '1')
        ituRadioBoxSetChecked(settingSensorGasOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorGasOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_AREA] == '1')
        ituRadioBoxSetChecked(settingSensorAreaOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorAreaOffRadioBox, true);

    if (theConfig.guard_sensor_initvalues[GUARD_ROB] == '1')
        ituRadioBoxSetChecked(settingSensorRobOnRadioBox, true);
    else
        ituRadioBoxSetChecked(settingSensorRobOffRadioBox, true);

    settingSensorChanged = false;

	return true;
}

bool SettingSensorOnLeave(ITUWidget* widget, char* param)
{
    if (settingSensorChanged)
        ConfigSave();

    return true;
}

void SettingSensorReset(void)
{
    settingSensorEmergencyOffRadioBox = NULL;
}
