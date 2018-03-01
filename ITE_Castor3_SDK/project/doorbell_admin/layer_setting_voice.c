#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* settingVoiceRecVolumeSprite;
static ITUMeter* settingVoiceRecVolumeMeter;
static ITUCircleProgressBar* settingVoiceRecVolumeCircleProgressBar;
static ITUSprite* settingVoicePlayVolumeSprite;
static ITUMeter* settingVoicePlayVolumeMeter;
static ITUCircleProgressBar* settingVoicePlayVolumeCircleProgressBar;
static ITUSprite* settingVoiceRecSprite;
static ITUSprite* settingVoicePlaySprite;

// status
static int settingVoiceRecVolumeOld;
static int settingVoicePlayVolumeOld;

bool SettingVoiceRecVolumeMeterOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    if (percent >= 98)
    {
        percent = 100;
        ituMeterSetValue(settingVoiceRecVolumeMeter, percent);
    }

    LinphoneSettingMode();//smooth the setting bar
    LinphoneSetMicLevel(percent);

    if (percent > 0)
    {
        ituSpriteGoto(settingVoiceRecVolumeSprite, 1);
    }
    else
    {
        ituSpriteGoto(settingVoiceRecVolumeSprite, 0);
        ituCircleProgressBarSetValue(settingVoiceRecVolumeCircleProgressBar, 0);
    }
    return true;
}

bool SettingVoicePlayVolumeMeterOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    if (percent >= 98)
    {
        percent = 100;
        ituMeterSetValue(settingVoicePlayVolumeMeter, percent);
    }

    LinphoneSettingMode();//smooth the setting bar
    LinphoneSetVoiceLevel(percent);

    if (percent > 0)
    {
        ituSpriteGoto(settingVoicePlayVolumeSprite, 1);
    }
    else
    {
        ituSpriteGoto(settingVoicePlayVolumeSprite, 0);
        ituCircleProgressBarSetValue(settingVoicePlayVolumeCircleProgressBar, 0);
    }
    return true;
}

bool SettingVoiceOnEnter(ITUWidget* widget, char* param)
{
    int value;

    if (!settingVoiceRecVolumeSprite)
    {
        settingVoiceRecVolumeSprite = ituSceneFindWidget(&theScene, "settingVoiceRecVolumeSprite");
        assert(settingVoiceRecVolumeSprite);

        settingVoiceRecVolumeMeter = ituSceneFindWidget(&theScene, "settingVoiceRecVolumeMeter");
        assert(settingVoiceRecVolumeMeter);

        settingVoiceRecVolumeCircleProgressBar = ituSceneFindWidget(&theScene, "settingVoiceRecVolumeCircleProgressBar");
        assert(settingVoiceRecVolumeCircleProgressBar);

        settingVoicePlayVolumeSprite = ituSceneFindWidget(&theScene, "settingVoicePlayVolumeSprite");
        assert(settingVoicePlayVolumeSprite);

        settingVoicePlayVolumeMeter = ituSceneFindWidget(&theScene, "settingVoicePlayVolumeMeter");
        assert(settingVoicePlayVolumeMeter);

        settingVoicePlayVolumeCircleProgressBar = ituSceneFindWidget(&theScene, "settingVoicePlayVolumeCircleProgressBar");
        assert(settingVoicePlayVolumeCircleProgressBar);
    }

    // current settings
    value = LinphoneGetMicLevel();
    ituCircleProgressBarSetValue(settingVoiceRecVolumeCircleProgressBar, value);
    ituSpriteGoto(settingVoiceRecVolumeSprite, value > 0 ? 1 : 0);
    settingVoiceRecVolumeOld = value;

    value = LinphoneGetVoiceLevel();
    ituCircleProgressBarSetValue(settingVoicePlayVolumeCircleProgressBar, value);
    ituSpriteGoto(settingVoicePlayVolumeSprite, value > 0 ? 1 : 0);
    settingVoicePlayVolumeOld = value;

    return true;
}

bool SettingVoiceOnLeave(ITUWidget* widget, char* param)
{
    if (settingVoiceRecVolumeOld != LinphoneGetMicLevel() ||
        settingVoicePlayVolumeOld != LinphoneGetVoiceLevel())
    {
        LinphoneSetMicLevel(LinphoneGetMicLevel());
        LinphoneSetVoiceLevel(LinphoneGetVoiceLevel());       
        ConfigSave();
    }
    return true;
}

void SettingVoiceReset(void)
{
    settingVoiceRecVolumeSprite = NULL;
}
