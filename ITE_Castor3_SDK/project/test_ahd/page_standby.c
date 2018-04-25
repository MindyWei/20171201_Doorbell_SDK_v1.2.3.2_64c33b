#include <assert.h>
#include "user_function.h"

static ITUIcon* ST_BG_MD_WIN;

static uint32_t md_start_time_t;
static bool cam_reboot = false;

static bool md_delay_over = false;

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

bool standby_init(ITUWidget* widget, char* param)
{
	cur_page  = page_standby;
	if (!ST_BG_MD_WIN)
	{
		ST_BG_MD_WIN = ituSceneFindWidget(&theScene, "ST_BG_MD_WIN");
		assert(ST_BG_MD_WIN);
		ituWidgetSetDraw(ST_BG_MD_WIN,VideoPlayerViewBackgroundDraw);
	}
	SceneEnterVideoState();		//ÇÐ»»Ö¡ÂÊ
	
	//md_delay_over = false;
#if TEST_CAM
	ituWidgetSetVisible(ST_BG_MD_WIN,true);
	cur_signal = 1;
	monitor_signal(cur_signal);
	PR2000_set_start();
	usleep(100*1000);
	gState = SEND_BEGIN;			//ÏÔÊ¾Í¼Ïñ±êÖ¾
#else
	standby_state = true;
	backlight_off();
	md_start_delay_start();
#endif
	//ithGpioClear(AUDIO_OUT);	
	return true;
}
void _md_start()
{
	printf("_md_start...................................................................\n");
	
	ithGpioClear(AUDIO_OUT);
	md_start_time_t = SDL_GetTicks();
	montion_enable = true;
	//clear_montion_start();
	ituWidgetSetVisible(ST_BG_MD_WIN,true);
	cur_signal = theConfig.mdcam+1;
	monitor_signal(cur_signal);
	PR2000_set_start();
	usleep(100*1000);
	gState = SEND_BEGIN;			//ÏÔÊ¾Í¼Ïñ±êÖ¾
	set_montion_pass_once();
	montion_start_again();
}
bool standby_time(ITUWidget* widget, char* param)
{
	if(!montion_enable && md_delay_over && !busy_over_3s)
	{
		if(!uart_is_busy())
		{
			if(theConfig.md && master_vdp)
			{
				if(theConfig.mdtime)
				{
					if(time_enable_montion())
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
			if(!time_enable_montion())
				montion_end();
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
		if(theConfig.mdcam == DOOR_1)
		{
			ithGpioSet(cam_gpio[DOOR_1][ON]);
			usleep(500*1000);
			ithGpioClear(cam_gpio[DOOR_1][ON]);
		}
		else if(theConfig.mdcam == DOOR_2)
		{
			ithGpioSet(cam_gpio[DOOR_2][ON]);
			usleep(500*1000);
			ithGpioClear(cam_gpio[DOOR_2][ON]);
		}
	}
	return true;
}

bool standby_leave(ITUWidget* widget, char* param)
{
	printf("standby_leave........................................1\n");
	if(montion_enable)
	{
		rec_start_time = 0;							//½áÊøÂ¼Ïñ
		cur_signal = 0;
		gState = SEND_STOP;							//Í£Ö¹Í¼Ïñ
		md_quit();
		SceneLeaveVideoState();						//ÇÐ»»Ö¡ÂÊ
		usleep(100*1000);
		PR2000_set_end();
	}
	montion_snap_reinit();
	standby_state = false;
	ithGpioSet(AUDIO_OUT);
	md_enable_reinit();
	return true;
}

