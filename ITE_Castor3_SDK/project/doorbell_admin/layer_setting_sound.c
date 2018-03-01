#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define KEY1 "key1.wav"
#define KEY2 "key2.wav"
#define KEY3 "key3.wav"
#define RING1 "ring1.wav"
#define RING2 "ring2.wav"
#define RING3 "ring3.wav"

static ITUSprite* settingSoundKeyVolumeSprite;
static ITUMeter* settingSoundKeyVolumeMeter;
static ITUCircleProgressBar* settingSoundKeyVolumeCircleProgressBar;
static ITUSprite* settingSoundRingVolumeSprite;
static ITUMeter* settingSoundRingVolumeMeter;
static ITUCircleProgressBar* settingSoundRingVolumeCircleProgressBar;

static ITUSprite* settingSoundKeySprite;
static ITUSprite* settingSoundRingSprite;
static ITUBackground* settingSoundGrayBackground;
static ITURadioBox* settingSoundKey1RadioBox;
static ITURadioBox* settingSoundKey2RadioBox;
static ITURadioBox* settingSoundKey3RadioBox;
static ITURadioBox* settingSoundRing1RadioBox;
static ITURadioBox* settingSoundRing2RadioBox;
static ITURadioBox* settingSoundRing3RadioBox;
static ITUBackground* settingBackground;
static ITUBackground* settingSoundBackground;
static ITUBackground* settingSoundKeyBackground;
static ITUBackground* settingSoundRingBackground;
static ITUBackground* mainTopBackground;

// status
static int settingSoundKeyVolumeOld;
static int settingSoundRingVolumeOld;
static char settingSoundKeyOld[PATH_MAX];
static char settingSoundRingOld[PATH_MAX];

bool SettingSoundKeyVolumeMeterOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    if (percent >= 98)
    {
        percent = 100;
        ituMeterSetValue(settingSoundKeyVolumeMeter, percent);
    }

    theConfig.keylevel = percent;
    LinphoneSetKeyLevel(theConfig.keylevel);

    if (percent > 0)
    {
        ituSpriteGoto(settingSoundKeyVolumeSprite, 1);
    }
    else
    {
        ituSpriteGoto(settingSoundKeyVolumeSprite, 0);
        ituCircleProgressBarSetValue(settingSoundKeyVolumeCircleProgressBar, 0);
    }
    return true;
}

bool SettingSoundRingVolumeMeterOnChanged(ITUWidget* widget, char* param)
{
    int percent = atoi(param);

    if (percent >= 98)
    {
        percent = 100;
        ituMeterSetValue(settingSoundRingVolumeMeter, percent);
    }

    LinphoneSettingMode();//smooth the setting bar
    LinphoneSetRingLevel(percent, false);

    if (percent > 0)
    {
        ituSpriteGoto(settingSoundRingVolumeSprite, 1);
    }
    else
    {
        ituSpriteGoto(settingSoundRingVolumeSprite, 0);
        ituCircleProgressBarSetValue(settingSoundRingVolumeCircleProgressBar, 0);
    }
    return true;
}

bool SettingSoundKeyRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingSoundKey1RadioBox))
    {
        strcpy(theConfig.keysound, KEY1);
        ituSpriteGoto(settingSoundKeySprite, 0);
    }
    else if (ituRadioBoxIsChecked(settingSoundKey2RadioBox))
    {
        strcpy(theConfig.keysound, KEY2);
        ituSpriteGoto(settingSoundKeySprite, 1);
    }
    else if (ituRadioBoxIsChecked(settingSoundKey3RadioBox))
    {
        strcpy(theConfig.keysound, KEY3);
        ituSpriteGoto(settingSoundKeySprite, 2);
    }
    return true;
}

bool SettingSoundRingRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingSoundRing1RadioBox))
    {
        LinphoneSetRingSound(RING1);
        ituSpriteGoto(settingSoundRingSprite, 0);
    }
    else if (ituRadioBoxIsChecked(settingSoundRing2RadioBox))
    {
        LinphoneSetRingSound(RING2);
        ituSpriteGoto(settingSoundRingSprite, 1);
    }
    else if (ituRadioBoxIsChecked(settingSoundRing3RadioBox))
    {
        LinphoneSetRingSound(RING3);
        ituSpriteGoto(settingSoundRingSprite, 2);
    }
    ituSceneExecuteCommand(&theScene, 6, LinphonePlayRingSound, 0);
    return true;
}

static bool SettingSoundGrayBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        if (ituWidgetIsEnabled(widget))
        {
            int x = arg2 - widget->rect.x;
            int y = arg3 - widget->rect.y;

            if (ituWidgetIsInside(widget, x, y))
            {
                int xx, yy, w, h;

                x = arg2;
                y = arg3;

                if (ituWidgetIsVisible(settingSoundKeyBackground))
                {
                    ituWidgetGetGlobalPosition(settingSoundKeyBackground, &xx, &yy);
                    w = ituWidgetGetWidth(settingSoundKeyBackground);
                    h = ituWidgetGetHeight(settingSoundKeyBackground);

                    if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                    {
                        ituWidgetSetVisible(settingSoundKeyBackground, false);
                        ituWidgetSetVisible(widget, false);
                        ituWidgetEnable(settingSoundBackground);
                        ituWidgetEnable(settingBackground);
                        ituWidgetEnable(mainTopBackground);
                        return widget->dirty;
                    }
                }
                else if (ituWidgetIsVisible(settingSoundRingBackground))
                {
                    ituWidgetGetGlobalPosition(settingSoundRingBackground, &xx, &yy);
                    w = ituWidgetGetWidth(settingSoundRingBackground);
                    h = ituWidgetGetHeight(settingSoundRingBackground);

                    if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                    {
                        ituWidgetSetVisible(settingSoundRingBackground, false);
                        ituWidgetSetVisible(widget, false);
                        ituWidgetEnable(settingSoundBackground);
                        ituWidgetEnable(settingBackground);
                        ituWidgetEnable(mainTopBackground);
                        return widget->dirty;
                    }
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool SettingSoundOnEnter(ITUWidget* widget, char* param)
{
    int value;

    if (!settingSoundKeyVolumeSprite)
    {
        settingSoundKeyVolumeSprite = ituSceneFindWidget(&theScene, "settingSoundKeyVolumeSprite");
        assert(settingSoundKeyVolumeSprite);

        settingSoundKeyVolumeMeter = ituSceneFindWidget(&theScene, "settingSoundKeyVolumeMeter");
        assert(settingSoundKeyVolumeMeter);

        settingSoundKeyVolumeCircleProgressBar = ituSceneFindWidget(&theScene, "settingSoundKeyVolumeCircleProgressBar");
        assert(settingSoundKeyVolumeCircleProgressBar);

        settingSoundRingVolumeSprite = ituSceneFindWidget(&theScene, "settingSoundRingVolumeSprite");
        assert(settingSoundRingVolumeSprite);

        settingSoundRingVolumeMeter = ituSceneFindWidget(&theScene, "settingSoundRingVolumeMeter");
        assert(settingSoundRingVolumeMeter);

        settingSoundRingVolumeCircleProgressBar = ituSceneFindWidget(&theScene, "settingSoundRingVolumeCircleProgressBar");
        assert(settingSoundRingVolumeCircleProgressBar);

        settingSoundKeySprite = ituSceneFindWidget(&theScene, "settingSoundKeySprite");
        assert(settingSoundKeySprite);

        settingSoundRingSprite = ituSceneFindWidget(&theScene, "settingSoundRingSprite");
        assert(settingSoundRingSprite);

        settingSoundGrayBackground = ituSceneFindWidget(&theScene, "settingSoundGrayBackground");
        assert(settingSoundGrayBackground);
        ituWidgetSetUpdate(settingSoundGrayBackground, SettingSoundGrayBackgroundUpdate);

        settingSoundKey1RadioBox = ituSceneFindWidget(&theScene, "settingSoundKey1RadioBox");
        assert(settingSoundKey1RadioBox);

        settingSoundKey2RadioBox = ituSceneFindWidget(&theScene, "settingSoundKey2RadioBox");
        assert(settingSoundKey2RadioBox);

        settingSoundKey3RadioBox = ituSceneFindWidget(&theScene, "settingSoundKey3RadioBox");
        assert(settingSoundKey3RadioBox);

        settingSoundRing1RadioBox = ituSceneFindWidget(&theScene, "settingSoundRing1RadioBox");
        assert(settingSoundRing1RadioBox);

        settingSoundRing2RadioBox = ituSceneFindWidget(&theScene, "settingSoundRing2RadioBox");
        assert(settingSoundRing2RadioBox);

        settingSoundRing3RadioBox = ituSceneFindWidget(&theScene, "settingSoundRing3RadioBox");
        assert(settingSoundRing3RadioBox);

        settingBackground = ituSceneFindWidget(&theScene, "settingBackground");
        assert(settingBackground);

        settingSoundBackground = ituSceneFindWidget(&theScene, "settingSoundBackground");
        assert(settingSoundBackground);

        settingSoundKeyBackground = ituSceneFindWidget(&theScene, "settingSoundKeyBackground");
        assert(settingSoundKeyBackground);

        settingSoundRingBackground = ituSceneFindWidget(&theScene, "settingSoundRingBackground");
        assert(settingSoundRingBackground);

        mainTopBackground = ituSceneFindWidget(&theScene, "mainTopBackground");
        assert(mainTopBackground);
    }

    // current settings
    ituCircleProgressBarSetValue(settingSoundKeyVolumeCircleProgressBar, theConfig.keylevel);
    ituSpriteGoto(settingSoundKeyVolumeSprite, theConfig.keylevel > 0 ? 1 : 0);
    settingSoundKeyVolumeOld = theConfig.keylevel;

    value = LinphoneGetRingLevel();
    ituCircleProgressBarSetValue(settingSoundRingVolumeCircleProgressBar, value);
    ituSpriteGoto(settingSoundRingVolumeSprite, value > 0 ? 1 : 0);
    settingSoundRingVolumeOld = value;

    if (strcmp(theConfig.keysound, KEY1) == 0)
    {
        ituRadioBoxSetChecked(settingSoundKey1RadioBox, true);
        ituSpriteGoto(settingSoundKeySprite, 0);
    }
    else if (strcmp(theConfig.keysound, KEY2) == 0)
    {
        ituRadioBoxSetChecked(settingSoundKey2RadioBox, true);
        ituSpriteGoto(settingSoundKeySprite, 1);
    }
    else if (strcmp(theConfig.keysound, KEY3) == 0)
    {
        ituRadioBoxSetChecked(settingSoundKey3RadioBox, true);
        ituSpriteGoto(settingSoundKeySprite, 2);
    }
    strcpy(settingSoundKeyOld, theConfig.keysound);

    if (strcmp(theConfig.ringsound, RING1) == 0)
    {
        ituRadioBoxSetChecked(settingSoundRing1RadioBox, true);
        ituSpriteGoto(settingSoundRingSprite, 0);
    }
    else if (strcmp(theConfig.ringsound, RING2) == 0)
    {
        ituRadioBoxSetChecked(settingSoundRing2RadioBox, true);
        ituSpriteGoto(settingSoundRingSprite, 1);
    }
    else if (strcmp(theConfig.ringsound, RING3) == 0)
    {
        ituRadioBoxSetChecked(settingSoundRing3RadioBox, true);
        ituSpriteGoto(settingSoundRingSprite, 2);
    }
    strcpy(settingSoundRingOld, theConfig.ringsound);

    return true;
}

bool SettingSoundOnLeave(ITUWidget* widget, char* param)
{
    if (settingSoundKeyVolumeOld != theConfig.keylevel ||
        settingSoundRingVolumeOld != LinphoneGetRingLevel() ||
        strcmp(theConfig.keysound, settingSoundKeyOld) ||
        strcmp(theConfig.ringsound, settingSoundRingOld))
    {
        LinphoneSetRingLevel(LinphoneGetRingLevel(),false);
        ConfigSave();
    }
    return true;
}

void SettingSoundReset(void)
{
    settingSoundKeyVolumeSprite = NULL;
}
