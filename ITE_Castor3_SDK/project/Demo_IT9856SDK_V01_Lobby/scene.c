#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "ite/itp.h"
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

//#define FPS_ENABLE

#define GESTURE_THRESHOLD       40
#define MAX_COMMAND_QUEUE_SIZE  8
#define WARNPLAY_DELAY          5
#define MS_PER_FRAME            33
#define PRDRAW_DELAY            2
#define CFG_CAMERA_TIMEOUT (35 * 1000 / 33) // 35 sec

extern ITUActionFunction actionFunctions[];
extern void resetScene(void);

static QuitValue quitValue;

// command
typedef enum
{
    CMD_NONE,
    CMD_CHANGE_SCENE,
    CMD_LOAD_SCENE,
    CMD_CALL_CONNECTED,
    CMD_CALL_RECEIVED,
    CMD_CALL_END,
    CMD_CALL_ERROR,
    CMD_OPENDOOR
} CommandID;

#define MAX_STRARG_LEN 32

typedef struct
{
    CommandID   id;
    int         arg1;
    int         arg2;
    char        strarg1[MAX_STRARG_LEN];
    char        strarg2[MAX_STRARG_LEN];
} Command;

static mqd_t commandQueue = -1;

// scene
ITUScene theScene;
static SDL_Window *window;
static ITUSurface* screenSurf;
static timer_t doorTimerId;
static bool isReady;
static int currCallID;
static char currRemoteAddr[16];
static bool callReceived, callConnected;
static int currCallingCount, currCameraCount;
static bool networkReady;
static int warnPlayDelay;
static int predrawDelay;
static int camera_timeout;

#ifdef CFG_LOOP_DIALING
    #define CFG_LOOP_DIALING_DELAY (10 * 1000 / 33) // 10 sec
    static bool loopDialing;
    static int loopDialingDelay;
#endif // CFG_LOOP_DIALING

static void CloseDoorHandler(timer_t timerid, int arg);

void SceneInit(void)
{
    struct mq_attr attr;

#ifdef CFG_LCD_ENABLE
    window = SDL_CreateWindow("Doorbell Lobby", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, CFG_LCD_WIDTH, CFG_LCD_HEIGHT, 0);
    if (!window)
    {
        printf("Couldn't create window: %s\n", SDL_GetError());
        return;
    }
#ifdef CFG_LCD_ENABLE
    // init itu
    ituLcdInit();
#endif

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

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    commandQueue = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(commandQueue != -1);

    isReady             = false;
    currCallID          = 0;
    callReceived        = false;
    callConnected       = false;
    currRemoteAddr[0]   = '\0';
    currCallingCount    = -1;
    currCameraCount     = 0;
	camera_timeout     = 0;
    networkReady        = false;
    warnPlayDelay       = 0;
    
    doorTimerId         = 0;
    timer_create(CLOCK_REALTIME, NULL, &doorTimerId);
    timer_connect(doorTimerId, CloseDoorHandler, 0);

#ifdef CFG_LOOP_DIALING
    loopDialing         = false;
    loopDialingDelay    = 0;
#endif // CFG_LOOP_DIALING
#endif
}

void SceneExit(void)
{
#ifdef CFG_LCD_ENABLE
    if (doorTimerId)
    {
        timer_delete(doorTimerId);
        doorTimerId = 0;
    }

    mq_close(commandQueue);
    commandQueue = -1;

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
    Command cmd;

    if (commandQueue == -1)
        return;

    isReady  = false;

    cmd.id = CMD_LOAD_SCENE;

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void SceneCallConnected(char* addr, int id)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_CONNECTED;
    cmd.arg1   = id;
    strcpy(cmd.strarg1, addr);

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void SceneCallReceived(char* username, char* addr, int id, int video)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_RECEIVED;
    strcpy(cmd.strarg1, username);
    strcpy(cmd.strarg2, addr);
    cmd.arg1   = id;

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void SceneCallEnd(char* addr, int id)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_END;
    strcpy(cmd.strarg1, addr);
    cmd.arg1   = id;

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void SceneCallError(char* addr, int id, int code)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_ERROR;
    strcpy(cmd.strarg1, addr);
    cmd.arg1   = id;
    cmd.arg2   = code;

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneOpenDoor(int updateUI)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_OPENDOOR;
    cmd.arg1   = updateUI;

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneSetRemoteIP(char* ip)
{
    if (ip)
        strcpy(currRemoteAddr, ip);
    else
        currRemoteAddr[0] = '\0';
}

bool SceneIsCallConnected(void)
{
    return callConnected;
}

void SceneHangUp(void)
{
    printf("SceneHangUp()\n");

    if (!callReceived)
    {
        EventWriteCalloutLog(currRemoteAddr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);
    }
    callReceived = false;
    callConnected = false;
    currCallID = 0;
    currRemoteAddr[0] = '\0';
    currCallingCount = -1;
    linphoneCastor3.calling_ip_count = 0;

    SceneContinuePlayGuardAlarm();

#ifdef CFG_LOOP_DIALING
    loopDialing         = false;
    loopDialingDelay    = 0;
#endif // CFG_LOOP_DIALING
}

static int SceneCallAllowCallWaiting(char* ip)
{
    int result = 0;

    if (!callReceived && !callConnected)
    {
        result = 1;
        goto end;
    }

    if (strcmp(theCenterInfo.ip, ip) == 0)
    {
        result = 1;
        goto end;
    }

end:
    printf("%s(%s): %d\n", __FUNCTION__, ip, result);
    return result;
}

void SceneContinuePlayGuardAlarm(void)
{
    if (GuardIsAlarming())
        warnPlayDelay = WARNPLAY_DELAY;
}

int SceneCameraIsViewing(void)
{
    if (currCameraCount > 0)
    {
        //currCameraCount = 0;
		return 1;
    }
	return 0;
}

static void LoadScene(void)
{
#ifdef CFG_LCD_ENABLE
    uint32_t tick1, tick2;
    char itupath[PATH_MAX];

    if (theScene.root)
    {
        ituSceneExit(&theScene);
    }

    // load itu file
    tick1 = SDL_GetTicks();

    strcpy(itupath, CFG_PRIVATE_DRIVE ":/");
    strcat(itupath, theConfig.itufile);
    ituSceneLoadFile(&theScene, itupath);

    tick2 = SDL_GetTicks();
    printf("itu loading time: %dms\n", tick2 - tick1);

    tick1 = tick2;
	resetScene();

    tick2 = SDL_GetTicks();
    printf("itu init time: %dms\n", tick2 - tick1);

    isReady         = true;

    linphonec_call_end_notify_callback                  = SceneCallEnd;
    linphonec_call_imcoming_received_notify_callback    = SceneCallReceived;
    linphonec_call_connected_notify_callback            = SceneCallConnected;
    linphonec_call_error_notify_callback                = SceneCallError;
    linphonec_call_allow_call_waiting_callback          = SceneCallAllowCallWaiting;
	linphonec_call_is_camera_viewing_callback           = SceneCameraIsViewing;
    
    predrawDelay = PRDRAW_DELAY;
#endif
}

static void CallConnected(char* addr, int id)
{
    printf("%s(%s, %d)\n", __FUNCTION__, addr, id);

	ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_CONNECTED, "0");

    strcpy(currRemoteAddr, addr);    
    
    callConnected = true;
    currCallID = id;
    LinphoneTerminateOthers(id);
    EventWriteCalloutLog(addr ? addr : currRemoteAddr, EVENT_CALLOUT_START);
}

static void CallReceived(char* username, char* addr, int id)
{
    printf("%s(%s,%s,%d): %d\n", __FUNCTION__, username, addr, id, isReady);

    ScreenSaverRefresh();

    if (strcmp(username, "camera") == 0)
    {
        DeviceInfo info;
        AddressBookGetDeviceInfo(&info, addr);
        if (info.type == DEVICE_MANAGER){
            sleep(1);//buff time for multicast
            LinphoneAnswer();
        }

        currCameraCount++;
		camera_timeout = 0;
    }
    else
    {
	    char buf[64];

        callReceived = true;
        currCameraCount = 0;
        currCallID = id;

        sprintf(buf, "%s %s %d", username, addr, id);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_INCOMMING, buf);
    }
}

static void CallEnd(char* addr, int id)
{
    printf("%s(%s, %d): %d\n", __FUNCTION__, addr, id, currCallID);

    if (id == currCallID || (currCallID == 0 && !callConnected))
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_END, "0");
        if (!callReceived)
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);

        currCallingCount = -1;
        linphoneCastor3.calling_ip_count = 0;

        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';

        SceneContinuePlayGuardAlarm();

    #ifdef CFG_LOOP_DIALING
        loopDialing         = false;
        loopDialingDelay    = 0;
    #endif // CFG_LOOP_DIALING
    }

    if (currCameraCount > 0)
        currCameraCount--;
	if(currCameraCount == 0)
		camera_timeout = 0;
}

static void CallError(char* addr, int id, int code)
{
    printf("%s(%s, %d, %d): %d\n", __FUNCTION__, addr, id, code, currCallingCount);

    if (currCallingCount == -1)
    {
        if (linphoneCastor3.calling_ip_count > 0)
            currCallingCount = linphoneCastor3.calling_ip_count;
        else
            currCallingCount = 1;
    }
    if (--currCallingCount == 0 || code == 600 || code == 486 || code == 603)
    {
        char buf[8];

        printf("CallError: %d\n", code);
        
        if (code != 487 && currRemoteAddr[0] != '\0')
        {
            if (code != 480 || currCallingCount == 0)
            {
                sprintf(buf, "%d", code);
                ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_ERROR, buf); 
            }
        }

        if (!callReceived)
            EventWriteCalloutLog(addr ? addr : currRemoteAddr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);

        if (linphoneCastor3.calling_ip_count > 0)
            LinphoneTerminateOthers(id);

        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        linphoneCastor3.calling_ip_count = 0;

        SceneContinuePlayGuardAlarm();

    #ifdef CFG_LOOP_DIALING
        loopDialing         = false;
        loopDialingDelay    = 0;
    #endif // CFG_LOOP_DIALING
    }
}

static void CloseDoorHandler(timer_t timerid, int arg)
{
    if (PeripheralDoorIsOpened())
        PeripheralCloseDoor();
}

static void OpenDoor(int updateUI)
{
    struct itimerspec value;

    value.it_value.tv_sec   = theConfig.door_delay;
    value.it_value.tv_nsec  = 0;
    value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
    
    timer_settime(doorTimerId, 0, &value, NULL);

    if (updateUI)
    {
        if (!callReceived && !callConnected && currRemoteAddr[0] == '\0')
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_OPEN_DOOR, "0"); 
    }
}

static void ProcessCommand(void)
{
    Command cmd;

    while (mq_receive(commandQueue, (char*)&cmd, sizeof(Command), 0) > 0)
    {
        switch (cmd.id)
        {
        case CMD_LOAD_SCENE:
            LoadScene();
            break;

        case CMD_CALL_CONNECTED:
            CallConnected(cmd.strarg1, cmd.arg1);
            break;

        case CMD_CALL_RECEIVED:
            CallReceived(cmd.strarg1, cmd.strarg2, cmd.arg1);
            break;

        case CMD_CALL_END:
            CallEnd(cmd.strarg1, cmd.arg1);
            break;

        case CMD_CALL_ERROR:
            CallError(cmd.strarg1, cmd.arg1, cmd.arg2);
            break;

        case CMD_OPENDOOR:
            OpenDoor(cmd.arg1);
            break;
        }
    }
}

static bool CheckQuitValue(void)
{
    if (quitValue)
    {
        if (ScreenSaverIsScreenSaving() && theConfig.screensaver_type == SCREENSAVER_BLANK)
            ScreenSaverRefresh();

        if (quitValue == QUIT_UPGRADE_ADDRESSBOOK || quitValue == QUIT_UPGRADE_CARDLIST)
        {
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_UPGRADE, "1");
        }
        else
            return true;
    }
    return false;
}

static void CheckGuard(void)
{
    int ret;

    if (!isReady)
        return;

    ret = GuardCheck();

    if (GuardIsAlarming())
    {
		ScreenSaverRefresh();
	
        if (ret == 0)
        {
            LinphoneStopSound();
            GuardSetAlarming(false);
        }
    }
    else
    {
        if (ret)
        {
            if (ret < 0)
            {
                LinphonePlayWarnSound();
            }
            GuardSetAlarming(true);
        }
    }

    if (warnPlayDelay > 0)
    {
        if (--warnPlayDelay == 0)
        {
            if (GuardIsAlarming())
                LinphonePlayWarnSound();
        }
    }
}

static void CheckNetwork(void)
{
    if (ScreenSaverIsScreenSaving())
        return;

	if (networkReady && !NetworkIsReady())
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");
        networkReady = false;
    }
	else if (!networkReady && NetworkIsReady())
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
        networkReady = true;
    }
}

int SceneRun(void)
{
    SDL_Event ev;
    int delay, frames, lastx, lasty;
    uint32_t tick, dblclk, lasttick;

    /* Watch keystrokes */
    dblclk = frames = lasttick = lastx = lasty = 0;
    for (;;)
    {
        bool result = false;

        if (CheckQuitValue())
            break;

        ProcessCommand();
        CheckGuard();
        CheckNetwork();

        tick = SDL_GetTicks();

    #ifdef CFG_LOOP_DIALING
        if (!loopDialing && !callReceived && !callConnected && currRemoteAddr[0] == '\0')
        {
            if (++loopDialingDelay >= CFG_LOOP_DIALING_DELAY)
            {
                char buf[128];
                char *token, *saveptr;
                DeviceInfo info;

                memset(&info, 0, sizeof(DeviceInfo));

                strcpy(buf, CFG_LOOP_DIALING_ADDRESS);
                token = strtok_r(buf, "-", &saveptr);
                if (token)
                {
                    info.area[0] = token[0];
                    info.area[1] = token[1];
                    info.area[2] = '\0';

                    token = strtok_r(NULL, "-", &saveptr);
                    if (token)
                    {
                        info.building[0] = token[0];
                        info.building[1] = token[1];
                        info.building[2] = '\0';

                        token = strtok_r(NULL, "-", &saveptr);
                        if (token)
                        {
                            info.unit[0] = token[0];
                            info.unit[1] = token[1];
                            info.unit[2] = '\0';

                            token = strtok_r(NULL, "-", &saveptr);
                            if (token)
                            {
                                info.floor[0] = token[0];
                                info.floor[1] = token[1];
                                info.floor[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    info.room[0] = token[0];
                                    info.room[1] = token[1];
                                    info.room[2] = '\0';
                                }
                            }
                        }
                    }
                }
                if (CallingCall(&info))
                {
                     // calling fail
                     LinphonePlayHintSound(HINT_SOUND_ROOM_NOT_EXIST);
                }
                else
                {
	                ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
                    assert(callingLayer);
                    ituLayerGoto(callingLayer);
                }
                loopDialingDelay = 0;
            }
        }
    #endif // CFG_LOOP_DIALING

    #ifdef FPS_ENABLE
        frames++;
        if (tick - lasttick >= 1000)
        {
            printf("fps: %d\n", frames);
            frames = 0;
            lasttick = tick;
        }
    #endif // FPS_ENABLE

		//check camera viewing timeout
		if (currCameraCount > 0)
        {
			if(++camera_timeout >= CFG_CAMERA_TIMEOUT)
			{
				LinphoneTerminate();
				SceneHangUp();
				currCameraCount = 0;
				camera_timeout = 0;
			}
        }
#ifdef CFG_LCD_ENABLE
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                ScreenSaverRefresh();
                if (currCameraCount > 0)
                {
                    LinphoneTerminate();
                    SceneHangUp();
                    currCameraCount = 0;
                }
                switch (ev.key.keysym.sym)
                {
				case SDLK_RETURN:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_STAR, (int)"*", 0, 0);  // *
					break;
				case SDLK_SPACE:
					result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_SHARP, (int)"#", 0, 0);  // #
				    break;
				case SDLK_0:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"0", 0, 0);  // 0
				    break;
				case SDLK_1:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"1", 0, 0);  // 1
				    break;
				case SDLK_2:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"2", 0, 0);  // 2
				    break;
				case SDLK_3:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"3", 0, 0);  // 3
				    break;
				case SDLK_4:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"4", 0, 0);  // 4
				    break;
				case SDLK_5:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"5", 0, 0);  // 5
				    break;
				case SDLK_6:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"6", 0, 0);  // 6
				    break;
				case SDLK_7:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"7", 0, 0);  // 7
				    break;
				case SDLK_8:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"8", 0, 0);  // 8
				    break;
				case SDLK_9:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"9", 0, 0);  // 9
				    break;
				case SDLK_a:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"a", 0, 0);  // a
				    break;
				case SDLK_b:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"b", 0, 0);  // b
				    break;
				case SDLK_c:
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_NUMBER, (int)"c", 0, 0);  // c
				    break;
				case SDLK_d:
                #ifdef CFG_SCREENSHOT_ENABLE
                    Screenshot(screenSurf);
                #else
                    result = ituSceneUpdate(&theScene, EVENT_CUSTOM_KEY_MANAGER, (int)"0", 0, 0);  // d
                #endif // CFG_SCREENSHOT_ENABLE
				    break;

            #ifdef _WIN32
                case SDLK_e:
                    {
                        char buf[8];
                        static bool netOn = false;
                        netOn = !netOn;
                        sprintf(buf, "%d\0", netOn);
                        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, buf);
                        result = true;
                    }
                    break;

                case SDLK_f:
                    SnapshotTake();
                    break;

            #endif // _WIN32
                }
                if (result && !ScreenIsOff() && currRemoteAddr[0] == '\0')
                    LinphonePlayKeySound();

                break;

            case SDL_KEYUP:
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYUP, ev.key.keysym.sym, 0, 0);
                break;

        #ifdef CFG_TOUCH_ENABLE
            case SDL_MOUSEMOTION:
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, ev.button.button, ev.button.x, ev.button.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
                ScreenSaverRefresh();
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

                        if (result && !ScreenIsOff())
                            LinphonePlayKeySound();
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
                ScreenSaverRefresh();
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
                        if (result && !ScreenIsOff())
                            LinphonePlayKeySound();
                    }
                }
                break;

            case SDL_FINGERUP:
                printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_SLIDEGESTURE:
                ScreenSaverRefresh();
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
            #endif // CFG_TOUCH_ENABLE
            }
        }

        if (callReceived || callConnected || currRemoteAddr[0] != '\0')
            ScreenSaverRefresh();


        if (PeripheralDoorIsOpened())
        {
            ScreenSaverRefresh();
            ituSceneDraw(&theScene, screenSurf);
            ituFlip(screenSurf);
        }


        if (!ScreenIsOff())
        {
            result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
            //printf("%d\n", result);
        #ifndef _WIN32
            if (result)
        #endif
            {
                ituSceneDraw(&theScene, screenSurf);
                ituFlip(screenSurf);

                if (predrawDelay > 0)
                {
                    predrawDelay--;
                    if (predrawDelay == 0)
                        ituScenePreDraw(&theScene, screenSurf);
                }
            }
    
            if (theConfig.screensaver_type != SCREENSAVER_NONE &&
                ScreenSaverCheck())
            {
                ituSceneSendEvent(&theScene, EVENT_CUSTOM_SCREENSAVER, "0");
                if (theConfig.screensaver_type == SCREENSAVER_BLANK)
                {
                    // have a change to flush action commands
                    ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);

                    // draw black screen
                    ituSceneDraw(&theScene, screenSurf);
                    ituFlip(screenSurf);
                    
                    ScreenOff();
                }
            }
        }
#endif        
        delay = MS_PER_FRAME - (SDL_GetTicks() - tick);
        //printf("scene loop delay=%d\n", delay);
        if (delay > 0)
        {
            SDL_Delay(delay);
        }
        else
            SDL_Delay(1);
    }

    return quitValue;
}

void SceneQuit(QuitValue value)
{
    quitValue = value;
}

QuitValue SceneGetQuitValue(void)
{
    return quitValue;
}
