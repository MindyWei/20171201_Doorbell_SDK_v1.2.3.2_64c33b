#include <assert.h>
#include "user_function.h"


static ITUIcon* monitorWindowIcon;
static ITUBackground* monitorHideBackground;
static ITUBackground* monitorHeadBackground;
static ITUCalendar* monitorDigitalClock;
static ITUBackgroundButton* monitorFullBackgroundButton;
static ITUBackground* monitorWindowBackground;
static ITUButton* monitorCameraSwitchButton;
static ITUButton* monitorBackButton;
static ITUButton* monitorRecordButton;
static ITUButton* monitorRecordingButton;
static ITUText* monitorRecTimeText;
static ITUSprite* monitorMotionDetectionSprite;


static bool play_call_ring_once = false;
static bool play_call_ring_same = false;
static bool auto_rec_once = false;
static bool dis_talk_bar = false;
static bool mute_icon_change = false;
static unsigned long md_start_time = 0;			//移动侦测时间
static bool monitor_need_update_ui = false;

static bool mon_rec_ing = false;
static bool call_cant_rec = false;

static bool monitor_call_start = false;
static bool monitor_talk_start = false;
static int monitor_time;	
static uint32_t monitor_start_time;

static uint8_t monitor_sd_icon = 0;

static int x, y, width, height = 0;

int get_monitor_time()
{
	return monitor_time;
}

void clear_play_call_ring_same()
{
	play_call_ring_same =false;
}

void clear_call_cant_rec()
{
	call_cant_rec = false;
}

void set_call_cant_rec()
{
	call_cant_rec = true;
}

bool get_mon_rec_ing()
{
	return mon_rec_ing;
}
void clear_mon_rec_ing()
{
	printf("clear_mon_rec_ing........................\n");
	mon_rec_ing = false;
}

void set_mon_rec_ing()
{
	printf("set_mon_rec_ing........................\n");
	mon_rec_ing = true;
}

void set_monitor_need_update_ui()
{
	monitor_need_update_ui = true;
}

bool get_auto_rec_once()
{
	return auto_rec_once;
}

void clear_auto_rec_once()
{
	auto_rec_once = false;
}

void set_auto_rec_once()
{
	auto_rec_once = true;
}

void set_play_call_ring_once()
{
	play_call_ring_once = true;
	play_call_ring_same = true;
}

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;
	destx = rect->x + x;
	desty = rect->y + y;
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

void user_itu_init()
{
	ituWidgetSetVisible(monitorWindowBackground, true);
	ituWidgetSetVisible(monitorRecordButton,true);
	ituWidgetSetVisible(monitorRecordingButton,false);
	
	if(pop_up[MONITOR_S_FULL] == true)
	{
		pop_up[MONITOR_S_FULL] = false;
		ituWidgetSetVisible(monitorHideBackground,true);
		ituWidgetSetVisible(monitorHeadBackground,true);
	}
	ituWidgetSetVisible(monitorHideBackground,true);
	ituWidgetSetVisible(monitorFullBackgroundButton,true);
	
	if(cur_page == page_monitor)
	{
		ituWidgetSetVisible(monitorBackButton,true);
		ituWidgetSetX(monitorDigitalClock, 131);
	}
	else if(cur_page == page_cctv)
	{
		ituWidgetSetVisible(monitorBackButton,false);
		ituWidgetSetX(monitorDigitalClock, 199);
	}
	else if(cur_page == page_motion)
	{
		ituWidgetHide(monitorHideBackground,0,0);
	}

	if(StorageGetCurrType() == STORAGE_NONE)
	{
	}
	else if(StorageGetCurrType() == STORAGE_SD)
	{
		user_sd_card_check();
	}
	
	if(monitorMotionDetectionSprite)
		ituSpriteGoto(monitorMotionDetectionSprite, theConfig.md);
}

void signal_itu_init(int val)
{
	ituWidgetSetVisible(monitorCameraSwitchButton,false);
		
	if(1 == val)
	{
		ituWidgetSetVisible(monitorCameraSwitchButton,true);
	}
}

void monitor_time_update(int val)
{
	char time_buf[10] = "\0";
	static ITUText* monitorRecTimeText_S ;
	monitorRecTimeText_S	= ituSceneFindWidget(&theScene, "monitorRecTimeText");
	assert(monitorRecTimeText_S);
	
	time_buf[0] = '0';
	time_buf[1] = '0';
	time_buf[2] = ':';
	time_buf[3] = val/10+'0';
	time_buf[4] = val%10+'0';
	ituTextSetString(monitorRecTimeText_S, time_buf);	
}

bool MonitorOnEnter(ITUWidget* widget, char* param)
{
	if (!monitorWindowIcon)
	{
		monitorWindowIcon = ituSceneFindWidget(&theScene, "monitorWindowIcon");
		assert(monitorWindowIcon);
		ituWidgetSetDraw(monitorWindowIcon, VideoPlayerViewBackgroundDraw);
		monitorHideBackground = ituSceneFindWidget(&theScene, "monitorHideBackground");
		assert(monitorHideBackground);
		monitorHeadBackground = ituSceneFindWidget(&theScene, "monitorHeadBackground");
		assert(monitorHeadBackground);

		monitorDigitalClock = ituSceneFindWidget(&theScene, "monitorDigitalClock");
		assert(monitorDigitalClock);
		monitorFullBackgroundButton = ituSceneFindWidget(&theScene, "monitorFullBackgroundButton");
		assert(monitorFullBackgroundButton);
		monitorWindowBackground = ituSceneFindWidget(&theScene, "monitorWindowBackground");
		assert(monitorWindowBackground);
		monitorCameraSwitchButton = ituSceneFindWidget(&theScene, "monitorCameraSwitchButton");
		assert(monitorCameraSwitchButton);
		monitorBackButton = ituSceneFindWidget(&theScene, "monitorBackButton");
		assert(monitorBackButton);
		monitorRecordButton = ituSceneFindWidget(&theScene, "monitorRecordButton");
		assert(monitorRecordButton);
		monitorRecordingButton = ituSceneFindWidget(&theScene, "monitorRecordingButton");
		assert(monitorRecordingButton);
		monitorRecTimeText = ituSceneFindWidget(&theScene, "monitorRecTimeText");
		assert(monitorRecTimeText);
		
		monitorMotionDetectionSprite = ituSceneFindWidget(&theScene, "monitorMotionDetectionSprite");
		assert(monitorMotionDetectionSprite);
	}
	printf("enter page monitor........................\n");
	//printf("monitor_1------------------------->%d\n",SDL_GetTicks()-test_tick);
	user_itu_init();

	SceneEnterVideoState();
	//printf("monitor_------------------------->%d\n",SDL_GetTicks()-test_tick);

	ituWidgetGetGlobalPosition(monitorWindowIcon, &x, &y);
	width = ituWidgetGetWidth(monitorWindowIcon);
	height = ituWidgetGetHeight(monitorWindowIcon);
#ifdef CFG_VIDEO_ENABLE        
	itv_set_video_window(x, y, width, height);
#endif        

	if(1)
	{
		printf("page_monitor.....................................................\n");
		if(master_vdp)
		{
			if(signal_insert[DOOR_1])
				cur_signal = signal_door_1;
			else if(signal_insert[DOOR_2])
				cur_signal = signal_door_2;
		}
		monitor_signal(cur_signal);
		monitor_time = 30;	
	}

#if TEST_CAM
	ituWidgetHide(monitorHideBackground,0,0);
	ituWidgetHide(monitorHeadBackground,0,0);
#endif
	if(ScreenIsOff())
		ScreenOn();
	monitor_start_time = SDL_GetTicks();
	monitor_time_update(monitor_time);
	ituWidgetSetVisible(monitorRecTimeText,true);
	ithGpioClear(AUDIO_IN);
	signal_itu_init(cur_signal);
	PR2000_set_start();
	usleep(100*1000);
	//printf(".....................................................\n");
	gState = SEND_BEGIN;
	return true;
	
}

bool MonitorOnTimer(ITUWidget* widget, char* param)
{
	uint32_t tick = SDL_GetTicks();
	uint32_t diff = tick - monitor_start_time;
	if(diff >= 1000)
	{
#if TEST_CAM	//my.wei add for debug
		if(cur_signal == signal_door_1)
		{
			//if(!signal_insert[DOOR_2])
			//	return true;
			cur_signal = signal_door_2;
			monitor_signal(cur_signal);
		}
		else if(cur_signal == signal_door_2)
		{
			//if(!signal_insert[DOOR_1])
			//	return true;
			cur_signal = signal_door_1;
			monitor_signal(cur_signal);
		}
#endif
		monitor_start_time = tick;
		monitor_time --;
		if(monitor_time == 0)
#if TEST_CAM
		{
			monitor_time = 30;
		}
#else
		{
			door_call_num = 0;
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
#endif
		monitor_time_update(monitor_time);
	}
		
	if(event_call > 0 && event_call < 0x10)		//处理CALL 机事件
	{
		//printf("change signal........................................................\n");
		rec_start_time = 0;				//结束录像
		usleep(50*1000);
		play_call_ring_once = true;
		if(cur_mon_state != C_TALK_STATE)
		{
			cur_mon_state = CALL_STATE;
			led_blink_1s_start();
		}
		if(!cur_talk_ing)
		{
			monitor_time = 30;	
			monitor_start_time = SDL_GetTicks();
			monitor_time_update(monitor_time);
		}
		/*
		if(cur_talk_ing)					//通话时 其他户外机call
			door_talk_start();
		else
			door_call_start();
		*/
		ituWidgetSetVisible(monitorWindowBackground, false);
		if(theConfig.zidong != 2)
		{
			auto_rec_once = true;			//自动录像
		}
		gState = SEND_STOP;
		usleep(50*1000);
		PR2000_set_end();
		cur_signal = event_call;
		event_call = 0;
		signal_itu_init(cur_signal);
		call_signal(cur_signal);
		usleep(10*1000);
		PR2000_set_start();
		usleep(50*1000);
		gState = SEND_BEGIN;
	}

#if 1
	if(pre_singnal_lock != pr2000_signal_lock)
	{
		pre_singnal_lock = pr2000_signal_lock;
		if(!pr2000_signal_lock)
		{
			ituWidgetSetVisible(monitorWindowBackground, true);
			cur_wind = false;
		}

	}
#endif

	if(play_call_ring_once)
	{
		play_call_ring_once = false;
		play_call_ring(play_call_ring_same);
	}

	if(sd_state_change)
	{
		sd_state_change = false;
		if(StorageGetCurrType() == STORAGE_NONE)
		{
		}
		else if(StorageGetCurrType() == STORAGE_SD)
			user_sd_card_check();
	}
	
	if(monitor_sd_icon)
	{
		monitor_sd_icon = 0;
	}
	if(mute_icon_change)
	{
		mute_icon_change = false;
	}

	return true;
}

bool MonitorOnLeave(ITUWidget* widget, char* param)
{
	printf("MonitorOnLeave........................................1\n");
	rec_start_time = 0;
	auto_rec_once = false;
	play_call_ring_once = false;
	mon_rec_ing = false;
	cur_call_ing = 0;
	gState = SEND_STOP;
	if(!pre_page)
	{
		 if(cur_mon_state == MON_STATE ||cur_mon_state == CCTV_STATE)
			mon_quit();
		else if(cur_mon_state == MTION_STATE)
			md_quit();
	}
	else
	{
		set_auto_rec_once();
		set_mon_rec_ing();	
		user_itu_init();
		pre_page = 0;
		return;
	}

	cur_mon_state = 0;
	cur_signal = 0;
	AudioStop();
	SceneLeaveVideoState();
	AudioResumeKeySound();
	usleep(100*1000);
	PR2000_set_end();
	user_amp_off();
	printf("MonitorOnLeave........................................2\n");
	
	return true;
}

bool MonitorRecordingButtonOnPress(ITUWidget* widget, char* param)
{
	user_snap(2);
	return true;
}

bool MonitorSnapButtonOnPress(ITUWidget* widget, char* param)
{
#if 0
	if(pr2000_test)
		pr2000_test = false;
	else 
		pr2000_test = true;
#else 
	if(rec_start_time  == 0 && (!auto_rec_once) && (!mon_rec_ing) && !sd_card_check)
	{
		mon_rec_ing = true;
		user_snap(1);
	}
#endif
	return true;
}

bool MonitorRecordButtonOnPress(ITUWidget* widget, char* param)
{
    struct statvfs info;
	if(StorageGetCurrType() == STORAGE_SD && (!auto_rec_once)&& (!mon_rec_ing) && !sd_card_check)
	{		
		if (statvfs("E:/", &info) == 0)
		{
			uint64_t avail = (uint64_t)info.f_bfree * info.f_bsize /1024 /1024;
			//printf("SD_CARD---------------->%ld------>%ld\n",(uint64_t)info.f_bfree * info.f_bsize,avail);
			if(avail < 200)
				return true;
		}
		mon_rec_ing = true;
		AudioStop();
		user_snap(2);		
	}
	return true;
}
static void _monitor_sw_cam()
{
	signal_itu_init(cur_signal);
	ituWidgetSetVisible(monitorWindowBackground, true);
	gState = SEND_STOP;
	usleep(50*1000);
	PR2000_set_end();
	rec_start_time = 0;
	if(cur_signal == signal_door_1 || cur_signal == signal_door_1)
		monitor_signal(cur_signal);
	else if(cur_signal == signal_cctv_1 || cur_signal == signal_cctv_2)
		cctv_signal(cur_signal);
	//signal_switch(cur_signal);
	usleep(10*1000);
	PR2000_set_start();
	usleep(50*1000);
	gState = SEND_BEGIN;
}

bool MonitorCameraSwitchButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(TARGET_BOARD_G)
	currCam = currCam ? 0 : 1;
	UserCameraSwitch(currCam);
	return true;
#endif
	return true;
}

bool MonitorFullBackgroundButtonOnPress(ITUWidget* widget, char* param)
{
	if(pop_up[MONITOR_S_FULL] == false)
	{
		pop_up[MONITOR_S_FULL] = true;
		ituWidgetHide(monitorHideBackground,0,0);
		ituWidgetHide(monitorHeadBackground,0,0);
	}
	else
	{
		pop_up[MONITOR_S_FULL] = false;
		ituWidgetShow(monitorHideBackground,0,0);
		ituWidgetShow(monitorHeadBackground,0,0);
	}
	return true;
}

bool MonitorMDButtonOnPress(ITUWidget* widget, char* param)
{
    int motionDetection = atoi(param);
	printf("motionDetection=%d\n", motionDetection);
	if(monitorMotionDetectionSprite)
		ituSpriteGoto(monitorMotionDetectionSprite, motionDetection);
	theConfig.md = motionDetection;
	return true;
}

