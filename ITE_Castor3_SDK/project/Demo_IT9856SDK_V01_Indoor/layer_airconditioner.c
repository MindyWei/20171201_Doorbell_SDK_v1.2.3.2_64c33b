#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "scene.h"
#include "doorbell.h"

static ITUBackground* airConditionerBackground;
static ITUBackground* airConditionerModeBackground;
static ITURadioBox* airConditionerAutoRadioBox;
static ITURadioBox* airConditionerCoolRadioBox;
static ITURadioBox* airConditionerDryRadioBox;
static ITURadioBox* airConditionerFanRadioBox;
static ITURadioBox* airConditionerHeatRadioBox;
static ITUBackground* airConditionerWindBackground;
static ITURadioBox* airConditionerWindSlowRadioBox;
static ITURadioBox* airConditionerWindNormalRadioBox;
static ITURadioBox* airConditionerWindFastRadioBox;
static ITURadioBox* airConditionerWindAutoRadioBox;
static ITUBackgroundButton* airConditionerCurrTempBackgroundButton;
static ITUText* airConditionerCurrTempText;
static ITUText* airConditionerCelsiusText;
static ITUText* airConditionerFahrenheitText;
static ITUCheckBox* airConditionerPowerButton;
static ITUBackground* airConditionerTempBackground;
static ITUWidget* airConditionerTempCContainer;
static ITUProgressBar* airConditionerTempCProgressBar;
static ITUTrackBar* airConditionerTempCTrackBar;
static ITUWidget* airConditionerTempFContainer;
static ITUProgressBar* airConditionerTempFProgressBar;
static ITUTrackBar* airConditionerTempFTrackBar;
static ITUSprite* airConditionerMinTempSprite;
static ITUSprite* airConditionerMaxTempSprite;
static ITUAnimation* airConditionerWindSlowAnimation;
static ITUAnimation* airConditionerWindNormalAnimation;
static ITUAnimation* airConditionerWindFastAnimation;
static ITUAnimation* airConditionerWindAutoAnimation;

extern uint8_t* mainMenuWallpaperData;
extern int mainMenuWallpaperSize;

static bool airConditionerCelsiusOld, airConditionerAvail;
static HouseInfo airConditionerInfo;
static HouseAirConditionerSetting airConditionerSetting;

static void AirConditionerPowerOn(void)
{
    float temperature;
    char buf[8];
    int tempf;

    ituWidgetEnable(airConditionerModeBackground);

    switch (airConditionerSetting.mode)
    {
    case HOUSE_AC_COOL:
        ituRadioBoxSetChecked(airConditionerCoolRadioBox, true);
        break;

    case HOUSE_AC_WARM:
        ituRadioBoxSetChecked(airConditionerHeatRadioBox, true);
        break;

    case HOUSE_AC_FAN:
        ituRadioBoxSetChecked(airConditionerFanRadioBox, true);
        break;

    case HOUSE_AC_DRY:
        ituRadioBoxSetChecked(airConditionerDryRadioBox, true);
        break;

    case HOUSE_AC_AUTO:
        ituRadioBoxSetChecked(airConditionerAutoRadioBox, true);
        break;
    }

    ituWidgetEnable(airConditionerWindBackground);

    if (airConditionerSetting.wind > 66)
        ituRadioBoxSetChecked(airConditionerWindFastRadioBox, true);
    else if (airConditionerSetting.wind > 33)
        ituRadioBoxSetChecked(airConditionerWindNormalRadioBox, true);
    else if (airConditionerSetting.wind > 0)
        ituRadioBoxSetChecked(airConditionerWindSlowRadioBox, true);
    else
        ituRadioBoxSetChecked(airConditionerWindAutoRadioBox, true);

    ituWidgetSetVisible(airConditionerCurrTempBackgroundButton, true);

    tempf = airConditionerSetting.temperature * 9.0f / 5.0f + 32.0f;

    if (theConfig.airconditioner_celsius)
    {
        temperature = airConditionerSetting.temperature;
    }
    else
    {
        temperature = tempf;
    }
    sprintf(buf, "%d", (int)roundf(temperature));
    ituTextSetString(airConditionerCurrTempText, buf);

    ituCheckBoxSetChecked(airConditionerPowerButton, true);

    ituWidgetSetVisible(airConditionerTempBackground, true);
    ituProgressBarSetValue(airConditionerTempCProgressBar, (int)roundf(airConditionerSetting.temperature));
    ituTrackBarSetValue(airConditionerTempCTrackBar, (int)roundf(airConditionerSetting.temperature));
    ituProgressBarSetValue(airConditionerTempFProgressBar, tempf);
    ituTrackBarSetValue(airConditionerTempFTrackBar, tempf);
}

static void AirConditionerPowerOff(void)
{
    ituCheckBoxSetChecked(airConditionerPowerButton, false);
    ituWidgetDisable(airConditionerModeBackground);
    ituRadioBoxSetChecked(airConditionerCoolRadioBox, false);
    ituRadioBoxSetChecked(airConditionerHeatRadioBox, false);
    ituRadioBoxSetChecked(airConditionerFanRadioBox, false);
    ituRadioBoxSetChecked(airConditionerDryRadioBox, false);
    ituRadioBoxSetChecked(airConditionerAutoRadioBox, false);
    ituWidgetDisable(airConditionerWindBackground);
    ituRadioBoxSetChecked(airConditionerWindFastRadioBox, false);
    ituRadioBoxSetChecked(airConditionerWindNormalRadioBox, false);
    ituRadioBoxSetChecked(airConditionerWindSlowRadioBox, false);
    ituRadioBoxSetChecked(airConditionerWindAutoRadioBox, false);
    ituWidgetSetVisible(airConditionerCurrTempBackgroundButton, false);
    ituWidgetSetVisible(airConditionerTempBackground, false);
    ituWidgetSetVisible(airConditionerWindSlowAnimation, false);
    ituWidgetSetVisible(airConditionerWindNormalAnimation, false);
    ituWidgetSetVisible(airConditionerWindFastAnimation, false);
    ituWidgetSetVisible(airConditionerWindAutoAnimation, false);
    ituWidgetSetVisible(airConditionerWindFastRadioBox, true);
    ituWidgetSetVisible(airConditionerWindNormalRadioBox, true);
    ituWidgetSetVisible(airConditionerWindSlowRadioBox, true);
    ituWidgetSetVisible(airConditionerWindAutoRadioBox, true);
}

bool AirConditionerPowerButtonOnPress(ITUWidget* widget, char* param)
{
    int ret;

    if (ituCheckBoxIsChecked(airConditionerPowerButton))
    {
        ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_OPEN, NULL);
        AirConditionerPowerOn();
    }
    else
    {
        ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_CLOSE, NULL);
        AirConditionerPowerOff();
    }
    assert(ret == 0);

    return true;
}

bool AirConditionerAutoRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.mode = HOUSE_AC_AUTO;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerCoolRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.mode = HOUSE_AC_COOL;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerDryRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.mode = HOUSE_AC_DRY;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerFanRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.mode = HOUSE_AC_FAN;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerHeatRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.mode = HOUSE_AC_WARM;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerWindSlowRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.wind = 25;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerWindNormalRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.wind = 50;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerWindFastRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.wind = 75;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerWindAutoRadioBoxOnPress(ITUWidget* widget, char* param)
{
    int ret;

    airConditionerSetting.wind = 0;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    return true;
}

bool AirConditionerCurrTempBackgroundButtonOnMouseLongPress(ITUWidget* widget, char* param)
{
    if (theConfig.airconditioner_celsius)
    {
        theConfig.airconditioner_celsius = 0;
        ituWidgetSetVisible(airConditionerCelsiusText, false);
        ituWidgetSetVisible(airConditionerFahrenheitText, true);
        ituWidgetSetVisible(airConditionerTempCContainer, false);
        ituWidgetSetVisible(airConditionerTempFContainer, true);
        ituSpriteGoto(airConditionerMinTempSprite, 1);
        ituSpriteGoto(airConditionerMaxTempSprite, 1);
    }
    else
    {
        theConfig.airconditioner_celsius = 1;
        ituWidgetSetVisible(airConditionerCelsiusText, true);
        ituWidgetSetVisible(airConditionerFahrenheitText, false);
        ituWidgetSetVisible(airConditionerTempCContainer, true);
        ituWidgetSetVisible(airConditionerTempFContainer, false);
        ituSpriteGoto(airConditionerMinTempSprite, 0);
        ituSpriteGoto(airConditionerMaxTempSprite, 0);
    }
    return true;
}

bool AirConditionerTempCTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int ret, tempF, value = atoi(param);

    airConditionerSetting.temperature = (float)value;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    tempF = (int)roundf(airConditionerSetting.temperature * 9.0f / 5.0f) + 32;
    ituProgressBarSetValue(airConditionerTempFProgressBar, tempF);
    ituTrackBarSetValue(airConditionerTempFTrackBar, tempF);

    return true;
}

bool AirConditionerTempFTrackBarOnChanged(ITUWidget* widget, char* param)
{
    int ret, value = atoi(param);

    airConditionerSetting.temperature = (value - 32) * 5.0f / 9.0f;
    ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_SET, &airConditionerSetting);
    assert(ret == 0);

    ituProgressBarSetValue(airConditionerTempCProgressBar, (int)roundf(airConditionerSetting.temperature));
    ituTrackBarSetValue(airConditionerTempCTrackBar, (int)roundf(airConditionerSetting.temperature));

    return true;
}

bool AirConditionerOnTimer(ITUWidget* widget, char* param)
{
    if (airConditionerAvail && airConditionerInfo.status == HOUSE_ON)
    {
        char buf[8];
        int temperature, ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_GET, &airConditionerSetting);
        assert(ret == 0);

        if (theConfig.airconditioner_celsius)
        {
            temperature = (int)roundf(airConditionerSetting.temperature);
        }
        else
        {
            temperature = (int)roundf(airConditionerSetting.temperature * 9.0f / 5.0f) + 32;
        }
        sprintf(buf, "%d", temperature);
        ituTextSetString(airConditionerCurrTempText, buf);
    }

    return true;
}

bool AirConditionerOnEnter(ITUWidget* widget, char* param)
{
    if (!airConditionerBackground)
    {
        airConditionerBackground = ituSceneFindWidget(&theScene, "airConditionerBackground");
        assert(airConditionerBackground);

        airConditionerModeBackground = ituSceneFindWidget(&theScene, "airConditionerModeBackground");
        assert(airConditionerModeBackground);

        airConditionerAutoRadioBox = ituSceneFindWidget(&theScene, "airConditionerAutoRadioBox");
        assert(airConditionerAutoRadioBox);

        airConditionerCoolRadioBox = ituSceneFindWidget(&theScene, "airConditionerCoolRadioBox");
        assert(airConditionerCoolRadioBox);

        airConditionerDryRadioBox = ituSceneFindWidget(&theScene, "airConditionerDryRadioBox");
        assert(airConditionerDryRadioBox);

        airConditionerFanRadioBox = ituSceneFindWidget(&theScene, "airConditionerFanRadioBox");
        assert(airConditionerFanRadioBox);

        airConditionerHeatRadioBox = ituSceneFindWidget(&theScene, "airConditionerHeatRadioBox");
        assert(airConditionerHeatRadioBox);

        airConditionerWindBackground = ituSceneFindWidget(&theScene, "airConditionerWindBackground");
        assert(airConditionerWindBackground);

        airConditionerWindSlowRadioBox = ituSceneFindWidget(&theScene, "airConditionerWindSlowRadioBox");
        assert(airConditionerWindSlowRadioBox);

        airConditionerWindNormalRadioBox = ituSceneFindWidget(&theScene, "airConditionerWindNormalRadioBox");
        assert(airConditionerWindNormalRadioBox);

        airConditionerWindFastRadioBox = ituSceneFindWidget(&theScene, "airConditionerWindFastRadioBox");
        assert(airConditionerWindFastRadioBox);

        airConditionerWindAutoRadioBox = ituSceneFindWidget(&theScene, "airConditionerWindAutoRadioBox");
        assert(airConditionerWindAutoRadioBox);

        airConditionerCurrTempBackgroundButton = ituSceneFindWidget(&theScene, "airConditionerCurrTempBackgroundButton");
        assert(airConditionerCurrTempBackgroundButton);

        airConditionerCurrTempText = ituSceneFindWidget(&theScene, "airConditionerCurrTempText");
        assert(airConditionerCurrTempText);

        airConditionerCelsiusText = ituSceneFindWidget(&theScene, "airConditionerCelsiusText");
        assert(airConditionerCelsiusText);

        airConditionerFahrenheitText = ituSceneFindWidget(&theScene, "airConditionerFahrenheitText");
        assert(airConditionerFahrenheitText);

        airConditionerPowerButton = ituSceneFindWidget(&theScene, "airConditionerPowerButton");
        assert(airConditionerPowerButton);

        airConditionerTempBackground = ituSceneFindWidget(&theScene, "airConditionerTempBackground");
        assert(airConditionerTempBackground);

        airConditionerTempCContainer = ituSceneFindWidget(&theScene, "airConditionerTempCContainer");
        assert(airConditionerTempCContainer);

        airConditionerTempCProgressBar = ituSceneFindWidget(&theScene, "airConditionerTempCProgressBar");
        assert(airConditionerTempCProgressBar);

        airConditionerTempCTrackBar = ituSceneFindWidget(&theScene, "airConditionerTempCTrackBar");
        assert(airConditionerTempCTrackBar);

        airConditionerTempFContainer = ituSceneFindWidget(&theScene, "airConditionerTempFContainer");
        assert(airConditionerTempFContainer);

        airConditionerTempFProgressBar = ituSceneFindWidget(&theScene, "airConditionerTempFProgressBar");
        assert(airConditionerTempFProgressBar);

        airConditionerTempFTrackBar = ituSceneFindWidget(&theScene, "airConditionerTempFTrackBar");
        assert(airConditionerTempFTrackBar);

        airConditionerMinTempSprite = ituSceneFindWidget(&theScene, "airConditionerMinTempSprite");
        assert(airConditionerMinTempSprite);

        airConditionerMaxTempSprite = ituSceneFindWidget(&theScene, "airConditionerMaxTempSprite");
        assert(airConditionerMaxTempSprite);

        airConditionerWindSlowAnimation = ituSceneFindWidget(&theScene, "airConditionerWindSlowAnimation");
        assert(airConditionerWindSlowAnimation);

        airConditionerWindNormalAnimation = ituSceneFindWidget(&theScene, "airConditionerWindNormalAnimation");
        assert(airConditionerWindNormalAnimation);

        airConditionerWindFastAnimation = ituSceneFindWidget(&theScene, "airConditionerWindFastAnimation");
        assert(airConditionerWindFastAnimation);

        airConditionerWindAutoAnimation = ituSceneFindWidget(&theScene, "airConditionerWindAutoAnimation");
        assert(airConditionerWindAutoAnimation);

        if (HouseGetInfoArray(HOUSE_AIRCONDITIONER, &airConditionerInfo, 1) > 0)
        {
            airConditionerAvail = true;
        }
        else
        {
            ituWidgetDisable(airConditionerPowerButton);
        }
    }

    airConditionerCelsiusOld = theConfig.airconditioner_celsius;
    if (theConfig.airconditioner_celsius)
    {
        ituWidgetSetVisible(airConditionerCelsiusText, true);
        ituWidgetSetVisible(airConditionerFahrenheitText, false);
        ituWidgetSetVisible(airConditionerTempCContainer, true);
        ituWidgetSetVisible(airConditionerTempFContainer, false);
        ituSpriteGoto(airConditionerMinTempSprite, 0);
        ituSpriteGoto(airConditionerMaxTempSprite, 0);
    }
    else
    {
        ituWidgetSetVisible(airConditionerCelsiusText, false);
        ituWidgetSetVisible(airConditionerFahrenheitText, true);
        ituWidgetSetVisible(airConditionerTempCContainer, false);
        ituWidgetSetVisible(airConditionerTempFContainer, true);
        ituSpriteGoto(airConditionerMinTempSprite, 1);
        ituSpriteGoto(airConditionerMaxTempSprite, 1);
    }

    if (mainMenuWallpaperData)
    {
        ituIconLoadJpegData((ITUIcon*)airConditionerBackground, mainMenuWallpaperData, mainMenuWallpaperSize);
    }
    else
    {
        ituSetColor(&((ITUWidget*)airConditionerBackground)->color, 255, theConfig.bgcolor_red, theConfig.bgcolor_green, theConfig.bgcolor_blue);
    }

    if (airConditionerAvail && airConditionerInfo.status == HOUSE_ON)
    {
        int ret = HouseExecAction(HOUSE_AIRCONDITIONER, airConditionerInfo.index, HOUSE_GET, &airConditionerSetting);
        assert(ret == 0);

        AirConditionerPowerOn();
    }
    else
    {
        AirConditionerPowerOff();
    }
    return true;
}

bool AirConditionerOnLeave(ITUWidget* widget, char* param)
{
    if (airConditionerCelsiusOld != theConfig.airconditioner_celsius)
        ConfigSave();

    return true;
}

void AirConditionerReset(void)
{
    airConditionerBackground = NULL;
    airConditionerAvail = false;
}
