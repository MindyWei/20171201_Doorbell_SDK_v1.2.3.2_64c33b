#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUBackground* settingVoiceBackground;
static ITUSprite* settingVoiceRecVolumeSprite;
static ITUMeter* settingVoiceRecVolumeMeter;
static ITUCircleProgressBar* settingVoiceRecVolumeCircleProgressBar;
static ITUSprite* settingVoicePlayVolumeSprite;
static ITUMeter* settingVoicePlayVolumeMeter;
static ITUCircleProgressBar* settingVoicePlayVolumeCircleProgressBar;
static ITUSprite* settingVoiceRecSprite;
static ITUSprite* settingVoicePlaySprite;
static ITUButton* settingVoiceNewMobileButton;
static ITUButton* settingVoiceDeleteMobileButton;
static ITUBackgroundButton* settingVoiceMaxUserBackgroundButton;

// status
static int settingVoiceRecVolumeOld;
static int settingVoicePlayVolumeOld;

bool SettingVoiceRecVolumeMeterOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);
    
    if (percent >= 100)
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

    if (percent >= 100)
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

bool SettingVoiceNewMobileButtonOnPress(ITUWidget* widget, char* param)
{
    if (theConfig.mobile_user4[0] != '\0')
    {
        ituWidgetDisable(settingVoiceBackground);
        ituWidgetSetVisible(settingVoiceMaxUserBackgroundButton, true);
        return false;
    }
    return true;
}

bool SettingVoiceOnEnter(ITUWidget* widget, char* param)
{
    int value;

    if (!settingVoiceBackground)
    {
        settingVoiceBackground = ituSceneFindWidget(&theScene, "settingVoiceBackground");
        assert(settingVoiceBackground);

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

        settingVoiceNewMobileButton = ituSceneFindWidget(&theScene, "settingVoiceNewMobileButton");
        assert(settingVoiceNewMobileButton);

        settingVoiceDeleteMobileButton = ituSceneFindWidget(&theScene, "settingVoiceDeleteMobileButton");
        assert(settingVoiceDeleteMobileButton);

        settingVoiceMaxUserBackgroundButton = ituSceneFindWidget(&theScene, "settingVoiceMaxUserBackgroundButton");
        assert(settingVoiceMaxUserBackgroundButton);

    #ifdef CFG_REGISTER_QR_CODE
        ituWidgetSetVisible(settingVoiceNewMobileButton, true);
        ituWidgetSetVisible(settingVoiceDeleteMobileButton, true);
    #else
        ituWidgetSetVisible(settingVoiceNewMobileButton, false);
        ituWidgetSetVisible(settingVoiceDeleteMobileButton, false);

    #endif // CFG_REGISTER_SIP_SERVER
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
    settingVoiceBackground = NULL;
}
