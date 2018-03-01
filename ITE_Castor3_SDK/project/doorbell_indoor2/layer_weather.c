#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

// need to install VS90SP1-KB980263-x86.exe for vs2008
#pragma execution_character_set("utf-8")

#define WEATHER_INTERVAL (1 * 60 * 60 / MS_PER_FRAME) // 1 hour

static ITUText* weatherLocationText;
static ITUSprite* weatherCurrSymbolSprite;
static ITUText* weatherCurrTemperatureText;
static ITUButton* weatherCurrTemperatureButton;
static ITUText* weatherCurrTemperatureMinText;
static ITUText* weatherCurrTemperatureMaxText;
static ITUText* weatherCurrWindDirectionText;
static ITUText* weatherCurrHumidityText;
static ITUText* weatherCurrWindSpeedNameText;
static ITUText* weatherCurrWindSpeedMpsText;

static ITUText* weatherDay1DateText;
static ITUSprite* weatherDay1SymbolSprite;
static ITUText* weatherDay1TemperatureMinText;
static ITUText* weatherDay1TemperatureMaxText;

static ITUText* weatherDay2DateText;
static ITUSprite* weatherDay2SymbolSprite;
static ITUText* weatherDay2TemperatureMinText;
static ITUText* weatherDay2TemperatureMaxText;

static ITUText* weatherDay3DateText;
static ITUSprite* weatherDay3SymbolSprite;
static ITUText* weatherDay3TemperatureMinText;
static ITUText* weatherDay3TemperatureMaxText;

static ITUText* weatherDay4DateText;
static ITUSprite* weatherDay4SymbolSprite;
static ITUText* weatherDay4TemperatureMinText;
static ITUText* weatherDay4TemperatureMaxText;

static ITUText* weatherDay5DateText;
static ITUSprite* weatherDay5SymbolSprite;
static ITUText* weatherDay5TemperatureMinText;
static ITUText* weatherDay5TemperatureMaxText;

static int weatherDelayCount;

bool WeatherCurrTemperatureButtonOnMouseLongPress(ITUWidget* widget, char* param)
{
    theConfig.airconditioner_celsius = theConfig.airconditioner_celsius ? 0 : 1;
    if (WeatherIsAvailable())
    {
        weatherDelayCount = WEATHER_INTERVAL;
    }
    else
    {
        if (theConfig.airconditioner_celsius)
            ituTextSetString(weatherCurrTemperatureText, "--°C");
        else
            ituTextSetString(weatherCurrTemperatureText, "--°F");

        ituTextSetString(weatherCurrTemperatureMinText, NULL);
        ituTextSetString(weatherCurrTemperatureMaxText, NULL);
        ituTextSetString(weatherCurrWindDirectionText, NULL);
        ituTextSetString(weatherCurrHumidityText, NULL);
        ituTextSetString(weatherCurrWindSpeedNameText, NULL);
        ituTextSetString(weatherCurrWindSpeedMpsText, NULL);
        ituTextSetString(weatherDay1TemperatureMinText, NULL);
        ituTextSetString(weatherDay1TemperatureMaxText, NULL);
        ituTextSetString(weatherDay2TemperatureMinText, NULL);
        ituTextSetString(weatherDay2TemperatureMaxText, NULL);
        ituTextSetString(weatherDay3TemperatureMinText, NULL);
        ituTextSetString(weatherDay3TemperatureMaxText, NULL);
        ituTextSetString(weatherDay4TemperatureMinText, NULL);
        ituTextSetString(weatherDay4TemperatureMaxText, NULL);
        ituTextSetString(weatherDay5TemperatureMinText, NULL);
        ituTextSetString(weatherDay5TemperatureMaxText, NULL);
    }
    return true;
}

bool WeatherOnTimer(ITUWidget* widget, char* param)
{
    if (weatherDelayCount++ >= WEATHER_INTERVAL)
    {
        if (WeatherIsAvailable())
        {
            char* ptr;
            char buf[16];
            int temperature;

            // current
            ptr = WeatherGetLocation();
            ituTextSetString(weatherLocationText, ptr);
            free(ptr);

            ituSpriteGoto(weatherCurrSymbolSprite, WeatherGetSymbolIndex(1));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetCurrTemperature(1);
                sprintf(buf, "%d°C", temperature);
            }
            else
            {
                temperature = (int)WeatherGetCurrTemperature(1) * 9 / 5 + 32;
                sprintf(buf, "%d°F", temperature);
            }
            ituTextSetString(weatherCurrTemperatureText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(1);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(1) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherCurrTemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(1);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(1) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherCurrTemperatureMaxText, buf);

            ptr = WeatherGetWindDirection(1);
            ituTextSetString(weatherCurrWindDirectionText, ptr);
            free(ptr);

            sprintf(buf, "%d%%", WeatherGetHumidity(1));
            ituTextSetString(weatherCurrHumidityText, buf);

            ptr = WeatherGetWindSpeedName(1);
            ituTextSetString(weatherCurrWindSpeedNameText, ptr);
            free(ptr);

            sprintf(buf, "%fm/s", WeatherGetWindSpeed(1));
            ituTextSetString(weatherCurrWindSpeedMpsText, buf);

            // day 1
            ptr = WeatherGetDate(2);
            ituTextSetString(weatherDay1DateText, ptr);
            free(ptr);

            ituSpriteGoto(weatherDay1SymbolSprite, WeatherGetSymbolIndex(2));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(2);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(2) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay1TemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(2);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(2) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay1TemperatureMaxText, buf);

            // day 2
            ptr = WeatherGetDate(3);
            ituTextSetString(weatherDay2DateText, ptr);
            free(ptr);

            ituSpriteGoto(weatherDay2SymbolSprite, WeatherGetSymbolIndex(3));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(3);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(3) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay2TemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(3);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(3) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay2TemperatureMaxText, buf);

            // day 3
            ptr = WeatherGetDate(4);
            ituTextSetString(weatherDay3DateText, ptr);
            free(ptr);

            ituSpriteGoto(weatherDay3SymbolSprite, WeatherGetSymbolIndex(4));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(4);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(4) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay3TemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(4);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(4) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay3TemperatureMaxText, buf);

            // day 4
            ptr = WeatherGetDate(5);
            ituTextSetString(weatherDay4DateText, ptr);
            free(ptr);

            ituSpriteGoto(weatherDay4SymbolSprite, WeatherGetSymbolIndex(5));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(5);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(5) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay4TemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(5);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(5) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay4TemperatureMaxText, buf);

            // day 5
            ptr = WeatherGetDate(6);
            ituTextSetString(weatherDay5DateText, ptr);
            free(ptr);

            ituSpriteGoto(weatherDay5SymbolSprite, WeatherGetSymbolIndex(6));

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMin(6);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMin(6) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay5TemperatureMinText, buf);

            if (theConfig.airconditioner_celsius)
            {
                temperature = (int)WeatherGetTemperatureMax(6);
                sprintf(buf, "%d°", temperature);
            }
            else
            {
                temperature = (int)WeatherGetTemperatureMax(6) * 9 / 5 + 32;
                sprintf(buf, "%d°", temperature);
            }
            ituTextSetString(weatherDay5TemperatureMaxText, buf);

			weatherDelayCount = 0;
		}
    }
	return true;
}

bool WeatherOnEnter(ITUWidget* widget, char* param)
{
    if (!weatherLocationText)
    {
        weatherLocationText = ituSceneFindWidget(&theScene, "weatherLocationText");
        assert(weatherLocationText);    

        weatherCurrSymbolSprite = ituSceneFindWidget(&theScene, "weatherCurrSymbolSprite");
        assert(weatherCurrSymbolSprite);    

        weatherCurrTemperatureText = ituSceneFindWidget(&theScene, "weatherCurrTemperatureText");
        assert(weatherCurrTemperatureText);    

        weatherCurrTemperatureButton = ituSceneFindWidget(&theScene, "weatherCurrTemperatureButton");
        assert(weatherCurrTemperatureButton);    

        weatherCurrTemperatureMinText = ituSceneFindWidget(&theScene, "weatherCurrTemperatureMinText");
        assert(weatherCurrTemperatureMinText);    

        weatherCurrTemperatureMaxText = ituSceneFindWidget(&theScene, "weatherCurrTemperatureMaxText");
        assert(weatherCurrTemperatureMaxText);    

        weatherCurrWindDirectionText = ituSceneFindWidget(&theScene, "weatherCurrWindDirectionText");
        assert(weatherCurrWindDirectionText);    

        weatherCurrHumidityText = ituSceneFindWidget(&theScene, "weatherCurrHumidityText");
        assert(weatherCurrHumidityText);    

        weatherCurrWindSpeedNameText = ituSceneFindWidget(&theScene, "weatherCurrWindSpeedNameText");
        assert(weatherCurrWindSpeedNameText);    

        weatherCurrWindSpeedMpsText = ituSceneFindWidget(&theScene, "weatherCurrWindSpeedMpsText");
        assert(weatherCurrWindSpeedMpsText);    

        weatherDay1DateText = ituSceneFindWidget(&theScene, "weatherDay1DateText");
        assert(weatherDay1DateText);    

        weatherDay1SymbolSprite = ituSceneFindWidget(&theScene, "weatherDay1SymbolSprite");
        assert(weatherDay1SymbolSprite);    

        weatherDay1TemperatureMinText = ituSceneFindWidget(&theScene, "weatherDay1TemperatureMinText");
        assert(weatherDay1TemperatureMinText);    

        weatherDay1TemperatureMaxText = ituSceneFindWidget(&theScene, "weatherDay1TemperatureMaxText");
        assert(weatherDay1TemperatureMaxText);    

        weatherDay2DateText = ituSceneFindWidget(&theScene, "weatherDay2DateText");
        assert(weatherDay2DateText);    

        weatherDay2SymbolSprite = ituSceneFindWidget(&theScene, "weatherDay2SymbolSprite");
        assert(weatherDay2SymbolSprite);    

        weatherDay2TemperatureMinText = ituSceneFindWidget(&theScene, "weatherDay2TemperatureMinText");
        assert(weatherDay2TemperatureMinText);    

        weatherDay2TemperatureMaxText = ituSceneFindWidget(&theScene, "weatherDay2TemperatureMaxText");
        assert(weatherDay2TemperatureMaxText);    

        weatherDay3DateText = ituSceneFindWidget(&theScene, "weatherDay3DateText");
        assert(weatherDay3DateText);    

        weatherDay3SymbolSprite = ituSceneFindWidget(&theScene, "weatherDay3SymbolSprite");
        assert(weatherDay3SymbolSprite);    

        weatherDay3TemperatureMinText = ituSceneFindWidget(&theScene, "weatherDay3TemperatureMinText");
        assert(weatherDay3TemperatureMinText);    

        weatherDay3TemperatureMaxText = ituSceneFindWidget(&theScene, "weatherDay3TemperatureMaxText");
        assert(weatherDay3TemperatureMaxText);    

        weatherDay4DateText = ituSceneFindWidget(&theScene, "weatherDay4DateText");
        assert(weatherDay4DateText);    

        weatherDay4SymbolSprite = ituSceneFindWidget(&theScene, "weatherDay4SymbolSprite");
        assert(weatherDay4SymbolSprite);    

        weatherDay4TemperatureMinText = ituSceneFindWidget(&theScene, "weatherDay4TemperatureMinText");
        assert(weatherDay4TemperatureMinText);    

        weatherDay4TemperatureMaxText = ituSceneFindWidget(&theScene, "weatherDay4TemperatureMaxText");
        assert(weatherDay4TemperatureMaxText);    

        weatherDay5DateText = ituSceneFindWidget(&theScene, "weatherDay5DateText");
        assert(weatherDay5DateText);    

        weatherDay5SymbolSprite = ituSceneFindWidget(&theScene, "weatherDay5SymbolSprite");
        assert(weatherDay5SymbolSprite);    

        weatherDay5TemperatureMinText = ituSceneFindWidget(&theScene, "weatherDay5TemperatureMinText");
        assert(weatherDay5TemperatureMinText);    

        weatherDay5TemperatureMaxText = ituSceneFindWidget(&theScene, "weatherDay5TemperatureMaxText");
        assert(weatherDay5TemperatureMaxText);    
    }
    weatherDelayCount = WEATHER_INTERVAL;
	return true;
}

bool WeatherOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void WeatherReset(void)
{
    weatherLocationText = NULL;
}
