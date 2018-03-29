#include <sys/time.h>
#include <assert.h>
#include <stdlib.h>
#include "scene.h"
#include "doorbell.h"

static ITUCoverFlow* calendarCoverFlow;
static ITUCalendar* calendar1ADCalendar;
static ITUCalendar* calendar1ChineseCalendar;
static ITUCalendar* calendar2ADCalendar;
static ITUCalendar* calendar2ChineseCalendar;
static ITUCalendar* calendar3ADCalendar;
static ITUCalendar* calendar3ChineseCalendar;
static ITUBackground* calendarChangeBackground;
static ITUWheel* calendarChangeMonthWheel;
static ITUWheel* calendarChangeYearWheel;
static ITUBackground* calendarSettingBackground;
static ITUWheel* calendarDayWheel;
static ITUWheel* calendarMonthWheel;
static ITUWheel* calendarYearWheel;

bool CalendarWheelOnChanged(ITUWidget* widget, char* param)
{
    LinphonePlayKeySound();
    return true;
}

bool CalendarConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(calendarChangeBackground))
    {
        ituCalendarGoto(calendar1ADCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);
        ituCalendarLastMonth(calendar1ADCalendar);
        ituCalendarGoto(calendar1ChineseCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);
        ituCalendarLastMonth(calendar1ChineseCalendar);

        ituCalendarGoto(calendar2ADCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);
        ituCalendarGoto(calendar2ChineseCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);

        ituCalendarGoto(calendar3ADCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);
        ituCalendarNextMonth(calendar3ADCalendar);
        ituCalendarGoto(calendar3ChineseCalendar, calendarChangeYearWheel->focusIndex + 2001, calendarChangeMonthWheel->focusIndex + 1);
        ituCalendarNextMonth(calendar3ChineseCalendar);
    }
    else
    {
        struct timeval tv;
        struct tm *tm, mytime;

        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);

        memcpy(&mytime, tm, sizeof (struct tm));

        mytime.tm_mday = calendarDayWheel->focusIndex + 1;
        mytime.tm_mon = calendarMonthWheel->focusIndex;
        mytime.tm_year = calendarYearWheel->focusIndex + 101;

        tv.tv_sec = mktime(&mytime);
        tv.tv_usec = 0;

        settimeofday(&tv, NULL);
    }
    return true;
}

bool CalendarCoverFlowOnChanged(ITUWidget* widget, char* param)
{
    int value = atoi(param);

    if (value < 1)
    {
        ituCalendarLastMonth(calendar1ADCalendar);
        ituCalendarLastMonth(calendar1ChineseCalendar);
        ituCalendarLastMonth(calendar2ADCalendar);
        ituCalendarLastMonth(calendar2ChineseCalendar);
        ituCalendarLastMonth(calendar3ADCalendar);
        ituCalendarLastMonth(calendar3ChineseCalendar);
    }
    else if (value > 1)
    {
        ituCalendarNextMonth(calendar1ADCalendar);
        ituCalendarNextMonth(calendar1ChineseCalendar);
        ituCalendarNextMonth(calendar2ADCalendar);
        ituCalendarNextMonth(calendar2ChineseCalendar);
        ituCalendarNextMonth(calendar3ADCalendar);
        ituCalendarNextMonth(calendar3ChineseCalendar);
    }
    calendarCoverFlow->focusIndex = 1;
    ituWidgetUpdate(calendarCoverFlow, ITU_EVENT_LAYOUT, 0, 0, 0);
    return true;
}

bool CalendarBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituCalendarToday(calendar1ADCalendar);
    ituCalendarLastMonth(calendar1ADCalendar);
    ituCalendarToday(calendar1ChineseCalendar);
    ituCalendarLastMonth(calendar1ChineseCalendar);

    ituCalendarToday(calendar2ADCalendar);
    ituCalendarToday(calendar2ChineseCalendar);

    ituCalendarToday(calendar3ADCalendar);
    ituCalendarNextMonth(calendar3ADCalendar);
    ituCalendarToday(calendar3ChineseCalendar);
    ituCalendarNextMonth(calendar3ChineseCalendar);

    return true;
}

bool CalendarOnEnter(ITUWidget* widget, char* param)
{
    if (!calendarCoverFlow)
    {
        calendarCoverFlow = ituSceneFindWidget(&theScene, "calendarCoverFlow");
        assert(calendarCoverFlow);

        calendar1ADCalendar = ituSceneFindWidget(&theScene, "calendar1ADCalendar");
        assert(calendar1ADCalendar);
        
        calendar1ChineseCalendar = ituSceneFindWidget(&theScene, "calendar1ChineseCalendar");
        assert(calendar1ChineseCalendar);

        calendar2ADCalendar = ituSceneFindWidget(&theScene, "calendar2ADCalendar");
        assert(calendar2ADCalendar);
        
        calendar2ChineseCalendar = ituSceneFindWidget(&theScene, "calendar2ChineseCalendar");
        assert(calendar2ChineseCalendar);

        calendar3ADCalendar = ituSceneFindWidget(&theScene, "calendar3ADCalendar");
        assert(calendar3ADCalendar);
        
        calendar3ChineseCalendar = ituSceneFindWidget(&theScene, "calendar3ChineseCalendar");
        assert(calendar3ChineseCalendar);

        calendarChangeBackground = ituSceneFindWidget(&theScene, "calendarChangeBackground");
        assert(calendarChangeBackground);

        calendarChangeMonthWheel = ituSceneFindWidget(&theScene, "calendarChangeMonthWheel");
        assert(calendarChangeMonthWheel);
        
        calendarChangeYearWheel = ituSceneFindWidget(&theScene, "calendarChangeYearWheel");
        assert(calendarChangeYearWheel);        

        calendarSettingBackground = ituSceneFindWidget(&theScene, "calendarSettingBackground");
        assert(calendarSettingBackground);

        calendarDayWheel = ituSceneFindWidget(&theScene, "calendarDayWheel");
        assert(calendarDayWheel);

        calendarMonthWheel = ituSceneFindWidget(&theScene, "calendarMonthWheel");
        assert(calendarMonthWheel);
        
        calendarYearWheel = ituSceneFindWidget(&theScene, "calendarYearWheel");
        assert(calendarYearWheel);        
    }

    calendarCoverFlow->focusIndex = 1;

    ituCalendarToday(calendar1ADCalendar);
    ituCalendarLastMonth(calendar1ADCalendar);
    ituCalendarToday(calendar1ChineseCalendar);
    ituCalendarLastMonth(calendar1ChineseCalendar);

    ituCalendarToday(calendar2ADCalendar);
    ituCalendarToday(calendar2ChineseCalendar);

    ituCalendarToday(calendar3ADCalendar);
    ituCalendarNextMonth(calendar3ADCalendar);
    ituCalendarToday(calendar3ChineseCalendar);
    ituCalendarNextMonth(calendar3ChineseCalendar);

    ituWheelGoto(calendarChangeMonthWheel, calendar2ADCalendar->month - 1);
    ituWheelGoto(calendarChangeYearWheel, calendar2ADCalendar->year - 2001);
    ituWheelGoto(calendarDayWheel, calendar2ADCalendar->day - 1);
    ituWheelGoto(calendarMonthWheel, calendar2ADCalendar->month - 1);
    ituWheelGoto(calendarYearWheel, calendar2ADCalendar->year - 2001);

    return true;
}

void CalendarReset(void)
{
    calendarCoverFlow = NULL;
}
