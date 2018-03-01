#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "ite/itu.h"
#include "scene.h"
#include "config.h"
#include "leaf/leaf_mediastream.h"


//#define FPS_ENABLE

#define GESTURE_THRESHOLD       40
//#define CFG_LOOP_DIALING        1
extern ITUActionFunction actionFunctions[];

// scene
ITUScene theScene;
static SDL_Window *window;
static ITUSurface* screenSurf;

bool networkReady;
// init call
LeafCall *call = NULL;

#ifdef CFG_LOOP_DIALING
    #define CFG_LOOP_DIALING_DELAY (10 * 1000 / 33) // 10 sec
    static bool loopDialing;
    static int loopDialingDelay;
	static int loopDialingTime;
	static int loopDialingCnt = 0;
#endif // CFG_LOOP_DIALING

static void CheckNetwork(void)
{
	if (networkReady && !NetworkIsReady())
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");
    }
	else if (!networkReady && NetworkIsReady())
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
    }
}

void SceneInit(void)
{
#ifdef CFG_LCD_ENABLE
    window = SDL_CreateWindow("Doorbell Lobby", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CFG_LCD_WIDTH, CFG_LCD_HEIGHT, 0);
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
	networkReady       = false;

#ifdef CFG_LOOP_DIALING
    loopDialing         = false;
    loopDialingDelay    = 0;
	loopDialingTime     = 0;
#endif // CFG_LOOP_DIALING

	//init leaf call
	call = leaf_init();
    leaf_set_play_level(call,theConfig.play_lev);
    leaf_set_ring_level(call,theConfig.ring_lev);
    leaf_set_rec_level(call,theConfig.rec_lev);    
#endif
}

void SceneExit(void)
{
#ifdef CFG_LCD_ENABLE
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
#endif
}

void SceneLoad(void)
{
#ifdef CFG_LCD_ENABLE
    // load itu file
    ituSceneLoadFile(&theScene, CFG_PRIVATE_DRIVE ":/doorbell_lobby_chs.itu");
#endif
}

int SceneRun(void)
{
    SDL_Event ev;
    int delay, frames, lastx, lasty;
    uint32_t tick, dblclk, lasttick;
	CALL_STATE call_state = CALL_IDLE, pre_call_state = CALL_IDLE;
#ifdef CFG_LCD_ENABLE
	ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
	ITULayer* userLayer = ituSceneFindWidget(&theScene, "userLayer");
    assert(callingLayer);
	assert(userLayer);
#endif

    /* Watch keystrokes */
    dblclk = frames = lasttick = lastx = lasty = 0;
    for (;;)
    {
        bool result = false;

#ifdef CFG_LCD_ENABLE
		CheckNetwork();
        tick = SDL_GetTicks();

        frames++;
        if (tick - lasttick >= 1000)
        {
            //printf("fps: %d\n", frames);
            frames = 0;
            lasttick = tick;
        }

#ifdef CFG_LOOP_DIALING
		if (loopDialingDelay >= CFG_LOOP_DIALING_DELAY)
		{
			NetworkCallSend(CALL_EARLYMEDIA);	
            if (NetworkCheckAck())
            {
            	loopDialingDelay = 0;
				loopDialingTime = 0;
				loopDialing = true;
				loopDialingCnt++;
				printf("######### loopDialingCnt = %d #############\n", loopDialingCnt);
                ituLayerGoto(callingLayer);
            }
		}
		else if(!loopDialing)
		{
			loopDialingDelay++;
		}
		else if (++loopDialingTime >= CFG_LOOP_DIALING_DELAY*6)
		{
			NetworkCallSend(CALL_STOP);
			if (NetworkCheckAck())
            {
            	loopDialing = false;
				leaf_stop_media_streams(call);
				NetworkCallIdle();
    			ituLayerGoto(userLayer);
			}
		}
#endif // CFG_LOOP_DIALING

		/*polling UDP call hand shaking from indoor*/
		call_state = NetworkCheckIncoming();
		if (call_state != pre_call_state)
		{
			switch (call_state)
			{
				case CALL_EARLYMEDIA:
					pre_call_state = call_state;
					break;

				case CALL_STOP:
					NetworkCallSend(CALL_ACK);
					leaf_stop_media_streams(call);
					NetworkCallIdle();
    				ituLayerGoto(userLayer);
					pre_call_state = call_state;
					break;

				case CALL_IDLE:
#ifdef CFG_LOOP_DIALING					
					loopDialingDelay = 0;
					loopDialing = false;
#endif					
					pre_call_state = call_state;
					break;
					
				default:
					pre_call_state = call_state;
					break;
			}
		}
		/*******************************/
		
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_KEYDOWN:
				switch (ev.key.keysym.sym)
                {
					case SDLK_RETURN:
					NetworkCallSend(CALL_EARLYMEDIA);	
					if (NetworkCheckAck())
                    	ituLayerGoto(callingLayer);
					break;

					case SDLK_SPACE:
					ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_END, "0");
				    break;
				}
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
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
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
                }
                break;

            case SDL_FINGERDOWN:
                printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                {
                    uint32_t t = SDL_GetTicks();
                    if (t - dblclk <= 600)
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = 0;
                    }
                    else
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = t;
                    }
                }
                break;

            case SDL_FINGERUP:
                printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
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
            }
        }
        
        result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
        //if (result)
        {
            ituSceneDraw(&theScene, screenSurf);
            ituFlip(screenSurf);
        }
#endif
        delay = 33 - (SDL_GetTicks() - tick);
        if (delay > 0)
        {
            SDL_Delay(delay);
        }
    }

	SDL_Quit();
    return 0;
}

