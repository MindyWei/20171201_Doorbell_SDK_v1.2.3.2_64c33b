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

static unsigned long md_start_time = 0;
static bool mon_rec_ing = false;
static int monitor_time;	
static uint32_t monitor_start_time;
static uint8_t monitor_sd_icon = 0;
static int x, y, width, height = 0;

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

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;
	destx = rect->x + x;
	desty = rect->y + y;
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

static void monitor_itu_init()
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

static void monitor_time_update(int val)
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
	monitor_itu_init();

	SceneEnterVideoState();

	ituWidgetGetGlobalPosition(monitorWindowIcon, &x, &y);
	width = ituWidgetGetWidth(monitorWindowIcon);
	height = ituWidgetGetHeight(monitorWindowIcon);
#ifdef CFG_VIDEO_ENABLE        
	itv_set_video_window(x, y, width, height);
#endif        

	if(1)
	{
#if defined(TARGET_BOARD_G)
		cur_signal = currCam + 1;
#else
		cur_signal = 1;
#endif
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
	PR2000_set_start();
	usleep(100*1000);
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
			cur_signal = signal_door_2;
		}
		else if(cur_signal == signal_door_2)
		{
			cur_signal = signal_door_1;
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
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
#endif
		monitor_time_update(monitor_time);
	}
		
#if 1
	if(pre_singnal_lock != pr2000_signal_lock)
	{
		pre_singnal_lock = pr2000_signal_lock;
		if(!pr2000_signal_lock)
		{
			ituWidgetSetVisible(monitorWindowBackground, true);
		}

	}
#endif

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

	return true;
}

bool MonitorOnLeave(ITUWidget* widget, char* param)
{
	printf("MonitorOnLeave........................................1\n");
	rec_start_time = 0;
	mon_rec_ing = false;
	gState = SEND_STOP;
	if(1)
	{
		 //if(cur_mon_state == MON_STATE ||cur_mon_state == CCTV_STATE)
		//	mon_quit();
		//else if(cur_mon_state == MTION_STATE)
		//	md_quit();
	}

	//cur_mon_state = 0;
	cur_signal = 0;
	AudioStop();
	SceneLeaveVideoState();
	AudioResumeKeySound();
	usleep(100*1000);
	PR2000_set_end();
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
	printf("Snap:rec_start_time, mon_rec_ing, sd_card_check(%d, %d, %d)\n", rec_start_time, mon_rec_ing, sd_card_check);
	if(rec_start_time  == 0 && (!mon_rec_ing) && !sd_card_check)
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
	printf("Rec: StorageGetCurrType, mon_rec_ing, sd_card_check(%d,%d, %d)\n", StorageGetCurrType(), mon_rec_ing, sd_card_check);
	if(StorageGetCurrType() == STORAGE_SD && (!mon_rec_ing) && !sd_card_check)
	{		
		if (statvfs("E:/", &info) == 0)
		{
			uint64_t avail = (uint64_t)info.f_bfree * info.f_bsize /1024 /1024;
			printf("SD_CARD---------------->%ld------>%ld\n",(uint64_t)info.f_bfree * info.f_bsize,avail);
			if(avail < 200)
				return true;
		}
		mon_rec_ing = true;
		AudioStop();
		user_snap(2);		
	}
	return true;
}

bool MonitorCameraSwitchButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(TARGET_BOARD_G)
	currCam = currCam ? 0 : 1;
	UserCameraSwitch(currCam);
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

