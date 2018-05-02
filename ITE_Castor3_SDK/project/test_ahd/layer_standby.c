#include <assert.h>
#include "user_function.h"

static ITUIcon* standbyMotionDectionBackground;

static uint32_t md_start_time_t;
static bool cam_reboot = false;

static bool md_delay_over = false;
static uint32_t standbyLastTick;

void set_md_delay_over()
{
	md_delay_over = true;
}

void clear_md_delay_over()
{
	md_delay_over = false;
}

void reset_md_start_time_t()
{
	md_start_time_t = SDL_GetTicks();
}

static void VideoPlayerViewBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	ITURectangle* rect = (ITURectangle*) &widget->rect;
	destx = rect->x + x;
	desty = rect->y + y;
	ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}

bool StandbyOnEnter(ITUWidget* widget, char* param)
{
	printf("standby page enter\r\n");
	standbyLastTick = SDL_GetTicks();
	cur_page  = page_standby;
	if (!standbyMotionDectionBackground)
	{
		standbyMotionDectionBackground = ituSceneFindWidget(&theScene, "standbyMotionDectionBackground");
		assert(standbyMotionDectionBackground);
		ituWidgetSetDraw(standbyMotionDectionBackground,VideoPlayerViewBackgroundDraw);
	}
	SceneEnterVideoState();
	
#if TEST_CAM
	ituWidgetSetVisible(standbyMotionDectionBackground,true);
	cur_signal = 1;
	PR2000_set_start();
	usleep(100*1000);
	gState = SEND_BEGIN;
#else
	ScreenOff();
	md_start_delay_start();
#endif
	return true;
}
static void _md_start()
{
	printf("_md_start...................................................................\n");
	
	ithGpioClear(AUDIO_OUT);
	md_start_time_t = SDL_GetTicks();
	montion_enable = true;
	ituWidgetSetVisible(standbyMotionDectionBackground,true);
	cur_signal = theConfig.mdcam+1;
	PR2000_set_start();
	usleep(100*1000);
	gState = SEND_BEGIN;
	set_montion_pass_once();
	montion_start_again();
}

bool StandbyOnTimer(ITUWidget* widget, char* param)
{
	uint32_t diff, tick = SDL_GetTicks();
	
	if (tick >= standbyLastTick)
	{
		diff = tick - standbyLastTick;
	}
	else
	{
		diff = 0xFFFFFFFF - standbyLastTick + tick;
	}
	
	if (diff >= 1000)
	{
		standbyLastTick = tick;
	}

	if(!montion_enable && md_delay_over && !busy_over_3s)
	{
		if(1)
		{
			if(theConfig.md)
			{
				if(theConfig.mdtime)
				{
					if(UserTimeEnableMontion())
						_md_start();
				}		
				else
					_md_start();
			}
		}
	}
	if(montion_enable)
	{
		if(theConfig.mdtime)
		{
			if(!UserTimeEnableMontion())
				UserMotionEnd();
		}
	}
	
	if(montion_enable)
	{
		uint32_t tick = SDL_GetTicks();
		uint32_t diff = tick - md_start_time_t;
		if(diff > 60*60*1000)
		{
			md_start_time_t = tick;
			cam_reboot = true;
		}
	}

	if(cam_reboot)
	{
		cam_reboot = false;
		currCam = currCam ? 0 : 1;
		UserCameraSwitch(currCam);
		usleep(500000);
		currCam = currCam ? 0 : 1;
		UserCameraSwitch(currCam);	
	}
	return false;
}

bool StandbyOnLeave(ITUWidget* widget, char* param)
{
	printf("standby_leave........................................1\n");
	if(montion_enable)
	{
		rec_start_time = 0;
		cur_signal = 0;
		gState = SEND_STOP;
		md_quit();
		SceneLeaveVideoState();
		usleep(100*1000);
		PR2000_set_end();
	}
	UserTimerMotionSnapReinit();
	ithGpioSet(AUDIO_OUT);
	md_enable_reinit();
	ScreenOn();	
	return true;
}

