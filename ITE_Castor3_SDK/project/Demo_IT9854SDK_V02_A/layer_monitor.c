#include <assert.h>
#include "scene.h"
#include "ctrlboard.h"
#include "user_capture.h"
#if defined(TARGET_BOARD_S)
#include "iic/mmp_iic.h"
#endif

#ifdef CFG_DBG_RMALLOC
static uint32_t rm_StatLastTick = 0;
#endif

static ITUBackground* monitorBackground;

#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board

extern SEND_STATE gState;
extern bool pr2000_in_capture;

static int currCam = 0;

#if defined(TARGET_BOARD_S)
enum {
	MONITOR_S_CAM= 0,
	MONITOR_S_MENU,		
	VIEW_PHOTO_S_MENU ,
	VIEW_PHOTO_S_DELETE,
	VIEW_PHOTO_S_FULL,
	VIEW_VIDEO_S_MENU ,
	VIEW_VIDEO_S_DELETE,
	VIEW_VIDEO_S_VOLUME,
	VIEW_VIDEO_S_FULL,
	SWITCH_TOTAL
};

static bool switch_state[SWITCH_TOTAL] ={false};
static int cur_cvbs_channel = 0;
static int change_cam_state = 0;
static int cur_cam_num = 0;
static int door_gpio[DOOR_MAX][2] =  {{DOOR_1_ON,DOOR_1_CALL},
								{DOOR_2_ON,DOOR_2_CALL},
								{DOOR_3_ON,DOOR_3_CALL},
								{DOOR_4_ON,DOOR_4_CALL}};

static void switch_cam_power( uint8_t val)
{
	if(val == 1)
	{	
		ithGpioClear(door_gpio[1][ON]);	
		ithGpioSet(door_gpio[0][ON]);
		ithGpioSet(CD4051_A);
		ithGpioClear(CD4051_B);	
	}
	else if(val == 2)
	{
		ithGpioClear(door_gpio[0][ON]);	
		ithGpioSet(door_gpio[1][ON]);
		ithGpioSet(CD4051_B);
		ithGpioClear(CD4051_A);	
	}
}

static bool switch_on_off(int num)
{
	if(switch_state[num])
	{
		switch_state[num] = false;
		return true;
	}
	else
	{
		switch_state[num] = true;
		return false;
	}
}

static void switch_init()
{
	int i;
	
	for(i = 0; i < SWITCH_TOTAL; i++)
	{
		switch_state[i] = false;
	}
}

static void HW_v6502_switch(int offset)
{
	uint8_t addr = 0x00;
	uint8_t val = 0x01<<(offset-1);

	mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 2);
}

static void switch_cam_fun( uint8_t val)
{
	char buf_1[6][10] = {"DOOR 1","DOOR 2","CCTV 1","CCTV 2","CCTV 3","CCTV4"};

	HW_v6502_switch(val);		
}

static void cam_switch(int iCamNum)
{
	uint8_t addr = 0x00;
	uint8_t val = 0x01;
	if(iCamNum == 0)
	{
		ithGpioClear(30);	
		ithGpioSet(31);
		mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 2);
	}
	else if(iCamNum == 1)
	{
		val = val<<1;
		ithGpioClear(31);	
		ithGpioSet(30);
		mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, addr, &val, 2);
	}
}

#elif defined(TARGET_BOARD_G)
#if defined(TARGET_BOARD_G_V03)
#define CAM_SWITCH_PIN  	22
#else
#define CAM_SWITCH_PIN  	63
#endif

static void cam_switch(int iCamNum)
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

#endif

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	//printf("VideoPlayerViewBackgroundDraw......................\n");
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;

	destx = rect->x + x;
	desty = rect->y + y;
#if defined(CFG_VIDEO_ENABLE) && !defined(CFG_FFMPEG_H264_SW)
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
#endif
}

bool monitorLayerOnEnter(ITUWidget* widget, char* param)
{
	if (!monitorBackground)
	{
		monitorBackground = ituSceneFindWidget(&theScene, "monitorBackground");
		assert(monitorBackground);
		ituWidgetSetDraw(monitorBackground, VideoPlayerViewBackgroundDraw);
	}
	
#if defined(TARGET_BOARD_S) && defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	switch_init();

	if(cur_cvbs_channel)
	{		
		cur_cam_num = cur_cvbs_channel;
		switch_cam_fun(cur_cvbs_channel);
		switch_cam_power(cur_cvbs_channel);
		cur_cvbs_channel = 0;
		//AudioStop();
		//AudioPlay("A:/sounds/call.wav", NULL);
	}
	else
	{
		cur_cam_num = 1;
		ithGpioSet(31);
		switch_cam_fun(1);
	}
	usleep(100*1000);
#endif

#ifdef CFG_DBG_RMALLOC
	rm_StatLastTick = SDL_GetTicks();
#endif

	AudioPauseKeySound();
	SceneEnterVideoState(33);
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	gState = SEND_BEGIN;
	pr2000_in_capture = true;
#endif
    return true;
}

bool monitorLayerOnLeave(ITUWidget* widget, char* param)
{
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	pr2000_in_capture = false;
	gState = SEND_STOP;
#endif
	AudioStop();
	AudioResumeKeySound();
	SceneLeaveVideoState();
    return true;
}

bool monitorLayerOnTimer(ITUWidget* widget, char* param)
{
#ifdef CFG_DBG_RMALLOC
	uint32_t tick = SDL_GetTicks();
	uint32_t diff = tick - rm_StatLastTick;
	if (diff >= 5000)
	{
		rm_StatLastTick = tick;
		Rmalloc_stat(__FILE__);		
	}
#endif

#if defined(TARGET_BOARD_S) && defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	if(cur_cvbs_channel)
	{
		cur_cam_num = cur_cvbs_channel;
		switch_cam_fun(cur_cvbs_channel);
		switch_cam_power(cur_cvbs_channel);
		cur_cvbs_channel = 0;
		//AudioStop();
		//AudioPlay("A:/sounds/call.wav", NULL);
		usleep(100*1000);
		gState = SEND_BEGIN;
	}
#endif
    return true;
}

bool monitorRecButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	user_snap(2);
#endif
    return true;
}

bool monitorSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	user_snap(1);
#endif
    return true;
}

bool monitorSwitchButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	gState = SEND_STOP;
	currCam = currCam ? 0 : 1;
	cam_switch(currCam);
	while(gState != SEND_IDLE)
	{
		usleep(100*1000);
	}
	gState = SEND_BEGIN;
#endif
    return true;
}

bool monitorOpendoorButtonOnPress(ITUWidget* widget, char* param)
{
#if defined(CFG_CAPTURE_MODULE_ENABLE) //add for user target board
	pr2000_in_capture = false;
#endif
    return true;
}


