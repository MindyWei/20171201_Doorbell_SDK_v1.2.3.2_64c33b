#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "libxml/xpath.h"
#include "doorbell.h"
#include "thread_helper.h"

#define WEATHER_INTERVAL (1 * 60 * 60) // 1 hour

static xmlDocPtr weatherDoc;
static pthread_mutex_t weatherMutex;
static bool weatherQuit;

static void WeatherGetWeather(char* xml, int size)
{
    if (weatherQuit)
        return;

    pthread_mutex_lock(&weatherMutex);
    
    if (weatherDoc)
    {
        xmlFreeDoc(weatherDoc);
        weatherDoc = NULL;
    }

    if (weatherQuit)
    {
        pthread_mutex_unlock(&weatherMutex);
        return;
    }
    weatherDoc = xmlParseMemory(xml, size);
    assert(weatherDoc);

    pthread_mutex_unlock(&weatherMutex);
}

static void* WeatherTask(void* arg)
{
    while (!weatherQuit)
    {
        if (NetworkServerIsReady())
        {
            RemoteGetWeather(WeatherGetWeather);

            sleep(WEATHER_INTERVAL);
        }
        else
        {
            sleep(1);
        }
    }

    // quit
    if (weatherDoc)
    {
        xmlFreeDoc(weatherDoc);
        weatherDoc = NULL;
    }
    pthread_mutex_destroy(&weatherMutex);
    return NULL;
}

void WeatherInit(void)
{
    weatherQuit = false;

    pthread_mutex_init(&weatherMutex, NULL);

    CreateWorkerThread(WeatherTask, NULL);
}

void WeatherExit(void)
{
    weatherQuit = true;
}

bool WeatherIsAvailable(void)
{
    return weatherDoc ? true : false;
}

char* WeatherGetDate(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    char* ret = NULL;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/@day", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        ret = xmlNodeListGetString(weatherDoc, node->children, 1);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

int WeatherGetSymbolIndex(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    int ret = -1;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/symbol/@var", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (strcmp(value, "01d") == 0 || strcmp(value, "01n") == 0)
            ret = 0;
        else if (strcmp(value, "02d") == 0 || strcmp(value, "02n") == 0)
            ret = 1;
        else if (strcmp(value, "03d") == 0 || strcmp(value, "03n") == 0)
            ret = 2;
        else if (strcmp(value, "04d") == 0 || strcmp(value, "04n") == 0)
            ret = 3;
        else if (strcmp(value, "09d") == 0 || strcmp(value, "09n") == 0)
            ret = 4;
        else if (strcmp(value, "10d") == 0 || strcmp(value, "10n") == 0)
            ret = 5;
        else if (strcmp(value, "11d") == 0 || strcmp(value, "11n") == 0)
            ret = 6;
        else if (strcmp(value, "13d") == 0 || strcmp(value, "13n") == 0)
            ret = 7;
        else if (strcmp(value, "50d") == 0 || strcmp(value, "50n") == 0)
            ret = 8;

        xmlFree(value);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

float WeatherGetTemperatureMin(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    float ret = 0.0f;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@min", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (value)
        {
            ret = (float)atof(value);
            xmlFree(value);
        }
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

float WeatherGetTemperatureMax(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    float ret = 0.0f;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@max", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (value)
        {
            ret = (float)atof(value);
            xmlFree(value);
        }
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

char* WeatherGetSymbolName(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    char* ret = NULL;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/symbol/@name", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        ret = xmlNodeListGetString(weatherDoc, node->children, 1);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

char* WeatherGetWindSpeedName(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    char* ret = NULL;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/windSpeed/@name", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        ret = xmlNodeListGetString(weatherDoc, node->children, 1);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

char* WeatherGetLocation(void)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    char* ret = NULL;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/location/name");

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        ret = xmlNodeListGetString(weatherDoc, node->children, 1);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

float WeatherGetCurrTemperature(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    float ret = 0.0f;
    struct timeval tv;
    struct tm* timeinfo;

    gettimeofday(&tv, NULL);
    timeinfo = localtime((const time_t*)&tv.tv_sec);

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    if (timeinfo->tm_hour >= 18)
        sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@eve", day);
    else if (timeinfo->tm_hour >= 12)
        sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@day", day);
    else if (timeinfo->tm_hour >= 6)
        sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@morn", day);
    else
        sprintf(buf, "/weatherdata/forecast/time[%d]/temperature/@night", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (value)
        {
            ret = (float)atof(value);
            xmlFree(value);
        }
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

char* WeatherGetWindDirection(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    char* ret = NULL;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/windDirection/@name", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        ret = xmlNodeListGetString(weatherDoc, node->children, 1);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

int WeatherGetHumidity(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    int ret = 0;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/humidity/@value", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (value)
        {
            ret = atoi(value);
            xmlFree(value);
        }
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}

float WeatherGetWindSpeed(int day)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128];
    float ret = 0.0f;

    pthread_mutex_lock(&weatherMutex);

    xpathCtx = xmlXPathNewContext(weatherDoc);
    if (!xpathCtx)
    {
        pthread_mutex_unlock(&weatherMutex);
        return ret;
    }
    sprintf(buf, "/weatherdata/forecast/time[%d]/windSpeed/@mps", day);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* value = xmlNodeListGetString(weatherDoc, node->children, 1);

        if (value)
        {
            ret = (float)atof(value);
            xmlFree(value);
        }
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    pthread_mutex_unlock(&weatherMutex);
    return ret;
}
