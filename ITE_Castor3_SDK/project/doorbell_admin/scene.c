#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

#ifdef _WIN32
    #include <crtdbg.h>
#endif

#define FPS_ENABLE
//#define DOUBLE_KEY_ENABLE

#define GESTURE_THRESHOLD           40
#define MAX_COMMAND_QUEUE_SIZE      8
#define MS_PER_FRAME                17
#define MAX_MISSED_CALL_COUNT       999
#define MOUSEDOWN_LONGPRESS_DELAY   1000
#define PRDRAW_DELAY                2

extern ITUActionFunction actionFunctions[];
extern void resetScene(void);
extern bool captureRecording, IPCamStreaming;

// status
static bool callReceived;
static bool callConnected;
static bool keypadEnabled;
static bool networkReady;
static int guardMsgCount;
static QuitValue quitValue;
static time_t lastLogTime;
static bool cameraViewing;
static DoNotDisturbMode lastDoNotDisturbMode;
static bool inVideoState;
SPOTTED_STATUS callSpottedStatus = SPOTTED_NONE;
static int predrawDelay;

// command
typedef enum
{
    CMD_NONE,
    CMD_LOAD_SCENE,
    CMD_CALL_CONNECTED,
    CMD_CALL_RECEIVED,
    CMD_CALL_SPOTTED,
    CMD_CALL_END,
    CMD_CALL_ERROR,
	CMD_CALL_FIRST_SNAPSHOT,
    CMD_UPDATE_INDICATORS,
    CMD_GOTO_MAINMENU,
    CMD_CHANGE_LANG
} CommandID;

#define MAX_STRARG_LEN 32

typedef struct
{
    CommandID   id;
    int         arg1;
    int         arg2;
    char        strarg1[MAX_STRARG_LEN];
} Command;

static mqd_t commandQueue = -1;

// scene
ITUScene theScene;
static SDL_Window *window;
static ITUSurface* screenSurf;
static bool isReady;
static int currCallID;
static char currRemoteAddr[16];
static int pendingCallID;
static char pendingRemoteAddr[16];
static int currCallingCount;
static int periodPerFrame;

void SceneInit(void)
{
    struct mq_attr attr;

    window = SDL_CreateWindow("Doorbell Admin", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ithLcdGetWidth(), ithLcdGetHeight(), 0);
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

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    commandQueue = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(commandQueue != -1);

    isReady                 = false;
    callReceived            = false;
    callConnected           = false;
    networkReady            = false;
    guardMsgCount           = 0;
    currCallingCount        = -1;
    currCallID              = 0;
    currRemoteAddr[0]       = '\0';
    pendingCallID           = 0;
    pendingRemoteAddr[0]    = '\0';
    cameraViewing           = false;
    lastDoNotDisturbMode    = DONOT_DISTURB_NONE;
    inVideoState            = true;
    keypadEnabled           = false;
    periodPerFrame          = MS_PER_FRAME;
}

void SceneExit(void)
{
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
    strcpy(cmd.strarg1, addr);
    cmd.arg1   = id;
    cmd.arg2   = video;

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

void SceneUpdateIndicators(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_UPDATE_INDICATORS;
    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneGotoMainMenu(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_GOTO_MAINMENU;
    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

void SceneChangeLanguage(void)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CHANGE_LANG;
    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static int SceneCallOnDoNotDisturb(char* ip)
{
    int result = 0;

    if (strcmp(theCenterInfo.ip, ip) == 0)
        result = 1;

    printf("%s(%s): %d\n", __FUNCTION__, ip, result);
    return result;
}

static int SceneCallAllowCallWaiting(char* ip)
{
    int result = 0;

    if (!callReceived && !callConnected)
    {
        result = 1;
        goto end;
    }

    if (SceneCameraIsViewing())
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

void SceneCameraSetViewing(bool viewing)
{
    cameraViewing = viewing;

    if (!viewing)
    {
        SceneSetRemoteIP(NULL);
    }
}

void SceneEnableKeypad(bool en)
{
    keypadEnabled = en;
}

int SceneCameraIsViewing(void)
{
    return cameraViewing ? 1 : 0;
}

void SceneSetReady(bool ready)
{
    isReady = ready;
}

static void SceneNewLog(time_t start_time)
{
    lastLogTime = start_time;
}

int SceneCallSpottedOrNot(char* addr, int id, int video)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    callSpottedStatus = SPOTTED_INIT;
    cmd.id     = CMD_CALL_SPOTTED;
    strcpy(cmd.strarg1, addr);
    cmd.arg1   = id;
    cmd.arg2   = video;
    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);

    while(callSpottedStatus == SPOTTED_INIT)
    {
        usleep(10000);
    }    

    if(callSpottedStatus == SPOTTED_ALLOW)
    {
        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        return true;
    }    
    else if(callSpottedStatus == SPOTTED_REJECT)
        return false;
}

void SceneCameraConnectError(bool bError)
{
    if(bError && captureRecording)
        captureRecording = false;
}

void SceneIPCamStreamingStatus(char bStatus)
{
    if(bStatus)
        IPCamStreaming = true;
    else
        IPCamStreaming = false;
}

void SceneCaptureRecordStatus(char bStatus)
{
    if(bStatus)
        captureRecording = true;
    else
        captureRecording = false;
}

static void SceneCallFirstSnapshot(char* addr)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_FIRST_SNAPSHOT;
    strcpy(cmd.strarg1, addr);

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void LoadScene(void)
{
    uint32_t tick1, tick2;

    if (theScene.root)
    {
        ituSceneExit(&theScene);
    }

    // load itu file
    tick1 = SDL_GetTicks();

    ituSceneLoadFile(&theScene, CFG_PRIVATE_DRIVE ":/doorbell_admin.itu");

    tick2 = SDL_GetTicks();
    printf("itu loading time: %dms\n", tick2 - tick1);

    tick1 = tick2;
	resetScene();
    ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.lang, 0, 0);

    tick2 = SDL_GetTicks();
    printf("itu init time: %dms\n", tick2 - tick1);

    linphonec_call_end_notify_callback                  = SceneCallEnd;
    linphonec_call_imcoming_received_notify_callback    = SceneCallReceived;
    linphonec_call_connected_notify_callback            = SceneCallConnected;
    linphonec_call_error_notify_callback                = SceneCallError;
    linphonec_call_on_do_not_disturb_callback           = SceneCallOnDoNotDisturb;
    linphonec_call_allow_call_waiting_callback          = SceneCallAllowCallWaiting;
    linphonec_call_is_camera_viewing_callback           = SceneCameraIsViewing;
    linphonec_new_log_notify_callback                   = SceneNewLog;
    linphonec_call_is_allowing_spotted_callback         = SceneCallSpottedOrNot;
    linphonec_call_camera_connect_error_callback        = SceneCameraConnectError;
    linphonec_call_ipcam_streaming_status_callback      = SceneIPCamStreamingStatus;
	linphonec_call_capture_record_status_callback       = SceneCaptureRecordStatus;
    linphonec_call_first_snapshot_callback              = SceneCallFirstSnapshot;

    predrawDelay = PRDRAW_DELAY;
}

#if (CFG_CHIP_FAMILY == 9850)
void SceneEnterVideoState(int timePerFrm)
{
    if (inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    ituFrameFuncInit();
    screenSurf   = ituGetDisplaySurface();
    inVideoState = true;
    if (timePerFrm != 0)
        periodPerFrame = timePerFrm;
#endif
}

void SceneLeaveVideoState(void)
{
    if (!inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    ituFrameFuncExit();
    ituLcdInit();

    #ifdef CFG_M2D_ENABLE
    ituM2dInit();
    #else
    ituSWInit();
    #endif

    screenSurf   = ituGetDisplaySurface();
    periodPerFrame = MS_PER_FRAME;
#endif
    inVideoState = false;
}
#else
void SceneEnterVideoState(void)
{
    if (inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    ituFrameFuncInit();
    screenSurf   = ituGetDisplaySurface();
    inVideoState = true;
#endif
}

void SceneLeaveVideoState(void)
{
    if (!inVideoState)
    {
        return;
    }

#ifndef DISABLE_SWITCH_VIDEO_STATE
    ituFrameFuncExit();
    ituLcdInit();

    #ifdef CFG_M2D_ENABLE
    ituM2dInit();
    #else
    ituSWInit();
    #endif

    screenSurf   = ituGetDisplaySurface();
#endif
    inVideoState = false;
}
#endif

static void CallConnected(char* addr, int id)
{
    printf("%s(%s, %d)\n", __FUNCTION__, addr, id);

    if (cameraViewing)
    {
        LinphoneMuteMic();
    }
    else if (!LinphoneIsInDoNotDisturb())
    {
        char buf[32];

        sprintf(buf, "%d", id);

	    ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_CONNECTED, buf);
        EventWriteCalloutLog(addr, EVENT_CALLOUT_START);
    }

    strcpy(currRemoteAddr, addr);

    callConnected = true;
    currCallID = id;
    LinphoneTerminateOthers(id);
}

static void CallReceived(char* addr, int id, int video)
{
    char buf[32];

    printf("%s(%s, %d, %d)\n", __FUNCTION__, addr, id, video);

    if (!isReady)
        return;

    ScreenSaverRefresh();

    sprintf(buf, "%s %d %d", addr, id, video);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_INCOMMING, buf);

    if (callConnected)
    {
        pendingCallID = id;
        strcpy(pendingRemoteAddr, addr);
	}
    else
    {
        callReceived = true;
        currCallID = id;
        strcpy(currRemoteAddr, addr);
    }
}

static void CallSpotted(char* addr, int id, int video)
{
    char buf[32];
    printf("%s(%s, %d, %d)\n", __FUNCTION__, addr, id, video);
    sprintf(buf, "%s %d %d", addr, id, video);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_INCOMMING, buf);
}

static void CallEnd(char* addr, int id)
{
    printf("%s(%s, %d): %d\n", __FUNCTION__, addr, id, currCallID);

    if (id == currCallID || (currCallID == 0 && !callConnected))
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_END, "0");
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
		if(callConnected)
        {
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);
		}
        currCallingCount = -1;

        if (linphoneCastor3.calling_ip_count > 0)
            LinphoneTerminateOthers(id);
        
        linphoneCastor3.calling_ip_count = 0;
        if (id == linphoneCastor3.camera_id)
        {
            linphoneCastor3.camera_id = 0;
            cameraViewing = false;
        }

        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
    }
}

static void CallError(char* addr, int id, int code)
{
    printf("%s(%s, %d, %d): %d\n", __FUNCTION__, addr, id, code, currCallingCount);
    
    if(captureRecording && code == 408)
        captureRecording = false;
    
    if (currCallingCount == -1)
    {
        if (linphoneCastor3.calling_ip_count > 0)
            currCallingCount = linphoneCastor3.calling_ip_count;
        else
            currCallingCount = 1;
    }
    if (--currCallingCount == 0 || code >= 600)
    {
        char buf[4];

        printf("CallError: %d\n", code);
        sprintf(buf, "%d", code);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_ERROR, buf);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
        if (!callReceived)
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);

        if (linphoneCastor3.calling_ip_count > 0)
            LinphoneTerminateOthers(id);

        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        linphoneCastor3.calling_ip_count = 0;
        if (id == linphoneCastor3.camera_id)
        {
            linphoneCastor3.camera_id = 0;
            cameraViewing = false;
        }
    }
}

static void CallFirstSnapshot(char* addr)
{
    printf("%s(%s)\n", __FUNCTION__, addr);
    CaptureSnapshot(addr, 0);
}

static void UpdateIndicators(void)
{
    char buf[8];

    sprintf(buf, "%d", CallLogGetCurrMissedCount());
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);

	if (!NetworkIsReady())
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");
    }
	else
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
    }

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_MUTE)
    {
        LinphoneDisableDoNotDisturb();
        AudioMute();
    }
    else
    {
        AudioUnMute();

        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_NONE)
        {
            LinphoneDisableDoNotDisturb();
        }
        else if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL)
        {
            LinphoneEnableDoNotDisturb();
        }
    }

    if (theConfig.muted)
    {
        puts("mute");

        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL)
            LinphoneDisableDoNotDisturb();

        theConfig.do_not_disturb_mode = DONOT_DISTURB_MUTE;
        AudioMute();
    }

    {
        sprintf(buf, "%d", theConfig.do_not_disturb_mode);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);
    }
    SceneSetReady(true);
}

static void GotoMainMenu(void)
{
    ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
    assert(mainMenuLayer);
    ituLayerGoto(mainMenuLayer);
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
            CallReceived(cmd.strarg1, cmd.arg1, cmd.arg2);
            break;

        case CMD_CALL_SPOTTED:
            CallSpotted(cmd.strarg1, cmd.arg1, cmd.arg2);
            break;

        case CMD_CALL_END:
            CallEnd(cmd.strarg1, cmd.arg1);
            break;

        case CMD_CALL_ERROR:
            CallError(cmd.strarg1, cmd.arg1, cmd.arg2);
            break;

        case CMD_CALL_FIRST_SNAPSHOT:
            CallFirstSnapshot(cmd.strarg1);
            break;

        case CMD_UPDATE_INDICATORS:
            UpdateIndicators();
            break;

        case CMD_GOTO_MAINMENU:
            GotoMainMenu();
            break;

        case CMD_CHANGE_LANG:
            ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.lang, 0, 0);
            break;
        }
    }
}

static void CheckMissedCalls(void)
{
	char buf[8];
    int count = CallLogGetCurrMissedCount();

    if (theConfig.missed_call_count == count || count > MAX_MISSED_CALL_COUNT)
        return;

    theConfig.missed_call_count = count;
    sprintf(buf, "%d", theConfig.missed_call_count);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);

    ConfigSave();
}

void SceneClearMissedCalls(void)
{
	char buf[8];

    theConfig.missed_call_count = 0;

    CallLogClearCurrMissedCount();

    sprintf(buf, "%d", theConfig.missed_call_count);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);

    ConfigSave();
}

void CheckNewMessages(void)
{
	char buf[8];
    int guardCount;

    if (!isReady)
        return;

    guardCount = GuardGetUnhandledLogCount();

	if (guardMsgCount != guardCount)
	{
	    sprintf(buf, "%d\0", guardCount);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_MESSAGE_GUARD, buf);
	}    
    guardMsgCount = guardCount;
}

static void CheckNetwork(void)
{
    bool ready = false;

    if (ScreenSaverIsScreenSaving())
        return;

    ready = NetworkIsReady();

	if (networkReady && !ready)
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");
        networkReady = false;
    }
	else if (!networkReady && ready)
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
        networkReady = true;
    }
}

static bool CheckQuitValue(void)
{
    if (quitValue)
    {
        if (ScreenSaverIsScreenSaving() && theConfig.screensaver_type == SCREENSAVER_BLANK)
            ScreenSaverRefresh();

        if (quitValue == QUIT_UPGRADE_ADDRESSBOOK || quitValue == QUIT_UPGRADE_USERLIST)
        {
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_UPGRADE, "1");
        }
        else
            return true;
    }
    return false;
}

void SceneOpenDoor(char* ip)
{
    if (ip)
    {
        RemoteOpenDoor(ip);
    }
    else if (currRemoteAddr[0] != '\0')
    {
        RemoteOpenDoor(currRemoteAddr);
    }
}

void SceneSetRemoteIP(char* ip)
{
    if (ip)
        strcpy(currRemoteAddr, ip);
    else
        currRemoteAddr[0] = '\0';
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
}

bool SceneIsCallConnected(void)
{
    return callConnected;
}

void SceneSetLastDoNotDisturbMode(DoNotDisturbMode mode)
{
    lastDoNotDisturbMode = mode;
}

int SceneRun(void)
{
    SDL_Event ev;
    int delay, frames, lastx, lasty;
    uint32_t tick, dblclk, lasttick, mouseDownTick;

    /* Watch keystrokes */
    dblclk = frames = lasttick = lastx = lasty = mouseDownTick = 0;
    for (;;)
    {
        bool result = false;

        if (CheckQuitValue())
            break;

        ProcessCommand();
        CheckMissedCalls();
        CheckNewMessages();
        CheckNetwork();

        tick = SDL_GetTicks();

    #ifdef FPS_ENABLE
        frames++;
        if (tick - lasttick >= 1000)
        {
            //printf("fps: %d\n", frames);
            frames = 0;
            lasttick = tick;
        }
    #endif // FPS_ENABLE

        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                if(callSpottedStatus == SPOTTED_INIT) break; //yc add
                ScreenSaverRefresh();
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYDOWN, ev.key.keysym.sym, 0, 0);
                switch (ev.key.keysym.sym)
                {
                case SDLK_UP:
                    if (!result && keypadEnabled)
                    {
                        // change disturb mode
                        puts("change disturb mode");
                        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CHANGE_DISTURB_MODE, NULL);
                        result = true;
                    }
                    break;

                case SDLK_DOWN:
                    if (!result && keypadEnabled && !callReceived && !callConnected)
                    {
                        if (currRemoteAddr[0] != '\0')
                        {
                            DeviceInfo info;
                            AddressBookGetDeviceInfo(&info, currRemoteAddr);

                            if (info.type == DEVICE_MANAGER || info.type == DEVICE_CENTER)
                                GotoMainMenu();
                        }
                        else
                        {
                            // call manager
                            puts("call manager");
                            if (CallingCallManager() == 0)
                            {
						        ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
                                assert(callingLayer);

                                ituLayerGoto(callingLayer);
                            }
                            else
                            {
						        ITULayer* errorLayer = ituSceneFindWidget(&theScene, "errorLayer");
                                assert(errorLayer);

                                ErrorShow(ERROR_NO_ADMIN);
                                ituLayerGoto(errorLayer);
                            }
                        }
                        result = true;
                    }
                    break;

                case SDLK_LEFT:
                    if (!result && keypadEnabled)
                    {
                        // open door
                        puts("open door");
                        SceneOpenDoor(NULL);
                        result = true;
                    }
                    break;

                case SDLK_RIGHT:
                    if (!result && keypadEnabled && !callReceived)
                    {
                        ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");

                        if (!ituWidgetIsVisible(callingLayer))
                        {
                            if (strcmp(currRemoteAddr, theLobbyInfo.ip) == 0)
                            {
                                GotoMainMenu();
				                callConnected = false;
                            }
                            else if (!callConnected)
                            {
                                ITULayer* cameraLobbyLayer = ituSceneFindWidget(&theScene, "cameraLobbyLayer");
                                assert(cameraLobbyLayer);

                                // view lobby
                                puts("view lobby");
                                ituLayerGoto(cameraLobbyLayer);
                            }
                            result = true;
                        }
                    }
                    break;

                case SDLK_INSERT:
                    if (!result && keypadEnabled)
                    {
                        // answer phone
                        puts("answer phone");
                        ituSceneSendEvent(&theScene, EVENT_CUSTOM_ANSWER, NULL);
                        result = true;
                    }
                    break;

            #ifdef _WIN32
                case SDLK_e:
                    {
                        time_t rawtime;
                        time (&rawtime);
                        EventWriteCalloutLog("192.168.191.200", EVENT_CALLOUT_START);
                        result = true;
                    }
                    break;
                case SDLK_f:
                    {
                        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_INCOMMING, "192.168.1.1 1 100");
                        result = true;
                    }
                    break;

            #endif // _WIN32
                }
                if (result && !ScreenIsOff())
                    LinphonePlayKeySound();

                break;

            case SDL_KEYUP:
                result = ituSceneUpdate(&theScene, ITU_EVENT_KEYUP, ev.key.keysym.sym, 0, 0);
                break;

            case SDL_MOUSEMOTION:
                ScreenSaverRefresh();
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, ev.button.button, ev.button.x, ev.button.y);
                break;

            case SDL_MOUSEBUTTONDOWN:
                ScreenSaverRefresh();
                {
                    mouseDownTick = SDL_GetTicks();
                #ifdef DOUBLE_KEY_ENABLE
                    if (mouseDownTick - dblclk <= 200)
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, ev.button.button, ev.button.x, ev.button.y);
                        dblclk = 0;
                    }
                    else
                #endif // DOUBLE_KEY_ENABLE
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, ev.button.button, ev.button.x, ev.button.y);
                        dblclk = mouseDownTick;
                        lastx = ev.button.x;
                        lasty = ev.button.y;

                        if (result && !ScreenIsOff())
                            LinphonePlayKeySound();
                    }
                #ifdef CFG_SCREENSHOT_ENABLE
                    if (ev.button.x < 50 && ev.button.y > CFG_LCD_HEIGHT - 50)
                        Screenshot(screenSurf);
                #endif // CFG_SCREENSHOT_ENABLE
                }
                break;

            case SDL_MOUSEBUTTONUP:
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                if (SDL_GetTicks() - dblclk <= 200)
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
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.button.x, ev.button.y);
                        }
                        else
                        {
                            printf("mouse: slide to up\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.button.x, ev.button.y);
                        }
                    }
                }
                mouseDownTick = 0;
                break;

            case SDL_FINGERMOTION:
                ScreenSaverRefresh();
                //printf("touch: move %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, 1, ev.tfinger.x, ev.tfinger.y);
                break;

            case SDL_FINGERDOWN:
                ScreenSaverRefresh();
                printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                {
                    mouseDownTick = SDL_GetTicks();
                #ifdef DOUBLE_KEY_ENABLE
                    if (mouseDownTick - dblclk <= 200)
                    {
                        printf("double touch!\n");
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOUBLECLICK, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = mouseDownTick = 0;
                    }
                    else
                #endif // DOUBLE_KEY_ENABLE
                    {
                        result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEDOWN, 1, ev.tfinger.x, ev.tfinger.y);
                        dblclk = mouseDownTick;
                        lastx = ev.tfinger.x;
                        lasty = ev.tfinger.y;
                    }
                        if (result && !ScreenIsOff())
                            LinphonePlayKeySound();
                            
                #ifdef CFG_SCREENSHOT_ENABLE
                    if (ev.tfinger.x < 50 && ev.tfinger.y > CFG_LCD_HEIGHT - 50)
                        Screenshot(screenSurf);
                #endif // CFG_SCREENSHOT_ENABLE
                }
                break;

            case SDL_FINGERUP:
                printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                if (SDL_GetTicks() - dblclk <= 200)
                {
                    int xdiff = abs(ev.tfinger.x - lastx);
                    int ydiff = abs(ev.tfinger.y - lasty);
					printf("[xdiff %d][ydiff %d][lastx %d][lasty %d]\n", xdiff, ydiff, lastx, lasty);
                    if (xdiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.tfinger.x > lastx)
                        {
                            printf("touch: slide to right\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.tfinger.x, ev.tfinger.y);
                        }
                        else
                        {
                            printf("touch: slide to left\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.tfinger.x, ev.tfinger.y);
                        }
                    }
                    else if (ydiff >= GESTURE_THRESHOLD)
                    {
                        if (ev.tfinger.y > lasty)
                        {
                            printf("touch: slide to down\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, ydiff, ev.tfinger.x, ev.tfinger.y);
                        }
                        else
                        {
                            printf("touch: slide to up\n");
                            result |= ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, ydiff, ev.tfinger.x, ev.tfinger.y);
                        }
                    }
                }
				result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                mouseDownTick = 0;
                break;

            /* case SDL_SLIDEGESTURE:
                ScreenSaverRefresh();
                switch (ev.dgesture.gestureId)
                {
                case SDL_TG_LEFT:
                    printf("touch: slide to left %d, %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDELEFT, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_UP:
                    printf("touch: slide to up %d, %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEUP, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_RIGHT:
                    printf("touch: slide to right %d, %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDERIGHT, 1, ev.dgesture.x, ev.dgesture.y);
                    break;

                case SDL_TG_DOWN:
                    printf("touch: slide to down %d, %d\n", ev.dgesture.x, ev.dgesture.y);
                    result = ituSceneUpdate(&theScene, ITU_EVENT_TOUCHSLIDEDOWN, 1, ev.dgesture.x, ev.dgesture.y);
                    break;
                }
                break; */
            }
        }
        if (callReceived || callConnected || SceneCameraIsViewing())
            ScreenSaverRefresh();

        if (!ScreenIsOff())
        {
            if (mouseDownTick > 0 && (SDL_GetTicks() - mouseDownTick >= MOUSEDOWN_LONGPRESS_DELAY))
            {
                printf("long press %d, %d\n", lastx, lasty);
                result |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSELONGPRESS, 1, lastx, lasty);
                mouseDownTick = 0;
            }
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

            if (theConfig.screensaver_type != SCREENSAVER_NONE && keypadEnabled &&
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

        delay = periodPerFrame - (SDL_GetTicks() - tick);
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
