#include <assert.h>
#include "user_function.h"

int  add_y[2][4] = {{200,236,272,308},{207,255,303,0}};

static ITUIcon* INTER_ING_ICON_MUTE;

static ITUBackground* INTER_ING_BG_TALK_ING;

static ITUText* INTER_ING_TEXT_CALL_OUT;
static ITUText* INTER_ING_TEXT_TALK_ING;
static ITUText* INTER_ING_TEXT_CALL_IN;

static ITUButton* INTER_ING_BTN_CALL;
static ITUButton* INTER_ING_BTN_HANG_1;
static ITUButton* INTER_ING_BTN_HANG_2;

static ITUText* INTER_ING_TEXT_ID1;
static ITUText* INTER_ING_TEXT_ID2;
static ITUText* INTER_ING_TEXT_ID3;
static ITUText* INTER_ING_TEXT_ID4;
static ITUText* INTER_ING_TEXT_INTER_TIME;
	
static ITUButton* INTER_ING_BTN_RING_VOL_SET;
static ITUButton* INTER_ING_BTN_TALK_VOL_SET;

static ITUBackground* INTER_ING_BG_HIDE;
static ITUBackground* INTER_ING_BG_RING_VOL_BAR;
static ITUBackground* INTER_ING_BG_TALK_VOL_BAR;
static ITUBackground* INTER_ING_BG_VOL_BACK;

static ITUTrackBar* INTER_ING_TR_RING_VOL_BAR;
static ITUProgressBar* INTER_ING_PR_RING_VOL_BAR;
static ITUIcon* INTER_ING_ICON_RING_VOL_1;
static ITUIcon* INTER_ING_ICON_RING_VOL_2;
static ITUIcon* INTER_ING_ICON_RING_VOL_3;
static ITUIcon* INTER_ING_ICON_RING_VOL_4;

static ITUTrackBar* INTER_ING_TR_TALK_VOL_BAR;
static ITUProgressBar* INTER_ING_PR_TALK_VOL_BAR;
static ITUIcon* INTER_ING_ICON_TALK_VOL_1;
static ITUIcon* INTER_ING_ICON_TALK_VOL_2;
static ITUIcon* INTER_ING_ICON_TALK_VOL_3;
static ITUIcon* INTER_ING_ICON_TALK_VOL_4;

static bool interphone_2 = false;				//被动进入(被呼叫)
static bool interphone_call_start = false;
static bool interphone_talk_start = false;
static int interphone_time;	
static uint32_t interphone_start_time;
static bool mute_icon_change = false;	

bool get_interphone_talk_start()
{
	return interphone_talk_start;
}

void inter_time_update(int val)
{
	char time_buf[10] = "\0";
	static ITUText* INTER_ING_TEXT_INTER_TIME_S ;
	INTER_ING_TEXT_INTER_TIME_S	= ituSceneFindWidget(&theScene, "INTER_ING_TEXT_INTER_TIME");
	assert(INTER_ING_TEXT_INTER_TIME_S);
	
	time_buf[0] = '0';
	time_buf[1] = '0';
	time_buf[2] = ':';
	time_buf[3] = val/10+'0';
	time_buf[4] = val%10+'0';
	ituTextSetString(INTER_ING_TEXT_INTER_TIME_S, time_buf);
}

void inter_id_init_s()
{
	int id = uart_get_inter_id();
	ITUText* INTER_ING_TEXT_ID[4];
	
	INTER_ING_TEXT_ID[0] = INTER_ING_TEXT_ID1;
		assert(INTER_ING_TEXT_ID[0]);
	INTER_ING_TEXT_ID[1] = INTER_ING_TEXT_ID2;
		assert(INTER_ING_TEXT_ID[1]);
	INTER_ING_TEXT_ID[2] = INTER_ING_TEXT_ID3;
		assert(INTER_ING_TEXT_ID[2]);
	INTER_ING_TEXT_ID[3] = INTER_ING_TEXT_ID4;
		assert(INTER_ING_TEXT_ID[3]);
		
	ituWidgetSetVisible(INTER_ING_TEXT_ID1,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID2,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID3,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID4,false);
	
	ituWidgetSetVisible(INTER_ING_TEXT_ID[id - 1],true);
	ituWidgetSetY(INTER_ING_TEXT_ID[id - 1],add_y[1][1]);
}

void inter_id_init()
{
	int i,n,m;
	int phone_call[4] = {0};
	ITUText* INTER_ING_TEXT_ID[4];
	
	INTER_ING_TEXT_ID[0] = INTER_ING_TEXT_ID1;
		assert(INTER_ING_TEXT_ID[0]);
	INTER_ING_TEXT_ID[1] = INTER_ING_TEXT_ID2;
		assert(INTER_ING_TEXT_ID[1]);
	INTER_ING_TEXT_ID[2] = INTER_ING_TEXT_ID3;
		assert(INTER_ING_TEXT_ID[2]);
	INTER_ING_TEXT_ID[3] = INTER_ING_TEXT_ID4;
		assert(INTER_ING_TEXT_ID[3]);
	
	ituWidgetSetVisible(INTER_ING_TEXT_ID1,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID2,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID3,false);
	ituWidgetSetVisible(INTER_ING_TEXT_ID4,false);
	n = 0;
	m = 0;
	for(i = 0;i < 4;i++)
	{
		if(interphone_number & (0x1 << i))
		{
			phone_call[i] = 1;
			n++;
		}
		else
			phone_call[i] = 0;
	}
	for(i = 0;i < 4;i++)
	{
		if(phone_call[i])
		{
			ituWidgetSetVisible(INTER_ING_TEXT_ID[i],true);
			if(n == 1)
				ituWidgetSetY(INTER_ING_TEXT_ID[i], add_y[1][1]);
			if(n == 2)
				ituWidgetSetY(INTER_ING_TEXT_ID[i], add_y[0][1+m]);
			if(n == 3)
				ituWidgetSetY(INTER_ING_TEXT_ID[i], add_y[1][m]);
			if(n == 4)
				ituWidgetSetY(INTER_ING_TEXT_ID[i], add_y[0][m]);
			m++;
		}
	}
}

void intercom_ing_itu_init()
{
	ituWidgetSetColor(INTER_ING_TEXT_ID1, 255, 255, 255, 255);
	ituWidgetSetColor(INTER_ING_TEXT_ID2, 255, 255, 255, 255);
	ituWidgetSetColor(INTER_ING_TEXT_ID3, 255, 255, 255, 255);
	ituWidgetSetColor(INTER_ING_TEXT_ID4, 255, 255, 255, 255);
	ituWidgetSetVisible(INTER_ING_TEXT_CALL_OUT, false);
	ituWidgetSetVisible(INTER_ING_TEXT_TALK_ING, false);
	ituWidgetSetVisible(INTER_ING_TEXT_CALL_IN, false);

	if(ituWidgetIsVisible(INTER_ING_BG_RING_VOL_BAR))
	{
		ituWidgetSetVisible(INTER_ING_BG_RING_VOL_BAR, false);
		ituWidgetSetVisible(INTER_ING_BG_VOL_BACK, false);
		ituWidgetSetVisible(INTER_ING_BG_HIDE, true);
	}

	if(ituWidgetIsVisible(INTER_ING_BG_TALK_VOL_BAR))
	{
		ituWidgetSetVisible(INTER_ING_BG_TALK_VOL_BAR, false);
		ituWidgetSetVisible(INTER_ING_BG_VOL_BACK, false);
		ituWidgetSetVisible(INTER_ING_BG_HIDE, true);
	}
	
}

bool intercom_ing_init(ITUWidget* widget, char* param)
{
	cur_page = page_inter_ing;
	if (!INTER_ING_BTN_CALL)
	{
		INTER_ING_ICON_MUTE = ituSceneFindWidget(&theScene, "INTER_ING_ICON_MUTE");
		assert(INTER_ING_ICON_MUTE);
		
		INTER_ING_BG_TALK_ING = ituSceneFindWidget(&theScene, "INTER_ING_BG_TALK_ING");
		assert(INTER_ING_BG_TALK_ING);
		
		INTER_ING_TEXT_CALL_OUT = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_CALL_OUT");
		assert(INTER_ING_TEXT_CALL_OUT);
		INTER_ING_TEXT_TALK_ING = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_TALK_ING");
		assert(INTER_ING_TEXT_TALK_ING);
		INTER_ING_TEXT_CALL_IN = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_CALL_IN");
		assert(INTER_ING_TEXT_CALL_IN);
		
		INTER_ING_BTN_CALL = ituSceneFindWidget(&theScene, "INTER_ING_BTN_CALL");
		assert(INTER_ING_BTN_CALL);
		INTER_ING_BTN_HANG_1 = ituSceneFindWidget(&theScene, "INTER_ING_BTN_HANG_1");
		assert(INTER_ING_BTN_HANG_1);
		INTER_ING_BTN_HANG_2 = ituSceneFindWidget(&theScene, "INTER_ING_BTN_HANG_2");
		assert(INTER_ING_BTN_HANG_2);
		
		INTER_ING_TEXT_ID1 = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_ID1");
		assert(INTER_ING_TEXT_ID1);
		INTER_ING_TEXT_ID2 = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_ID2");
		assert(INTER_ING_TEXT_ID2);
		INTER_ING_TEXT_ID3 = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_ID3");
		assert(INTER_ING_TEXT_ID3);
		INTER_ING_TEXT_ID4 = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_ID4");
		assert(INTER_ING_TEXT_ID4);
		INTER_ING_TEXT_INTER_TIME = ituSceneFindWidget(&theScene, "INTER_ING_TEXT_INTER_TIME");
		assert(INTER_ING_TEXT_INTER_TIME);
	
		INTER_ING_BTN_RING_VOL_SET = ituSceneFindWidget(&theScene, "INTER_ING_BTN_RING_VOL_SET");
		assert(INTER_ING_BTN_RING_VOL_SET);
		INTER_ING_BTN_TALK_VOL_SET = ituSceneFindWidget(&theScene, "INTER_ING_BTN_TALK_VOL_SET");
		assert(INTER_ING_BTN_TALK_VOL_SET);
		
		INTER_ING_BG_HIDE = ituSceneFindWidget(&theScene, "INTER_ING_BG_HIDE");
		assert(INTER_ING_BG_HIDE);
		INTER_ING_BG_RING_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_BG_RING_VOL_BAR");
		assert(INTER_ING_BG_RING_VOL_BAR);
		INTER_ING_BG_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_BG_TALK_VOL_BAR");
		assert(INTER_ING_BG_TALK_VOL_BAR);
		INTER_ING_BG_VOL_BACK = ituSceneFindWidget(&theScene, "INTER_ING_BG_VOL_BACK");
		assert(INTER_ING_BG_VOL_BACK);
		
		INTER_ING_TR_RING_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_TR_RING_VOL_BAR");
		assert(INTER_ING_TR_RING_VOL_BAR);
		INTER_ING_PR_RING_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_PR_RING_VOL_BAR");
		assert(INTER_ING_PR_RING_VOL_BAR);
		INTER_ING_ICON_RING_VOL_1 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_RING_VOL_1");
		assert(INTER_ING_ICON_RING_VOL_1);
		INTER_ING_ICON_RING_VOL_2 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_RING_VOL_2");
		assert(INTER_ING_ICON_RING_VOL_2);
		INTER_ING_ICON_RING_VOL_3 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_RING_VOL_3");
		assert(INTER_ING_ICON_RING_VOL_3);
		INTER_ING_ICON_RING_VOL_4 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_RING_VOL_4");
		assert(INTER_ING_ICON_RING_VOL_4);
		
		INTER_ING_TR_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_TR_TALK_VOL_BAR");
		assert(INTER_ING_TR_TALK_VOL_BAR);
		INTER_ING_PR_TALK_VOL_BAR = ituSceneFindWidget(&theScene, "INTER_ING_PR_TALK_VOL_BAR");
		assert(INTER_ING_PR_TALK_VOL_BAR);
		INTER_ING_ICON_TALK_VOL_1 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_TALK_VOL_1");
		assert(INTER_ING_ICON_TALK_VOL_1);
		INTER_ING_ICON_TALK_VOL_2 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_TALK_VOL_2");
		assert(INTER_ING_ICON_TALK_VOL_2);
		INTER_ING_ICON_TALK_VOL_3 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_TALK_VOL_3");
		assert(INTER_ING_ICON_TALK_VOL_3);
		INTER_ING_ICON_TALK_VOL_4 = ituSceneFindWidget(&theScene, "INTER_ING_ICON_TALK_VOL_4");
		assert(INTER_ING_ICON_TALK_VOL_4);

	}

	 intercom_ing_itu_init();
	 
	ituWidgetSetVisible(INTER_ING_BG_TALK_ING,false);

	if(uart_get_inter_id())
	{
		printf("enter by call......................................\n");
		ituWidgetSetVisible(INTER_ING_BTN_CALL,true);
		ituWidgetSetVisible(INTER_ING_BTN_HANG_1,true);
		ituWidgetSetVisible(INTER_ING_BTN_HANG_2,false);
		ituWidgetSetVisible(INTER_ING_BTN_RING_VOL_SET,true);
		ituWidgetSetVisible(INTER_ING_BTN_TALK_VOL_SET,false);
		ituWidgetSetVisible(INTER_ING_TEXT_CALL_IN, true);
		inter_id_init_s();
		play_inter_ring();
		backlight_on();
		interphone_2 =  true;
	}
	else
	{
		printf("enter by hand......................................\n");
		ituWidgetSetVisible(INTER_ING_BTN_CALL,false);
		ituWidgetSetVisible(INTER_ING_BTN_HANG_1,false);
		ituWidgetSetVisible(INTER_ING_BTN_HANG_2,true);
		ituWidgetSetVisible(INTER_ING_BTN_RING_VOL_SET,false);
		ituWidgetSetVisible(INTER_ING_BTN_TALK_VOL_SET,true);
		ituWidgetSetVisible(INTER_ING_TEXT_CALL_OUT, true);
		inter_id_init();	
		ithGpioSet(INTER_CON_CNT);	
	}	
	
	ithGpioClear(CONV_CONT);
	if(theConfig.Iringvol == 0)
		ituWidgetSetVisible(INTER_ING_ICON_MUTE,true);
	else
		ituWidgetSetVisible(INTER_ING_ICON_MUTE,false);
	
	inter_time_update(30);
	interphone_start_time = SDL_GetTicks();
	interphone_call_start = true;
	interphone_talk_start = false;
	interphone_time = 30;
	
	led_blink_1s_start();
	//standby_mode_reinit();
	return true;
}

bool intercom_quit(ITUWidget* widget, char* param)
{
	uart_set_mode(UART_INTER_QUIT);
	return true;
}

bool intercom_hang(ITUWidget* widget, char* param)
{
	uart_set_mode(UART_INTER_OVER);
	cur_inter_call = false;
	cur_inter_ing = false; 
	uart_clear_busy();
	uart_clear_inter_id();
	uart_clear_inter_link();
	return true;
}


bool intercom_ing_timer(ITUWidget* widget, char* param)
{
	if(event_intercom == 0x2)
	{
		event_intercom = 0;
		ithGpioSet(CONV_CONT);
	}

	if(interphone_call_start || interphone_talk_start)
	{
		uint32_t tick = SDL_GetTicks();
		uint32_t diff = tick - interphone_start_time;
		if(diff >= 1000)
		{
			interphone_start_time = tick;
			interphone_time --;
			if(interphone_time == 0)
			{
				if(interphone_2)
					intercom_quit(widget,param);
				else
					intercom_hang(widget,param);
				ITULayer* PAGE_HOME = ituSceneFindWidget(&theScene, "PAGE_HOME");
				assert(PAGE_HOME);
				ituLayerGoto(PAGE_HOME);
			}
			inter_time_update(interphone_time);
		}
	}

	if(mute_icon_change)
	{
		mute_icon_change = false;
		if(theConfig.Iringvol == 0)
			ituWidgetSetVisible(INTER_ING_ICON_MUTE,true);
		else
			ituWidgetSetVisible(INTER_ING_ICON_MUTE,false);
	}
		
	return true;
}

bool intercom_ing_leave(ITUWidget* widget, char* param)
{
	led_blink_1s_end();
	uart_clear_inter_id();
	AudioStop();
	interphone_mast = false;
	interphone_2 =  false;
	interphone_talk_start = false;
	ithGpioClear(INTER_CON_CNT);	
	//ithGpioSet(RING_POWER);			//打开功放
	user_amp_off();
	ithGpioClear(CONV_CONT);
	ithGpioSet(MIC_NUTE);
	ithGpioClear(AUDIO_IN);	
	return true;
}

void intercom_ing_talk_volume_bar(int val)
{
	if(val == 0)
	{
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_1, true);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_2, true);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_3, false);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_4, false);
	}
	else if(val == 3)
	{
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_1, false);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_2, false);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_3, true);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_4, true);
	}
	else
	{
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_1, false);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_2, true);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_3, true);
		ituWidgetSetVisible(INTER_ING_ICON_TALK_VOL_4, false);
	}
}

void intercom_ing_ring_volume_bar(int val)
{
	if(val == 0)
	{
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_1, true);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_2, true);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_3, false);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_4, false);
	}
	else if(val == 3)
	{
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_1, false);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_2, false);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_3, true);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_4, true);
	}
	else
	{
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_1, false);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_2, true);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_3, true);
		ituWidgetSetVisible(INTER_ING_ICON_RING_VOL_4, false);
	}
}

bool intercom_ing_vol_set(ITUWidget* widget, char* param)
{
	printf("---------------->intercom_ing_vol_set---->%d\n",param[0]-'0');
	switch(param[0])
	{
	case '1':
		intercom_ing_ring_volume_bar(theConfig.Iringvol);
		ituTrackBarSetValue(INTER_ING_TR_RING_VOL_BAR,theConfig.Iringvol);
		ituProgressBarSetValue(INTER_ING_PR_RING_VOL_BAR,theConfig.Iringvol);
		break;
	case '2':
		intercom_ing_talk_volume_bar(theConfig.Italkvol);
		ituTrackBarSetValue(INTER_ING_TR_TALK_VOL_BAR,theConfig.Italkvol);
		ituProgressBarSetValue(INTER_ING_PR_TALK_VOL_BAR,theConfig.Italkvol);
		break;
	}
	return true;
}


bool intercom_ing_adjust(ITUWidget* widget, char* param)
{
	int bar_val;
	printf("---------------->intercom_ing_adjust---->%d\n",param[0]-'0');
	switch(param[0])
	{
		case '1':
			theConfig.Iringvol--;
			if(theConfig.Iringvol < 0)
				theConfig.Iringvol = 0;
			if(theConfig.ringvol == 0 && theConfig.Iringvol == 0)
				theConfig.mute = 1;
			else
				theConfig.mute = 0;
			bar_val = theConfig.Iringvol;
			ring_volume_set(theConfig.Iringvol);
			break;
		case '2':
			theConfig.Iringvol++;
			if(theConfig.Iringvol > 3)
				theConfig.Iringvol = 3;
			user_amp_on();
			theConfig.mute = 0;
			bar_val = theConfig.Iringvol;
			ring_volume_set(theConfig.Iringvol);
		case '3':
			theConfig.Italkvol--;
			if(theConfig.Italkvol < 1)
				theConfig.Italkvol = 1;
			bar_val = theConfig.Italkvol;
			talk_volume_set(theConfig.Italkvol);
			break;
		case '4':
			theConfig.Italkvol++;
			if(theConfig.Italkvol > 3)
				theConfig.Italkvol = 3;
			bar_val = theConfig.Italkvol;
			talk_volume_set(theConfig.Italkvol);
			break;
			break;
	}
		
	intercom_ing_ring_volume_bar(theConfig.Iringvol);
	ituTrackBarSetValue(INTER_ING_TR_RING_VOL_BAR,theConfig.Iringvol);
	ituProgressBarSetValue(INTER_ING_PR_RING_VOL_BAR,theConfig.Iringvol);
	intercom_ing_talk_volume_bar(theConfig.Italkvol);
	ituTrackBarSetValue(INTER_ING_TR_TALK_VOL_BAR,theConfig.Italkvol);
	ituProgressBarSetValue(INTER_ING_PR_TALK_VOL_BAR,theConfig.Italkvol);
	ConfigSave();
	return true;
}

void master_inter_id_change_color()
{
	int id;
	id = uart_get_anser_id();
	if(id == 1)
		ituWidgetSetColor(INTER_ING_TEXT_ID1, 255, 94, 255, 215);
	else if(id == 2)
		ituWidgetSetColor(INTER_ING_TEXT_ID2, 255, 94, 255, 215);
	else if(id == 3)
		ituWidgetSetColor(INTER_ING_TEXT_ID3, 255, 94, 255, 215);
	else if(id == 4)
		ituWidgetSetColor(INTER_ING_TEXT_ID4, 255, 94, 255, 215);
}
void intercom_get_answer()
{
	if(interphone_mast)
	{
		ithGpioSet(AUDIO_IN);	
		
	ithGpioSet(CONV_CONT);
		master_inter_id_change_color();
		ituWidgetSetVisible(INTER_ING_BG_TALK_ING,true);
		ituWidgetSetVisible(INTER_ING_TEXT_CALL_OUT, false);
		ituWidgetSetVisible(INTER_ING_TEXT_TALK_ING, true);
	}
		
	if(interphone_talk_start || (!interphone_2))
	{
		user_amp_on();
		led_blink_1s_end();
		inter_time_update(60);
		interphone_start_time = SDL_GetTicks();
		interphone_call_start = false;
		interphone_talk_start = true;
		interphone_time = 60;
	}
}

void inter_id_change_color()
{
	int id;
	id = uart_get_inter_id();
	if(id == 1)
		ituWidgetSetColor(INTER_ING_TEXT_ID1, 255, 94, 255, 215);
	else if(id == 2)
		ituWidgetSetColor(INTER_ING_TEXT_ID2, 255, 94, 255, 215);
	else if(id == 3)
		ituWidgetSetColor(INTER_ING_TEXT_ID3, 255, 94, 255, 215);
	else if(id == 4)
		ituWidgetSetColor(INTER_ING_TEXT_ID4, 255, 94, 255, 215);
}

bool intercom_answer(ITUWidget* widget, char* param)
{
	led_blink_1s_end();
	AudioStop();
	uart_set_mode(UART_INTER_ANSWER);
	inter_id_change_color();
	ituWidgetSetVisible(INTER_ING_BG_TALK_ING,true);
	ituWidgetSetVisible(INTER_ING_TEXT_CALL_IN, false);
	ituWidgetSetVisible(INTER_ING_TEXT_TALK_ING, true);
	ituWidgetSetVisible(INTER_ING_BTN_CALL,false);
	ituWidgetSetVisible(INTER_ING_BTN_HANG_1,false);
	ituWidgetSetVisible(INTER_ING_BTN_HANG_2,true);
	ituWidgetSetVisible(INTER_ING_BTN_RING_VOL_SET,false);
	ituWidgetSetVisible(INTER_ING_BTN_TALK_VOL_SET,true);
	talk_volume_set(theConfig.Italkvol);
	ithGpioSet(INTER_CON_CNT);	
	user_amp_on();
	ithGpioSet(CONV_CONT);
	ithGpioClear(MIC_NUTE);
	ithGpioSet(AUDIO_IN);	
	inter_time_update(60);
	interphone_start_time = SDL_GetTicks();
	interphone_call_start = false;
	interphone_talk_start = true;
	interphone_time = 60;
	return true;
}
