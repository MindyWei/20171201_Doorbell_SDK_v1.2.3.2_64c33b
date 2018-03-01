#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUWheel* timeHourWheel;
static ITUWheel* timeMinuteWheel;

bool TimeConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    struct timeval tv;
    struct tm *tm, mytime;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    memcpy(&mytime, tm, sizeof (struct tm));

    mytime.tm_hour = timeHourWheel->focusIndex;
    mytime.tm_min = timeMinuteWheel->focusIndex;
    mytime.tm_sec = 0;

    tv.tv_sec = mktime(&mytime);
    tv.tv_usec = 0;

    settimeofday(&tv, NULL);

    return true;
}

bool TimeOnEnter(ITUWidget* widget, char* param)
{
    struct timeval tv;
    struct tm* tm;

    if (!timeHourWheel)
    {
        timeHourWheel = ituSceneFindWidget(&theScene, "timeHourWheel");
        assert(timeHourWheel);

        timeMinuteWheel = ituSceneFindWidget(&theScene, "timeMinuteWheel");
        assert(timeMinuteWheel);
    }

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    ituWheelGoto(timeHourWheel, tm->tm_hour);
    ituWheelGoto(timeMinuteWheel, tm->tm_min);

    return true;
}

void TimeReset(void)
{
    timeHourWheel = NULL;
}
