#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* settingDisturbModeSprite;
static ITUSprite* settingDisturbContentSprite;
static ITUSprite* settingDisturbAllSprite;
static ITUSprite* settingDisturbTimeSprite;
static ITUText* settingDisturbTimeStartText;
static ITUText* settingDisturbTimeEndText;
static ITUWheel* settingDisturbTimeStartHourWheel;
static ITUWheel* settingDisturbTimeStartMinuteWheel;
static ITUWheel* settingDisturbTimeEndHourWheel;
static ITUWheel* settingDisturbTimeEndMinuteWheel;

static int settingDisturbModeOld;
static int settingDisturbOptionOld;
static int settingDisturbStartHourOld;
static int settingDisturbStartMinOld;
static int settingDisturbEndHourOld;
static int settingDisturbEndMinOld;

bool SettingDisturbTimeStartHourWheelOnChanged(ITUWidget* widget, char* param)
{
    char buf[32];
    int i = atoi(param);
    theConfig.do_not_disturb_start_hour = i;

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_start_hour, theConfig.do_not_disturb_start_min);
    ituTextSetString(settingDisturbTimeStartText, buf);

    return true;
}

bool SettingDisturbTimeStartMinuteWheelOnChanged(ITUWidget* widget, char* param)
{
    char buf[32];
    int i = atoi(param);
    theConfig.do_not_disturb_start_min = i;

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_start_hour, theConfig.do_not_disturb_start_min);
    ituTextSetString(settingDisturbTimeStartText, buf);

    return true;
}

bool SettingDisturbTimeEndHourWheelOnChanged(ITUWidget* widget, char* param)
{
    char buf[32];
    int i = atoi(param);
    theConfig.do_not_disturb_end_hour = i;

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_end_hour, theConfig.do_not_disturb_end_min);
    ituTextSetString(settingDisturbTimeEndText, buf);

    return true;
}

bool SettingDisturbTimeEndMinuteWheelOnChanged(ITUWidget* widget, char* param)
{
    char buf[32];
    int i = atoi(param);
    theConfig.do_not_disturb_end_min = i;

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_end_hour, theConfig.do_not_disturb_end_min);
    ituTextSetString(settingDisturbTimeEndText, buf);

    return true;
}

bool SettingDisturbNormalRadioBoxOnPress(ITUWidget* widget, char* param)
{
    char buf[8];

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_MUTE)
        AudioUnMute();

    LinphoneDisableDoNotDisturb();

    theConfig.do_not_disturb_mode = DONOT_DISTURB_NONE;
    theConfig.muted = 0;
    
    sprintf(buf, "%d", theConfig.do_not_disturb_mode);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);

    SceneSetLastDoNotDisturbMode(theConfig.do_not_disturb_mode);
    
    return true;
}

bool SettingDisturbMuteRadioBoxOnPress(ITUWidget* widget, char* param)
{
    char buf[8];

    if (GuardIsAlarming())
        return false;

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL || theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME)
        LinphoneDisableDoNotDisturb();

    theConfig.do_not_disturb_mode = DONOT_DISTURB_MUTE;
    theConfig.muted = 1;
    
    AudioMute();

    sprintf(buf, "%d", theConfig.do_not_disturb_mode);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);

    SceneSetLastDoNotDisturbMode(DONOT_DISTURB_NONE);

    return true;
}

bool SettingDisturbAllRadioBoxOnPress(ITUWidget* widget, char* param)
{
    char buf[8];

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_MUTE)
        AudioUnMute();

    LinphoneEnableDoNotDisturb();

    theConfig.do_not_disturb_mode = DONOT_DISTURB_ALL;

    sprintf(buf, "%d", theConfig.do_not_disturb_mode);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);

    SceneSetLastDoNotDisturbMode(theConfig.do_not_disturb_mode);

    return true;
}

bool SettingDisturbTimeRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_MUTE)
        AudioUnMute();

    theConfig.do_not_disturb_mode = DONOT_DISTURB_TIME;

    SceneSetLastDoNotDisturbMode(theConfig.do_not_disturb_mode);

    return true;
}

bool SettingDisturbHangUpRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.do_not_disturb_option = DONOT_DISTURB_HANGUP;
    return true;
}

bool SettingDisturbVoiceMemoRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.do_not_disturb_option = DONOT_DISTURB_VOICEMEMO;
    return true;
}

bool SettingDisturbMessageRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.do_not_disturb_option = DONOT_DISTURB_MESSAGE;
    return true;
}

bool SettingDisturbTransferRadioBoxOnPress(ITUWidget* widget, char* param)
{
    theConfig.do_not_disturb_option = DONOT_DISTURB_REDIRECT;
    return true;
}

bool SettingDisturbOnEnter(ITUWidget* widget, char* param)
{
    char buf[32];

    if (!settingDisturbModeSprite)
    {
        settingDisturbModeSprite = ituSceneFindWidget(&theScene, "settingDisturbModeSprite");
        assert(settingDisturbModeSprite);

        settingDisturbContentSprite = ituSceneFindWidget(&theScene, "settingDisturbContentSprite");
        assert(settingDisturbContentSprite);

        settingDisturbAllSprite = ituSceneFindWidget(&theScene, "settingDisturbAllSprite");
        assert(settingDisturbAllSprite);

        settingDisturbTimeSprite = ituSceneFindWidget(&theScene, "settingDisturbTimeSprite");
        assert(settingDisturbTimeSprite);

        settingDisturbTimeStartText = ituSceneFindWidget(&theScene, "settingDisturbTimeStartText");
        assert(settingDisturbTimeStartText);

        settingDisturbTimeEndText = ituSceneFindWidget(&theScene, "settingDisturbTimeEndText");
        assert(settingDisturbTimeEndText);

        settingDisturbTimeStartHourWheel = ituSceneFindWidget(&theScene, "settingDisturbTimeStartHourWheel");
        assert(settingDisturbTimeStartHourWheel);

        settingDisturbTimeStartMinuteWheel = ituSceneFindWidget(&theScene, "settingDisturbTimeStartMinuteWheel");
        assert(settingDisturbTimeStartMinuteWheel);

        settingDisturbTimeEndHourWheel = ituSceneFindWidget(&theScene, "settingDisturbTimeEndHourWheel");
        assert(settingDisturbTimeEndHourWheel);

        settingDisturbTimeEndMinuteWheel = ituSceneFindWidget(&theScene, "settingDisturbTimeEndMinuteWheel");
        assert(settingDisturbTimeEndMinuteWheel);
    }

    switch (theConfig.do_not_disturb_mode)
    {
    case DONOT_DISTURB_NONE:
        ituSpriteGoto(settingDisturbModeSprite, 0);
        ituSpriteGoto(settingDisturbContentSprite, 0);
        break;

    case DONOT_DISTURB_MUTE:
        ituSpriteGoto(settingDisturbModeSprite, 1);
        ituSpriteGoto(settingDisturbContentSprite, 1);
        break;

    case DONOT_DISTURB_ALL:
        ituSpriteGoto(settingDisturbModeSprite, 2);
        ituSpriteGoto(settingDisturbContentSprite, 2);
        break;

    case DONOT_DISTURB_TIME:
        ituSpriteGoto(settingDisturbModeSprite, 3);
        ituSpriteGoto(settingDisturbContentSprite, 3);
        break;
    }

    switch (theConfig.do_not_disturb_option)
    {
    case DONOT_DISTURB_HANGUP:
        ituSpriteGoto(settingDisturbAllSprite, 0);
        ituSpriteGoto(settingDisturbTimeSprite, 0);
        break;

    case DONOT_DISTURB_VOICEMEMO:
        ituSpriteGoto(settingDisturbAllSprite, 1);
        ituSpriteGoto(settingDisturbTimeSprite, 1);
        break;

    case DONOT_DISTURB_MESSAGE:
        ituSpriteGoto(settingDisturbAllSprite, 2);
        ituSpriteGoto(settingDisturbTimeSprite, 2);
        break;

    case DONOT_DISTURB_REDIRECT:
        ituSpriteGoto(settingDisturbAllSprite, 3);
        ituSpriteGoto(settingDisturbTimeSprite, 3);
        break;
    }

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_start_hour, theConfig.do_not_disturb_start_min);
    ituTextSetString(settingDisturbTimeStartText, buf);

    sprintf(buf, "%02d:%02d", theConfig.do_not_disturb_end_hour, theConfig.do_not_disturb_end_min);
    ituTextSetString(settingDisturbTimeEndText, buf);


    ituWheelGoto(settingDisturbTimeStartHourWheel, theConfig.do_not_disturb_start_hour);
    ituWheelGoto(settingDisturbTimeStartMinuteWheel, theConfig.do_not_disturb_start_min);
    ituWheelGoto(settingDisturbTimeEndHourWheel, theConfig.do_not_disturb_end_hour);
    ituWheelGoto(settingDisturbTimeEndMinuteWheel, theConfig.do_not_disturb_end_min);

    settingDisturbModeOld = theConfig.do_not_disturb_mode;
    settingDisturbOptionOld = theConfig.do_not_disturb_option;
    settingDisturbStartHourOld = theConfig.do_not_disturb_start_hour;
    settingDisturbStartMinOld = theConfig.do_not_disturb_start_min;
    settingDisturbEndHourOld = theConfig.do_not_disturb_end_hour;
    settingDisturbEndMinOld = theConfig.do_not_disturb_end_min;

    return true;
}

bool SettingDisturbOnLeave(ITUWidget* widget, char* param)
{
    if (settingDisturbModeOld != theConfig.do_not_disturb_mode ||
        settingDisturbOptionOld != theConfig.do_not_disturb_option ||
        settingDisturbStartHourOld != theConfig.do_not_disturb_start_hour ||
        settingDisturbStartMinOld != theConfig.do_not_disturb_start_min ||
        settingDisturbEndHourOld != theConfig.do_not_disturb_end_hour ||
        settingDisturbEndMinOld != theConfig.do_not_disturb_end_min)
    {
        ConfigSave();
#ifndef BACKUP_FLAG
        ConfigSaveTemp();
#endif
        if (strcmp(theDeviceInfo.ext, "01") == 0)
            RemoteSetDontDisturbConfig();
    }
    return true;
}

void SettingDisturbReset(void)
{
    settingDisturbModeSprite = NULL;
}
