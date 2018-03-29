#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* settingMiscWarnSprite;

static int settingMiscWarnMaxVol;
static int settingMiscWarnVolOld;

bool SettingMiscWarnTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int value = atoi(param);

    if (value < 5)
        value = 0;
    else if (value >= 95)
        value = 100;

    LinphoneSetWarnLevel(value);
    
    value = value * settingMiscWarnMaxVol / 100;
    ituSpriteGoto(settingMiscWarnSprite, value);

    return true;
}

bool SettingMiscWarnOnEnter(ITUWidget* widget, char* param)
{
    int value;

    if (!settingMiscWarnSprite)
    {
        settingMiscWarnSprite = ituSceneFindWidget(&theScene, "settingMiscWarnSprite");
        assert(settingMiscWarnSprite);

        // status
        settingMiscWarnMaxVol = itcTreeGetChildCount(settingMiscWarnSprite) - 1;
    }

    // current settings
    value = theConfig.warnlevel * settingMiscWarnMaxVol / 100;
    ituSpriteGoto(settingMiscWarnSprite, value);
    settingMiscWarnVolOld = theConfig.warnlevel;

    return true;
}

bool SettingMiscWarnOnLeave(ITUWidget* widget, char* param)
{
    if (settingMiscWarnVolOld != theConfig.warnlevel)
    {
        ConfigSave();
    }
    return true;
}

void SettingMiscWarnReset(void)
{
    settingMiscWarnSprite = NULL;
}
