#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* settingDisplayBrightnessSprite;
static ITUSprite* settingDisplayScreensaverTimeSprite;
static ITUSprite* settingDisplayScreensaverTypeSprite;
static ITUBackground* settingDisplayGrayBackground;
static ITURadioBox* settingDisplayScreensaverTime1MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime2MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime3MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime5MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime10MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime20MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTime30MinRadioBox;
static ITURadioBox* settingDisplayScreensaverTypeClockRadioBox;
static ITURadioBox* settingDisplayScreensaverTypeBlankRadioBox;
static ITURadioBox* settingDisplayScreensaverTypePictureRadioBox;
static ITUBackground* settingBackground;
static ITUBackground* settingDisplayBackground;
static ITUBackground* settingDisplayScreensaverTimeBackground;
static ITUBackground* settingDisplayScreensaverTypeBackground;
static ITUBackground* mainTopBackground;

// status
static int settingDisplayBrightnessOld;
static int settingDisplayScreensaverTimeOld;
static ScreensaverType settingDisplayScreensaverTypeOld;

bool SettingDisplayBrightnessTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int value = atoi(param);

    theConfig.brightness = value;
    ScreenSetBrightness(theConfig.brightness);

    ituSpriteGoto(settingDisplayBrightnessSprite, value);

    return true;
}

bool SettingDisplayScreensaverTimeRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingDisplayScreensaverTime1MinRadioBox))
    {
        theConfig.screensaver_time = 1;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 0);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime2MinRadioBox))
    {
        theConfig.screensaver_time = 2;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 1);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime3MinRadioBox))
    {
        theConfig.screensaver_time = 3;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 2);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime5MinRadioBox))
    {
        theConfig.screensaver_time = 5;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 3);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime10MinRadioBox))
    {
        theConfig.screensaver_time = 10;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 4);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime20MinRadioBox))
    {
        theConfig.screensaver_time = 20;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 5);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTime30MinRadioBox))
    {
        theConfig.screensaver_time = 30;
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 6);
    }
    return true;
}

bool SettingDisplayScreensaverTypeRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingDisplayScreensaverTypeClockRadioBox))
    {
        theConfig.screensaver_type = SCREENSAVER_CLOCK;
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 0);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTypeBlankRadioBox))
    {
        theConfig.screensaver_type = SCREENSAVER_BLANK;
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 1);
    }
    else if (ituRadioBoxIsChecked(settingDisplayScreensaverTypePictureRadioBox))
    {
        theConfig.screensaver_type = SCREENSAVER_PHOTO;
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 2);
    }
    return true;
}

static bool SettingDisplayGrayBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
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

                if (ituWidgetIsVisible(settingDisplayScreensaverTimeBackground))
                {
                    ituWidgetGetGlobalPosition(settingDisplayScreensaverTimeBackground, &xx, &yy);
                    w = ituWidgetGetWidth(settingDisplayScreensaverTimeBackground);
                    h = ituWidgetGetHeight(settingDisplayScreensaverTimeBackground);

                    if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                    {
                        ituWidgetSetVisible(settingDisplayScreensaverTimeBackground, false);
                        ituWidgetSetVisible(widget, false);
                        ituWidgetEnable(settingDisplayBackground);
                        ituWidgetEnable(settingBackground);
                        ituWidgetEnable(mainTopBackground);
                        return widget->dirty;
                    }
                }
                else if (ituWidgetIsVisible(settingDisplayScreensaverTypeBackground))
                {
                    ituWidgetGetGlobalPosition(settingDisplayScreensaverTypeBackground, &xx, &yy);
                    w = ituWidgetGetWidth(settingDisplayScreensaverTypeBackground);
                    h = ituWidgetGetHeight(settingDisplayScreensaverTypeBackground);

                    if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                    {
                        ituWidgetSetVisible(settingDisplayScreensaverTypeBackground, false);
                        ituWidgetSetVisible(widget, false);
                        ituWidgetEnable(settingDisplayBackground);
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

bool SettingDisplayOnEnter(ITUWidget* widget, char* param)
{
    if (!settingDisplayBrightnessSprite)
    {
        settingDisplayBrightnessSprite = ituSceneFindWidget(&theScene, "settingDisplayBrightnessSprite");
        assert(settingDisplayBrightnessSprite);

        settingDisplayScreensaverTimeSprite = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTimeSprite");
        assert(settingDisplayScreensaverTimeSprite);

        settingDisplayScreensaverTypeSprite = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTypeSprite");
        assert(settingDisplayScreensaverTypeSprite);

        settingDisplayGrayBackground = ituSceneFindWidget(&theScene, "settingDisplayGrayBackground");
        assert(settingDisplayGrayBackground);
        ituWidgetSetUpdate(settingDisplayGrayBackground, SettingDisplayGrayBackgroundUpdate);

        settingDisplayScreensaverTime1MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime1MinRadioBox");
        assert(settingDisplayScreensaverTime1MinRadioBox);

        settingDisplayScreensaverTime2MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime2MinRadioBox");
        assert(settingDisplayScreensaverTime2MinRadioBox);

        settingDisplayScreensaverTime3MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime3MinRadioBox");
        assert(settingDisplayScreensaverTime3MinRadioBox);

        settingDisplayScreensaverTime5MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime5MinRadioBox");
        assert(settingDisplayScreensaverTime5MinRadioBox);

        settingDisplayScreensaverTime10MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime10MinRadioBox");
        assert(settingDisplayScreensaverTime10MinRadioBox);

        settingDisplayScreensaverTime20MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime20MinRadioBox");
        assert(settingDisplayScreensaverTime20MinRadioBox);

        settingDisplayScreensaverTime30MinRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTime30MinRadioBox");
        assert(settingDisplayScreensaverTime30MinRadioBox);

        settingDisplayScreensaverTypeClockRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTypeClockRadioBox");
        assert(settingDisplayScreensaverTypeClockRadioBox);

        settingDisplayScreensaverTypeBlankRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTypeBlankRadioBox");
        assert(settingDisplayScreensaverTypeBlankRadioBox);

        settingDisplayScreensaverTypePictureRadioBox = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTypePictureRadioBox");
        assert(settingDisplayScreensaverTypePictureRadioBox);

        settingBackground = ituSceneFindWidget(&theScene, "settingBackground");
        assert(settingBackground);

        settingDisplayBackground = ituSceneFindWidget(&theScene, "settingDisplayBackground");
        assert(settingDisplayBackground);

        settingDisplayScreensaverTimeBackground = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTimeBackground");
        assert(settingDisplayScreensaverTimeBackground);

        settingDisplayScreensaverTypeBackground = ituSceneFindWidget(&theScene, "settingDisplayScreensaverTypeBackground");
        assert(settingDisplayScreensaverTypeBackground);

        mainTopBackground = ituSceneFindWidget(&theScene, "mainTopBackground");
        assert(mainTopBackground);
    }

    // current settings
    ituSpriteGoto(settingDisplayBrightnessSprite, theConfig.brightness);
    settingDisplayBrightnessOld = theConfig.brightness;

    switch (theConfig.screensaver_time)
    {
    case 1:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime1MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 0);
        break;

    case 2:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime2MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 1);
        break;

    case 3:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime3MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 2);
        break;

    case 5:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime5MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 3);
        break;

    case 10:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime10MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 4);
        break;

    case 20:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime20MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 5);
        break;

    case 30:
        ituRadioBoxSetChecked(settingDisplayScreensaverTime30MinRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTimeSprite, 6);
        break;
	}
    settingDisplayScreensaverTimeOld = theConfig.screensaver_time;

    switch (theConfig.screensaver_type)
    {
    case SCREENSAVER_CLOCK:
        ituRadioBoxSetChecked(settingDisplayScreensaverTypeClockRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 0);
        break;

    case SCREENSAVER_BLANK:
        ituRadioBoxSetChecked(settingDisplayScreensaverTypeBlankRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 1);
        break;

    case SCREENSAVER_PHOTO:
        ituRadioBoxSetChecked(settingDisplayScreensaverTypePictureRadioBox, true);
        ituSpriteGoto(settingDisplayScreensaverTypeSprite, 2);
        break;
	}
    settingDisplayScreensaverTypeOld = theConfig.screensaver_type;

    return true;
}

bool SettingDisplayOnLeave(ITUWidget* widget, char* param)
{
    if (settingDisplayBrightnessOld != theConfig.brightness ||
        settingDisplayScreensaverTimeOld != theConfig.screensaver_time ||
        settingDisplayScreensaverTypeOld != theConfig.screensaver_type)
    {
        ConfigSave();
    }
    return true;
}

void SettingDisplayReset(void)
{
    settingDisplayBrightnessSprite = NULL;
}
