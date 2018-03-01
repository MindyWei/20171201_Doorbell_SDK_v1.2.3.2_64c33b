#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUWheel* dateDayWheel;
static ITUWheel* dateMonthWheel;
static ITUWheel* dateYearWheel;

bool DateConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    struct timeval tv;
    struct tm *tm, mytime;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    memcpy(&mytime, tm, sizeof (struct tm));

    mytime.tm_mday = dateDayWheel->focusIndex + 1;
    mytime.tm_mon = dateMonthWheel->focusIndex;
    mytime.tm_year = dateYearWheel->focusIndex + 101;

    tv.tv_sec = mktime(&mytime);
    tv.tv_usec = 0;

    settimeofday(&tv, NULL);

    return true;
}

bool DateOnEnter(ITUWidget* widget, char* param)
{
    struct timeval tv;
    struct tm* tm;

    if (!dateDayWheel)
    {
        dateDayWheel = ituSceneFindWidget(&theScene, "dateDayWheel");
        assert(dateDayWheel);

        dateMonthWheel = ituSceneFindWidget(&theScene, "dateMonthWheel");
        assert(dateMonthWheel);

        dateYearWheel = ituSceneFindWidget(&theScene, "dateYearWheel");
        assert(dateYearWheel);
    }

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    ituWheelGoto(dateDayWheel, tm->tm_mday - 1);
    ituWheelGoto(dateMonthWheel, tm->tm_mon);
    ituWheelGoto(dateYearWheel, tm->tm_year - 101);

    return true;
}

void DateReset(void)
{
    dateDayWheel = NULL;
}
