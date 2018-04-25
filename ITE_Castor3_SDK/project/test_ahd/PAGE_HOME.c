#include <assert.h>
#include "user_function.h"

char home_icon_h_t_str[3][16] = {"HOME_ICON_H_T_0","HOME_ICON_H_T_1","HOME_ICON_H_T_2"};

char home_icon_h_s_str[10][16] = {"HOME_ICON_H_S_0","HOME_ICON_H_S_1","HOME_ICON_H_S_2",
                                  "HOME_ICON_H_S_3","HOME_ICON_H_S_4","HOME_ICON_H_S_5",
                                  "HOME_ICON_H_S_6","HOME_ICON_H_S_7","HOME_ICON_H_S_8","HOME_ICON_H_S_9"};

char home_icon_m_t_str[6][16] = {"HOME_ICON_M_T_0","HOME_ICON_M_T_1","HOME_ICON_M_T_2",
                                 "HOME_ICON_M_T_3","HOME_ICON_M_T_4","HOME_ICON_M_T_5"};

char home_icon_m_s_str[10][16] = {"HOME_ICON_M_S_0","HOME_ICON_M_S_1","HOME_ICON_M_S_2",
                                  "HOME_ICON_M_S_3","HOME_ICON_M_S_4","HOME_ICON_M_S_5",
                                  "HOME_ICON_M_S_6","HOME_ICON_M_S_7","HOME_ICON_M_S_8","HOME_ICON_M_S_9"};

				      //星期 日   一   二    三    四  五  六   
char home_week_str[7][5] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
					//               1月         2月      3月    4月 5月   6月   7月   8月        9月        10月       11月         12月			  
char home_month_str[12][10] = {"January","February","March","April","May","June","July","August","September","October","November","December"};

static ITUIcon* HOME_ICON_H_T[3];
static ITUIcon* HOME_ICON_H_S[10];
static ITUIcon* HOME_ICON_M_T[6];
static ITUIcon* HOME_ICON_M_S[10];
static ITUText* HOME_TEXT_DATE;
static ITUIcon* HOME_ICON_BUSY_1;
static ITUIcon* HOME_ICON_BUSY_2;
static ITUIcon* HOME_ICON_BUSY_3;
static ITUIcon* HOME_ICON_NEW_FILE;
static ITUCheckBox* HOME_CHBOX_MUTE_SW;
static ITUDigitalClock* HOME_DCLOCK;

uint32_t home_time_tick;
bool home_busy_icon = false;
static bool new_file_icon  = false;

void update_home_time()
{
	int i;
	char home_date[25];
	char temp_str[10];
    struct timeval tv;
    struct tm *tm, mytime;

    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
	memcpy(&mytime, tm, sizeof (struct tm));
	
	for(i = 0; i < 3; i++)
	{
		if(i == (mytime.tm_hour/10))
			ituWidgetSetVisible(HOME_ICON_H_T[i], true);
		else
			ituWidgetSetVisible(HOME_ICON_H_T[i], false);
	}
	
	for(i = 0; i < 10; i++)
	{
		if(i == (mytime.tm_hour%10))
			ituWidgetSetVisible(HOME_ICON_H_S[i], true);
		else
			ituWidgetSetVisible(HOME_ICON_H_S[i], false);
	}
	
	for(i = 0; i < 6; i++)
	{
		if(i == (mytime.tm_min/10))
			ituWidgetSetVisible(HOME_ICON_M_T[i], true);
		else
			ituWidgetSetVisible(HOME_ICON_M_T[i], false);
	}
	
	for(i = 0; i < 10; i++)
	{
		if(i == (mytime.tm_min%10))
			ituWidgetSetVisible(HOME_ICON_M_S[i], true);
		else
			ituWidgetSetVisible(HOME_ICON_M_S[i], false);
	}
	strcpy(home_date,home_week_str[mytime.tm_wday]);
	strcat(home_date,", ");
	strcat(home_date,home_month_str[mytime.tm_mon]);
	strcat(home_date," ");
	sprintf(temp_str,"%d",mytime.tm_mday);
	strcat(home_date,temp_str);
	strcat(home_date,", ");
	sprintf(temp_str,"%d",(mytime.tm_year+1900));
	strcat(home_date,temp_str);
	ituTextSetString(HOME_TEXT_DATE, home_date);
	//user_amp_off();
	//printf("----------------->%s<--------------\n",home_date);
}

bool home_init(ITUWidget* widget, char* param)
{
	
#if TEST_CAM
	event_call_s = 1;
	return true;
#endif
	int i;
	cur_page  = page_home;
	if(!HOME_ICON_H_T[0])
	{
		for(i = 0; i < 3; i++)
		{
			HOME_ICON_H_T[i] = ituSceneFindWidget(&theScene, home_icon_h_t_str[i]);
			assert(HOME_ICON_H_T[i]);
		}
		for(i = 0; i < 10; i++)
		{
			HOME_ICON_H_S[i] = ituSceneFindWidget(&theScene, home_icon_h_s_str[i]);
			assert(HOME_ICON_H_S[i]);
		}
		for(i = 0; i < 6; i++)
		{
			HOME_ICON_M_T[i] = ituSceneFindWidget(&theScene, home_icon_m_t_str[i]);
			assert(HOME_ICON_M_T[i]);
		}
		for(i = 0; i < 10; i++)
		{
			HOME_ICON_M_S[i] = ituSceneFindWidget(&theScene, home_icon_m_s_str[i]);
			assert(HOME_ICON_M_S[i]);
		}
		HOME_TEXT_DATE = ituSceneFindWidget(&theScene, "HOME_TEXT_DATE");
		assert(HOME_TEXT_DATE);
		HOME_ICON_BUSY_1 = ituSceneFindWidget(&theScene, "HOME_ICON_BUSY_1");
		assert(HOME_ICON_BUSY_1);
		HOME_ICON_BUSY_2 = ituSceneFindWidget(&theScene, "HOME_ICON_BUSY_2");
		assert(HOME_ICON_BUSY_2);
		HOME_ICON_BUSY_3 = ituSceneFindWidget(&theScene, "HOME_ICON_BUSY_3");
		assert(HOME_ICON_BUSY_3);
		HOME_ICON_NEW_FILE = ituSceneFindWidget(&theScene, "HOME_ICON_NEW_FILE");
		assert(HOME_ICON_NEW_FILE);
		HOME_CHBOX_MUTE_SW = ituSceneFindWidget(&theScene, "HOME_CHBOX_MUTE_SW");
		assert(HOME_CHBOX_MUTE_SW);
		HOME_DCLOCK = ituSceneFindWidget(&theScene, "HOME_DCLOCK");
		assert(HOME_DCLOCK);
	}
	home_busy_icon = uart_is_busy();
	if(home_busy_icon)
	{
		ituWidgetSetVisible(HOME_ICON_BUSY_1, true);
		ituWidgetSetVisible(HOME_ICON_BUSY_2, true);
		ituWidgetSetVisible(HOME_ICON_BUSY_3, true);
	}
	else
	{
		ituWidgetSetVisible(HOME_ICON_BUSY_1, false);
		ituWidgetSetVisible(HOME_ICON_BUSY_2, false);
		ituWidgetSetVisible(HOME_ICON_BUSY_3, false);
	}
	if(theConfig.mute)
		ituCheckBoxSetChecked(HOME_CHBOX_MUTE_SW,true);
	else
		ituCheckBoxSetChecked(HOME_CHBOX_MUTE_SW,false);
	
	if(new_file_icon)
		ituWidgetSetVisible(HOME_ICON_NEW_FILE, true);
	else
		ituWidgetSetVisible(HOME_ICON_NEW_FILE, false);

#if TEST_SET_ON
	ituWidgetSetVisible(HOME_DCLOCK, true);
#else
	ituWidgetSetVisible(HOME_DCLOCK, false);
#endif
	update_home_time();
	home_time_tick = SDL_GetTicks();
	if(theConfig.lcdout)
	{
		standby_mode_start();
		no_touch_reinit();
	}
	else
		no_touch_start();
	backlight_on();
	return false;
}

bool home_timer(ITUWidget* widget, char* param)
{
	uint32_t cur_tick;
	cur_tick = SDL_GetTicks();
	if((cur_tick - home_time_tick) > 1000)
	{
		home_time_tick = cur_tick;
		update_home_time();
		return true;
	}
	if(home_busy_icon != uart_is_busy())
	{
		if(home_busy_icon)
		{
			home_busy_icon = false;
			ituWidgetSetVisible(HOME_ICON_BUSY_1, false);
			ituWidgetSetVisible(HOME_ICON_BUSY_2, false);
			ituWidgetSetVisible(HOME_ICON_BUSY_3, false);
		}
		else
		{
			home_busy_icon = true;
			ituWidgetSetVisible(HOME_ICON_BUSY_1, true);
			ituWidgetSetVisible(HOME_ICON_BUSY_2, true);
			ituWidgetSetVisible(HOME_ICON_BUSY_3, true);
		}
		return true;
	}

	if(new_file_icon)
	{
		if((!ImageMemoGetUnPlay()) && (!VideoMemoGetUnPlay()))
		{
			ituWidgetSetVisible(HOME_ICON_NEW_FILE, false);
			new_file_icon = false;
		}
	}
	else
	{
		if((ImageMemoGetUnPlay()) || (VideoMemoGetUnPlay()))
		{
			ituWidgetSetVisible(HOME_ICON_NEW_FILE, true);
			new_file_icon = true;
		}
	}
#if 0
	if(standby_state && !montion_end_event)
	{
		if(!uart_is_busy())
		{
			if(theConfig.md && master_vdp)
			{
				if(theConfig.mdtime)
				{
					if(time_enable_montion())
					{
						cur_page  = page_motion;
						event_home_go = page_motion;
						set_montion_pass_once();
						set_motion_satrt();
						montion_start_again();
						montion_enable = true;
					}
				}		
				else
				{
					cur_page  = page_motion;
					event_home_go = page_motion;
					set_montion_pass_once();
					set_motion_satrt();
					montion_start_again();
					montion_enable = true;
				}
			}
		}
	}
#endif
	return false;
}

bool home_leave(ITUWidget* widget, char* param)
{
	return true;
}

bool home_change_mute(ITUWidget* widget, char* param)
{
	if(ituCheckBoxIsChecked(HOME_CHBOX_MUTE_SW))
	{
		theConfig.mute = 1;
		theConfig.ringvol = 0;
		theConfig.Iringvol = 0;
	}
	else
	{
		theConfig.mute = 0;
		theConfig.ringvol = 2;
		theConfig.Iringvol = 2;
	}
	ConfigSave();
	return false;
}

bool home_go(ITUWidget* widget, char* param)
{
	
	switch(param[0])
	{
	case '1':
		usleep(theConfig.id*100*1000);
		if(uart_is_busy())
			return true;
		if(master_vdp || theConfig.id == 0)
		{
			if(signal_insert[DOOR_1] || signal_insert[DOOR_2])
			{
				//uart_set_mode(UART_SIGNAL_BUSY);
				uart_set_mode(UART_SIGNAL_BUSY_30);
				cur_page  = page_monitor;
				event_home_go = page_monitor;
				cur_mon_state = MON_STATE;
				//monitor_start();
			}
		}
		else
		{
			uart_set_mode(UART_MONITOR);
		}
		break;
	case '2':
		usleep(theConfig.id*100*1000);
		if(uart_is_busy())
			return true;
		if(master_vdp || theConfig.id == 0)
		{
			if(signal_insert[CCTV_1] || signal_insert[CCTV_2])
			{
				//uart_set_mode(UART_SIGNAL_BUSY);
				uart_set_mode(UART_SIGNAL_BUSY_60);
				cur_page  = page_cctv;
				event_home_go = page_monitor;
				cur_mon_state = CCTV_STATE;
				//monitor_start();
			}
		}
		else
		{
			uart_set_mode(UART_CCTV);
		}
		break;
	case '3':
		if(uart_is_busy())
			return true;
		cur_page  = page_inter;
		event_home_go = page_inter;
		no_touch_start();
		break;
	case '4':
		if(sd_card_check)
			return true;
		cur_page  = page_media;
		event_home_go = page_media;
		no_touch_start();
		break;
	case '5':
		cur_page  = page_date;
		event_home_go = page_date;
		no_touch_start();
		break;
	case '6':
		printf("[my.wei1] goto page setting\r\n");
		cur_page  = page_setting;
		event_home_go = page_setting;
		no_touch_start();
		break;
	}
	return true;
}

