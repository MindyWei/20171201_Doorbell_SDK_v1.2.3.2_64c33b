#include "user_function.h"
#include <assert.h>

 timer_t door_open_TimerId;		//开锁定时器	1S
 #define	 door_open_Time		1
 timer_t black_wind_TimerId;		//黑屏定时器	1S
 #define	 black_wind_Time		1000*1000
 
 timer_t standby_mode_TimerId;		//背光定时器	60S
 #define	 standby_mode_Time		60
 
 timer_t no_touch_TimerId;		//背光定时器	60S
 #define	 no_touch_Time		30
 
 timer_t auto_start_TimerId;		//相框启动定时器	10S
 #define	 auto_start_Time		10
 timer_t auto_view_TimerId;		//相框切换定时器	5S
 #define	 auto_view_Time		10
 
 timer_t inter_phone_TimerId;		//interphone	3S
 #define	inter_phone_Time		3*1000*1000
 
 timer_t monitor_TimerId;		//背光定时器	60S
 #define	 monitor_Time		60
 
 timer_t door_delay_TimerId;		//背光定时器	60S
 #define	 door_delay_Time		60
 
 timer_t door_call_TimerId;		//背光定时器	60S
 #define	 door_call_Time		60
 
 timer_t door_talk_TimerId;		//背光定时器	60S
 #define	 door_talk_Time		60

 
 timer_t photo_icon_TimerId;		//背光定时器	60S
 #define	 photo_icon_Time		1

 timer_t auto_TimerId;		//背光定时器	60S
 #define	 auto_Time		3

 timer_t md_begin_TimerId;		//背光定时器	60S
 #define	 md_begin_Time		5

 timer_t md_snap_TimerId;		//背光定时器	60S
 #define	 md_snap_Time		30

 timer_t call_delay_TimerId;		//背光定时器	60S
 #define	 call_delay_Time		5

 timer_t power_delay_TimerId;		//背光定时器	60S
 #define	 power_delay_Time		5

 timer_t power_ack_time_TimerId;		//背光定时器	60S
 #define	 power_ack_time_Time		5

 timer_t inter_delay_TimerId;		//背光定时器	60S
 #define	 inter_delay_Time		5

 timer_t inter_call_TimerId;		//背光定时器	60S
 #define	 inter_call_Time		30

 timer_t inter_talk_TimerId;		//背光定时器	60S
 #define	 inter_talk_Time		30

 timer_t busy_30_TimerId;		//背光定时器	60S
 #define	 busy_30_Time		30

 timer_t busy_60_TimerId;		//背光定时器	60S
 #define	 busy_60_Time		60

 timer_t busy_over_3s_TimerId;		//背光定时器	60S
 #define	busy_over_3_Time		3

 timer_t video_first_TimerId;		//背光定时器	60S
 #define	 video_first_Time		60


 timer_t key_sound_TimerId;		//背光定时器	60S
 #define	 key_sound_Time		60
 
 timer_t md_enable_TimerId;		//背光定时器	60S
 #define	 md_enable_Time		1
 
 timer_t md_start_delay_TimerId;		//背光定时器	60S
 #define	 md_start_delay_Time		1


static bool need_back_home = true;
static bool monitor_back_home = false;
static bool avi_is_playing = false;
static bool door_is_calling = false;
static bool auto_start_ing = false;

 bool get_auto_start_ing()
 {
	return auto_start_ing;
 }


 bool get_door_is_calling()
 {
	return door_is_calling;
 }

 bool get_back_home_flag()
 {
	return need_back_home;
 }

 void set_back_home_flag()
 {
	need_back_home = true;
 }

 void clear_back_home_flag()
 {
	need_back_home = false;
 }

 bool get_mon_back_home_flag()
 {
	return monitor_back_home;
 }

void set_avi_playing()
{
	avi_is_playing = true;
}

void clear_avi_playing()
{
	avi_is_playing = false;
}


void door_open_timer_start()
{
    struct itimerspec value;
	
    value.it_value.tv_sec   = door_open_Time;
    value.it_value.tv_nsec  = 0;
    value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
    timer_settime(door_open_TimerId, 0, &value, NULL);
    printf("---------->door_open_timer_end<------>%ld<--------\n",value.it_value.tv_sec);
}

void door_open_timer_end(timer_t timerid, int arg)
{
	printf("---------->door_open_timer_end<--------------\n");
	ithGpioSet(DOOR_1_OPEN);	
	ithGpioSet(DOOR_2_OPEN);	
	cur_open = false;
}

void black_wind_timer_start()
{
    struct itimerspec value;
	
    value.it_value.tv_sec   = 0;
    value.it_value.tv_nsec  = 1300*1000*1000;//1000*1000*1000;
    value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
    timer_settime(black_wind_TimerId, 0, &value, NULL);
	//printf("---------->black_wind_timer_start\n");
}
void black_wind_timer_end(timer_t timerid, int arg)
{
	printf("---------->black_wind_timer_end<--------------\n");
	//2018.3.28 my.wei add for UI command queue
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
	SceneWidgetSetVisible("MON_BG_WIN", false);
#else	
	ITUBackground* MON_BG_WIN = ituSceneFindWidget(&theScene, "MON_BG_WIN");
	assert(MON_BG_WIN);
	ituWidgetSetVisible(MON_BG_WIN, false);
#endif
	cur_wind = true;
}

void photo_icon_start()
{
	struct itimerspec value;
	
	value.it_value.tv_sec   = 0;
	value.it_value.tv_nsec  = 500*1000*1000;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(photo_icon_TimerId, 0, &value, NULL);
}

void photo_icon_end(timer_t timerid, int arg)
{
	printf("---------->photo_icon_end<--------------\n");
	show_snap_rec_icon = icon_clear;
	ImageMemoRecEnd();
}

void montion_begin_start()
{
	printf("---------->montion_begin_start<--------------\n");
	struct itimerspec value;
	
	value.it_value.tv_sec   = md_begin_Time;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(md_begin_TimerId, 0, &value, NULL);
}

void montion_begin_end(timer_t timerid, int arg)
{
	printf("---------->montion_begin_end<--------------\n");
	cur_page  = page_motion;
	event_home_go = page_motion;
	set_motion_satrt();
	montion_start_again();
}

void montion_begin_reinit()
{
	timer_delete(md_begin_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &md_begin_TimerId);
	timer_connect(md_begin_TimerId, montion_begin_end, 0);
}

void montion_snap_start()
{
	struct itimerspec value;
	if(theConfig.mdcam == DOOR_1)
		ithGpioSet(cam_gpio[DOOR_1][ON]);
	else if(theConfig.mdcam == DOOR_2)
		ithGpioSet(cam_gpio[DOOR_2][ON]);

	value.it_value.tv_sec   = 30;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(md_snap_TimerId, 0, &value, NULL);
}

void montion_snap_end(timer_t timerid, int arg)
{
	printf("---------->montion_snap_end<--------------\n");
	reset_md_start_time_t();//my.wei mask for test ahd
	if(theConfig.mdcam == DOOR_1)
		ithGpioClear(cam_gpio[DOOR_1][ON]);
	else if(theConfig.mdcam == DOOR_2)
		ithGpioClear(cam_gpio[DOOR_2][ON]);
	set_montion_pass_once();
	montion_start_again();
}

void montion_snap_reinit()
{
	timer_delete(md_snap_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &md_snap_TimerId);
	timer_connect(md_snap_TimerId, montion_snap_end, 0);
}
void call_delay_start(long time)
{
	struct itimerspec value;
	printf("---------->inter_delay_start<--------------\n");
	
	value.it_value.tv_sec   = 0;
	value.it_value.tv_nsec  = time*1000;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(call_delay_TimerId, 0, &value, NULL);
}

void call_delay_end(timer_t timerid, int arg)
{
	printf("---------->inter_delay_end<--------------\n");
}

void power_delay_start(long time)
{
	struct itimerspec value;
	printf("---------->inter_delay_start<--------------\n");
	value.it_value.tv_sec   = 0;
	value.it_value.tv_nsec  = time*1000;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(power_delay_TimerId, 0, &value, NULL);
}

void power_delay_end(timer_t timerid, int arg)
{
	printf("---------->inter_delay_end<--------------\n");
}

void power_ack_time_start()
{
	struct itimerspec value;
	printf("---------->inter_delay_start<--------------\n");
	value.it_value.tv_sec   = 0;
	value.it_value.tv_nsec  = 100*1000*1000;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(power_ack_time_TimerId, 0, &value, NULL);
}

void power_ack_time_end(timer_t timerid, int arg)
{
	printf("---------->inter_delay_end<--------------\n");
}
void power_ack_time_reinit()
{
	timer_delete(power_ack_time_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &power_ack_time_TimerId);
	timer_connect(power_ack_time_TimerId, power_ack_time_end, 0);
}

void inter_delay_start(long time)
{
	struct itimerspec value;
	printf("---------->inter_delay_start<--------------\n");
	
	value.it_value.tv_sec   = 0;
	value.it_value.tv_nsec  = time*1000;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(inter_delay_TimerId, 0, &value, NULL);
}

void inter_delay_end(timer_t timerid, int arg)
{
	printf("---------->inter_delay_end<--------------\n");
}

void inter_delay_reinit()
{
	timer_delete(inter_delay_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &inter_delay_TimerId);
	timer_connect(inter_delay_TimerId, inter_delay_end, 0);
}

void inter_call_start()
{
	struct itimerspec value;
	printf("---------->inter_call_start<--------------\n");
	
	value.it_value.tv_sec   = 30;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(inter_call_TimerId, 0, &value, NULL);
}

void inter_call_end(timer_t timerid, int arg)
{
	printf("---------->inter_call_end<--------------\n");
}

void inter_call_reinit()
{
	timer_delete(inter_call_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &inter_call_TimerId);
	timer_connect(inter_call_TimerId, inter_call_end, 0);
}

void inter_talk_start()
{
	struct itimerspec value;
	printf("---------->inter_call_start<--------------\n");
	
	value.it_value.tv_sec   = 60;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(inter_talk_TimerId, 0, &value, NULL);
	inter_call_reinit();
}

void inter_talk_end(timer_t timerid, int arg)
{
	printf("---------->inter_call_end<--------------\n");
}

void inter_talk_reinit()
{
	timer_delete(inter_talk_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &inter_talk_TimerId);
	timer_connect(inter_talk_TimerId, inter_talk_end, 0);
}

void busy_30_start()
{
	struct itimerspec value;
	printf("---------->busy_30_start<--------------\n");
	value.it_value.tv_sec   = 30;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(busy_30_TimerId, 0, &value, NULL);
	busy_60_reinit();
}
void busy_30_end(timer_t timerid, int arg)
{
	printf("---------->busy_30_end<--------------\n");
	busy_over_3s_start();
}
void busy_30_reinit()
{
	printf("---------->busy_30_reinit<--------------\n");
	timer_delete(busy_30_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &busy_30_TimerId);
	timer_connect(busy_30_TimerId, busy_30_end, 0);
}

void busy_60_start()
{
	struct itimerspec value;
	printf("---------->busy_60_start<--------------\n");
	value.it_value.tv_sec   = 60;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(busy_60_TimerId, 0, &value, NULL);
	busy_30_reinit();
}
void busy_60_end(timer_t timerid, int arg)
{
	printf("---------->busy_60_end<--------------\n");
	if(!theConfig.lcdout)
		md_start_delay_start();
}
void busy_60_reinit()
{
	printf("---------->busy_60_reinit<--------------\n");
	timer_delete(busy_60_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &busy_60_TimerId);
	timer_connect(busy_60_TimerId, busy_60_end, 0);
}

void busy_over_3s_start()
{
	struct itimerspec value;
	printf("---------->busy_over_3s_start<--------------\n");
	busy_over_3s = true;
	value.it_value.tv_sec   = 3;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(busy_over_3s_TimerId, 0, &value, NULL);
}
void busy_over_3s_end(timer_t timerid, int arg)
{
	printf("---------->busy_over_3s_end<--------------\n");
	busy_over_3s = false;
}

void key_sound_start()
{
	struct itimerspec value;
	printf("---------->key_sound_start<--------------\n");
	value.it_value.tv_sec   = 1;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(key_sound_TimerId, 0, &value, NULL);
}

void key_sound_end(timer_t timerid, int arg)
{
	printf("---------->key_sound_end<--------------\n");
	//my.wei mask for test ahd
	//if(!cur_talk_ing && !get_interphone_talk_start() && !user_get_videoPlayerIsPlaying() && !call_ring_playing)
	//	user_amp_off();
}

void key_sound_reinit()
{
	printf("---------->key_sound_reinit<--------------\n");
	timer_delete(key_sound_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &key_sound_TimerId);
	timer_connect(key_sound_TimerId, key_sound_end, 0);
}

void md_enable_start()
{
	struct itimerspec value;
	printf("---------->md_enable_start<--------------\n");
	value.it_value.tv_sec   = 3;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(md_enable_TimerId, 0, &value, NULL);
}

void md_enable_end(timer_t timerid, int arg)
{
	printf("---------->md_enable_end<--------------\n");
	clear_montion_pass_once();
}

void md_enable_reinit()
{
	printf("---------->md_enable_reinit<--------------\n");
	timer_delete(md_enable_TimerId);
	timer_create(CLOCK_REALTIME, NULL, &md_enable_TimerId);
	timer_connect(md_enable_TimerId, md_enable_end, 0);
}

void md_start_delay_start()
{
	struct itimerspec value;
	printf("---------->md_start_delay_start<--------------\n");
	clear_md_delay_over();//my.wei mask for test ahd
	value.it_value.tv_sec   = 30;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(md_start_delay_TimerId, 0, &value, NULL);
}

void md_start_delay_end(timer_t timerid, int arg)
{
	printf("---------->md_start_delay_end<--------------\n");
	set_md_delay_over();//my.wei mask for test ahd
}

void user_timer_init()
{
    timer_create(CLOCK_REALTIME, NULL, &door_open_TimerId);
    timer_connect(door_open_TimerId, door_open_timer_end, 0);
	
    timer_create(CLOCK_REALTIME, NULL, &black_wind_TimerId);
    timer_connect(black_wind_TimerId, black_wind_timer_end, 0);

//photo icon
    timer_create(CLOCK_REALTIME, NULL, &photo_icon_TimerId);
    timer_connect(photo_icon_TimerId, photo_icon_end, 0);

//md_begin_time
    timer_create(CLOCK_REALTIME, NULL, &md_begin_TimerId);
    timer_connect(md_begin_TimerId, montion_begin_end, 0);
//md_snap_time
    timer_create(CLOCK_REALTIME, NULL, &md_snap_TimerId);
    timer_connect(md_snap_TimerId, montion_snap_end, 0);
//call_delay
    timer_create(CLOCK_REALTIME, NULL, &call_delay_TimerId);
    timer_connect(call_delay_TimerId, call_delay_end, 0);
//power_delay
    timer_create(CLOCK_REALTIME, NULL, &power_delay_TimerId);
    timer_connect(power_delay_TimerId, power_delay_end, 0);
//power_ack_time
    timer_create(CLOCK_REALTIME, NULL, &power_ack_time_TimerId);
    timer_connect(power_ack_time_TimerId, power_ack_time_end, 0);
	
//inter_delay
    timer_create(CLOCK_REALTIME, NULL, &inter_delay_TimerId);
    timer_connect(inter_delay_TimerId, inter_delay_end, 0);
//inter_call
    timer_create(CLOCK_REALTIME, NULL, &inter_call_TimerId);
    timer_connect(inter_call_TimerId, inter_call_end, 0);
//inter_talk
    timer_create(CLOCK_REALTIME, NULL, &inter_talk_TimerId);
    timer_connect(inter_talk_TimerId, inter_talk_end, 0);
//busy_30
    timer_create(CLOCK_REALTIME, NULL, &busy_30_TimerId);
    timer_connect(busy_30_TimerId, busy_30_end, 0);
//busy_60
    timer_create(CLOCK_REALTIME, NULL, &busy_60_TimerId);
    timer_connect(busy_60_TimerId, busy_60_end, 0);
//busy_over_3s
    timer_create(CLOCK_REALTIME, NULL, &busy_over_3s_TimerId);
    timer_connect(busy_over_3s_TimerId, busy_over_3s_end, 0);
//key_sound
    timer_create(CLOCK_REALTIME, NULL, &key_sound_TimerId);
    timer_connect(key_sound_TimerId, key_sound_end, 0);
//md_enable
    timer_create(CLOCK_REALTIME, NULL, &md_enable_TimerId);
    timer_connect(md_enable_TimerId, md_enable_end, 0);
//md_start_delay
    timer_create(CLOCK_REALTIME, NULL, &md_start_delay_TimerId);
    timer_connect(md_start_delay_TimerId, md_start_delay_end, 0);
	
	
   // timer_create(CLOCK_REALTIME, NULL, &auto_start_TimerId);
    //timer_connect(auto_start_TimerId, auto_start_over, 0);
	
    //timer_create(CLOCK_REALTIME, NULL, &auto_view_TimerId);
    //timer_connect(auto_view_TimerId, auto_view_over, 0);
}





