#include "user_function.h"
#include <assert.h>
#include <fat/Api_f.h>

bool pr2000_test = false;

//
bool master_vdp= false;
bool montion_enable = false;
int montion_end_event = 0;
int montion_event_cmd = 0;
uint8_t door_call_num = 0;
bool need_check_video = false;

bool boot_logo  = false;
bool busy_over_3s = false;
bool video_first = false;


bool call_ring_playing = false;
bool format_ing = false;
bool format_over = false;
bool delete_ing = false;
bool delete_over = false;
bool set_display_signal = false;

bool door_is_call = false;
bool door_call_out = false;
bool door_talk_out = false;
bool standby_state = false;

uint8_t ring_play = 0; 


//当前状态标志
int pre_page = 0;
int cur_page = 0;						//当前页面
int pre_mon_state = 0;				//当前监控状态
int cur_mon_state = 0;				//当前监控状态
int cur_call_ing = 0;					
int cur_signal = 0;					//当前信号通道
bool cur_wind = false;					//当前信号通道
bool pre_open = false;					//开锁标志
bool cur_open = false;					//当前开锁标志
uint8_t cam_detecting = 0;				//户外机检测状
bool cam_detect_over = false;				//户外机检测状
bool signal_insert[4] = {false};			//door 1 接入状态
//bool door_1_insert = false;				//door 1 接入状态
//bool door_2_insert = false;				//door 2 接入状态
//bool cctv_1_insert = false;				//cctv 1 接入状态
//bool cctv_2_insert = false;				//cctv 2 接入状态
uint8_t offer_signal_channel = 0;		//当前提供的信号
uint8_t use_signal_channel = 0;		//当前使用的信号
bool cur_signal_call = false;			//当前显示信号的状态
bool signal_channel_busy = false;		//当前信号通道忙
bool interphone_mast = false;
uint8_t interphone_number = 0;		//interphone 号码
bool cur_inter_call = false;				//interphone 状态
uint8_t cur_inter_times = 3;			//interphone 状态
bool cur_inter_ing = false;				//interphone状态
bool cur_talk_ing = false;				//通话状态
bool other_talk_ing = false;				//通话状态
int other_talk_id = 0;					//通话状态
uint8_t cur_wash_mode = 0;				//通话状态
bool sd_state_change = false;			//SD卡状态变化

	
//事件标志
int event_call = 0;					//call 机事件
int event_call_s = 0;					//call 机事件
int event_intercom = 0;				//intercom 事件
int event_uart = 0;					//串口 事件
uint8_t event_home_go = 0;			//主界面切换事件
bool event_go_home = false;				//切换到主界面

//定时器事件
bool call_is_end = false;				

//逻辑标志
int show_snap_rec_icon = 0;				//显示拍照/录像图标
unsigned long rec_start_time = 0;			//更新录像时间
int temp_rec_time = 60;					//录像时间累计值
bool pop_up[POP_UP_TOTAL] ={false};		//弹窗标志
uint8_t once_key_ring = 0;					//跳过一次按键音
bool set_to_time_set_flag = false;	
bool auto_snap_filename = false;
bool led_blink_enable = false;
bool led_blink_ing = false;

bool auto_rec_start = false;

static bool isSnapRecIconDisplay = false;

bool get_call_ring_play()
{
	return call_ring_playing;
}

void clear_call_ring_play()
{
	call_ring_playing = false;
	ithGpioSet(DOOR_RING);
}

uint8_t get_once_key_ring()
{
	return once_key_ring;
}
void clear_once_key_ring()
{
	once_key_ring--;
}
int cam_gpio[signal_total][2] =  {{DOOR_1_ON,DOOR_1_CALL},
							{DOOR_2_ON,DOOR_2_CALL},
							{CCTV_1_ON,0},
							{CCTV_2_ON,0}
							};

uint32_t test_tick;
static char ring_str[6][25] 		= {"A:/sounds/sound1.mp3",
								"A:/sounds/sound2.mp3",
								"A:/sounds/sound3.mp3",
								"A:/sounds/sound4.mp3",
								"A:/sounds/sound5.mp3",
								"A:/sounds/sound6.mp3"};

#define AVDD_COUNT			37		//图像
#define DVDD_COUNT			38		//图像
#define BL_EN_PWM			64		//背光

#if defined(TARGET_BOARD_G)

#if defined(TARGET_BOARD_G_V03)
#define CAM_VOL_PIN  	63//32
#define BL_VOL_PIN  	36//34
#define BL_GPIO_PIN  	64//35

#define PR2000_MPP3 	72//73
#define PR2000_MPP4 	73//72
#else
#define CAM_VOL_PIN  	32
#define BL_VOL_PIN  	34
#define BL_GPIO_PIN  	35

#define PR2000_MPP3 	73
#define PR2000_MPP4 	72
#endif

void user_gpio_init()
{
	ithGpioSetMode(BL_GPIO_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_GPIO_PIN);
	ithGpioEnable(BL_GPIO_PIN); 
	ithGpioSet(BL_GPIO_PIN);	
	
	ithGpioSetMode(BL_VOL_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_VOL_PIN);
	ithGpioEnable(BL_VOL_PIN);			
	ithGpioSet(BL_VOL_PIN);
	
	ithGpioSetMode(CAM_VOL_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(CAM_VOL_PIN);
	ithGpioEnable(CAM_VOL_PIN); 
	ithGpioSet(CAM_VOL_PIN);	

#if 0
	ithGpioSetMode(PR2000_MPP3, ITH_GPIO_MODE0);
	ithGpioSetOut(PR2000_MPP3);
	ithGpioEnable(PR2000_MPP3); 
	ithGpioClear(PR2000_MPP3);	

	ithGpioSetMode(PR2000_MPP4, ITH_GPIO_MODE0);
	ithGpioSetOut(PR2000_MPP4);
	ithGpioEnable(PR2000_MPP4); 
	ithGpioClear(PR2000_MPP4);		
#endif	
}

#else

void user_gpio_init()					//GPIO 初始化
{
	/*
	ithGpioSetMode(DVDD_COUNT, ITH_GPIO_MODE0);
	ithGpioSetOut(DVDD_COUNT);	
	ithGpioSet(DVDD_COUNT);	
	ithGpioEnable(DVDD_COUNT);	
*/	
	ithGpioSetMode(AVDD_COUNT, ITH_GPIO_MODE0);
	ithGpioSetOut(AVDD_COUNT);	
	ithGpioSet(AVDD_COUNT);	
	ithGpioEnable(AVDD_COUNT);	
	
	ithGpioSetMode(BL_EN_PWM, ITH_GPIO_MODE0);
	ithGpioSetOut(BL_EN_PWM);
	ithGpioSet(BL_EN_PWM);	
	ithGpioEnable(BL_EN_PWM);	

	ithGpioSetMode(DOOR_RING, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_RING);
	ithGpioSet(DOOR_RING);	
	ithGpioEnable(DOOR_RING);
	
//	4066 声音控制开关------------------------------
	ithGpioSetMode(DOOR_1_AUDIO, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_1_AUDIO);
	ithGpioClear(DOOR_1_AUDIO);	
	ithGpioEnable(DOOR_1_AUDIO);

	ithGpioSetMode(DOOR_2_AUDIO, ITH_GPIO_MODE0);
	ithGpioSetOut(DOOR_2_AUDIO);
	ithGpioClear(DOOR_2_AUDIO);	
	ithGpioEnable(DOOR_2_AUDIO);
	
	ithGpioSetMode(AUDIO_IN, ITH_GPIO_MODE0);
	ithGpioSetOut(AUDIO_IN);
	ithGpioClear(AUDIO_IN);	
	ithGpioEnable(AUDIO_IN);
	
	ithGpioSetMode(AUDIO_OUT, ITH_GPIO_MODE0);
	ithGpioSetOut(AUDIO_OUT);
	ithGpioSet(AUDIO_OUT);	
	ithGpioEnable(AUDIO_OUT);
//--------------------------------------------------------	
	ithGpioSetMode(CONV_CONT, ITH_GPIO_MODE0);
	ithGpioSetOut(CONV_CONT);
	ithGpioClear(CONV_CONT);	
	ithGpioEnable(CONV_CONT);
	
	ithGpioSetMode(MIC_NUTE, ITH_GPIO_MODE0);
	ithGpioSetOut(MIC_NUTE);
	ithGpioSet(MIC_NUTE);	
	ithGpioEnable(MIC_NUTE);

	ithGpioSetMode(RING_OPEN, ITH_GPIO_MODE0);
	ithGpioSetOut(RING_OPEN);
	ithGpioSet(RING_OPEN);	
	ithGpioEnable(RING_OPEN);
	
	ithGpioSetMode(RING_POWER, ITH_GPIO_MODE0);
	ithGpioSetOut(RING_POWER);
	ithGpioClear(RING_POWER);	
	ithGpioEnable(RING_POWER);
	
	ithGpioSetMode(RING_VOL_1, ITH_GPIO_MODE0);
	ithGpioSetOut(RING_VOL_1);
	ithGpioSet(RING_VOL_1);	
	ithGpioEnable(RING_VOL_1);
	
	ithGpioSetMode(RING_VOL_2, ITH_GPIO_MODE0);
	ithGpioSetOut(RING_VOL_2);
	ithGpioSet(RING_VOL_2);	
	ithGpioEnable(RING_VOL_2);
	
	ithGpioSetMode(TALK_VOL_1, ITH_GPIO_MODE0);
	ithGpioSetOut(TALK_VOL_1);
	ithGpioSet(TALK_VOL_1);	
	ithGpioEnable(TALK_VOL_1);
	
	ithGpioSetMode(TALK_VOL_2, ITH_GPIO_MODE0);
	ithGpioSetOut(TALK_VOL_2);
	ithGpioSet(TALK_VOL_2);	
	ithGpioEnable(TALK_VOL_2);
	
	ithGpioSetMode(69, ITH_GPIO_MODE0);
	ithGpioSetOut(69);
	ithGpioSet(69);	
	ithGpioEnable(69);
	
	ithGpioSetMode(LED_CONT, ITH_GPIO_MODE0);
	ithGpioSetOut(LED_CONT);
	ithGpioSet(LED_CONT);	
	ithGpioEnable(LED_CONT);
	
	ithGpioSetMode(INTER_CON_CNT, ITH_GPIO_MODE0);
	ithGpioSetOut(INTER_CON_CNT);
	ithGpioClear(INTER_CON_CNT);	
	ithGpioEnable(INTER_CON_CNT);
	
}
#endif

void pop_up_init()
{
	int i;
	
	for(i = 0; i < POP_UP_TOTAL; i++)
	{
		pop_up[i] = false;
	}
}

void cam_all_off()						//关闭所有摄像头电源
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
}

#if defined(TARGET_BOARD_G)
#if defined(TARGET_BOARD_G_V03)
#define CAM_SWITCH_PIN  	22
#else
#define CAM_SWITCH_PIN  	63
#endif
int currCam = 0;

void UserCameraSwitch(int iCamNum)
{
	ithGpioSetMode(CAM_SWITCH_PIN, ITH_GPIO_MODE0);
	ithGpioSetOut(CAM_SWITCH_PIN);
	ithGpioEnable(CAM_SWITCH_PIN); 

	printf("cam_switch: to %d\r\n", iCamNum);
	if(iCamNum == 0)
		ithGpioSet(CAM_SWITCH_PIN);	
	else if(iCamNum == 1)
		ithGpioClear(CAM_SWITCH_PIN);	
}
#endif

void call_master_v6502(uint8_t offset)
{
	uint8_t addr = 0x00;
	uint8_t addr_1 = 0x02;
	uint8_t offset_1 = offset<<4;

	usleep(10*1000);
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &offset, 1);
	usleep(10*1000);
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr_1, &offset_1, 1);
	usleep(10*1000);
}

void call_master_cam(int val)
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	if(1 == val)
		ithGpioClear(cam_gpio[DOOR_1][ON]);
	else if(2 == val)
		ithGpioClear(cam_gpio[DOOR_2][ON]);

}

void call_master_voice(int val)
{
	ithGpioClear(DOOR_1_AUDIO);
	ithGpioClear(DOOR_2_AUDIO);
	if(val == 1)
		ithGpioSet(DOOR_1_AUDIO);
	else if(val == 2)
		ithGpioSet(DOOR_2_AUDIO);
}

void call_slave_v6502(uint8_t offset)
{
	uint8_t addr = 0x00;
	uint8_t val = 0x05;
	uint8_t addr_1 = 0x02;
	uint8_t val_1 = 0x50;
	
	usleep(10*1000);
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
	usleep(10*1000);
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr_1, &val_1, 1);
	usleep(10*1000);
}

void call_slave_cam(int val)
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	if(1 == val)
		ithGpioClear(cam_gpio[DOOR_1][ON]);
	else if(2 == val)
		ithGpioClear(cam_gpio[DOOR_2][ON]);

}

void call_slave_voice(int val)
{
	ithGpioClear(DOOR_1_AUDIO);
	ithGpioClear(DOOR_2_AUDIO);
	if(val == 1)
		ithGpioSet(DOOR_1_AUDIO);
	else if(val == 2)
		ithGpioSet(DOOR_2_AUDIO);
}
void call_signal(int val)				//CALL 机显示图像切换
{
	if(master_vdp)
	{
		call_master_v6502(val);
		call_master_cam(val);
		call_master_voice(val);
	}
	else
		call_slave_v6502(val);
}

void call_quit()
{
	printf("call_quit...........................................\n");
	if(master_vdp)
	{
		if(!get_monitor_time())
		{
			door_call_num = 0;
			ithGpioSet(cam_gpio[DOOR_1][ON]);
			ithGpioSet(cam_gpio[DOOR_2][ON]);
			//door_call_reinit();
		}
	}
	led_blink_1s_end();
}

void c_talk_quit()
{
	printf("c_talk_quit...........................................\n");
	//door_talk_reinit();			
	if(master_vdp)
	{
		ithGpioSet(cam_gpio[DOOR_1][ON]);
		ithGpioSet(cam_gpio[DOOR_2][ON]);
	}
	cur_talk_ing = 0;
	door_call_num = 0;
	user_amp_off();
	ithGpioClear(CONV_CONT);
	ithGpioSet(MIC_NUTE);
	ithGpioClear(AUDIO_IN);	
}

void mon_quit()
{
	printf("mon_quit...........................................\n");
	if(master_vdp)
	{
		ithGpioSet(cam_gpio[DOOR_1][ON]);
		ithGpioSet(cam_gpio[DOOR_2][ON]);
	}
	cur_talk_ing = 0;
	//monitor_reinit();
}


void md_quit()
{
	printf("md_quit...........................................\n");
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	usleep(500*1000);
	UserTimerMotionBeginReinit();
	UserTimerMotionSnapReinit();
	ithGpioSet(AUDIO_OUT);
	montion_enable = false;
}

void m_talk_quit()
{
	printf("m_talk_quit...........................................\n");
	if(master_vdp)
	{
		ithGpioSet(cam_gpio[DOOR_1][ON]);
		ithGpioSet(cam_gpio[DOOR_2][ON]);
	}
	cur_talk_ing = 0;
	//monitor_reinit();
	user_amp_off();
	ithGpioClear(CONV_CONT);
	ithGpioSet(MIC_NUTE);
	ithGpioClear(AUDIO_IN);	
}

void monitor_signal(int val)				//CALL 机显示图像切换
{
	if(master_vdp)
	{
		call_master_v6502(val);
		call_master_cam(val);
		call_master_voice(val);
	}
	else
		call_slave_v6502(val);
}

void cctv_signal(int val)				//CALL 机显示图像切换
{
	if(master_vdp)
		call_master_v6502(val);
	else
		call_slave_v6502(val);
}

void master_offer_v6502(uint8_t offset)			
{
	uint8_t addr = 0x02;
	uint8_t val = offset<<4;
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
}

void master_offer_signal(uint8_t val)
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	if(val == 1 && signal_insert[DOOR_1])
	{
		ithGpioClear(cam_gpio[DOOR_1][ON]);
		ithGpioSet(DOOR_1_AUDIO);
		ithGpioClear(DOOR_2_AUDIO);
	}
	else if(val == 2 && signal_insert[DOOR_2])
	{
		ithGpioClear(cam_gpio[DOOR_2][ON]);
		ithGpioClear(DOOR_1_AUDIO);
		ithGpioSet(DOOR_2_AUDIO);
	}
	master_offer_v6502(val);
	//uart_set_busy();
}

void master_offer_ctalk_signal(uint8_t val)
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	if(val == 1)
	{
		ithGpioClear(cam_gpio[DOOR_1][ON]);
		ithGpioSet(DOOR_1_AUDIO);
		ithGpioClear(DOOR_2_AUDIO);
	}
	else if(val == 2)
	{
		ithGpioClear(cam_gpio[DOOR_2][ON]);
		ithGpioClear(DOOR_1_AUDIO);
		ithGpioSet(DOOR_2_AUDIO);
	}
	master_offer_v6502(val);
	//uart_set_busy();
}
static int play_call_ring_back(int state)
{
	switch (state)
	{
		case 0://AUDIOMGR_STATE_CALLBACK_PLAYING_FINISH:
			printf("call ring over /..................................\n");
			if(get_auto_rec_once())
			{
				clear_auto_rec_once();
				auto_rec_start = true;
			}
			call_ring_playing = false;
			ithGpioSet(DOOR_RING);	
			if(!cur_talk_ing)
				user_amp_off();
			//talk_volume_set(theConfig.talkvol);
			if(theConfig.mute || (theConfig.ringvol == 0))
				ithGpioSet(RING_POWER);
			break;
	}
	return 0;
}

void play_call_ring(bool same)					//播放铃声
{
	if((cur_talk_ing && same) || rec_start_time)
	{
		clear_play_call_ring_same();
		return;
	}
	ring_play = 2;
}

static int play_inter_ring_back(int state)
{
	switch (state)
	{
		case 0://AUDIOMGR_STATE_CALLBACK_PLAYING_FINISH:
			if(theConfig.mute || (theConfig.Iringvol == 0))
				user_amp_on();
			break;
	}
	return 0;
}

void play_inter_ring()					//播放铃声
{
	if(theConfig.mute || (theConfig.Iringvol == 0))
		user_amp_off();
	else
		user_amp_on();
	AudioStop();
	AudioSetVolume(ALC5616_VOL);		
	ring_volume_set(theConfig.Iringvol);
	AudioPlay(ring_str[theConfig.interphone], play_inter_ring_back);
}

static int play_open_back(int state)
{
	switch (state)
	{
		case 0:
			if(!cur_talk_ing)
				user_amp_off();
			break;
	}
	return 0;
}

static int gpioDoor[] = {DOOR_1_OPEN, DOOR_2_OPEN};

void user_open_door(int num)					//开锁
{
	if(num < 2)
		ithGpioClear(gpioDoor[num]);
	cur_open = true;
}


static int play_inter_Key_back(int state)
{
	switch (state)
	{
		case 0:
			user_amp_off();
			break;
	}
	return 0;
}

void _user_ring_play()
{
	while(1)
	{
		usleep(100*1000);
		if(ring_play)
		{
			if(get_avi_start_flag())
				continue;
			AudioSetVolume(ALC5616_VOL);		//声音初始化
			switch(ring_play)
			{
				case 1:					//按键音
					AudioPlay( CFG_PRIVATE_DRIVE ":/sounds/key1.wav", play_inter_Key_back);
					break;
				case 2:					//DOOR CALL 铃声
					if(theConfig.mute || (theConfig.ringvol == 0))
						user_amp_off();
					else
						user_amp_on();
					if(master_vdp)
						ithGpioClear(DOOR_RING);	
					call_ring_playing = true;
					if(cur_talk_ing)
						ring_volume_set(1);
					else
						ring_volume_set(theConfig.ringvol);
					if(cur_signal == 1)
						AudioPlay(ring_str[theConfig.door1], play_call_ring_back);
					else if(cur_signal == 2)
						AudioPlay(ring_str[theConfig.door2], play_call_ring_back);
					break;
				case 3:					//Interphone 铃声
					ring_volume_set(theConfig.Iringvol);
					AudioPlay(ring_str[theConfig.interphone], play_inter_ring_back);
					break;
				case 4:					//开锁铃声
					ring_volume_set(theConfig.ringvol);
					AudioPlay("A:/sounds/open.mp3", NULL);
					break;
			}
			ring_play = 0;
		}
	}
}

void user_ring_play()
{
	pthread_t task;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, _user_ring_play, NULL);
}

void snap_rec_state()
{
	if(show_snap_rec_icon)
	{
		if(show_snap_rec_icon == sanp_display)
		{
			show_snap_rec_icon = icon_displaying;
			ITUButton* monitorSnapButton = ituSceneFindWidget(&theScene, "monitorSnapButton");
			assert(monitorSnapButton);
			ITUIcon* monitorSnapIcon = ituSceneFindWidget(&theScene, "monitorSnapIcon");
			assert(monitorSnapIcon);
			ituWidgetSetVisible(monitorSnapButton,false);
			ituWidgetSetVisible(monitorSnapIcon,true);
			
		}
		else if(show_snap_rec_icon == rec_display)
		{
			show_snap_rec_icon = icon_displaying;
			ITUButton* monitorRecordButton = ituSceneFindWidget(&theScene, "monitorRecordButton");
			assert(monitorRecordButton);
			ITUButton* monitorRecordingButton = ituSceneFindWidget(&theScene, "monitorRecordingButton");
			assert(monitorRecordingButton);
			//ITUText* monitorRecTimeText = ituSceneFindWidget(&theScene, "monitorRecTimeText");
			//assert(monitorRecTimeText);
			ituWidgetSetVisible(monitorRecordButton,false);
			ituWidgetSetVisible(monitorRecordingButton,true);
			//ituWidgetSetVisible(monitorRecTimeText,true);
			//ituTextSetString(monitorRecTimeText, "01:00");
		}
		else if(show_snap_rec_icon == icon_displaying)
		{
			isSnapRecIconDisplay = true;
		}
		else if(isSnapRecIconDisplay) //just need to clear icon one time
		{
			isSnapRecIconDisplay = false;
			temp_rec_time = 60;
			ITUButton* monitorRecordButton = ituSceneFindWidget(&theScene, "monitorRecordButton");
			assert(monitorRecordButton);
			ITUButton* monitorRecordingButton = ituSceneFindWidget(&theScene, "monitorRecordingButton");
			assert(monitorRecordingButton);
			//ITUText* monitorRecTimeText = ituSceneFindWidget(&theScene, "monitorRecTimeText");
			//assert(monitorRecTimeText);
			ituWidgetSetVisible(monitorRecordButton,true);
			ituWidgetSetVisible(monitorRecordingButton,false);
			//ituWidgetSetVisible(monitorRecTimeText,false);
			ITUButton* monitorSnapButton = ituSceneFindWidget(&theScene, "monitorSnapButton");
			assert(monitorSnapButton);
			ITUIcon* monitorSnapIcon = ituSceneFindWidget(&theScene, "monitorSnapIcon");
			assert(monitorSnapIcon);
			if(ituWidgetIsVisible(monitorSnapIcon))
			{
				clear_mon_rec_ing();
				ituWidgetSetVisible(monitorSnapButton,true);
				ituWidgetSetVisible(monitorSnapIcon,false);
			}
		}
	}
}

void event_process()					//事件处理
{
	snap_rec_state();						//显示拍照/录像图标
}

void _user_cam_detect()
{
	uint8_t val = 0x08;
	int i;
	if(theConfig.id)
	{
		cam_detect_over = true;
		return;
	}
	while(1)
	{
		if(pr2000_is_ok)
			break;
		usleep(10);
	}
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	for(i =1;i<5;i++)
	{
		cam_detecting = i;
		mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, 0x00, &cam_detecting, 2);
		if(i == 1)
			ithGpioClear(cam_gpio[DOOR_1][ON]);
		else if(i == 2)
			ithGpioClear(cam_gpio[DOOR_2][ON]);
		usleep(500*1000);
		if(PR2000K_Check_signal())
		{
			if(cam_detecting == 1)
				signal_insert[DOOR_1] = true;
			else if(cam_detecting == 2)
				signal_insert[DOOR_2] = true;
			else if(cam_detecting == 3)
				signal_insert[CCTV_1] = true;
			else if(cam_detecting == 4)
				signal_insert[CCTV_2] = true;
		}
		usleep(10*1000);
			
	}
	if(signal_insert[DOOR_1] || signal_insert[DOOR_2] || signal_insert[CCTV_1] || signal_insert[CCTV_2])
		master_vdp = true;
	cam_all_off();
	cam_detecting = 0;
	cam_detect_over = true;
}

void user_cam_detect()
{
	pthread_t cam_det_p;			//线程ID
	pthread_attr_t cam_det_pat;
	pthread_attr_init(&cam_det_pat);
	pthread_attr_setdetachstate(&cam_det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&cam_det_p,&cam_det_pat, _user_cam_detect, NULL);
}


user_time_init()
{
	if(theConfig.firstboot)
	//if(get_rtc_reset() || theConfig.firstboot)//my.wei mask because G doesn't have rtc
	{
		printf("rtc_reset------------------------------------->\n");
		struct timeval tv;
		tv.tv_sec = CFG_RTC_DEFAULT_TIMESTAMP;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);
		//clear_rtc_reset(); //my.wei mask because G doesn't have rtc
		theConfig.firstboot = 0;
		ConfigSave();
	}
}

void ring_volume_set(uint8_t val)
{
	printf("ring_volume_set................%d\n",val);
	if(val == 0)
	{
		ithGpioClear(RING_VOL_1);
		ithGpioClear(RING_VOL_2);
	}
	else if(val == 1)
	{
		ithGpioSet(RING_VOL_1);
		ithGpioClear(RING_VOL_2);
	}
	else if(val == 2)
	{
		ithGpioClear(RING_VOL_1);
		ithGpioSet(RING_VOL_2);
	}
	else if(val == 3)
	{
		ithGpioSet(RING_VOL_1);
		ithGpioSet(RING_VOL_2);
	}
}

void talk_volume_set(uint8_t val)
{
	printf("talk_volume_set................%d\n",val);
	if(val == 0)
	{
		ithGpioClear(TALK_VOL_1);
		ithGpioClear(TALK_VOL_2);
	}
	else if(val == 1)
	{
		ithGpioSet(TALK_VOL_1);
		ithGpioClear(TALK_VOL_2);
	}
	else if(val == 2)
	{
		ithGpioClear(TALK_VOL_1);
		ithGpioSet(TALK_VOL_2);
	}
	else if(val == 3)
	{
		ithGpioSet(TALK_VOL_1);
		ithGpioSet(TALK_VOL_2);
	}
}

static float screenSaverCountDown;
static uint32_t screenSaverLastTick;
static bool screenOff = false;

void ScreenInit(void)
{
    screenSaverLastTick = SDL_GetTicks();
    screenSaverCountDown = theConfig.screensaver_time * 60.0f;
    screenOff = false;
}

void ScreenOn()
{
	ithGpioSet(BL_EN_PWM);
	screenOff = false;
}

void ScreenOff()
{
	ithGpioClear(BL_EN_PWM);
	screenOff = true;
}

bool ScreenIsOff()
{
	return screenOff;
}

void ScreenSaverRefresh(void)
{	
    screenSaverLastTick = SDL_GetTicks();
    screenSaverCountDown = theConfig.screensaver_time * 60.0f;

    if (screenOff && theConfig.screensaver_type == SCREENSAVER_BLANK)
    {
        ScreenOn();
    }
}

int ScreenSaverCheck(void)
{
    uint32_t diff, tick = SDL_GetTicks();

    if (tick >= screenSaverLastTick)
    {
        diff = tick - screenSaverLastTick;
    }
    else
    {
        diff = 0xFFFFFFFF - screenSaverLastTick + tick;
    }

    //printf("ScreenSaverCheck: tick: %d diff: %d countdown: %d\n", tick, diff, (int)screenSaverCountDown);

    if (diff >= 1000)
    {
        screenSaverCountDown -= (float)diff / 1000.0f;
        screenSaverLastTick = tick;

        if (screenSaverCountDown <= 0.0f)
            return -1;
    }
    return 0;
}

bool ScreenSaverIsScreenSaving(void)
{
    return screenOff || (screenSaverCountDown <= 0.0f);
}

#define IMAGEMEMO_PATH  	"E:/photo"
#define IMAGEMEMO_PATH_F	"C:/photo"
#define VIDEOMEMO_PATH 	"E:/video"
#define BACKUP_PATH 		"E:/backup"

void user_format_sd()
{
  	ITPPartition par;
	int ret;
	
	format_ing = true;
	par.disk = ITP_DISK_SD0;
	ioctl(ITP_DEVICE_FAT, ITP_IOCTL_GET_PARTITION, &par);
	ioctl(ITP_DEVICE_FAT, ITP_IOCTL_UNMOUNT, (void*)ITP_DISK_SD0);
	ioctl(ITP_DEVICE_FAT, ITP_IOCTL_CREATE_PARTITION, &par);
	ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORCE_MOUNT, (void*)ITP_DISK_SD0);
	usleep(200*1000);
	ret = ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORMAT, (void*)4);
	if (ret)
		printf("sd card format fail..............\n");
	else
	{
		printf("sd card format success..............\n");
		f_mkdir(IMAGEMEMO_PATH);
		f_mkdir(VIDEOMEMO_PATH);
		f_mkdir(BACKUP_PATH);
	}
	image_memo_init();
	video_memo_init();
	format_ing = false;
	format_over = true;
}


void user_format_init()
{
	pthread_t format_det_p;			//线程ID
	pthread_attr_t format_det_pat;
	pthread_attr_init(&format_det_pat);
	pthread_attr_setdetachstate(&format_det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&format_det_p,&format_det_pat, user_format_sd, NULL);
}

void user_delete_photo_dir()
{
	printf("user_delete_photo_dir..............\n");
	if(StorageGetCurrType() == STORAGE_SD)
	{
		remove_dir(IMAGEMEMO_PATH);
		f_mkdir(IMAGEMEMO_PATH);
	}
	else
	{
		remove_dir(IMAGEMEMO_PATH_F);
		f_mkdir(IMAGEMEMO_PATH_F);
	}
}

void user_delete_video_dir()
{
	printf("user_delete_video_dir..............\n");
	if(StorageGetCurrType() == STORAGE_SD)
	{
		video_delete_all();
		//remove_dir(VIDEOMEMO_PATH);
		f_mkdir(VIDEOMEMO_PATH);
	}
}

void user_delete_file()
{
	ITUCheckBox* DEL_CHBOX_1;
	ITUCheckBox* DEL_CHBOX_2;
	
	delete_ing = true;
	DEL_CHBOX_1 = ituSceneFindWidget(&theScene, "DEL_CHBOX_1");
	assert(DEL_CHBOX_1);
	DEL_CHBOX_2 = ituSceneFindWidget(&theScene, "DEL_CHBOX_2");
	assert(DEL_CHBOX_2);
	
	if(DEL_CHBOX_1->checked)
		user_delete_photo_dir();
	if(DEL_CHBOX_2->checked)
		user_delete_video_dir();
	image_memo_init();
	video_memo_init();
	delete_ing = false;
	delete_over = true;

}

void user_delete_init()
{
	pthread_t delete_det_p;			//线程ID
	pthread_attr_t delete_det_pat;
	pthread_attr_init(&delete_det_pat);
	pthread_attr_setdetachstate(&delete_det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&delete_det_p,&delete_det_pat, user_delete_file, NULL);
}

void rand_1_sec_delay()
{
	unsigned temp_rand = 0;
	srand((unsigned)time(NULL));
	temp_rand = (rand()%5+1)*100*1000;
	usleep(temp_rand);
}

void user_amp_on()
{
	printf("----------------->AMP_ON\n");
	ithGpioSet(RING_POWER);			//打开功放
}

void user_amp_off()
{
	printf("----------------->AMP_OFF\n");
	ithGpioClear(RING_POWER);		//关闭功放
}

void _user_auto_init()
{
    struct statvfs info;
	while(1)
	{
		usleep(20*1000);
		if(auto_rec_start)
		{
			if(theConfig.zidong == 1 && (StorageGetCurrType() == STORAGE_SD))
			{
				auto_snap_filename = true;
				set_mon_rec_ing();
				AudioResumeKeySound();		//打开按键音
				if (statvfs("E:/", &info) == 0)
				{
					uint64_t avail = (uint64_t)info.f_bfree * info.f_bsize /1024 /1024;
					if(avail < 200)
						user_snap(1);
					else
						user_snap(2);
				}
			}
			else
			{
				auto_snap_filename = true;
				set_mon_rec_ing();
				AudioResumeKeySound();		//打开按键音
				user_snap(1);
			}
			auto_rec_start = false;
		}
	}
}

void user_auto_init()
{
	pthread_t auto_det_p;			//线程ID
	pthread_attr_t auto_det_pat;
	pthread_attr_init(&auto_det_pat);
	pthread_attr_setdetachstate(&auto_det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&auto_det_p,&auto_det_pat, _user_auto_init, NULL);
}

void flash_file_init()
{
	if(theConfig.firstboot)
	{
		if(opendir("C:/photo") == NULL)
			f_mkdir("C:/photo");
	}
}

void led_blink()
{
	led_blink_ing = true;
	led_blink_enable = true;
	
	while(led_blink_enable)
	{
		if(!led_blink_enable)
			break;
		sleep(1);
		if(!led_blink_enable)
			break;
		ithGpioClear(LED_CONT);	
		if(!led_blink_enable)
			break;
		sleep(1);
		if(!led_blink_enable)
			break;
		ithGpioSet(LED_CONT);	
	}
	
	led_blink_ing = false;
}

void led_blink_1s_start()
{
	pthread_t task;
	pthread_attr_t attr;
	if(led_blink_ing)
		return;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&task, &attr, led_blink, NULL);
}

void led_blink_1s_end()
{
	led_blink_enable = false;
	ithGpioSet(LED_CONT);	
}

bool time_enable_montion()
{
        struct timeval tv_cur,tv_s_md,tv_e_md;
        struct tm md_time, mytime;

        gettimeofday(&tv_cur, NULL);

        md_time.tm_year 	= theConfig.mdsyear - 1900;
        md_time.tm_mon 	= theConfig.mdsmonth -1;
        md_time.tm_mday	= theConfig.mdsday;
        md_time.tm_hour 	= theConfig.mdshour;
        md_time.tm_min 	= theConfig.mdsmin;
        md_time.tm_sec	= theConfig.mdssec;

        tv_s_md.tv_sec = mktime(&md_time);

        md_time.tm_year 	= theConfig.mdeyear - 1900;
        md_time.tm_mon 	= theConfig.mdemonth -1;
        md_time.tm_mday	= theConfig.mdeday;
        md_time.tm_hour 	= theConfig.mdehour;
        md_time.tm_min 	= theConfig.mdemin;
        md_time.tm_sec	= theConfig.mdesec;

        tv_e_md.tv_sec = mktime(&md_time);

	//printf("time_enable_montion----------->%ld   %ld    %ld\n",tv_cur.tv_sec,tv_s_md.tv_sec,tv_e_md.tv_sec);
	if(tv_cur.tv_sec < tv_s_md.tv_sec || tv_cur.tv_sec > tv_e_md.tv_sec)
		return false;
	else
		return true;
}

void UserMotionEnd()
{
	printf("montion_end....................................\n");
	montion_enable = false;
	rec_start_time = 0;
	gState = SEND_STOP;
	md_quit();
	usleep(100*1000);
	PR2000_set_end();
}

void _user_video_time_check()
{
	VideoMemoCheck();
}

void user_video_time_check()
{
	pthread_t det_p;
	pthread_attr_t det_pat;
	pthread_attr_init(&det_pat);
	pthread_attr_setdetachstate(&det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&det_p, &det_pat, _user_video_time_check, NULL);
}

void user_vdp_power_on()
{
}

bool sd_card_check = false;
void _user_sd_card_check()
{	
	sd_card_check = true;
	printf("_user_sd_card_check..................................\n");
	struct statvfs info;
	if (statvfs("E:/", &info) == 0)
	{
		uint64_t avail = (uint64_t)info.f_bfree * info.f_bsize /1024 /1024;
		if(avail < 200)
			printf("no enough space!\n");
	}
	printf("_user_sd_card_check...over..............................\n");
	sd_card_check = false;
}

void user_sd_card_check()
{
	if(sd_card_check)
		return;
	pthread_t det_p;
	pthread_attr_t det_pat;
	pthread_attr_init(&det_pat);
	pthread_attr_setdetachstate(&det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&det_p, &det_pat, _user_sd_card_check, NULL);
}

#define VOICEMEMO_PATH "C:/"
void UserCheckDiskSpace(void)
{
    struct statvfs info;

    // check capacity
    if (statvfs("C:/", &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;
        printf("C-disk space: %llu\n", avail/1024);
    }
	/*
    if (statvfs("B:/", &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;
        printf("B-disk space: %llu\n", avail/1024);
    }
	
    if (statvfs("C:/", &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;
        printf("C-disk space: %llu\n", avail/1024);
    }
	
    if (statvfs("D:/", &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;
        printf("D-disk space: %llu\n", avail/1024);
    }
    */
}

