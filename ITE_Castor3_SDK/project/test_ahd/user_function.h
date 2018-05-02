#ifndef _USER_FUNCTION_H
#define _USER_FUNCTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "iic/mmp_iic.h"
#include "capture_module/pr2000.h"
#include "ite/itu.h"
#include "itu_cfg.h"
#include "itu_private.h"
#include "mjpeg/mjpeg_player.h"
#include "project.h"

#define TARGET_BOARD_G
#define TARGET_BOARD_G_V03


#define TEST_SET_ON		0

#define TEST_CAM		0 //1//my.wei enable for test

#define TEST_AUTO_CALL	1

#if defined(TARGET_BOARD_G)
#define DOOR_1_ON			0//98		//户外机1 供电GPIO
#define DOOR_2_ON			0//99		//户外机2 供电GPIO
#define DOOR_3_ON			0		//户外机3 供电GPIO
#define DOOR_4_ON			0		//户外机4 供电GPIO

#define DOOR_1_CALL			0//6		//户外机1 检测GPIO
#define DOOR_2_CALL			0//5		//户外机2 检测GPIO
#define DOOR_3_CALL			0		//户外机3 检测GPIO
#define DOOR_4_CALL			0		//户外机4检测GPIO

#define DOOR_1				0
#define DOOR_2				1
#define CCTV_1				2
#define CCTV_2				3

#define DOOR_1_OPEN		0//88		//户外机1 开锁GPIO	
#define DOOR_2_OPEN		0//89		//户外机2 开锁GPIO	

#define CCTV_1_ON			0
#define CCTV_2_ON			0

#define DOOR_RING			0//29		//控制铃声输出到户外机
#define DOOR_1_AUDIO		0//28
#define AUDIO_IN			0//27
#define AUDIO_OUT			0//26
#define DOOR_2_AUDIO		0//25

#define CONV_CONT			0//22		//34118 IC 控制开关
#define MIC_NUTE			0//30		// 控制通话声音输出到户外机



#define RING_OPEN			0//96		//免提IC
#define RING_POWER			0//83		//功放供电

#define RING_VOL_1			0//93		
#define RING_VOL_2			0//94		

#define TALK_VOL_1			0//92
#define TALK_VOL_2			0//95

#define LED_CONT			0//63		//指示灯


#define INTER_CON_CNT 		0//38		//interphone 音量


#define ON		0
#define CALL		1

#define PR2000_CONT		0x20
#define PR2000_BRGT		0x21
#define PR2000_HUE		0x22

#define ALC5616_VOL 		0//90

#else

#define DOOR_1_ON			98		//户外机1 供电GPIO
#define DOOR_2_ON			99		//户外机2 供电GPIO
#define DOOR_3_ON			0		//户外机3 供电GPIO
#define DOOR_4_ON			0		//户外机4 供电GPIO

#define DOOR_1_CALL			6		//户外机1 检测GPIO
#define DOOR_2_CALL			5		//户外机2 检测GPIO
#define DOOR_3_CALL			0		//户外机3 检测GPIO
#define DOOR_4_CALL			0		//户外机4检测GPIO

#define DOOR_1				0
#define DOOR_2				1
#define CCTV_1				2
#define CCTV_2				3

#define DOOR_1_OPEN		88		//户外机1 开锁GPIO	
#define DOOR_2_OPEN		89		//户外机2 开锁GPIO	

#define CCTV_1_ON			0
#define CCTV_2_ON			0

#define DOOR_RING			29		//控制铃声输出到户外机
#define DOOR_1_AUDIO		28
#define AUDIO_IN			27
#define AUDIO_OUT			26
#define DOOR_2_AUDIO		25

#define CONV_CONT			22		//34118 IC 控制开关
#define MIC_NUTE			30		// 控制通话声音输出到户外机



#define RING_OPEN			96		//免提IC
#define RING_POWER			83		//功放供电

#define RING_VOL_1			93		
#define RING_VOL_2			94		

#define TALK_VOL_1			92
#define TALK_VOL_2			95

#define LED_CONT			63		//指示灯


#define INTER_CON_CNT 		38		//interphone 音量


#define ON		0
#define CALL		1

#define PR2000_CONT		0x20
#define PR2000_BRGT		0x21
#define PR2000_HUE		0x22

#define ALC5616_VOL 		90
#endif

typedef enum
{
    page_home = 0,
    page_monitor,
    page_cctv,
    page_inter,
    page_inter_ing,
    page_date,
    page_media,
    page_setting,
    page_motion,
    page_standby,

    page_total
} page_id;

typedef enum
{
    signal_door_1 = 1,
    signal_door_2,
    signal_cctv_1,
    signal_cctv_2,
    signal_total = 4,
} signal_id;
#define signal_door_1_out		5
#define signal_door_2_out		6
#define signal_cctv_1_out		7
#define signal_cctv_1_out		8

typedef enum MON_STATE_TAG
{
    MON_STATE = 1,
    M_TALK_STATE,
    CALL_STATE,
    C_TALK_STATE,
    CCTV_STATE,
    MTION_STATE,
}MON_STATE_S;

typedef enum MD_EVENT_TAG
{
    MD_EVENT_CALL = 1,
    MD_EVENT_OFFER,
    MD_EVENT_INTER,
    MD_EVENT_KEY,
}MD_EVENT_S;

typedef enum
{
    sanp_display = 1,
    rec_display,
    icon_displaying,
    icon_clear,
} SNAP_REC_ICON;

typedef enum 
{
	MONITOR_S_CAM= 0,
	MONITOR_S_DIS,	
	MONITOR_S_FULL,		
	VIEW_PHOTO_S_LIST ,
	VIEW_PHOTO_S_DELETE,
	VIEW_PHOTO_S_FULL,
	VIEW_VIDEO_S_MENU ,
	VIEW_VIDEO_S_DELETE,
	VIEW_VIDEO_S_VOLUME,
	VIEW_VIDEO_S_FULL,
	POP_UP_TOTAL
}POP_UP_STATE;

typedef enum SEND_STATE_TAG
{
    SEND_BEGIN,
    SEND_START,
    SEND_STOP,
    SEND_TERMINATE,
    SEND_IDLE,
} SEND_STATE;


typedef enum VIDEO_PLAY_TAG
{
    PLAY_START,
    PLAY_ING,
    PLAY_PAUSE,
} VIDEO_PLAY_STATE;

#define IMAGEMEMO_MAX_NAME_SIZE      20

typedef struct
{
    char name[IMAGEMEMO_MAX_NAME_SIZE]; 		//< Name displayed in UI
    char cam;									//< cam        
    char type;									//< type
    char played;    								//< Is played or not
} ImageMemoEntry;

typedef struct VideoNode
{
    char name[IMAGEMEMO_MAX_NAME_SIZE]; 		//< Name displayed in UI
    char cam;									//< cam        
    char type;									//< type
    bool played;    								//< Is played or not
} VideoMemoEntry;

#if defined(TARGET_BOARD_G)
extern int currCam;
#endif

extern bool pr2000_test;

//
extern bool master_vdp;				//主机标志
extern bool montion_enable;
extern int montion_end_event;
extern int montion_event_cmd;
extern bool need_check_video;

extern bool boot_logo;
extern bool busy_over_3s;
extern bool video_first;

extern uint8_t door_call_num;
extern bool call_ring_playing;
extern bool format_ing;
extern bool format_over;
extern bool delete_ing;
extern bool delete_over;
extern bool set_display_signal;

extern bool door_is_call;
extern bool door_call_out;
extern bool door_talk_out;
extern bool standby_state;

extern uint8_t ring_play; 

extern bool back_light;

//当前状态标志
extern int pre_page;
extern int cur_page;					//当前页面
extern int pre_mon_state;				//当前监控状态
extern int cur_mon_state;				//当前监控状态
extern int cur_call_ing;					
extern int cur_signal;					//当前通道
extern bool cur_wind;					//当前信号通道
extern bool pre_open;					//开锁标志
extern bool cur_open;					//当前开锁标志
extern uint8_t cam_detecting;				//户外机检测状态
extern bool cam_detect_over;				//户外机检测状
extern bool signal_insert[4];			//door 1 接入状态
//extern bool door_1_insert;				//door 1 接入状态
//extern bool door_2_insert;				//door 2 接入状态
//extern bool cctv_1_insert;				//cctv 1 接入状态
//extern bool cctv_2_insert;				//cctv 2 接入状态
extern uint8_t offer_signal_channel;		//当前提供的信号
extern uint8_t use_signal_channel;		//当前使用的信号
extern bool cur_signal_call;				//当前显示信号的状态
extern bool signal_channel_busy ;		//当前信号通道忙
extern bool interphone_mast;
extern uint8_t interphone_number;		//interphone 号码
extern bool cur_inter_call;				//interphone 状态
extern uint8_t cur_inter_times;			//interphone 状态
extern bool cur_inter_ing;				//interphone状态
extern bool cur_talk_ing;				//通话状态
extern bool other_talk_ing;				//通话状态
extern int other_talk_id;					//通话状态
extern uint8_t cur_wash_mode;			//通话状态
extern bool sd_state_change;			//SD卡状态变化


//事件标志
extern int event_call;					//call 机事件
extern int event_call_s;					//call 机事件
extern int event_intercom;				//intercom 事件
extern int event_uart;					//串口 事件
extern uint8_t event_home_go;				//主界面切换事件
extern bool event_go_home;				//切换到主界面


//逻辑标志
extern int show_snap_rec_icon;			//显示拍照/录像图标
extern unsigned long rec_start_time;		//更新录像时间
extern int temp_rec_time;				//录像时间累计值
extern bool pop_up[POP_UP_TOTAL];		//弹窗标志
extern uint8_t once_key_ring;					//跳过一次按键音
extern int cam_gpio[signal_total][2];		//	
extern SEND_STATE  gState;				//信号标志
extern bool pr2000_signal_lock;			//信号锁
extern bool g_videoPlayerIsFileEOF;		//视频播放标志
extern bool PlayerStop;					//播放视频结束标志
extern bool PlayerStart;
extern bool PlayerRead;
extern bool pr2000_signal_lock;
extern bool pre_singnal_lock;
extern bool pr2000_is_ok;
extern bool pr2000_start;
extern bool set_to_time_set_flag;	
extern bool auto_snap_filename;	
extern bool sd_card_check;

//extern StorageType storageCurrType;
extern uint32_t test_tick;

extern bool auto_rec_start;

typedef void (*PhotoLoadCallback)(uint8_t* data, int size);
extern uint8_t pr2000_i2c_read(uint8_t slaveAddr,uint8_t regAddr);
extern void pr2000_i2c_write(uint8_t slaveAddr,uint8_t regAddr,uint8_t val);
extern void* TestFunc_t(void* arg);

#endif
