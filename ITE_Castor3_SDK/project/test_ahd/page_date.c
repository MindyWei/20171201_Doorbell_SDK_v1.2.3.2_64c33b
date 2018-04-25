#include <assert.h>
#include "user_function.h"

char date_listbox_str[7][20] = {"DATE_LISTB_WEEK_SUN","DATE_LISTB_WEEK_MON","DATE_LISTB_WEEK_TUE",
							"DATE_LISTB_WEEK_WED","DATE_LISTB_WEEK_THU","DATE_LISTB_WEEK_FRI",
							"DATE_LISTB_WEEK_SAT"};

char date_wheel_str[6][20] = {"DATE_SET_WHEEL_1","DATE_SET_WHEEL_2","DATE_SET_WHEEL_3",
							"DATE_SET_WHEEL_4","DATE_SET_WHEEL_5","DATE_SET_WHEEL_6"};

//星期 日   一   二    三    四  五  六
char date_week_str[7][5] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
//               1月         2月      3月    4月 5月   6月   7月   8月        9月        10月       11月         12月
char date_month_str[12][10] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

static ITUText* DATE_TEXT_CUR_DATE;
static ITUText* DATE_TEXT_YEAR;
static ITUText* DATE_TEXT_MON;
static ITUText* DATE_TEXT_DAY;
static ITUIcon* DATE_ICON_FOCUS;
static ITUBackground* DATE_BG;
static ITUBackground* DATE_SET_BG;
static ITUListBox* DATE_LISTB_WEEK[7];
static ITUWheel* DATE_SET_WHEEL[6];



bool date_to_set = false;
uint32_t date_time_tick;

void update_date()
{
	int u_year,u_mon,u_mday;
	char date[25] = {'\0'};
	char temp_str[10] = {'\0'};
	char *str_p = NULL;

	str_p = ituTextGetString(DATE_TEXT_YEAR);
	u_year = atoi(str_p);
	str_p = ituTextGetString(DATE_TEXT_MON);
	u_mon = atoi(str_p);
	str_p = ituTextGetString(DATE_TEXT_DAY);
	u_mday = atoi(str_p);

	//printf("-------------->year:%d    month:%d      mday:%d<----------------\n",u_year,u_mon,u_mday);


	strcpy(date,date_month_str[u_mon-1]);
	strcat(date," ");
	if(u_mday)
	{
		ituWidgetSetVisible(DATE_ICON_FOCUS, true);
		sprintf(temp_str,"%d",u_mday);
		strcat(date,temp_str);
		strcat(date,", ");
	}
	else
	{
		ituWidgetSetVisible(DATE_ICON_FOCUS, false);
	}
	sprintf(temp_str,"%d",u_year);
	strcat(date,temp_str);
	ituTextSetString(DATE_TEXT_CUR_DATE, date);
	//printf("----------------->%s<--------------\n",home_date);
}
void month_focus_update()
{
	int i;
	for(i = 0;i<7;i++)
	{
		//printf("-------------------->%d\n",DATE_LISTB_WEEK[i]->focusIndex);
		if(DATE_LISTB_WEEK[i]->focusIndex +1)
		{
			ituWidgetSetX(DATE_ICON_FOCUS, (31+(62*i)));
			ituWidgetSetY(DATE_ICON_FOCUS, (58*DATE_LISTB_WEEK[i]->focusIndex)+3);
		}
	}
}

void date_wheel_update()
{
        struct timeval tv;
        struct tm *tm, mytime;

        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);

        memcpy(&mytime, tm, sizeof (struct tm));

	ituWheelGoto(DATE_SET_WHEEL[0],(mytime.tm_year - 100)-1);
	ituWheelGoto(DATE_SET_WHEEL[1],mytime.tm_mon);
	ituWheelGoto(DATE_SET_WHEEL[2],(mytime.tm_mday -1));
	ituWheelGoto(DATE_SET_WHEEL[3],mytime.tm_hour);
	ituWheelGoto(DATE_SET_WHEEL[4],mytime.tm_min);
	ituWheelGoto(DATE_SET_WHEEL[5],mytime.tm_sec );
}

bool date_init(ITUWidget* widget, char* param)
{
	int i;
	if(!DATE_TEXT_CUR_DATE)
	{
		DATE_TEXT_CUR_DATE = ituSceneFindWidget(&theScene, "DATE_TEXT_CUR_DATE");
		assert(DATE_TEXT_CUR_DATE);
		DATE_TEXT_YEAR = ituSceneFindWidget(&theScene, "DATE_TEXT_YEAR");
		assert(DATE_TEXT_YEAR);
		DATE_TEXT_MON = ituSceneFindWidget(&theScene, "DATE_TEXT_MON");
		assert(DATE_TEXT_MON);
		DATE_TEXT_DAY = ituSceneFindWidget(&theScene, "DATE_TEXT_DAY");
		assert(DATE_TEXT_DAY);
		DATE_ICON_FOCUS = ituSceneFindWidget(&theScene, "DATE_ICON_FOCUS");
		assert(DATE_ICON_FOCUS);
		for(i = 0; i < 7; i++)
		{
			DATE_LISTB_WEEK[i] = ituSceneFindWidget(&theScene, date_listbox_str[i]);
			assert(DATE_LISTB_WEEK[i]);
		}
		DATE_BG = ituSceneFindWidget(&theScene, "DATE_BG");
		assert(DATE_BG);
		DATE_SET_BG = ituSceneFindWidget(&theScene, "DATE_SET_BG");
		assert(DATE_SET_BG);
		for(i = 0; i < 6; i++)
		{
			DATE_SET_WHEEL[i] = ituSceneFindWidget(&theScene, date_wheel_str[i]);
			assert(DATE_SET_WHEEL[i]);
		}
	}
	month_focus_update();
	update_date();
	date_wheel_update();
	if(set_to_time_set_flag)
	{
		ituWidgetSetVisible(DATE_BG, false);
		ituWidgetSetVisible(DATE_SET_BG, true);
	}
	else
	{
		ituWidgetSetVisible(DATE_BG, true);
		ituWidgetSetVisible(DATE_SET_BG, false);
	}
	date_time_tick = SDL_GetTicks();
	return true;
}

bool date_timer(ITUWidget* widget, char* param)
{
	uint32_t cur_tick;
	cur_tick = SDL_GetTicks();
	if(((cur_tick - date_time_tick) > 1000) && (!date_to_set)&& (!set_to_time_set_flag))
	{
		date_time_tick = cur_tick;
		month_focus_update();
		update_date();
		date_wheel_update();
	}
	return true;
}

bool date_leave(ITUWidget* widget, char* param)
{
	date_to_set = false;
	set_to_time_set_flag= false;
	return true;
}

bool date_set_back(ITUWidget* widget, char* param)
{
	if(date_to_set)
	{
		date_to_set = false;
		ituWidgetSetVisible(DATE_BG, true);
		ituWidgetSetVisible(DATE_SET_BG, false);
	}
	else
	{
		ITULayer* PAGE_HOME = ituSceneFindWidget(&theScene, "PAGE_HOME");
		assert(PAGE_HOME);
		ituLayerGoto(PAGE_HOME);
	}
        struct timeval tv;
        struct tm *tm, mytime;

        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);

        memcpy(&mytime, tm, sizeof (struct tm));

        mytime.tm_year = DATE_SET_WHEEL[0]->focusIndex + 100+1;
        mytime.tm_mon = DATE_SET_WHEEL[1]->focusIndex;
        mytime.tm_mday = DATE_SET_WHEEL[2]->focusIndex + 1;
        mytime.tm_hour = DATE_SET_WHEEL[3]->focusIndex;
        mytime.tm_min = DATE_SET_WHEEL[4]->focusIndex;
        mytime.tm_sec = DATE_SET_WHEEL[5]->focusIndex;

        tv.tv_sec = mktime(&mytime);
        tv.tv_usec = 0;
	settimeofday(&tv, NULL);
	update_date();
	month_focus_update();
	return true;
}

bool date_set_change(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
		case '1':
			break;
		case '2':
			break;
		case '3':
			break;
	}
	return true;
}

bool update_date_text(ITUWidget* widget, char* param)
{
	update_date();
	return true;
}

bool date_to_time_set(ITUWidget* widget, char* param)
{
	date_to_set = true;
	return true;
}
