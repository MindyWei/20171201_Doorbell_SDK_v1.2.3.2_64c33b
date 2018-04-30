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
void v6502_init()			//6502 初始化配置
{
	uint8_t addr = 0x02;
	uint8_t val = 0x50;
	uint8_t addr_1 = 0x04;
	uint8_t val_1 = 0x3F;
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
	//mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr_1, &val_1, 1);			//关闭放大功能
}

void v6502_switch(uint8_t offset)			//6502 信号切换
{
	uint8_t addr = 0x00;
	uint8_t addr_1 = 0x02;
	uint8_t val = 0x5;
	uint8_t val_1 = 0x50;
	uint8_t offset_1 = offset<<4;

		usleep(10*1000);
		mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &offset, 1);
		usleep(10*1000);
		if(master_vdp)
		{
			mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr_1, &offset_1, 1);
			usleep(10*1000);
		}
}
void v6502_in_2_1_out(uint8_t offset)			//6502 信号切换
{
	uint8_t addr = 0x00;
	uint8_t val = 0x05;
	uint8_t addr_1 = 0x02;
	uint8_t val_1 = 0x50;
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
}

void v6502_switch_nodis(uint8_t offset)			//6502 信号切换
{
	uint8_t addr = 0x02;
	uint8_t val = offset<<4;
	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 1);
}
void voice_switch(int val)				//声音通道切换
{
	if(val == 1 && master_vdp)
	{
		ithGpioSet(DOOR_1_AUDIO);
		ithGpioClear(DOOR_2_AUDIO);
	}
	else if(val == 2 && master_vdp)
	{
		ithGpioClear(DOOR_1_AUDIO);
		ithGpioSet(DOOR_2_AUDIO);
	}
	else
	{
		
	}
}

void cam_all_off()						//关闭所有摄像头电源
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
}

void cur_cam_off()						//关闭当前摄像头电源
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
}

void cam_power_off()						//关闭当前摄像头电源
{
	if(offer_signal_channel)
	{
		//if(cur_talk_ing || door_call_out || door_talk_out)
		if(cur_talk_ing)
		{
		//	door_call_out = false;
		//	door_talk_out = false;
			cur_talk_ing = 0;
			offer_signal_channel = 0;
			ithGpioSet(cam_gpio[DOOR_1][ON]);
			ithGpioSet(cam_gpio[DOOR_2][ON]);
		}
	}
	else if(cur_signal_call)
	{	
		if(cur_talk_ing)
		{
			cur_talk_ing = 0;
		}
	}
	else
	{
		ithGpioSet(cam_gpio[DOOR_1][ON]);
		ithGpioSet(cam_gpio[DOOR_2][ON]);
	}
}

void offer_cam_off()						//关闭当前摄像头电源
{
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
}

void master_cam_off()
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

void cam_switch(int iCamNum)
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

#else

void cam_switch( uint8_t val)				//摄像头电源切换		
{
	char buf_1[6][10] = {"DOOR1","DOOR2","CCTV 1","CCTV 2","CCTV 3","CCTV4"};
	
	ITUText* MON_TEXT_CAM = ituSceneFindWidget(&theScene, "MON_TEXT_CAM");
	assert(MON_TEXT_CAM);
	ITUButton* MON_BTN_CAM_SW_1 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_1");
	assert(MON_BTN_CAM_SW_1);
	ITUButton* MON_BTN_CAM_SW_2 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_2");
	assert(MON_BTN_CAM_SW_2);
	ITUButton* MON_BTN_CAM_SW_3 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_3");
	assert(MON_BTN_CAM_SW_3);
	ITUButton* MON_BTN_CAM_SW_4 = ituSceneFindWidget(&theScene, "MON_BTN_CAM_SW_4");
	assert(MON_BTN_CAM_SW_4);
	
	ithGpioSet(cam_gpio[DOOR_1][ON]);
	ithGpioSet(cam_gpio[DOOR_2][ON]);
	ituWidgetSetVisible(MON_BTN_CAM_SW_1,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_2,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_3,false);
	ituWidgetSetVisible(MON_BTN_CAM_SW_4,false);
	ituTextSetString(MON_TEXT_CAM,buf_1[val-1]);
	if(1 == val)
	{
		ituWidgetSetVisible(MON_BTN_CAM_SW_1,true);
		if(signal_insert[DOOR_1])
		{
			ithGpioClear(cam_gpio[DOOR_1][ON]);
		}
	}
	else if(2 == val)
	{
		ituWidgetSetVisible(MON_BTN_CAM_SW_2,true);
		if(signal_insert[DOOR_2])
			ithGpioClear(cam_gpio[DOOR_2][ON]);
	}
	else if(3 == val)
	{
		ituWidgetSetVisible(MON_BTN_CAM_SW_3,true);
	}
	else if(4 == val)
	{
		ituWidgetSetVisible(MON_BTN_CAM_SW_4,true);
	}
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
	montion_begin_reinit();
	montion_snap_reinit();
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
#if 0
	if(theConfig.mute || (theConfig.ringvol == 0))
		user_amp_off();
	else
		user_amp_on();
		
	ithGpioClear(DOOR_RING);	
	AudioStop();
	AudioSetVolume(99);		//声音初始化test_call_1.wav
	call_ring_playing = true;
	if(cur_talk_ing)
		ring_volume_set(1);
	else
		ring_volume_set(theConfig.ringvol);
	if(cur_signal == 1)
		AudioPlay(ring_str[theConfig.door1], play_call_ring_back);
	else if(cur_signal == 2)
		AudioPlay(ring_str[theConfig.door2], play_call_ring_back);
	#else
	ring_play = 2;
	#endif
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
static void event_call_process(void)		//call 机事件处理
{
	//printf("proscess------------------------->%d\n",SDL_GetTicks()-test_tick);
	if(get_door_is_calling())
	{
		event_call_s = 0;
		return;
	}
	//door_delay_start();
	
	event_call =  event_call_s;
	event_call_s = 0;
	if(other_talk_ing || format_ing ||delete_ing)
	{
		master_offer_ctalk_signal(event_call);
		if(format_ing ||delete_ing)
			door_call_num = event_call;
		event_call = 0;
	}
	else if(door_call_num == event_call)
	{
		printf("the same door call...............................\n");
		event_call = 0;
		if(!call_ring_playing)
			set_play_call_ring_once();
	}
	else if(cur_mon_state == M_TALK_STATE)
	{
		door_call_num = event_call;
		cur_mon_state = C_TALK_STATE;
		if(door_call_num == cur_signal)
			event_call = 0;
	}
	else if(door_call_num != event_call)
	{
		printf("the other door call...............................\n");
		if(montion_enable)      //退出移动侦测
			montion_end();
		standby_state= false;
		master_vdp = true;
		door_call_num = event_call;
		if(cur_mon_state != C_TALK_STATE)
		{
			busy_30_start();
			cur_mon_state = CALL_STATE;
			led_blink_1s_start();
		}
		set_back_home_flag();
		uart_set_busy();
		uart_clear_id_link();
		uart_clear_inter_link();
		if(event_call == 0x1)
		{
			signal_insert[DOOR_1] = true;
		}
		else if(event_call == 0x2)
		{
			signal_insert[DOOR_2] = true;
		}
		if(theConfig.zidong != 2)
		{
			set_auto_rec_once();
			set_mon_rec_ing();
		}
		if(cur_page  != page_monitor)
		{ 
			if(cur_page == page_cctv)
				pre_page = 1;
			cur_page  = page_monitor;
			ITULayer* monitorLayer = ituSceneFindWidget(&theScene, "monitorLayer");
			assert(monitorLayer);
			ituLayerGoto(monitorLayer);
		}
	}
	else
		event_call = 0;
}

static void event_intercom_process(void)		//call 机事件处理
{	
	event_intercom = 0x1;
}

static void event_uart_process(void)		//uart 事件处理
{
	if(event_uart == CMD_MONITOR)
	{
		if(signal_insert[DOOR_1] || signal_insert[DOOR_2])
			uart_set_busy();
		if(montion_enable)      //退出移动侦测
			montion_end();
		if(signal_insert[DOOR_1])
			master_offer_signal(1);
		else if(signal_insert[DOOR_2])
			master_offer_signal(2);
	}
	else if(event_uart == CMD_MON_DOOR1 ||event_uart == CMD_MON_DOOR2)
	{
		if(event_uart == CMD_MON_DOOR1)
			cur_signal = signal_door_1;
		else if(event_uart == CMD_MON_DOOR2)
			cur_signal = signal_door_2;
		if(set_display_signal)							//设置界面     display set
		{
			_set_enter_display();
		}
		else
		{
			cur_page  = page_monitor;
			event_home_go = page_monitor;
			cur_mon_state = MON_STATE;
			//monitor_start();
		}
	}
	else if(event_uart == CMD_CCTV)
	{
		if(signal_insert[CCTV_1] || signal_insert[CCTV_2])
			uart_set_busy();
		if(montion_enable)      //退出移动侦测
			montion_end();
		if(signal_insert[CCTV_1])
			master_offer_signal(3);
		else if(signal_insert[CCTV_2])
			master_offer_signal(4);
	}
	else if(event_uart == CMD_MON_CCTV1 ||event_uart == CMD_MON_CCTV2)
	{
		if(event_uart == CMD_MON_CCTV1)
			cur_signal = signal_cctv_1;
		else if(event_uart == CMD_MON_CCTV2)
			cur_signal = signal_cctv_2;
		cur_page  = page_cctv;
		event_home_go = page_monitor;
		cur_mon_state = CCTV_STATE;
		//monitor_start();
	}
	else if(event_uart == CMD_SWITCH_1 ||event_uart == CMD_SWITCH_2 || event_uart == CMD_SWITCH_3 || event_uart == CMD_SWITCH_4)
	{
		if(event_uart == CMD_SWITCH_1)
			master_offer_signal(1);
		else if(event_uart == CMD_SWITCH_2)
			master_offer_signal(2);
		else if(event_uart == CMD_SWITCH_3)
			master_offer_signal(3);
		else if(event_uart == CMD_SWITCH_4)
			master_offer_signal(4);
	}
	else if(event_uart == CMD_SWITCH_OK)
	{
		if(cur_signal == signal_door_1)
			cur_signal = signal_door_2;
		else if(cur_signal == signal_door_2)
			cur_signal = signal_door_1;
		else if(cur_signal == signal_cctv_1)
			cur_signal = signal_cctv_2;
		else if(cur_signal == signal_cctv_2)
			cur_signal = signal_cctv_1;
		_monitor_sw_cam();
	}
	else if(event_uart == CMD_RELEASE )
	{
		offer_cam_off();
		other_talk_ing = false;
		if(cur_page  == page_monitor || cur_page  == page_cctv)		//同时进入监控/CCTV  一个退出全退出
		{
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
	}
	else if((event_uart == CMD_CALL_1) ||(event_uart == CMD_CALL_2) )
	{	
		if(event_uart == CMD_CALL_1)
			event_call = 0x1;	
		else 
			event_call = 0x2;	
		if(other_talk_ing || format_ing ||delete_ing)
		{
			if(format_ing ||delete_ing)
				uart_set_busy();
			event_call = 0;
		}
		else if(door_call_num == event_call)
		{
			printf("the same door call...............................\n");
			event_call = 0;
			if(!call_ring_playing)
				set_play_call_ring_once();
		}
		else if(cur_mon_state == M_TALK_STATE)
		{
			door_call_num = event_call;
			cur_mon_state = C_TALK_STATE;
			if(door_call_num == cur_signal)
				event_call = 0;
		}
		else if(door_call_num  != event_call)
		{
			standby_state= false;
			if(cur_mon_state != C_TALK_STATE)
			{
				busy_30_start();
				cur_mon_state = CALL_STATE;
				led_blink_1s_start();
			}
			door_call_num  = event_call;
			//door_is_call = true;
			if(theConfig.zidong != 2)
			{
				set_auto_rec_once();
				set_mon_rec_ing();
			}
			if(cur_page  != page_monitor)
			{
				if(cur_page == page_cctv)
					pre_page = 1;
				cur_page  = page_monitor;
				ITULayer* monitorLayer = ituSceneFindWidget(&theScene, "monitorLayer");
				assert(monitorLayer);
				ituLayerGoto(monitorLayer);
			}
			//cur_signal_call = true;
		}
		else
			event_call = 0;
		//door_call_start();
	}
	else if(event_uart == CMD_CALL_JOIN)
	{	
		//door_call_start();
		uart_set_busy();
	}
	else if(event_uart == CMD_CALL_QUIT)
	{	
		if(cur_page != page_monitor)
		{
			door_call_num = 0;
			master_cam_off();
		}
	}
	else if(event_uart == CMD_CALL_OVER)
	{	
		if(master_vdp)
		{
			uart_clear_id_link();
			master_cam_off();
		}
		other_talk_ing = 0;
		other_talk_id = 0;
		door_call_num = 0;
		uart_clear_busy();
		busy_30_reinit();
		busy_60_reinit();
	}
	else if(event_uart == CMD_CALL_ANSWER)
	{	
		other_talk_ing = true;
		busy_60_start();
		if(cur_page  == page_monitor)
		{
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
	}
	else if(event_uart == CMD_SIGNAL_BUSY)
	{	
		uart_set_busy();
	}
	else if(event_uart == CMD_INTER_CALL)
	{	
		if(montion_enable)      //退出移动侦测
			montion_end();
		if(format_ing || delete_ing)
		{
			uart_set_busy();
		}
		else
		{
			standby_state= false;
			cur_inter_call = true;
			uart_set_busy();
			ithGpioSet(CONV_CONT);
			ithGpioClear(MIC_NUTE);
			if(cur_page  != page_inter_ing)
			{
				cur_page  = page_inter_ing;
				ITULayer* PAGE_INTERCOM_ING = ituSceneFindWidget(&theScene, "PAGE_INTERCOM_ING");
				assert(PAGE_INTERCOM_ING);
				ituLayerGoto(PAGE_INTERCOM_ING);
			}
		}
	}
	else if(event_uart == CMD_INTER_JOIN)
	{	
		interphone_mast = true;
		ithGpioSet(CONV_CONT);
		ithGpioClear(MIC_NUTE);
		//ithGpioSet(AUDIO_IN);	
		cur_page  = page_inter_ing;
		ITULayer* PAGE_INTERCOM_ING = ituSceneFindWidget(&theScene, "PAGE_INTERCOM_ING");
		assert(PAGE_INTERCOM_ING);
		ituLayerGoto(PAGE_INTERCOM_ING);
	}
	else if(event_uart == CMD_INTER_QUIT)
	{	
		cur_inter_call = false;
		cur_inter_ing = false; 
		ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
		assert(mainMenuLayer);
		ituLayerGoto(mainMenuLayer);
	}
	else if(event_uart == CMD_INTER_OVER)
	{	
		cur_inter_call = false;
		cur_inter_ing = false; 
		uart_clear_inter_link();
		uart_clear_inter_id();
		uart_clear_busy();
		inter_call_reinit();
		if(cur_page  == page_inter_ing)
		{
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
	}
	else if(event_uart == CMD_INTER_ANSWER)
	{	
		intercom_get_answer(); //my.wei mask for test ahd
		if(cur_page != page_inter_ing)
		{
			inter_talk_start();
		}
	}
	else if(event_uart == CMD_DOOR_1_OPEN)
	{	
		printf("door_1_open..................\n");
		ithGpioClear(DOOR_1_OPEN);
		door_open_timer_start();
	}
	else if(event_uart == CMD_DOOR_2_OPEN)
	{	
		printf("door_2_open..................\n");
		ithGpioClear(DOOR_2_OPEN);
		door_open_timer_start();
	}
	else if(event_uart == CMD_SIGNAL_NOT_BUSY)
	{	
		uart_clear_id_link();
		master_cam_off();
		other_talk_ing = 0;
		other_talk_id = 0;
		door_call_num = 0;
		busy_30_reinit();
		busy_60_reinit();
		other_talk_ing = false;
		cur_inter_call = false;
		cur_inter_ing = false; 
		uart_clear_inter_link();
		uart_clear_inter_id();
		uart_clear_busy();
		inter_call_reinit();
		offer_cam_off();
		ITUBackground* SET_MAIN_5_DIS_S = ituSceneFindWidget(&theScene, "SET_MAIN_5_DIS");
		assert(SET_MAIN_5_DIS_S);
		if(cur_page  == page_monitor|| cur_page  == page_cctv|| cur_page  == page_inter_ing || ituWidgetIsVisible(SET_MAIN_5_DIS_S))		//同时进入监控/CCTV  一个退出全退出
		{
			ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
			assert(mainMenuLayer);
			ituLayerGoto(mainMenuLayer);
		}
	}
		

	event_uart = 0;
}

void snap_rec_state()
{
	if(show_snap_rec_icon)
	{
		if(show_snap_rec_icon == sanp_display)
		{
			show_snap_rec_icon = icon_displaying;
			ITUButton* MON_BTN_SNAP = ituSceneFindWidget(&theScene, "MON_BTN_SNAP");
			assert(MON_BTN_SNAP);
			ITUIcon* MON_ICON_SNAP = ituSceneFindWidget(&theScene, "MON_ICON_SNAP");
			assert(MON_ICON_SNAP);
			ituWidgetSetVisible(MON_BTN_SNAP,false);
			ituWidgetSetVisible(MON_ICON_SNAP,true);
			
		}
		else if(show_snap_rec_icon == rec_display)
		{
			show_snap_rec_icon = icon_displaying;
			ITUButton* MON_BTN_REC = ituSceneFindWidget(&theScene, "MON_BTN_REC");
			assert(MON_BTN_REC);
			ITUButton* MON_BTN_REC_ING = ituSceneFindWidget(&theScene, "MON_BTN_REC_ING");
			assert(MON_BTN_REC_ING);
			//ITUText* MON_TEXT_REC_TIME = ituSceneFindWidget(&theScene, "MON_TEXT_REC_TIME");
			//assert(MON_TEXT_REC_TIME);
			ituWidgetSetVisible(MON_BTN_REC,false);
			ituWidgetSetVisible(MON_BTN_REC_ING,true);
			//ituWidgetSetVisible(MON_TEXT_REC_TIME,true);
			//ituTextSetString(MON_TEXT_REC_TIME, "01:00");
		}
		else if(show_snap_rec_icon == icon_displaying)
		{
		}
		else
		{
			temp_rec_time = 60;
			ITUButton* MON_BTN_REC = ituSceneFindWidget(&theScene, "MON_BTN_REC");
			assert(MON_BTN_REC);
			ITUButton* MON_BTN_REC_ING = ituSceneFindWidget(&theScene, "MON_BTN_REC_ING");
			assert(MON_BTN_REC_ING);
			//ITUText* MON_TEXT_REC_TIME = ituSceneFindWidget(&theScene, "MON_TEXT_REC_TIME");
			//assert(MON_TEXT_REC_TIME);
			ituWidgetSetVisible(MON_BTN_REC,true);
			ituWidgetSetVisible(MON_BTN_REC_ING,false);
			//ituWidgetSetVisible(MON_TEXT_REC_TIME,false);
			ITUButton* MON_BTN_SNAP = ituSceneFindWidget(&theScene, "MON_BTN_SNAP");
			assert(MON_BTN_SNAP);
			ITUIcon* MON_ICON_SNAP = ituSceneFindWidget(&theScene, "MON_ICON_SNAP");
			assert(MON_ICON_SNAP);
			if(ituWidgetIsVisible(MON_ICON_SNAP))
			{
				clear_mon_rec_ing();
				ituWidgetSetVisible(MON_BTN_SNAP,true);
				ituWidgetSetVisible(MON_ICON_SNAP,false);
			}
		}
	}
}

static void event_home_go_process(void)		//主界面切换事件处理
{
	if(event_home_go == page_monitor || event_home_go == page_cctv || event_home_go == page_motion )
	{
		ITULayer* monitorLayer = ituSceneFindWidget(&theScene, "monitorLayer");
		assert(monitorLayer);
		ituLayerGoto(monitorLayer);
	}
	else if(event_home_go == page_inter)
	{
		ITULayer* PAGE_INTERCOM = ituSceneFindWidget(&theScene, "PAGE_INTERCOM");
		assert(PAGE_INTERCOM);
		ituLayerGoto(PAGE_INTERCOM);
	}
	else if(event_home_go == page_media)
	{
		ITULayer* PAGE_MEDIA = ituSceneFindWidget(&theScene, "PAGE_MEDIA");
		assert(PAGE_MEDIA);
		ituLayerGoto(PAGE_MEDIA);
	}
	else if(event_home_go == page_date)
	{
		ITULayer* PAGE_DATE = ituSceneFindWidget(&theScene, "PAGE_DATE");
		assert(PAGE_DATE);
		ituLayerGoto(PAGE_DATE);
	}
	else if(event_home_go == page_setting)
	{
		ITULayer* PAGE_SETING = ituSceneFindWidget(&theScene, "PAGE_SETING");
		assert(PAGE_SETING);
		ituLayerGoto(PAGE_SETING);
	}
	event_home_go = 0;
}


static void event_go_home_process(void)		//主界面切换事件处理
{
	ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
	assert(mainMenuLayer);
	ituLayerGoto(mainMenuLayer);
	event_go_home = false;
}

static void montion_end_event_process()
{
	if(montion_end_event == MD_EVENT_CALL)
	{
		standby_state= false;
		master_vdp = true;
		door_call_num = event_call;
		if(cur_mon_state != C_TALK_STATE)
		{
			cur_mon_state = CALL_STATE;
			led_blink_1s_start();
		}
		set_back_home_flag();
		
		if(cur_page  != page_monitor)
		{ 
			if(cur_page == page_motion || cur_page == page_cctv)
				pre_page = 1;
			cur_page  = page_monitor;
			ITULayer* monitorLayer = ituSceneFindWidget(&theScene, "monitorLayer");
			assert(monitorLayer);
			ituLayerGoto(monitorLayer);
		}
	}
	else if(montion_end_event == MD_EVENT_OFFER)
	{
		master_offer_signal(montion_event_cmd);
	}
	else if(montion_end_event == MD_EVENT_INTER)
	{
			standby_state= false;
			cur_inter_call = true;
			if(cur_page  != page_inter_ing)
			{
				cur_page  = page_inter_ing;
				ITULayer* PAGE_INTERCOM_ING = ituSceneFindWidget(&theScene, "PAGE_INTERCOM_ING");
				assert(PAGE_INTERCOM_ING);
				ituLayerGoto(PAGE_INTERCOM_ING);
			}
	}
	else if(montion_end_event == MD_EVENT_KEY)
	{
		ScreenOn();
	}
	montion_end_event = 0;
	montion_event_cmd = 0;
}
void event_process()					//事件处理
{
	if(event_call_s)
		event_call_process();
	
	snap_rec_state();	//my.wei mask for debug					//显示拍照/录像图标
	
	if(event_intercom > 0x0F)
		event_intercom_process();

	if(event_uart)
		event_uart_process();

	if(event_home_go)
		event_home_go_process();
	
	if(event_go_home)
		event_go_home_process();
	
	if(montion_end_event && !montion_enable)
		montion_end_event_process();
		
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
	v6502_init();
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

void backlight_init(void)
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

void enter_stanby()
{
	//if()
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

void montion_end()
{
	printf("montion_end....................................\n");
	montion_enable = false;
	rec_start_time = 0;							//结束录像
	gState = SEND_STOP;							//停止图像
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
	pthread_t det_p;			//线程ID
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
		if(cur_page == page_monitor || cur_page == page_cctv)
		{
			if(avail < 200)
				set_monitor_sd_icon(2);
			else
				set_monitor_sd_icon(1);
		}
	}
	printf("_user_sd_card_check...over..............................\n");
	sd_card_check = false;
}

void user_sd_card_check()
{
	if(sd_card_check)
		return;
	pthread_t det_p;			//线程ID
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
        printf("A-disk space: %llu\n", avail/1024);
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

