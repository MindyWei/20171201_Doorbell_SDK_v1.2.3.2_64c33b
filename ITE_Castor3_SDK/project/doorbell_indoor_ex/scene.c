#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "ite/itu.h"
#include "config.h"
#include "scene.h"
#include "leaf/leaf_mediastream.h"

#define GESTURE_THRESHOLD 40

extern ITUActionFunction actionFunctions[];

ITUScene theScene;
static SDL_Window *window;
static ITUSurface* screenSurf;
static ITULayer* calledLayer;
static ITULayer* mainMenuLayer;

static int callingCnt = 0;
static bool inVideoState;
static int periodPerFrame;

// init call
//LeafCall *call = NULL;

void SceneEnterVideoState(int timePerFrm)
{
    if (inVideoState)
    {
        return;
    }

    ituFrameFuncInit();
    screenSurf   = ituGetDisplaySurface();
    inVideoState = true;
    if (timePerFrm != 0)
        periodPerFrame = timePerFrm;
}

void SceneLeaveVideoState(void)
{
    if (!inVideoState)
    {
        return;
    }

    ituFrameFuncExit();
    ituLcdInit();

    #ifdef CFG_M2D_ENABLE
    ituM2dInit();
    #else
    ituSWInit();
    #endif

    screenSurf   = ituGetDisplaySurface();
    periodPerFrame = MS_PER_FRAME;
    inVideoState = false;
}

void SceneInit(void)
{
    window = SDL_CreateWindow("Doorbell Indoor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CFG_LCD_WIDTH, CFG_LCD_HEIGHT, 0);
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

    screenSurf = ituGetDisplaySurface();

    ituFtInit();
    ituFtLoadFont(0, CFG_PRIVATE_DRIVE ":/font/" CFG_FONT_FILENAME, ITU_GLYPH_8BPP);

    ituSceneInit(&theScene, NULL);
	ituSceneSetFunctionTable(&theScene, actionFunctions);

    theScene.leftKey   = SDLK_LEFT;
    theScene.upKey     = SDLK_UP;
    theScene.rightKey  = SDLK_RIGHT;
    theScene.downKey   = SDLK_DOWN;
    
    LeafInit();
	//init leaf call
	//call = leaf_init();
    //leaf_set_play_level(call,theConfig.play_lev);
    //leaf_set_ring_level(call,theConfig.ring_lev);
    //leaf_set_rec_level(call,theConfig.rec_lev);

    inVideoState            = false;
    periodPerFrame          = MS_PER_FRAME;
}

void SceneExit(void)
{
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
}

void SceneLoad(void)
{
    // load itu file
    ituSceneLoadFile(&theScene, CFG_PRIVATE_DRIVE ":/doorbell_indoor.itu");
}

int SceneRun(void)
{
    SDL_Event ev, lastev;
    int done, delay, frames, lastx, lasty;
    uint32_t tick, dblclk, lasttick;
	CALL_STATE call_state = CALL_IDLE, pre_call_state = CALL_IDLE;
    //uint32_t gotoTime, lastGotoTime = 0;

    /* Watch keystrokes */
    done = dblclk = frames = lasttick = lastx = lasty = 0;
    while (!done)
    {
        bool result = false;

        tick = SDL_GetTicks();
        frames++;
        if (tick - lasttick >= 1000)
        {
            //printf("fps: %d\n", frames);
            frames = 0;
            lasttick = tick;
        }

		/*polling UDP call hand shaking from indoor*/
		if(NetworkIsReady())
			call_state = NetworkCheckIncoming();
		if (call_state != pre_call_state)
		{
			switch (call_state)
			{
				case CALL_EARLYMEDIA:
					NetworkCallSend(CALL_ACK);
					if (!calledLayer)
					{
						calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
						assert(calledLayer);
					}
    				ituLayerGoto(calledLayer);
					pre_call_state = call_state;
					printf("###callingCnt = %d ###\n", callingCnt++);
					break;

				case CALL_STOP:
					NetworkCallSend(CALL_ACK);
					//leaf_stop_media_streams(call);
                    LeafTerminate();
					NetworkCallIdle();
					if (!mainMenuLayer)
					{
						mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
                    	assert(mainMenuLayer);
					}
                    ituLayerGoto(mainMenuLayer);
					pre_call_state = call_state;
					break;

				default:
					pre_call_state = call_state;
					break;
			}
		}
		/*******************************/

        //gotoTime = SDL_GetTicks();

        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYDOWN, ev.key.keysym.sym, 0, 0);
                if (result)
                    LeafPlayKeySound();
                
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
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
                        dblclk = t;
                        lastx = ev.button.x;
                        lasty = ev.button.y;
                        
                        if (result)
                            LeafPlayKeySound();                        
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (SDL_GetTicks() - dblclk <= 300)
                {
                    int xdiff = abs(ev.button.x - lastx);
                    int ydiff = abs(ev.button.y - lasty);

                    if (xdiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.button.x > lastx)
                        {
                            printf("mouse: slide to right\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.button.x, ev.button.y);
                        }
                        else
                        {
                            printf("mouse: slide to left\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.button.x, ev.button.y);
                        }
                    }
                    else if (ydiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.button.y > lasty)
                        {
                            printf("mouse: slide to down\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, 1, ev.button.x, ev.button.y);
                        }
                        else
                        {
                            printf("mouse: slide to up\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, 1, ev.button.x, ev.button.y);
                        }
                    }
                    else
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                    }
                }
                else
                {
                    result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                }
                break;

            case SDL_FINGERDOWN:
                printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                {
                    uint32_t t = SDL_GetTicks();
                    if (t - dblclk <= 300)
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = 0;
                    }
                    else
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = t;
                        lastx = ev.tfinger.x;
                        lasty = ev.tfinger.y;
                        
                        if (result)
                            LeafPlayKeySound();                        
                    }
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

                    if (lastev.type != SDL_SLIDEGESTURE)
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                    }
                }
                else
                {
                    result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                }
                break;

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

            case SDL_QUIT:
                done = 1;
                break;
            }
            lastev = ev;
        }

        result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
        if (result)
        {
            ituSceneDraw(&theScene, screenSurf);
            ituFlip(screenSurf);
        }

        //if (gotoTime - lastGotoTime > 10000)
        //{
        //    ITULayer* calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
        //    //assert(calledLayer);

        //    lastGotoTime = gotoTime;
        //    ituLayerGoto(calledLayer);
        //}

        delay = periodPerFrame - (SDL_GetTicks() - tick);
        if (delay > 0)
        {
            SDL_Delay(delay);
        }
        else
            sched_yield();
    }

    SDL_Quit();
    return (0);
}
