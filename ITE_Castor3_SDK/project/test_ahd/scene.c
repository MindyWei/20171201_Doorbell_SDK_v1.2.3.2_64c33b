#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "ite/itu.h"
#include "scene.h"
#include "project.h"
#include "user_function.h"

#define FPS_ENABLE
#define GESTURE_THRESHOLD 40
//#define DOUBLE_KEY_ENABLE

extern ITUActionFunction actionFunctions[];

//2018.3.28 my.wei add for UI command queue -
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
// command
typedef enum
{
    CMD_NONE,
    CMD_GOTO_LAYER,
    CMD_SET_VISIBLE,
    CMD_SET_ENABLE,
    CMD_EVENT_HANDLER,
    CMD_CHANGE_LANG,
    CMD_PREDRAW
} CommandID;
	
#define MAX_STRARG_LEN 32

typedef struct
{
    CommandID   id;
    int         arg1;
    int         arg2;
    char        strarg1[MAX_STRARG_LEN];
} Command;

#define MAX_COMMAND_QUEUE_SIZE      8
static mqd_t uiCmdQueue = -1;
#endif
//2018.3.28 my.wei add for UI command queue +

// scene
ITUScene theScene;
static SDL_Window *window;
static ITUSurface* screenSurf;
static int periodPerFrame;

//2018.3.28 my.wei add for UI command queue -
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
static void GotoLayer(char *layerName)
{
    ITULayer* destLayer = ituSceneFindWidget(&theScene, layerName);
    assert(destLayer);
    ituLayerGoto(destLayer);
}

static void WidgetSetVisible(char *widgetName, bool visible)
{
	ITUWidget* widgetTemp = ituSceneFindWidget(&theScene, widgetName);
	assert(widgetTemp);
	ituWidgetSetVisible(widgetTemp, visible);
}

static void WidgetSetEnable(char *widgetName, bool enable)
{
	ITUWidget* widgetTemp = ituSceneFindWidget(&theScene, widgetName);
	assert(widgetTemp);
	if(enable)
		ituWidgetEnable(widgetTemp);
	else
		ituWidgetDisable(widgetTemp);
}


static bool ProcessCommand(void)
{
    bool result = false;
    Command cmd;

    while (mq_receive(uiCmdQueue, (char*)&cmd, sizeof(Command), 0) > 0)
    {
        switch (cmd.id)
        {
        case CMD_GOTO_LAYER:
            GotoLayer(cmd.strarg1);
            break;

        case CMD_SET_VISIBLE:
            WidgetSetVisible(cmd.strarg1, cmd.arg1);
            break;

        case CMD_SET_ENABLE:
            WidgetSetEnable(cmd.strarg1, cmd.arg1);
            break;

        case CMD_EVENT_HANDLER:
            media_event_handler_ui();//my.wei mask for test ahd
            break;

        case CMD_CHANGE_LANG:
            ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.language, 0, 0);
            ituSceneUpdate(&theScene, ITU_EVENT_LAYOUT, 0, 0, 0);
            break;

        case CMD_PREDRAW:
            ituScenePreDraw(&theScene, screenSurf);
            break;
        }
        result = true;
    }
    return result;
}

void SceneGotoLayer(char *layerName)
{
    Command cmd;
	printf("SceneGotoLayer:%s\r\n", layerName);

    if (uiCmdQueue == -1)
        return;

    cmd.id     = CMD_GOTO_LAYER;
    strcpy(cmd.strarg1, layerName);
    mq_send(uiCmdQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneWidgetSetVisible(char *widgetName, bool visible)
{
    Command cmd;
	printf("%s:%s\r\n", __FUNCTION__, widgetName);

    if (uiCmdQueue == -1)
        return;

    cmd.id     = CMD_SET_VISIBLE;
	cmd.arg1   = visible;
    strcpy(cmd.strarg1, widgetName);
    mq_send(uiCmdQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneWidgetSetEnable(char *widgetName, bool enable)
{
    Command cmd;
	printf("%s:%s\r\n", __FUNCTION__, widgetName);

    if (uiCmdQueue == -1)
        return;

    cmd.id     = CMD_SET_ENABLE;
	cmd.arg1   = enable;
    strcpy(cmd.strarg1, widgetName);
    mq_send(uiCmdQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneEventHandler(void)
{
    Command cmd;
	printf("%s:\r\n", __FUNCTION__);

    if (uiCmdQueue == -1)
        return;

    cmd.id     = CMD_EVENT_HANDLER;
    mq_send(uiCmdQueue, (const char*)&cmd, sizeof (Command), 0);
}

#endif
//2018.3.28 my.wei add for UI command queue +

void SceneEnterVideoState(void)
{
        periodPerFrame = 33;
}

void SceneLeaveVideoState(void)
{
    periodPerFrame = MS_PER_FRAME;
}

void SceneInit(void)
{
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
    struct mq_attr attr; //2018.3.28 my.wei add for UI command queue
#endif

	window = SDL_CreateWindow("Template", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CFG_LCD_WIDTH, CFG_LCD_HEIGHT, 0);
	if (!window)
	{
		printf("Couldn't create window: %s\n", SDL_GetError());
		return;
	}

	// init itu
	ituLcdInit();

#ifdef CFG_M2D_ENABLE
	ituM2dInit();
	ituFrameFuncInit();
#else
	ituSWInit();
#endif

#ifdef CFG_ENABLE_ROTATE
	ituSceneSetRotation(&theScene, ITU_ROT_90, ithLcdGetWidth(), ithLcdGetHeight());
#endif

#ifdef CFG_PLAY_VIDEO_ON_BOOTING
#ifndef CFG_BOOT_VIDEO_ENABLE_WINDOW_MODE
#ifndef CFG_ENABLE_ROTATE
	PlayVideo(0, 0, ithLcdGetWidth(), ithLcdGetHeight(), CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
#else
	PlayVideo(0, 0, ithLcdGetHeight(), ithLcdGetWidth(), CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
#endif
#else
	PlayVideo(CFG_VIDEO_WINDOW_X_POS, CFG_VIDEO_WINDOW_Y_POS, CFG_VIDEO_WINDOW_WIDTH, CFG_VIDEO_WINDOW_HEIGHT, CFG_BOOT_VIDEO_BGCOLOR, CFG_BOOT_VIDEO_VOLUME);
#endif
	WaitPlayVideoFinish();
#endif

#ifdef CFG_PLAY_MJPEG_ON_BOOTING
#ifndef CFG_BOOT_VIDEO_ENABLE_WINDOW_MODE
#ifndef CFG_ENABLE_ROTATE
	PlayMjpeg(0, 0, ithLcdGetWidth(), ithLcdGetHeight(), CFG_BOOT_VIDEO_BGCOLOR, 0);
#else
	PlayMjpeg(0, 0, ithLcdGetHeight(), ithLcdGetWidth(), CFG_BOOT_VIDEO_BGCOLOR, 0);
#endif
#else
	PlayMjpeg(CFG_VIDEO_WINDOW_X_POS, CFG_VIDEO_WINDOW_Y_POS, CFG_VIDEO_WINDOW_WIDTH, CFG_VIDEO_WINDOW_HEIGHT, CFG_BOOT_VIDEO_BGCOLOR, 0);
#endif
	WaitPlayMjpegFinish();
#endif

	screenSurf = ituGetDisplaySurface();

	ituFtInit();
	ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/" CFG_FONT_FILENAME, ITU_GLYPH_8BPP);

	ituSceneInit(&theScene, NULL);
	ituSceneSetFunctionTable(&theScene, actionFunctions);

	//2018.3.28 my.wei add for UI command queue -
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    uiCmdQueue = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(uiCmdQueue != -1);
#endif	
	//2018.3.28 my.wei add for UI command queue +

	theScene.leftKey   = SDLK_LEFT;
	theScene.upKey     = SDLK_UP;
	theScene.rightKey  = SDLK_RIGHT;
	theScene.downKey   = SDLK_DOWN;

	periodPerFrame = MS_PER_FRAME;
}

void SceneExit(void)
{
	//2018.3.28 my.wei add for UI command queue -
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
    mq_close(uiCmdQueue);
    uiCmdQueue = -1;
#endif
	//2018.3.28 my.wei add for UI command queue +

	if (theScene.root)
	{
		ituSceneExit(&theScene);
	}
	ituFtExit();

#ifdef CFG_M2D_ENABLE
	ituM2dExit();
#ifdef CFG_VIDEO_ENABLE
	ituFrameFuncExit();
#endif // CFG_VIDEO_ENABLE
#else
	ituSWExit();
#endif // CFG_M2D_ENABLE

	SDL_DestroyWindow(window);
}

void SceneLoad(void)
{
	// load itu file
	ituSceneLoadFile(&theScene, CFG_PRIVATE_DRIVE ":/template.itu");
}

int SceneRun(void)
{
	SDL_Event ev, lastev;
	int done, delay, frames, lastx, lasty;
	uint32_t tick, dblclk, lasttick;

	/* Watch keystrokes */
	done = dblclk = frames = lasttick = lastx = lasty = 0;
#ifndef _WIN32
	AudioPlayKeySound(); // init i2s
#endif
	ituScenePreDraw(&theScene, screenSurf);

	user_vdp_power_on();

	while (!done)
	{
		bool result = false;
		
		//2018.3.28 my.wei add for UI command queue
#if defined(SCENE_MSG_CMD_QUEUE_ENABLE)
		ProcessCommand();
#endif
		tick = SDL_GetTicks();
		StorageCheck();
		event_process();

		frames++;

#if 0//def FPS_ENABLE
		if (tick - lasttick >= 1000)
		{
		#if 0
			if(sure_PR2000_lock())
				printf("------------->1\n");
			else
				printf("------------->0\n");
		#endif
			//UserCheckDiskSpace();
			printf("fps: %d\n", frames);
			frames = 0;
			lasttick = tick;
		}
#endif

		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_KEYDOWN:
				result = ituSceneUpdate(&theScene, ITU_EVENT_KEYDOWN, ev.key.keysym.sym, 0, 0);
#ifndef _WIN32
				if (result)
					AudioPlayKeySound();
#endif
				break;

			case SDL_KEYUP:
				result = ituSceneUpdate(&theScene, ITU_EVENT_KEYUP, ev.key.keysym.sym, 0, 0);
				break;

			case SDL_MOUSEMOTION:
				result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, ev.button.button, ev.button.x, ev.button.y);
				break;

			case SDL_MOUSEBUTTONDOWN:
			{
				uint32_t t = SDL_GetTicks();
				if (t - dblclk <= 300)
				{
					result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, ev.button.button, ev.button.x, ev.button.y);
					dblclk = 0;
				}
				else
				{
					printf("mouse: down %d, %d\n", ev.button.x, ev.button.y);
					result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
					dblclk = t;
					lastx = ev.button.x;
					lasty = ev.button.y;
				}
#ifndef _WIN32
				if (result)
					AudioPlayKeySound();
#endif
			}
			break;

			case SDL_MOUSEBUTTONUP:
				if (SDL_GetTicks() - dblclk <= 300)
				{
					int xdiff = abs(ev.button.x - lastx);
					int ydiff = abs(ev.button.y - lasty);

					if (xdiff >= GESTURE_THRESHOLD && xdiff > ydiff)
					{
						if (ev.button.x > lastx)
						{
							printf("mouse: slide to right\n");
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, xdiff, ev.button.x, ev.button.y);
						}
						else
						{
							printf("mouse: slide to left\n");
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, xdiff, ev.button.x, ev.button.y);
						}
					}
					else if (ydiff >= GESTURE_THRESHOLD)
					{
						if (ev.button.y > lasty)
						{
							printf("mouse: slide to down\n");
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.button.x, ev.button.y);
						}
						else
						{
							printf("mouse: slide to up\n");
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.button.x, ev.button.y);
						}
					}
				}

				printf("mouse: up %d, %d\n", ev.button.x, ev.button.y);
				result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);

				break;

			case SDL_FINGERDOWN:
				printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
				{
					
#if TEST_CAM
					break;
#endif
					if((cur_page == page_setting) || (cur_page == page_inter) || (cur_page == page_date) ||(cur_page == page_media))
						no_touch_start();
					if(cur_page == page_home && theConfig.lcdout)
						standby_mode_start();
					else if(cur_page == page_home && !theConfig.lcdout)
						no_touch_start();
					/*
					if(cur_page == page_monitor || cur_page == page_cctv)
					{
						if(cur_mon_state == MON_STATE || cur_mon_state == M_TALK_STATE ||cur_mon_state == CCTV_STATE)
						{
							monitor_start();
						}
					}
					*/
					if(standby_state)
					{
						event_go_home = true;
						break;
					}
						
					uint32_t t = SDL_GetTicks();
#ifdef DOUBLE_KEY_ENABLE
					if (t - dblclk <= 300)
					{
						result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, 1, ev.tfinger.x, ev.tfinger.y);
						dblclk = 0;
					}
					else
#endif
					{
						result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
						dblclk = t;
						lastx = ev.tfinger.x;
						lasty = ev.tfinger.y;
					}
#ifndef _WIN32
					if (result)
						AudioPlayKeySound();
#endif
				}
				break;

			case SDL_FINGERMOTION:
				printf("touch: move %d, %d\n", ev.tfinger.x, ev.tfinger.y);
				result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, 1, ev.tfinger.x, ev.tfinger.y);
				break;

			case SDL_FINGERUP:
				printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
				if (SDL_GetTicks() - dblclk <= 300)
				{
					int xdiff = abs(ev.tfinger.x - lastx);
					int ydiff = abs(ev.tfinger.y - lasty);

					if (xdiff >= GESTURE_THRESHOLD && xdiff > ydiff)
					{
						if (ev.tfinger.x > lastx)
						{
							printf("touch: slide to right %d %d\n", ev.tfinger.x, ev.tfinger.y);
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, xdiff, ev.tfinger.x, ev.tfinger.y);
						}
						else
						{
							printf("touch: slide to left %d %d\n", ev.tfinger.x, ev.tfinger.y);
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, xdiff, ev.tfinger.x, ev.tfinger.y);
						}
					}
					else if (ydiff >= GESTURE_THRESHOLD)
					{
						if (ev.tfinger.y > lasty)
						{
							printf("touch: slide to down %d %d\n", ev.tfinger.x, ev.tfinger.y);
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.tfinger.x, ev.tfinger.y);
						}
						else
						{
							printf("touch: slide to up %d %d\n", ev.tfinger.x, ev.tfinger.y);
							result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.tfinger.x, ev.tfinger.y);
						}
					}
				}

				result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);

				break;
				/*
				case SDL_SLIDEGESTURE:
				switch (ev.dgesture.gestureId)
				{
				case SDL_TG_LEFT:
				    printf("touch: slide to left\n");
				    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.dgesture.x, ev.dgesture.y);
				    break;

				case SDL_TG_UP:
				    printf("touch: slide to up\n");
				    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, 1, ev.dgesture.x, ev.dgesture.y);
				    break;

				case SDL_TG_RIGHT:
				    printf("touch: slide to right\n");
				    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.dgesture.x, ev.dgesture.y);
				    break;

				case SDL_TG_DOWN:
				    printf("touch: slide to down\n");
				    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, 1, ev.dgesture.x, ev.dgesture.y);
				    break;
				}
				break;
				*/
			case SDL_QUIT:
				done = 1;
				break;
			}
			lastev = ev;
		}

		result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
#ifndef _WIN32
		if (result)
#endif
		{
			ituSceneDraw(&theScene, screenSurf);
			ituFlip(screenSurf);
		}

		delay = periodPerFrame - (SDL_GetTicks() - tick);
		//printf("scene loop delay=%d\n", delay);
		if (delay > 0)
		{
			SDL_Delay(delay);
		}
		else
			sched_yield();

		//delay = 33 - (SDL_GetTicks() - tick);
		//if (delay > 0)
		//{
		//    SDL_Delay(delay);
		//}
	}

	//SDL_Quit();
	return 0;
}
