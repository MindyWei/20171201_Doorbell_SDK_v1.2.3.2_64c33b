#include "user_function.h"
#include <assert.h>
#include <fat/Api_f.h>

bool pr2000_test = false;
bool montion_enable = false;
int montion_end_event = 0;
int montion_event_cmd = 0;
bool busy_over_3s = false;
int cur_page = 0;
int cur_signal = 0;
uint8_t cam_detecting = 0;
bool cam_detect_over = false;
bool sd_state_change = false;
int show_snap_rec_icon = 0;
unsigned long rec_start_time = 0;
int temp_rec_time = 60;
bool pop_up[POP_UP_TOTAL] ={false};
bool auto_snap_filename = false;
uint32_t test_tick;

static bool isSnapRecIconDisplay = false;
static float screenSaverCountDown;
static uint32_t screenSaverLastTick;
static bool screenOff = false;

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

void mon_quit()
{
	printf("mon_quit...........................................\n");
}


void md_quit()
{
	printf("md_quit...........................................\n");
	usleep(500*1000);
	UserTimerMotionBeginReinit();
	UserTimerMotionSnapReinit();
	ithGpioSet(AUDIO_OUT);
	montion_enable = false;
}

static void snap_rec_state()
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

void UserEventProcess()	
{
	snap_rec_state();
}

static void _user_cam_detect()
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
	for(i =1;i<5;i++)
	{
		cam_detecting = i;
		mmpIicSendData(IIC_PORT_1, IIC_MASTER_MODE, 0x06>>1, 0x00, &cam_detecting, 2);
		usleep(500*1000);
		if(UserPr2000CheckSignal())
		{
		}
		usleep(10*1000);
			
	}
	cam_detecting = 0;
	cam_detect_over = true;
}

void UserCameraDetect()
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

void ScreenInit(void)
{
    screenSaverLastTick = SDL_GetTicks();
    screenSaverCountDown = theConfig.screensaver_time * 60.0f;
    screenOff = false;
}

void ScreenOn()
{
	ithGpioSet(BL_GPIO_PIN);
	screenOff = false;
}

void ScreenOff()
{
	ithGpioClear(BL_GPIO_PIN);
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

}

void user_delete_init()
{
	pthread_t delete_det_p;			//线程ID
	pthread_attr_t delete_det_pat;
	pthread_attr_init(&delete_det_pat);
	pthread_attr_setdetachstate(&delete_det_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&delete_det_p,&delete_det_pat, user_delete_file, NULL);
}

bool UserTimeEnableMontion()
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

	//printf("UserTimeEnableMontion----------->%ld   %ld    %ld\n",tv_cur.tv_sec,tv_s_md.tv_sec,tv_e_md.tv_sec);
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
	UserPr2000SetStart(false);
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

