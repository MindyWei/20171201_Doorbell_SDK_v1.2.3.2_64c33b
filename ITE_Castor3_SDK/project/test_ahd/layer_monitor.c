#include <assert.h>
#include "user_function.h"


static ITUIcon* MON_ICON_WIN;
static ITUIcon* MON_ICON_OPEN;
static ITUBackground* MON_BG_HIDE;
static ITUBackground* MON_HEAD;
static ITUText* MON_TEXT_CAM;
static ITUText* MON_TEXT_DOOR_1;
static ITUText* MON_TEXT_DOOR_2;
static ITUText* MON_TEXT_CCTV_1;
static ITUText* MON_TEXT_CCTV_2;
static ITUIcon* MON_ICON_STATE_CARD_1;
static ITUIcon* MON_ICON_STATE_CARD_2;
static ITUIcon* MON_ICON_STATE_CARD_3;
static ITUButton* MON_HEAD_BTN_BACK;
static ITUCalendar* MON_CLOCK;
static ITUBackground* MON_BG_RING_VOL_BAR;
static ITUBackground* MON_BG_TALK_VOL_BAR;
static ITUBackground* MON_BG_VOL_BACK;
static ITUBackground* MON_BG_DIS_BAR;
static ITUBackground* MON_BG_DIS_BACK;
static ITUBackground* MON_BG_FULL;
static ITUBackground* MON_BG_WIN;
static ITUButton* MON_BTN_CAM_SW_1;
static ITUButton* MON_BTN_CAM_SW_2;
static ITUButton* MON_BTN_CAM_SW_3;
static ITUButton* MON_BTN_CAM_SW_4;
static ITUButton* MON_BTN_RING_VOL_SET;
static ITUButton* MON_BTN_TALK_VOL_SET;
static ITUButton* MON_BTN_TALK;
static ITUButton* MON_BTN_BACK;
static ITUButton* MON_BTN_OPEN;
static ITUButton* MON_BTN_HANG;
static ITUButton* MON_BTN_REC;
static ITUButton* MON_BTN_REC_ING;
static ITUText* MON_TEXT_REC_TIME;
static ITUTrackBar* MON_TR_BAR_BRI;
static ITUTrackBar* MON_TR_BAR_CON;
static ITUTrackBar* MON_TR_BAR_HUE;
static ITUProgressBar* MON_PR_BAR_BRI;
static ITUProgressBar* MON_PR_BAR_CON;
static ITUProgressBar* MON_PR_BAR_HUE;
static ITUIcon* MON_ICON_STATE_MUTE;
static ITUIcon* MON_ICON_STATE_PHOTO;
static ITUIcon* MON_ICON_STATE_VIDEO;
static ITUIcon* MON_ICON_STATE_AUTO;
static ITUIcon* MON_ICON_STATE_MNAUAL;

static ITUTrackBar* MON_TR_RING_VOL_BAR;
static ITUProgressBar* MON_PR_RING_VOL_BAR;
static ITUIcon* MON_ICON_RING_VOL_1;
static ITUIcon* MON_ICON_RING_VOL_2;
static ITUIcon* MON_ICON_RING_VOL_3;
static ITUIcon* MON_ICON_RING_VOL_4;

static ITUTrackBar* MON_TR_TALK_VOL_BAR;
static ITUProgressBar* MON_PR_TALK_VOL_BAR;
static ITUIcon* MON_ICON_TALK_VOL_1;
static ITUIcon* MON_ICON_TALK_VOL_2;
static ITUIcon* MON_ICON_TALK_VOL_3;
static ITUIcon* MON_ICON_TALK_VOL_4;
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

void set_monitor_sd_icon(int val)
{
	monitor_sd_icon = val;
	return;
}

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
	ituWidgetSetVisible(MON_BG_WIN, true);
	ituWidgetSetVisible(MON_BTN_OPEN,false);
	ituWidgetSetVisible(MON_BTN_HANG,false);
	ituWidgetSetVisible(MON_BTN_TALK_VOL_SET,false);
	ituWidgetSetVisible(MON_BTN_REC,true);
	ituWidgetSetVisible(MON_BTN_REC_ING,false);
	
	if(ituWidgetIsVisible(MON_BG_RING_VOL_BAR))
	{
		ituWidgetSetVisible(MON_BG_RING_VOL_BAR,false);		//声音UI 恢复
		ituWidgetSetVisible(MON_BG_VOL_BACK,false);
	}
	
	if(ituWidgetIsVisible(MON_BG_TALK_VOL_BAR))
	{
		ituWidgetSetVisible(MON_BG_TALK_VOL_BAR,false);		//声音UI 恢复
		ituWidgetSetVisible(MON_BG_VOL_BACK,false);
	}
		
	if(MON_BG_DIS_BAR)
	{
		ituWidgetSetVisible(MON_BG_DIS_BAR,false);		//显示设置 UI 恢复
		ituWidgetSetVisible(MON_BG_DIS_BACK,false);
	}
	if(pop_up[MONITOR_S_FULL] == true)
	{
		pop_up[MONITOR_S_FULL] = false;
		ituWidgetSetVisible(MON_BG_HIDE,true);
		ituWidgetSetVisible(MON_HEAD,true);
	}
	ituWidgetSetVisible(MON_BG_HIDE,true);
	ituWidgetSetVisible(MON_BG_FULL,true);
	
	if(cur_page == page_monitor)
	{
		if(theConfig.ringvol == 0)	//静音图标
			ituWidgetSetVisible(MON_ICON_STATE_MUTE,true);
		else
			ituWidgetSetVisible(MON_ICON_STATE_MUTE,false);
		
		if(!theConfig.open)	//未开启快捷开锁功能
		{
			ituWidgetSetVisible(MON_BTN_OPEN,false);
			ituWidgetSetVisible(MON_BTN_TALK,true);
		}
		else
		{
			ituWidgetSetVisible(MON_BTN_TALK,false);
			ituWidgetSetVisible(MON_BTN_OPEN,true);
		}
		
		ituWidgetSetVisible(MON_BTN_BACK,true);
		ituWidgetSetVisible(MON_BTN_RING_VOL_SET,true);
		ituWidgetSetVisible(MON_HEAD_BTN_BACK,false);
	//	ituWidgetSetX(MON_TEXT_CAM, 26);
		ituWidgetSetX(MON_CLOCK, 131);
	}
	else if(cur_page == page_cctv)
	{
		ituWidgetSetVisible(MON_ICON_STATE_MUTE,false);
		ituWidgetSetVisible(MON_BTN_TALK,false);
		ituWidgetSetVisible(MON_BTN_BACK,false);
		ituWidgetSetVisible(MON_BTN_RING_VOL_SET,false);
		ituWidgetSetVisible(MON_HEAD_BTN_BACK,true);
		//ituWidgetSetX(MON_TEXT_CAM, 94);
		ituWidgetSetX(MON_CLOCK, 199);
	}
	else if(cur_page == page_motion)
	{
		ituWidgetHide(MON_BG_HIDE,0,0);
	}

	if(StorageGetCurrType() == STORAGE_NONE)
	{
		ituWidgetSetVisible(MON_ICON_STATE_CARD_1,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_2,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_3,false);
	}
	else if(StorageGetCurrType() == STORAGE_SD)
	{
		ituWidgetSetVisible(MON_ICON_STATE_CARD_1,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_2,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_3,false);
		user_sd_card_check();
	}

	if(theConfig.zidong == 2)
	{
		ituWidgetSetVisible(MON_ICON_STATE_MNAUAL,true);
		ituWidgetSetVisible(MON_ICON_STATE_AUTO,false);
	}
	else
	{
		ituWidgetSetVisible(MON_ICON_STATE_AUTO,true);
		ituWidgetSetVisible(MON_ICON_STATE_MNAUAL,false);
	}
	
	if(theConfig.md)
	{
		if(theConfig.mdsave)
		{
			ituWidgetSetVisible(MON_ICON_STATE_PHOTO,false);
			ituWidgetSetVisible(MON_ICON_STATE_VIDEO,true);
		}
		else
		{
			ituWidgetSetVisible(MON_ICON_STATE_PHOTO,true);
			ituWidgetSetVisible(MON_ICON_STATE_VIDEO,false);
		}
	}
	else
	{
		ituWidgetSetVisible(MON_ICON_STATE_VIDEO,false);
		ituWidgetSetVisible(MON_ICON_STATE_PHOTO,false);
	}
	if(monitorMotionDetectionSprite)
		ituSpriteGoto(monitorMotionDetectionSprite, theConfig.md);
}

void signal_itu_init(int val)
{
	char buf_1[6][10] = {"DOOR1","DOOR2","CCTV 1","CCTV 2","CCTV 3","CCTV4"};
	
	ituWidgetSetVisible(MON_BTN_CAM_SW_1,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_2,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_3,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_4,false);
		
	ituWidgetSetVisible(MON_TEXT_DOOR_1,false);
	ituWidgetSetVisible(MON_TEXT_DOOR_2,false);
	ituWidgetSetVisible(MON_TEXT_CCTV_1,false);
	ituWidgetSetVisible(MON_TEXT_CCTV_2,false);
	//ituTextSetString(MON_TEXT_CAM,buf_1[val-1]);
	if(1 == val)
	{
		ituWidgetSetVisible(MON_TEXT_DOOR_1,true);
		ituWidgetSetVisible(MON_BTN_CAM_SW_1,true);
	}
	else if(2 == val)
	{
		ituWidgetSetVisible(MON_TEXT_DOOR_2,true);
		ituWidgetSetVisible(MON_BTN_CAM_SW_2,true);
	}
	else if(3 == val)
	{
		ituWidgetSetVisible(MON_TEXT_CCTV_1,true);
		ituWidgetSetVisible(MON_BTN_CAM_SW_3,true);
	}
	else if(4 == val)
	{
		ituWidgetSetVisible(MON_TEXT_CCTV_2,true);
		ituWidgetSetVisible(MON_BTN_CAM_SW_4,true);
	}
}

void monitor_time_update(int val)
{
	char time_buf[10] = "\0";
	static ITUText* MON_TEXT_REC_TIME_S ;
	MON_TEXT_REC_TIME_S	= ituSceneFindWidget(&theScene, "MON_TEXT_REC_TIME");
	assert(MON_TEXT_REC_TIME_S);
	
	time_buf[0] = '0';
	time_buf[1] = '0';
	time_buf[2] = ':';
	time_buf[3] = val/10+'0';
	time_buf[4] = val%10+'0';
	ituTextSetString(MON_TEXT_REC_TIME_S, time_buf);	
}

bool monitor_init(ITUWidget* widget, char* param)
{
	if (!MON_ICON_WIN)
	{
		MON_ICON_WIN = ituSceneFindWidget(&theScene, "MON_ICON_WIN");
		assert(MON_ICON_WIN);
		ituWidgetSetDraw(MON_ICON_WIN, VideoPlayerViewBackgroundDraw);
		MON_ICON_OPEN = ituSceneFindWidget(&theScene, "MON_ICON_OPEN");
		assert(MON_ICON_OPEN);
		MON_BG_HIDE = ituSceneFindWidget(&theScene, "MON_BG_HIDE");
		assert(MON_BG_HIDE);
		MON_HEAD = ituSceneFindWidget(&theScene, "MON_HEAD");
		assert(MON_HEAD);
		MON_TEXT_CAM = ituSceneFindWidget(&theScene, "MON_TEXT_CAM");
		assert(MON_TEXT_CAM);
		MON_TEXT_DOOR_1 = ituSceneFindWidget(&theScene, "MON_TEXT_DOOR_1");
		assert(MON_TEXT_DOOR_1);
		MON_TEXT_DOOR_2 = ituSceneFindWidget(&theScene, "MON_TEXT_DOOR_2");
		assert(MON_TEXT_DOOR_2);
		MON_TEXT_CCTV_1 = ituSceneFindWidget(&theScene, "MON_TEXT_CCTV_1");
		assert(MON_TEXT_CCTV_1);
		MON_TEXT_CCTV_2 = ituSceneFindWidget(&theScene, "MON_TEXT_CCTV_2");
		assert(MON_TEXT_CCTV_2);

		MON_ICON_STATE_CARD_1 = ituSceneFindWidget(&theScene, "MON_ICON_STATE_CARD_1");
		assert(MON_ICON_STATE_CARD_1);
		MON_ICON_STATE_CARD_2 = ituSceneFindWidget(&theScene, "MON_ICON_STATE_CARD_2");
		assert(MON_ICON_STATE_CARD_2);
		MON_ICON_STATE_CARD_3 = ituSceneFindWidget(&theScene, "MON_ICON_STATE_CARD_3");
		assert(MON_ICON_STATE_CARD_3);
		MON_HEAD_BTN_BACK = ituSceneFindWidget(&theScene, "MON_HEAD_BTN_BACK");
		assert(MON_HEAD_BTN_BACK);
		MON_CLOCK = ituSceneFindWidget(&theScene, "MON_CLOCK");
		assert(MON_CLOCK);
		MON_BG_RING_VOL_BAR = ituSceneFindWidget(&theScene, "MON_BG_RING_VOL_BAR");
		assert(MON_BG_RING_VOL_BAR);
		MON_BG_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "MON_BG_TALK_VOL_BAR");
		assert(MON_BG_TALK_VOL_BAR);
		MON_BG_VOL_BACK = ituSceneFindWidget(&theScene, "MON_BG_VOL_BACK");
		assert(MON_BG_VOL_BACK);
		MON_BG_DIS_BAR = ituSceneFindWidget(&theScene, "MON_BG_DIS_BAR");
		assert(MON_BG_DIS_BAR);
		MON_BG_DIS_BACK = ituSceneFindWidget(&theScene, "MON_BG_DIS_BACK");
		assert(MON_BG_DIS_BACK);
		MON_BG_FULL = ituSceneFindWidget(&theScene, "MON_BG_FULL");
		assert(MON_BG_FULL);
		MON_BG_WIN = ituSceneFindWidget(&theScene, "MON_BG_WIN");
		assert(MON_BG_WIN);
		MON_BTN_CAM_SW_1 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_1");
		assert(MON_BTN_CAM_SW_1);
		MON_BTN_CAM_SW_2 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_2");
		assert(MON_BTN_CAM_SW_2);
		MON_BTN_CAM_SW_3 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_3");
		assert(MON_BTN_CAM_SW_3);
		MON_BTN_CAM_SW_4 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_4");
		assert(MON_BTN_CAM_SW_4);
		MON_BTN_RING_VOL_SET = ituSceneFindWidget(&theScene, "MON_BTN_RING_VOL_SET");
		assert(MON_BTN_RING_VOL_SET);
		MON_BTN_TALK_VOL_SET = ituSceneFindWidget(&theScene, "MON_BTN_TALK_VOL_SET");
		assert(MON_BTN_TALK_VOL_SET);
		MON_BTN_TALK = ituSceneFindWidget(&theScene, "MON_BTN_TALK");
		assert(MON_BTN_TALK);
		MON_BTN_BACK = ituSceneFindWidget(&theScene, "MON_BTN_BACK");
		assert(MON_BTN_BACK);
		MON_BTN_OPEN = ituSceneFindWidget(&theScene, "MON_BTN_OPEN");
		assert(MON_BTN_OPEN);
		MON_BTN_HANG = ituSceneFindWidget(&theScene, "MON_BTN_HANG");
		assert(MON_BTN_HANG);
		MON_BTN_REC = ituSceneFindWidget(&theScene, "MON_BTN_REC");
		assert(MON_BTN_REC);
		MON_BTN_REC_ING = ituSceneFindWidget(&theScene, "MON_BTN_REC_ING");
		assert(MON_BTN_REC_ING);
		MON_TEXT_REC_TIME = ituSceneFindWidget(&theScene, "MON_TEXT_REC_TIME");
		assert(MON_TEXT_REC_TIME);
		MON_TR_BAR_BRI = ituSceneFindWidget(&theScene, "MON_TR_BAR_BRI");
		assert(MON_TR_BAR_BRI);
		MON_TR_BAR_CON = ituSceneFindWidget(&theScene, "MON_TR_BAR_CON");
		assert(MON_TR_BAR_CON);
		MON_TR_BAR_HUE = ituSceneFindWidget(&theScene, "MON_TR_BAR_HUE");
		assert(MON_TR_BAR_HUE);
		MON_PR_BAR_BRI = ituSceneFindWidget(&theScene, "MON_PR_BAR_BRI");
		assert(MON_PR_BAR_BRI);
		MON_PR_BAR_CON = ituSceneFindWidget(&theScene, "MON_PR_BAR_CON");
		assert(MON_PR_BAR_CON);
		MON_PR_BAR_HUE = ituSceneFindWidget(&theScene, "MON_PR_BAR_HUE");
		assert(MON_PR_BAR_HUE);
		MON_ICON_STATE_MUTE = ituSceneFindWidget(&theScene, "MON_ICON_STATE_MUTE");
		assert(MON_ICON_STATE_MUTE);
		MON_ICON_STATE_PHOTO = ituSceneFindWidget(&theScene, "MON_ICON_STATE_PHOTO");
		assert(MON_ICON_STATE_PHOTO);
		MON_ICON_STATE_VIDEO = ituSceneFindWidget(&theScene, "MON_ICON_STATE_VIDEO");
		assert(MON_ICON_STATE_VIDEO);
		MON_ICON_STATE_AUTO = ituSceneFindWidget(&theScene, "MON_ICON_STATE_AUTO");
		assert(MON_ICON_STATE_AUTO);
		MON_ICON_STATE_MNAUAL = ituSceneFindWidget(&theScene, "MON_ICON_STATE_MNAUAL");
		assert(MON_ICON_STATE_MNAUAL);
		
		MON_TR_RING_VOL_BAR = ituSceneFindWidget(&theScene, "MON_TR_RING_VOL_BAR");
		assert(MON_TR_RING_VOL_BAR);
		MON_PR_RING_VOL_BAR = ituSceneFindWidget(&theScene, "MON_PR_RING_VOL_BAR");
		assert(MON_PR_RING_VOL_BAR);
		MON_ICON_RING_VOL_1 = ituSceneFindWidget(&theScene, "MON_ICON_RING_VOL_1");
		assert(MON_ICON_RING_VOL_1);
		MON_ICON_RING_VOL_2 = ituSceneFindWidget(&theScene, "MON_ICON_RING_VOL_2");
		assert(MON_ICON_RING_VOL_2);
		MON_ICON_RING_VOL_3 = ituSceneFindWidget(&theScene, "MON_ICON_RING_VOL_3");
		assert(MON_ICON_RING_VOL_3);
		MON_ICON_RING_VOL_4 = ituSceneFindWidget(&theScene, "MON_ICON_RING_VOL_4");
		assert(MON_ICON_RING_VOL_4);
		
		MON_TR_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "MON_TR_TALK_VOL_BAR");
		assert(MON_TR_TALK_VOL_BAR);
		MON_PR_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "MON_PR_TALK_VOL_BAR");
		assert(MON_PR_TALK_VOL_BAR);
		MON_ICON_TALK_VOL_1 = ituSceneFindWidget(&theScene, "MON_ICON_TALK_VOL_1");
		assert(MON_ICON_TALK_VOL_1);
		MON_ICON_TALK_VOL_2 = ituSceneFindWidget(&theScene, "MON_ICON_TALK_VOL_2");
		assert(MON_ICON_TALK_VOL_2);
		MON_ICON_TALK_VOL_3= ituSceneFindWidget(&theScene, "MON_ICON_TALK_VOL_3");
		assert(MON_ICON_TALK_VOL_3);
		MON_ICON_TALK_VOL_4 = ituSceneFindWidget(&theScene, "MON_ICON_TALK_VOL_4");
		assert(MON_ICON_TALK_VOL_4);

		monitorMotionDetectionSprite = ituSceneFindWidget(&theScene, "monitorMotionDetectionSprite");
		assert(monitorMotionDetectionSprite);
	}
	printf("enter page monitor........................\n");
	//printf("monitor_1------------------------->%d\n",SDL_GetTicks()-test_tick);
	//ITU 初始化
	user_itu_init();

	//AudioPauseKeySound();		//关闭按键音
	SceneEnterVideoState();		//切换帧率
	//printf("monitor_------------------------->%d\n",SDL_GetTicks()-test_tick);

	ituWidgetGetGlobalPosition(MON_ICON_WIN, &x, &y);
	width = ituWidgetGetWidth(MON_ICON_WIN);
	height = ituWidgetGetHeight(MON_ICON_WIN);
#ifdef CFG_VIDEO_ENABLE        
	itv_set_video_window(x, y, width, height);
#endif        


#if 1	//my.wei add for test ahd
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

#else
	if(event_call > 0)				//CALL 机进入处理
	{
		printf("call----------------------->%d\n",event_call);
		cur_page = page_monitor;
		cur_signal = event_call;
		event_call = 0;
		call_signal(cur_signal);
		play_call_ring_once = true;
		if(theConfig.zidong != 2)
			AudioPauseKeySound();		//关闭按键音
		monitor_time = 30;	
	}
	else if(cur_page == page_monitor)						//按键进入处理
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
	else if(cur_page == page_cctv)
	{
		printf("page_cctv.....................................................\n");
		if(master_vdp)
		{
			if(signal_insert[CCTV_1])
				cur_signal = signal_cctv_1;
			else if(signal_insert[CCTV_2])
				cur_signal = signal_cctv_2;
		}
		cctv_signal(cur_signal);
		monitor_time = 60;	
	}
#endif	
	//led_blink_1s_start();
#if TEST_CAM
	ituWidgetHide(MON_BG_HIDE,0,0);
	ituWidgetHide(MON_HEAD,0,0);
#endif
	if(ScreenIsOff())
		ScreenOn();
	monitor_start_time = SDL_GetTicks();
	monitor_time_update(monitor_time);
	ituWidgetSetVisible(MON_TEXT_REC_TIME,true);
	ithGpioClear(AUDIO_IN);
	printf("cur_page, cur_signal= (%d, %d)........\r\n", cur_page, cur_signal);
	signal_itu_init(cur_signal);
	PR2000_set_start();
	usleep(100*1000);
	//printf(".....................................................\n");
	gState = SEND_BEGIN;			//显示图像标志
	return true;
	
}

bool monitor_timer(ITUWidget* widget, char* param)
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
		ituWidgetSetVisible(MON_BG_WIN, false);
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

	if(cur_open != pre_open)		//开锁图标
	{
		ituWidgetSetVisible(MON_ICON_OPEN,cur_open);
		pre_open = cur_open;
	}
#if 1
	if(pre_singnal_lock != pr2000_signal_lock)
	{
		pre_singnal_lock = pr2000_signal_lock;
		if(!pr2000_signal_lock)
		{
			ituWidgetSetVisible(MON_BG_WIN, true);
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
			ituWidgetSetVisible(MON_ICON_STATE_CARD_1,false);
			ituWidgetSetVisible(MON_ICON_STATE_CARD_2,false);
			ituWidgetSetVisible(MON_ICON_STATE_CARD_3,false);
		}
		else if(StorageGetCurrType() == STORAGE_SD)
			user_sd_card_check();
	}
	
	if(monitor_sd_icon)
	{
		ituWidgetSetVisible(MON_ICON_STATE_CARD_1,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_2,false);
		ituWidgetSetVisible(MON_ICON_STATE_CARD_3,false);
		if(monitor_sd_icon == 1)
			ituWidgetSetVisible(MON_ICON_STATE_CARD_1,true);
		else if(monitor_sd_icon == 2)
			ituWidgetSetVisible(MON_ICON_STATE_CARD_2,true);
		monitor_sd_icon = 0;
	}
	if(mute_icon_change)
	{
		mute_icon_change = false;
		if(theConfig.ringvol == 0)
			ituWidgetSetVisible(MON_ICON_STATE_MUTE,true);
		else
			ituWidgetSetVisible(MON_ICON_STATE_MUTE,false);
	}

	return true;
}

bool monitor_leave(ITUWidget* widget, char* param)
{
	printf("monitor_leave........................................1\n");
	rec_start_time = 0;							//结束录像
	//while(get_record_stop_over())
	auto_rec_once = false;						//清除自动标志
	play_call_ring_once = false;					//清除CALL机铃声播放标志
	mon_rec_ing = false;
	cur_call_ing = 0;
	//usleep(50*1000);
	gState = SEND_STOP;							//停止图像
	if(!pre_page)
	{
		 if(cur_mon_state == MON_STATE ||cur_mon_state == CCTV_STATE)				//普通监控状态退出处理
			mon_quit();
		else if(cur_mon_state == M_TALK_STATE)			// 普通监控TALK 状态下退出处理
			m_talk_quit();
		else if(cur_mon_state == CALL_STATE)			//CALL机状态下退出处理
			call_quit();
		else if(cur_mon_state == C_TALK_STATE)			//CALL机TALK 状态下退出处理
			c_talk_quit();
		else if(cur_mon_state == MTION_STATE)			//CALL机TALK 状态下退出处理
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
	cur_signal = 0;								//当前信号标志初始化
	AudioStop();									//停止铃声
	SceneLeaveVideoState();						//切换帧率
	AudioResumeKeySound();						//恢复按键音
	usleep(100*1000);
	PR2000_set_end();
	if(cur_mon_state == M_TALK_STATE)
	{
		clear_back_home_flag();
	}
	user_amp_off();
	//ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
	printf("monitor_leave........................................2\n");
	
	return true;
}

bool monitor_rec_end(ITUWidget* widget, char* param)
{
	user_snap(2);
	//ituWidgetSetVisible(MON_BTN_REC,true);//use timer
	//ituWidgetSetVisible(MON_BTN_REC_ING,false);
	return true;
}

bool monitor_dis_set(ITUWidget* widget, char* param)
{
	if(cur_page == page_monitor)
	{
		ituTrackBarSetValue(MON_TR_BAR_BRI,theConfig.brightness);
		ituProgressBarSetValue(MON_PR_BAR_BRI,theConfig.brightness);
		ituTrackBarSetValue(MON_TR_BAR_CON,theConfig.contrast);
		ituProgressBarSetValue(MON_PR_BAR_CON,theConfig.contrast);
		ituTrackBarSetValue(MON_TR_BAR_HUE,theConfig.hue);
		ituProgressBarSetValue(MON_PR_BAR_HUE,theConfig.hue);
	}
	else
	{
		ituTrackBarSetValue(MON_TR_BAR_BRI,theConfig.brightness_c);
		ituProgressBarSetValue(MON_PR_BAR_BRI,theConfig.brightness_c);
		ituTrackBarSetValue(MON_TR_BAR_CON,theConfig.contrast_c);
		ituProgressBarSetValue(MON_PR_BAR_CON,theConfig.contrast_c);
		ituTrackBarSetValue(MON_TR_BAR_HUE,theConfig.hue_c);
		ituProgressBarSetValue(MON_PR_BAR_HUE,theConfig.hue_c);
	}
	return true;
}
bool monitor_snap(ITUWidget* widget, char* param)
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

bool monitor_rec_start(ITUWidget* widget, char* param)
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
		//ituWidgetSetVisible(MON_BTN_REC,false);//use timer
		//ituWidgetSetVisible(MON_BTN_REC_ING,true);
		
	}
	return true;
}

bool monitor_open(ITUWidget* widget, char* param)
{
	user_open_door(cur_signal);
	door_open_timer_start();
	AudioPlaySound(CFG_PRIVATE_DRIVE ":/sounds/open.mp3");	
	return true;
}

bool monitor_talk(ITUWidget* widget, char* param)
{
	printf("monitor_talk.................\n");
	cur_talk_ing = true;
	if(cur_mon_state == MON_STATE)
	{
		cur_mon_state = M_TALK_STATE;
	}
	else if(cur_mon_state == CALL_STATE)
	{
		cur_mon_state = C_TALK_STATE;
		AudioStop();							//停止铃声
	}
	busy_30_reinit();
	if(auto_rec_once)
	{
		if(theConfig.zidong == 1 && (StorageGetCurrType() == STORAGE_SD))
		{
			auto_snap_filename = true;
			user_snap(2);
		}
		else
		{
			auto_snap_filename = true;
			user_snap(1);
		}
		auto_rec_once = false;
	}
	monitor_time = 60;	
	monitor_start_time = SDL_GetTicks();
	monitor_time_update(monitor_time);
	
	led_blink_1s_end();
	talk_volume_set(theConfig.talkvol);
	ithGpioSet(CONV_CONT);
	ithGpioClear(MIC_NUTE);
	ithGpioSet(AUDIO_IN);	
	user_amp_on();
	talk_volume_set(theConfig.talkvol);
	ituWidgetSetVisible(MON_BTN_TALK,false);
	ituWidgetSetVisible(MON_BTN_BACK,false);
	ituWidgetSetVisible(MON_BTN_RING_VOL_SET,false);
	ituWidgetSetVisible(MON_BTN_OPEN,true);
	ituWidgetSetVisible(MON_BTN_HANG,true);
	ituWidgetSetVisible(MON_BTN_TALK_VOL_SET,true);

	return true;
}
void _monitor_sw_cam()
{
	signal_itu_init(cur_signal);
	ituWidgetSetVisible(MON_BG_WIN, true);
	gState = SEND_STOP;
	usleep(50*1000);
	PR2000_set_end();
	rec_start_time = 0;				//结束录像
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

bool monitor_sw_cam(ITUWidget* widget, char* param)
{
#if defined(TARGET_BOARD_G)
	currCam = currCam ? 0 : 1;
	cam_switch(currCam);
	return true;
#endif

	if(cur_mon_state == CALL_STATE)
		return true;
	if(master_vdp)
	{
		if(cur_signal == signal_door_1)
		{
			if(!signal_insert[DOOR_2])
				return true;
			cur_signal = signal_door_2;
			monitor_signal(cur_signal);
		}
		else if(cur_signal == signal_door_2)
		{
			if(!signal_insert[DOOR_1])
				return true;
			cur_signal = signal_door_1;
			monitor_signal(cur_signal);
		}
		else if(cur_signal == signal_cctv_1)
		{
			if(!signal_insert[CCTV_2])
				return true;
			cur_signal = signal_cctv_2;
			cctv_signal(cur_signal);
		}
		else if(cur_signal == signal_cctv_2)
		{
			if(!signal_insert[CCTV_1])
				return true;
			cur_signal = signal_cctv_1;
			cctv_signal(cur_signal);
		}
		_monitor_sw_cam();
	}
	return true;
}

void monitor_talk_volume_bar(int val)
{
	if(val == 0)
	{
		ituWidgetSetVisible(MON_ICON_TALK_VOL_1, true);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_2, true);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_3, false);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_4, false);
	}
	else if(val == 3)
	{
		ituWidgetSetVisible(MON_ICON_TALK_VOL_1, false);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_2, false);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_3, true);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_4, true);
	}
	else
	{
		ituWidgetSetVisible(MON_ICON_TALK_VOL_1, false);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_2, true);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_3, true);
		ituWidgetSetVisible(MON_ICON_TALK_VOL_4, false);
	}
}

void monitor_ring_volume_bar(int val)
{
	if(val == 0)
	{
		ituWidgetSetVisible(MON_ICON_RING_VOL_1, true);
		ituWidgetSetVisible(MON_ICON_RING_VOL_2, true);
		ituWidgetSetVisible(MON_ICON_RING_VOL_3, false);
		ituWidgetSetVisible(MON_ICON_RING_VOL_4, false);
	}
	else if(val == 3)
	{
		ituWidgetSetVisible(MON_ICON_RING_VOL_1, false);
		ituWidgetSetVisible(MON_ICON_RING_VOL_2, false);
		ituWidgetSetVisible(MON_ICON_RING_VOL_3, true);
		ituWidgetSetVisible(MON_ICON_RING_VOL_4, true);
	}
	else
	{
		ituWidgetSetVisible(MON_ICON_RING_VOL_1, false);
		ituWidgetSetVisible(MON_ICON_RING_VOL_2, true);
		ituWidgetSetVisible(MON_ICON_RING_VOL_3, true);
		ituWidgetSetVisible(MON_ICON_RING_VOL_4, false);
	}
}

bool monitor_vol_set(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
	case '1':
		monitor_ring_volume_bar(theConfig.ringvol);
		ituTrackBarSetValue(MON_TR_RING_VOL_BAR,theConfig.ringvol);
		ituProgressBarSetValue(MON_PR_RING_VOL_BAR,theConfig.ringvol);
		break;
	case '2':
		monitor_talk_volume_bar(theConfig.talkvol);
		ituTrackBarSetValue(MON_TR_TALK_VOL_BAR,theConfig.talkvol);
		ituProgressBarSetValue(MON_PR_TALK_VOL_BAR,theConfig.talkvol);
		break;
	}
	return true;
}


bool monitor_vol_adjust(ITUWidget* widget, char* param)
{
	int bar_val;
	
	switch(param[0])
	{
		case '1':
			theConfig.ringvol--;
			if(theConfig.ringvol < 0)
				theConfig.ringvol = 0;
			if(theConfig.ringvol == 0 && theConfig.Iringvol == 0)
				theConfig.mute = 1;
			else
				theConfig.mute = 0;
			if(theConfig.ringvol == 0)
				AudioStop();							//停止铃声
			mute_icon_change = true;
			bar_val = theConfig.ringvol;
			ring_volume_set(theConfig.ringvol);
			break;
		case '2':
			theConfig.ringvol++;
			if(theConfig.ringvol > 3)
				theConfig.ringvol = 3;
			user_amp_on();
			theConfig.mute = 0;
			mute_icon_change = true;
			bar_val = theConfig.ringvol;
			ring_volume_set(theConfig.ringvol);
		case '3':
			theConfig.talkvol--;
			if(theConfig.talkvol < 1)
				theConfig.talkvol = 1;
			bar_val = theConfig.talkvol;
			talk_volume_set(theConfig.talkvol);
			break;
		case '4':
			theConfig.talkvol++;
			if(theConfig.talkvol > 3)
				theConfig.talkvol = 3;
			bar_val = theConfig.talkvol;
			talk_volume_set(theConfig.talkvol);
			break;
			break;
	}
		
	monitor_ring_volume_bar(theConfig.ringvol);
	ituTrackBarSetValue(MON_TR_RING_VOL_BAR,theConfig.ringvol);
	ituProgressBarSetValue(MON_PR_RING_VOL_BAR,theConfig.ringvol);
	monitor_talk_volume_bar(theConfig.talkvol);
	ituTrackBarSetValue(MON_TR_TALK_VOL_BAR,theConfig.talkvol);
	ituProgressBarSetValue(MON_PR_TALK_VOL_BAR,theConfig.talkvol);
	ConfigSave();
	return true;
}

bool monitor_hang(ITUWidget* widget, char* param)
{
	return true;
}

bool monitor_dis_reset(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	if(cur_page == page_monitor)
	{
		theConfig.brightness = 50;
		theConfig.contrast = 50;
		theConfig.hue = 50;
	}
	else
	{
		theConfig.brightness_c = 50;
		theConfig.contrast_c = 50;
		theConfig.hue_c = 50;
	}
	ConfigSave();
	ituTrackBarSetValue(MON_TR_BAR_BRI,50);
	ituProgressBarSetValue(MON_PR_BAR_BRI,50);
	ituTrackBarSetValue(MON_TR_BAR_CON,50);
	ituProgressBarSetValue(MON_PR_BAR_CON,50);
	ituTrackBarSetValue(MON_TR_BAR_HUE,50);
	ituProgressBarSetValue(MON_PR_BAR_HUE,50);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_BRGT,0X80-20);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_CONT,0X80+20);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_HUE,0X80+40);
	return true;
}

bool monitor_dis_change(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
	case '1':
		printf("------------------------->value = %d\n",MON_TR_BAR_BRI->value);
		theConfig.brightness = MON_TR_BAR_BRI->value;
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.brightness*200/100+28);
		break;
	case '2':
		printf("------------------------->value = %d\n",MON_TR_BAR_CON->value);
		theConfig.contrast = MON_TR_BAR_CON->value;
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.contrast*200/100+28);
		break;
	case '3':
		printf("------------------------->value = %d\n",MON_TR_BAR_HUE->value);
		theConfig.hue = MON_TR_BAR_HUE->value;
		pr2000_i2c_write(0xb8,0xff,0x01);
		pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.hue*200/100+28);
		break;
	}
	//ConfigSave();
	return true;
}

bool monitor_dis_ch_bri(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_1->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_BRGT,MON_TR_BAR_BRI->value+58);
	if(cur_page == page_monitor)
		theConfig.brightness = MON_TR_BAR_BRI->value;
	else
		theConfig.brightness_c = MON_TR_BAR_BRI->value;
	ConfigSave();
	return true;
}

bool monitor_dis_ch_con(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_2->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_CONT,MON_TR_BAR_CON->value+98);
	if(cur_page == page_monitor)
		theConfig.contrast = MON_TR_BAR_CON->value;
	else 
		theConfig.contrast_c = MON_TR_BAR_CON->value;
	ConfigSave();
	return true;
}

bool monitor_dis_ch_hue(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	//printf("-------------->%d\n",monitor_menu_2_TrackBar_3->value);
	pr2000_i2c_write(0xb8,0xff,0x01);
	pr2000_i2c_write(0xb8,PR2000_HUE,MON_TR_BAR_HUE->value+118);
	if(cur_page == page_monitor)
		theConfig.hue = MON_TR_BAR_HUE->value;
	else
		theConfig.hue_c= MON_TR_BAR_HUE->value;
	ConfigSave();
	return true;
}

bool monitor_dis_dajust(ITUWidget* widget, char* param)
{
	if(!sure_PR2000_lock())
		return;
	switch(param[0])
	{
	case '1':
	case '2':
		if(cur_page == page_monitor)
		{
			if(param[0] == '1')
				theConfig.brightness--;
			else
				theConfig.brightness++;
			if(theConfig.brightness < 0)
				theConfig.brightness = 0;
			else if(theConfig.brightness > 100)
				theConfig.brightness = 100;
			ituTrackBarSetValue(MON_TR_BAR_BRI,theConfig.brightness);
			ituProgressBarSetValue(MON_PR_BAR_BRI,theConfig.brightness);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.brightness+58);
		}
		else
		{
			if(param[0] == '1')
				theConfig.brightness_c--;
			else
				theConfig.brightness_c++;
			if(theConfig.brightness_c < 0)
				theConfig.brightness_c = 0;
			else if(theConfig.brightness_c > 100)
				theConfig.brightness_c = 100;
			ituTrackBarSetValue(MON_TR_BAR_BRI,theConfig.brightness_c);
			ituProgressBarSetValue(MON_PR_BAR_BRI,theConfig.brightness_c);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_BRGT,theConfig.brightness_c+58);
		}	
		break;
	case '3':
	case '4':
		if(cur_page == page_monitor)
		{
			if(param[0] == '3')
				theConfig.contrast--;
			else
				theConfig.contrast++;
			if(theConfig.contrast < 0)
				theConfig.contrast = 0;
			else if(theConfig.contrast > 100)
				theConfig.contrast = 100;
			ituTrackBarSetValue(MON_TR_BAR_CON,theConfig.contrast);
			ituProgressBarSetValue(MON_PR_BAR_CON,theConfig.contrast);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_CONT,theConfig.contrast+98);
		}
		else
		{
			if(param[0] == '3')
				theConfig.contrast_c--;
			else
				theConfig.contrast_c++;
			if(theConfig.contrast_c < 0)
				theConfig.contrast_c = 0;
			else if(theConfig.contrast_c > 100)
				theConfig.contrast = 100;
			ituTrackBarSetValue(MON_TR_BAR_CON,theConfig.contrast_c);
			ituProgressBarSetValue(MON_PR_BAR_CON,theConfig.contrast_c);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_CONT,theConfig.contrast_c+98);
		}
		break;
	case '5':
	case '6':
		if(cur_page == page_monitor)
		{
			if(param[0] == '5')
				theConfig.hue--;
			else
				theConfig.hue++;
			if(theConfig.hue < 0)
				theConfig.hue = 0;
			else if(theConfig.hue > 100)
				theConfig.hue = 100;
			ituTrackBarSetValue(MON_TR_BAR_HUE,theConfig.hue);
			ituProgressBarSetValue(MON_PR_BAR_HUE,theConfig.hue);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_HUE,theConfig.hue+118);
		}
		else
		{
			if(param[0] == '5')
				theConfig.hue_c--;
			else
				theConfig.hue_c++;
			if(theConfig.hue_c < 0)
				theConfig.hue_c = 0;
			else if(theConfig.hue_c > 100)
				theConfig.hue_c = 100;
			ituTrackBarSetValue(MON_TR_BAR_HUE,theConfig.hue_c);
			ituProgressBarSetValue(MON_PR_BAR_HUE,theConfig.hue_c);
			pr2000_i2c_write(0xb8,0xff,0x01);
			pr2000_i2c_write(0xb8,PR2000_HUE,theConfig.hue_c+118);
		}
		break;
	}
	ConfigSave();
	return true;
}

bool monitor_dis_full(ITUWidget* widget, char* param)
{
	if(pop_up[MONITOR_S_FULL] == false)
	{
		pop_up[MONITOR_S_FULL] = true;
		ituWidgetHide(MON_BG_HIDE,0,0);
		ituWidgetHide(MON_HEAD,0,0);
	}
	else
	{
		pop_up[MONITOR_S_FULL] = false;
		ituWidgetShow(MON_BG_HIDE,0,0);
		ituWidgetShow(MON_HEAD,0,0);
	}
	return true;
}

bool monitor_gain_adjust(ITUWidget* widget, char* param)
{
	switch(param[0])
	{
		case '1':
			pr2000_gain(1);
			break;
		case '2':
			pr2000_gain(2);
			break;
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

