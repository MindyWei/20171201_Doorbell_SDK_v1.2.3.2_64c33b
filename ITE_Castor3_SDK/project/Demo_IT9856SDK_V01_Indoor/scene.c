#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include "SDL/SDL.h"
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"
#include "leaf/leaf_mediastream.h"
#ifndef _WIN32
    #include "ite/itv.h"
#endif
#ifdef _WIN32
    #include <crtdbg.h>
#endif

// need to install VS90SP1-KB980263-x86.exe for vs2008
#pragma execution_character_set("utf-8")

#define FPS_ENABLE
#define DOUBLE_KEY_ENABLE

#define GESTURE_THRESHOLD           40
#define MOUSEDOWN_LONGPRESS_DELAY   1000
#define MAX_COMMAND_QUEUE_SIZE      8
#define HOTKEY_DELAY                5
#define WARNPLAY_DELAY              6
#define MAX_MISSED_CALL_COUNT       999

#ifdef CFG_BROADCAST_CLIENT_ENABLE
#define broadcast_cmd_port          33333 //broadcast cmd port
#define broadcast_data_port         6666  //broadcast data port
#define broadcast_addr              "255.255.255.255" //broadcast addr

static int data_receiving = 0;
static int stop_audio_broadcast_transfer = 0;
#endif

extern ITUActionFunction actionFunctions[];
extern void resetScene(void);
extern bool captureRecording, IPCamStreaming;

// status
static bool callReceived;
static bool callConnected;
static bool haveExtCalling;
static bool networkReady;
static bool guardOn;
static bool guardRobAlarming;
static int textMsgCount;
static int videoMsgCount;
static QuitValue quitValue;
static time_t lastLogTime;
static bool cameraViewing;
static DoNotDisturbMode lastDoNotDisturbMode;
static int hotkeyDelay;
static int warnPlayDelay;
static bool      inVideoState;
SPOTTED_STATUS callSpottedStatus = SPOTTED_NONE;
static int periodPerFrame;
static uint32_t mouseDownTick;

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

void SceneInit(void)
{
    struct mq_attr attr;

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

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    commandQueue = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(commandQueue != -1);

    isReady                 = false;
    callReceived            = false;
    callConnected           = false;
    haveExtCalling          = false; 
    networkReady            = false;
    guardOn                 = false;
    guardRobAlarming        = false;
    textMsgCount            = 0;
    videoMsgCount           = 0;
    currCallingCount        = -1;
    currCallID              = 0;
    currRemoteAddr[0]       = '\0';
    pendingCallID           = 0;
    pendingRemoteAddr[0]    = '\0';
    cameraViewing           = false;
    lastDoNotDisturbMode    = DONOT_DISTURB_NONE;
    hotkeyDelay             = 0;
    warnPlayDelay           = 0;
    inVideoState            = false;
    periodPerFrame          = MS_PER_FRAME;

#if defined(CFG_BROADCAST_SERVER_ENABLE) | defined(CFG_BROADCAST_CLIENT_ENABLE)
    LeafInit();
#endif
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

void ScenePredraw(int arg)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_PREDRAW;
    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static int SceneCallOnDoNotDisturb(char* ip)
{
    int result = 0;

    if (strcmp(theCenterInfo.ip, ip) == 0)
        result = 1;

    if (theConfig.do_not_disturb_option == DONOT_DISTURB_HANGUP)
    {
        if (strcmp(theDeviceInfo.ext, "01"))
		{
			result = -3;
		}
		else
		{
            result = -2;
        }
    }
    else if (theConfig.do_not_disturb_option == DONOT_DISTURB_VOICEMEMO)
    {
        if (callConnected || strcmp(theDeviceInfo.ext, "01"))
        {
            result = 0;
        }
        else
        {
            result = -1;
        }
    }
    else if (theConfig.do_not_disturb_option == DONOT_DISTURB_MESSAGE)
    {
        EventWriteSmsLog(ip);
        result = 0;
    }
    else if (theConfig.do_not_disturb_option == DONOT_DISTURB_REDIRECT)
    {
        if (strcmp(theDeviceInfo.ext, "01"))
		{
			result = -3;
		}
		else
		{
            LinphoneRedirect(theCenterInfo.ip);
            result = 2;
        }
    }

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

    if (strcmp(theCenterInfo.ip, currRemoteAddr))
    {
        if (strcmp(theEntranceInfo.ip, ip) == 0 || strcmp(theOutdoorInfo.ip, ip) == 0)
        {
            result = 1;
            goto end;
        }
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
        if(!callReceived)
	        SceneSetRemoteIP(NULL);
        hotkeyDelay = HOTKEY_DELAY;
    }
}

int SceneCameraIsViewing(void)
{
    return cameraViewing ? 1 : 0;
}

void SceneSetExtCalling(int ext_calling)
{
    if(ext_calling)
        haveExtCalling = true;
    else
        haveExtCalling = false;
}
    
int SceneIsCalling(void)
{
	//printf("++++++++++++++++++++%d, %d++++++++++\n", callConnected, haveExtCalling);
    if(callConnected || haveExtCalling)
        return 1;
    else
        return 0;
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

void SceneContinuePlayGuardAlarm(void)
{
    if (GuardIsAlarming() && !guardRobAlarming)
        warnPlayDelay = WARNPLAY_DELAY;
}

void SceneCaptureRecordStatus(char bStatus)
{
    if(bStatus)
        captureRecording = true;
    else
        captureRecording = false;
}

void SceneIPCamStreamingStatus(char bStatus)
{
    if(bStatus)
        IPCamStreaming = true;
    else
        IPCamStreaming = false;
}

#ifdef CFG_BROADCAST_CLIENT_ENABLE
int SceneCallCheckAudioBroadcast(void)
{
    stop_audio_broadcast_transfer = 1;
    if(data_receiving)
    {
        ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);
        ituLayerGoto(mainMenuLayer);
        LeafTerminate();
        usleep(50000);
        data_receiving = 0;
    }
}
#endif
static void SceneCallFirstSnapshot(char* addr)
{
    Command cmd;

    if (commandQueue == -1)
        return;

    cmd.id     = CMD_CALL_FIRST_SNAPSHOT;
    strcpy(cmd.strarg1, addr);

    mq_send(commandQueue, (const char*)&cmd, sizeof (Command), 0);
}

static void SceneStopAudioPlay(void)
{
    AudioPlayerStop();
}

static void SceneStopVideoPlay(void)
{
    VideoPlayerStop();
}

void SceneResetLongPressTime(void)
{
    if (mouseDownTick > 0)
        mouseDownTick = SDL_GetTicks();
}

#ifdef _WIN32
    // check memory leak
    //static _CrtMemState state;
#endif

static void LoadScene(void)
{
    uint32_t tick1, tick2;

    if (theScene.root)
    {
        ituSceneExit(&theScene);
    #ifdef _WIN32
        //_CrtMemDumpAllObjectsSince(&state);
    #endif
    }
#ifdef _WIN32
    else
    {
        //_CrtMemCheckpoint(&state);
        //_CrtSetBreakAlloc(8003);
    }
#endif // _WIN32

    // load itu file
    tick1 = SDL_GetTicks();

    ituSceneLoadFile(&theScene, CFG_PRIVATE_DRIVE ":/doorbell_indoor.itu");

    tick2 = SDL_GetTicks();
    printf("itu loading time: %dms\n", tick2 - tick1);

    tick1 = tick2;
	resetScene();
    ituSceneUpdate(&theScene, ITU_EVENT_LANGUAGE, theConfig.lang, 0, 0);
    ituSceneUpdate(&theScene, ITU_EVENT_LAYOUT, 0, 0, 0);

    tick2 = SDL_GetTicks();
    printf("itu init time: %dms\n", tick2 - tick1);

    linphonec_call_end_notify_callback                  = SceneCallEnd;
    linphonec_call_imcoming_received_notify_callback    = SceneCallReceived;
    linphonec_call_connected_notify_callback            = SceneCallConnected;
    linphonec_call_error_notify_callback                = SceneCallError;
    linphonec_call_on_do_not_disturb_callback           = SceneCallOnDoNotDisturb;
    linphonec_call_allow_call_waiting_callback          = SceneCallAllowCallWaiting;
    linphonec_call_is_camera_viewing_callback           = SceneCameraIsViewing;
	linphonec_call_is_calling_callback                  = SceneIsCalling;
    linphonec_new_log_notify_callback                   = SceneNewLog;
    linphonec_call_is_allowing_spotted_callback         = SceneCallSpottedOrNot;
    linphonec_call_camera_connect_error_callback        = SceneCameraConnectError;
    linphonec_call_capture_record_status_callback       = SceneCaptureRecordStatus;
    linphonec_call_first_snapshot_callback              = SceneCallFirstSnapshot;
    linphonec_call_stop_audio_play_callback             = SceneStopAudioPlay;
    linphonec_call_stop_video_play_callback             = SceneStopVideoPlay;
    linphonec_call_ipcam_streaming_status_callback      = SceneIPCamStreamingStatus;
#ifdef CFG_BROADCAST_CLIENT_ENABLE    
    linphonec_call_check_audio_broadcast_callback       = SceneCallCheckAudioBroadcast;
#endif
}

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
#ifdef CFG_BROADCAST_CLIENT_ENABLE
    if(data_receiving)
    {
        LeafTerminate();
        data_receiving = 0;
    }
#endif

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


static void CallConnected(char* addr, int id)
{
    char buf[32];

    printf("%s(%s, %d)\n", __FUNCTION__, addr, id);
    sprintf(buf, "%d", id);

    ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_CONNECTED, buf);

    if (linphoneCastor3.video_msg_uri[0] == '\0')
        EventWriteCalloutLog(addr, EVENT_CALLOUT_START);

    strcpy(currRemoteAddr, addr);

    callConnected = true;
    currCallID = id;
    RemoteSetExtCallingState(true);
    LinphoneTerminateOthers(id);
}

static void CallReceived(char* addr, int id, int video)
{
    char buf[32];

    printf("%s(%s, %d, %d)\n", __FUNCTION__, addr, id, video);

    if (!isReady)
        return;

    ScreenSaverRefresh();

    if (LinphoneIsInDoNotDisturb())
    {
        if (callConnected || theConfig.do_not_disturb_option != DONOT_DISTURB_VOICEMEMO)
            return;

        sprintf(buf, "%s %d %d", addr, id, video);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_VOICEMEMO, buf);
    }
    else
    {
        sprintf(buf, "%s %d %d", addr, id, video);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_INCOMMING, buf);
    }

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
#ifdef CFG_BROADCAST_CLIENT_ENABLE    
    stop_audio_broadcast_transfer = 0;
#endif

    if (id == currCallID || (currCallID == 0 && !callConnected))
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CALL_END, "0");
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
		if(callConnected && linphoneCastor3.video_msg_uri[0] == '\0')
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
		
		if(callConnected == true)
		{
			RemoteSetExtCallingState(false);
			haveExtCalling = false;
		}
        callReceived = false;
        callConnected = false;
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        linphoneCastor3.video_msg_uri[0] = '\0';

        SceneContinuePlayGuardAlarm();
    }
}

static void CallError(char* addr, int id, int code)
{
    printf("%s(%s, %d, %d): %d\n", __FUNCTION__, addr, id, code, currCallingCount);
#ifdef CFG_BROADCAST_CLIENT_ENABLE    
    stop_audio_broadcast_transfer = 0;
#endif

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
        if (!callReceived && linphoneCastor3.video_msg_uri[0] == '\0')
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);

        if (linphoneCastor3.calling_ip_count > 0)
            LinphoneTerminateOthers(id);

		if(callConnected == true)
		{
			RemoteSetExtCallingState(false);	
			haveExtCalling = false;
		}
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
        linphoneCastor3.video_msg_uri[0] = '\0';

        SceneContinuePlayGuardAlarm();
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

    videoMsgCount = textMsgCount = 0;

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
        else if (theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME)
        {
            if (theConfig.do_not_disturb_start_hour == theConfig.do_not_disturb_end_hour && theConfig.do_not_disturb_start_min == theConfig.do_not_disturb_end_min)
            {
                theConfig.do_not_disturb_mode = DONOT_DISTURB_NONE;
                LinphoneDisableDoNotDisturb();
            }
        }
    }

    if (theConfig.muted)
    {
        puts("mute");

        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL || theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME)
            LinphoneDisableDoNotDisturb();

        theConfig.do_not_disturb_mode = DONOT_DISTURB_MUTE;
        AudioMute();
    }

    if (theConfig.do_not_disturb_mode != DONOT_DISTURB_TIME)
    {
        sprintf(buf, "%d", theConfig.do_not_disturb_mode);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);
    }

    if (theConfig.guard_mode == GUARD_NONE)
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "0");
        guardOn = false;
    }
    else if (theConfig.guard_mode != GUARD_NONE)
    {
		ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "1");
        guardOn = true;
    }
}

static void GotoMainMenu(void)
{
    ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
    assert(mainMenuLayer);
    ituLayerGoto(mainMenuLayer);
}

static bool ProcessCommand(void)
{
    bool result = false;
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

static void CheckMissedCalls(void)
{
	char buf[8];
    int count = CallLogGetCurrMissedCount();

    if (theConfig.missed_call_count == count || count > MAX_MISSED_CALL_COUNT)
        return;

    theConfig.missed_call_count = count;
    sprintf(buf, "%d", theConfig.missed_call_count);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);

#ifdef BACKUP_FLAG
    ConfigSaveTempForBackup();
#else
    ConfigSaveTemp();
#endif
}

void SceneClearMissedCalls(void)
{
	char buf[8];

    theConfig.missed_call_count = 0;

    CallLogClearCurrMissedCount();

    sprintf(buf, "%d", theConfig.missed_call_count);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);

#ifdef BACKUP_FLAG
    ConfigSaveTempForBackup();
#else
    ConfigSaveTemp();
#endif
}

void CheckNewMessages(void)
{
	char buf[8];
    int videoCount, textCount = WebServerGetNewTextMsgCount();

    if (theDeviceInfo.type == DEVICE_INDOOR2)
        videoCount = VideoMemoGetUnplayCount();
    else
        videoCount = WebServerGetNewVideoMsgCount();

    if (!isReady)
        return;

    if (videoMsgCount == videoCount && textMsgCount == textCount)
        return;

	sprintf(buf, "%d\0", videoCount + textCount);
    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MESSAGE, buf);

    if(videoMsgCount != videoCount)
	{
	    sprintf(buf, "%d\0", videoCount);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_MESSAGE_VIDEO, buf);
	}    
	if(textMsgCount != textCount)
	{
	    sprintf(buf, "%d\0", textCount);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_MESSAGE_TEXT, buf);
	}    
    videoMsgCount = videoCount;
    textMsgCount = textCount;
}

static void CheckGuard(void)
{
    static bool isMuted;
    int ret;

    if (!isReady)
        return;

    if (guardOn && theConfig.guard_mode == GUARD_NONE)
    {
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "0");
        guardOn = false;
    }
    else if (!guardOn && theConfig.guard_mode != GUARD_NONE)
    {
		ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "1");
        guardOn = true;
    }

    ret = GuardCheck();

    if (GuardIsAlarming())
    {
		ScreenSaverRefresh();
	
        if (ret == 0)
        {
            if (isMuted)
                AudioMute();

            LinphoneStopSound();
            GuardSetAlarming(false);
            guardRobAlarming = false;
        }
    }
    else
    {
        if (ret)
        {
			ITULayer* guardCheckLayer;
			
			ScreenSaverRefresh();

			guardCheckLayer = ituSceneFindWidget(&theScene, "guardCheckLayer");
            assert(guardCheckLayer);

            if (ret < 0)
            {
                isMuted = AudioIsMuted();
                AudioUnMute();
                LinphonePlayWarnSound();
                guardRobAlarming = false;
            }
            else
            {
                guardRobAlarming = true;
            }

            GuardSetAlarming(true);

			if(!callReceived) {
				ituLayerGoto(guardCheckLayer);
			}
        }
    }

    if (warnPlayDelay > 0)
    {
        if (--warnPlayDelay == 0)
        {
            if (GuardIsAlarming()) {
				ITULayer* guardCheckLayer;

				guardCheckLayer = ituSceneFindWidget(&theScene, "guardCheckLayer");
				assert(guardCheckLayer);
				
                LinphonePlayWarnSound();
				ituLayerGoto(guardCheckLayer);
            }
        }
    }
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

static void CheckDoNotDisturb(void)
{
    int ret;
    char buf[8];

    if (!isReady)
        return;

    ret = LinphoneCheckDoNotDisturb();
    if (ret == 1)
    {
        if (!AudioIsMuted())
        {
            sprintf(buf, "%d", theConfig.do_not_disturb_mode);
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);
        }
    }
    else if (ret == -1)
    {
        if (!AudioIsMuted())
        {
            sprintf(buf, "%d", DONOT_DISTURB_NONE);
            ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);
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
#ifdef CFG_BROADCAST_CLIENT_ENABLE    
    stop_audio_broadcast_transfer = 0;
#endif

    if (!callReceived && linphoneCastor3.video_msg_uri[0] == '\0')
    {
        EventWriteCalloutLog(currRemoteAddr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);
    }
	if(callConnected == true)
	{
        RemoteSetExtCallingState(false);
		haveExtCalling = false;
	}
    callReceived = false;
    callConnected = false;
    currCallID = 0;
    currRemoteAddr[0] = '\0';
    currCallingCount = -1;
    linphoneCastor3.calling_ip_count = 0;
    hotkeyDelay = HOTKEY_DELAY;
    linphoneCastor3.video_msg_uri[0] = '\0';

    SceneContinuePlayGuardAlarm();
}

bool SceneIsCallConnected(void)
{
    return callConnected;
}

void SceneSetLastDoNotDisturbMode(DoNotDisturbMode mode)
{
    lastDoNotDisturbMode = mode;
}

static void ToggleMute(void)
{
    char buf[8];

    if (GuardIsAlarming())
        return;

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_MUTE)
    {
        puts("unmute");
        theConfig.do_not_disturb_mode = lastDoNotDisturbMode;
        AudioUnMute();
//yc mark        
/*
        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_NONE)
            LinphoneDisableDoNotDisturb();
        else
*/        
        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL)
            LinphoneEnableDoNotDisturb();

        theConfig.muted = 0;
    }
    else
    {
        puts("mute");

        lastDoNotDisturbMode = theConfig.do_not_disturb_mode;

        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_ALL || theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME)
            LinphoneDisableDoNotDisturb();

        theConfig.do_not_disturb_mode = DONOT_DISTURB_MUTE;
        AudioMute();
        theConfig.muted = 1;
    }

    if (theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME && !LinphoneIsInDoNotDisturb())
        sprintf(buf, "%d", DONOT_DISTURB_NONE);
    else
        sprintf(buf, "%d", theConfig.do_not_disturb_mode);

    ituSceneSendEvent(&theScene, EVENT_CUSTOM_DISTURB_MODE, buf);

#ifdef BACKUP_FLAG
    ConfigSaveTempForBackup();
#else
    ConfigSaveTemp();
#endif
}

static void CheckStorage(void)
{
    StorageAction action = StorageCheck();

    switch (action)
    {
    case STORAGE_SD_INSERTED:
        if (theDeviceInfo.type == DEVICE_INDOOR2)
        {
            VideoMemoInit();
            CaptureInit();
        }
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_INSERTED, NULL);
        break;

    case STORAGE_SD_REMOVED:
        if (theDeviceInfo.type == DEVICE_INDOOR2)
        {
            ITULayer* messageRecordLayer = ituSceneFindWidget(&theScene, "messageRecordLayer");
            ITULayer* messageRecordViewLayer = ituSceneFindWidget(&theScene, "messageRecordViewLayer");
            ITULayer* messageVideoLayer = ituSceneFindWidget(&theScene, "messageVideoLayer");
            ITULayer* messageVideoViewLayer = ituSceneFindWidget(&theScene, "messageVideoViewLayer");

            assert(messageRecordLayer);
            assert(messageRecordViewLayer);
            assert(messageVideoLayer);
            assert(messageVideoViewLayer);

            if (ituWidgetIsVisible(messageRecordLayer) || ituWidgetIsVisible(messageRecordLayer) || ituWidgetIsVisible(messageRecordLayer) || ituWidgetIsVisible(messageRecordLayer))
            {
                ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
                assert(mainMenuLayer);
                ituLayerGoto(mainMenuLayer);
            }

            CaptureExit();
            VideoMemoExit();
        }
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_SD_REMOVED, NULL);
        break;

    case STORAGE_USB_INSERTED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_INSERTED, NULL);
        break;

    case STORAGE_USB_REMOVED:
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_USB_REMOVED, NULL);
        break;
    }
}

#ifdef CFG_BROADCAST_CLIENT_ENABLE
void Broadcast_Recv(int sockfd)
{
    char buf[32];
    struct sockaddr_in sender_addr;
    int recvlen;
    int addr_len = sizeof (struct sockaddr_in);
    struct timeval timeout = {0};
    fd_set readfds;
    
    /*set receive form broadcast side with timeout*/
    FD_ZERO(&readfds);
    FD_SET(sockfd,&readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1000;
    select(sockfd+1,&readfds,NULL,NULL,&timeout);
    if(FD_ISSET(sockfd,&readfds))
    {
        recvlen = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&sender_addr, &addr_len);
        if (recvlen > 0 && stop_audio_broadcast_transfer == 0) 
        {			
            buf[recvlen] = 0;
            sender_addr.sin_port = htons(8888);
            printf("received message: \"%s\" (%d bytes), from:(%s)\n", buf, recvlen, inet_ntoa(sender_addr.sin_addr));	
            if(strcmp(buf, "Broadcast=1") == 0)
            {
                data_receiving = 1;
                LeafStartConnect(broadcast_addr, broadcast_data_port);
            }
            else if(strcmp(buf, "Broadcast=0") == 0 && data_receiving == 1)
            {   
                data_receiving = 0;
                LeafTerminate();
                ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
                assert(mainMenuLayer);
                ituLayerGoto(mainMenuLayer);
            }    
            sprintf(buf, "broadcast client ack");		
            printf("sending response \"%s\"\n", buf);		
            if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&sender_addr, addr_len) < 0)			
                printf("sendto error\n");

            if(data_receiving)
            {
                ITULayer* calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
			    assert(calledLayer);
    		    ituLayerGoto(calledLayer);
            }
        }
        //else
        //{
        //    printf("no broadcast recv\n");
        //}
    }
}
#endif //CFG_BROADCAST_CLIENT_ENABLE

int SceneRun(void)
{
    SDL_Event ev;
    int delay, frames, lastx, lasty;
    uint32_t tick, dblclk, lasttick;
#ifdef CFG_BROADCAST_CLIENT_ENABLE  
    int sockfd;
    struct sockaddr_in myaddr;
    int broadcastPermission=1;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket error\n");
    }
    if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,
                            &broadcastPermission, sizeof(broadcastPermission))) < 0)
    {
        printf("setsockopt error\n");
    }
    printf("Socket created\n");

    memset((char *)&myaddr, 0, sizeof(myaddr));    
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(broadcast_cmd_port);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockfd, (struct sockaddr*) &myaddr, sizeof(myaddr)) < 0)
    {
        printf("bind failed\n");
    }
#endif //CFG_BROADCAST_CLIENT_ENABLE

    /* Watch keystrokes */
    dblclk = frames = lasttick = lastx = lasty = mouseDownTick = 0;
    //LinphonePlayKeySound();//(add dummysound to avoid abnormal sound)
    //usleep(500000);
    for (;;)
    {
        bool result = false;
        bool hasExtEvent;

        if (CheckQuitValue())
            break;

        hasExtEvent = ProcessCommand();
        CheckMissedCalls();
        CheckNewMessages();
        CheckGuard();
        CheckNetwork();
        CheckDoNotDisturb();
        CheckStorage();

#ifdef CFG_BROADCAST_CLIENT_ENABLE
        Broadcast_Recv(sockfd);
#endif //CFG_BROADCAST_CLIENT_ENABLE       
        tick = SDL_GetTicks();

    #ifdef FPS_ENABLE
        frames++;
        if (tick - lasttick >= 1000)
        {
            printf("fps: %d\n", frames);
			wprintf(L"你好");
            frames = 0;
            lasttick = tick;
        }
    #endif // FPS_ENABLE

        if (hotkeyDelay > 0)
            hotkeyDelay--;

        if (!hasExtEvent)
        {
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
                        {
                            // toggle mute
                            puts("toggle mute");
                            ToggleMute();
                            result = true;
                        }
                        break;

                    case SDLK_DOWN:
                        if (!callReceived && !callConnected && !hotkeyDelay)
                        {
                            if (currRemoteAddr[0] != '\0')
                            {
                                DeviceInfo info;
                                AddressBookGetDeviceInfo(&info, currRemoteAddr);

                                if (info.type == DEVICE_MANAGER || info.type == DEVICE_CENTER)
                                {
                                    ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
                                    assert(mainMenuLayer);
                                    ituLayerGoto(mainMenuLayer);
                                }
                            }
                            else
                            {
						        ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");
                                assert(callingLayer);

                                // call manager
                                puts("call manager");
                                if (CallingCallManager(NULL) == 0)
                                {
                                    ituLayerGoto(callingLayer);
                                }
                            }
                            result = true;
                        }
                        break;

                    case SDLK_LEFT:
                        {
                            // open door
                            puts("open door");
                            SceneOpenDoor(NULL);
                            result = true;
                        }
                        break;

                    case SDLK_RIGHT:
                        if (!callReceived && !callConnected && !hotkeyDelay)
                        {
                            ITULayer* callingLayer = ituSceneFindWidget(&theScene, "callingLayer");

                            if (!ituWidgetIsVisible(callingLayer))
                            {
                                if (strcmp(currRemoteAddr, theEntranceInfo.ip) == 0)
                                {
                                    ITULayer* mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
                                    assert(mainMenuLayer);
                                    ituLayerGoto(mainMenuLayer);
                                }
                                else
                                {
                                    ITULayer* cameraOutdoorLayer = ituSceneFindWidget(&theScene, "cameraOutdoorLayer");
                                    assert(cameraOutdoorLayer);

                                    // view outdoor monitor
                                    puts("view outdoor monitor");
                                    ituLayerGoto(cameraOutdoorLayer);
                                }
                                result = true;
                            }
                        }
                        break;

                    case SDLK_INSERT:
                        {
                            // answer phone
                            puts("answer phone");
                            ituSceneSendEvent(&theScene, EVENT_CUSTOM_ANSWER, NULL);
                            result = true;
                        }
                        break;

                #ifdef _WIN32
                    case SDLK_a:
                        {
                            char buf[8];
                            if (++theConfig.missed_call_count > 5)
                                theConfig.missed_call_count = 0;

						    sprintf(buf, "%d", theConfig.missed_call_count);
						    ituSceneSendEvent(&theScene, EVENT_CUSTOM_MISS_CALLS, buf);
                            result = true;
                        }
                        break;

                    case SDLK_b:
                        {
                            char buf[8];
                            if (++textMsgCount > 5)
                                textMsgCount = 0;

						    sprintf(buf, "%d", textMsgCount);
                            ituSceneSendEvent(&theScene, EVENT_CUSTOM_MESSAGE, buf);
                            result = true;
                        }
                        break;

                    case SDLK_c:
                        {
                            guardOn = !guardOn;
		                    if(guardOn)
                                ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "1");
		                    else
                                ituSceneSendEvent(&theScene, EVENT_CUSTOM_GUARD, "0");
                            result = true;
                        }
                        break;

                    case SDLK_d:
                        {
                            static bool netOn = false;
                            netOn = !netOn;
		                    if(netOn)
                                ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "1");
		                    else
                                ituSceneSendEvent(&theScene, EVENT_CUSTOM_NETWORK, "0");
                            result = true;
                        }
                        break;

                    case SDLK_e:
                        {
                            time_t rawtime;
                            time (&rawtime);
                            EventWriteGuardLog(GUARD_INFRARED, GUARD_DISABLE, rawtime);
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
                    if (SDL_GetTicks() - dblclk <= 200)
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
                    result |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, ev.button.button, ev.button.x, ev.button.y);
                    mouseDownTick = 0;
                    break;

                case SDL_FINGERMOTION:
                    ScreenSaverRefresh();
                    //printf("touch: move %d, %d\n", ev.tfinger.x, ev.tfinger.y);
                    result = ituSceneUpdate(&theScene, ITU_EVENT_MOUSEMOVE, 1, ev.tfinger.x, ev.tfinger.y);
                    break;

                case SDL_FINGERDOWN:
                    ScreenSaverRefresh();
                    //printf("touch: down %d, %d\n", ev.tfinger.x, ev.tfinger.y);
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

                            if (result && !ScreenIsOff())
                                LinphonePlayKeySound();
                        }
                    #ifdef CFG_SCREENSHOT_ENABLE
                        if (ev.tfinger.x < 50 && ev.tfinger.y > CFG_LCD_HEIGHT - 50)
                            Screenshot(screenSurf);
                    #endif // CFG_SCREENSHOT_ENABLE
                    }
                    break;

                case SDL_FINGERUP:
                    //printf("touch: up %d, %d\n", ev.tfinger.x, ev.tfinger.y);
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
                                printf("touch: slide to left %d %d\n", ev.button.x, ev.button.y);
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
                    result |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSEUP, 1, ev.tfinger.x, ev.tfinger.y);
                    mouseDownTick = 0;
                    break;
                }
            }
        }
        if (callReceived || callConnected || SceneCameraIsViewing())
            ScreenSaverRefresh();

        if (!ScreenIsOff())
        {
            if (!hasExtEvent && mouseDownTick > 0 && (SDL_GetTicks() - mouseDownTick >= MOUSEDOWN_LONGPRESS_DELAY))
            {
                printf("long press: %d %d\n", lastx, lasty);
                result |= ituSceneUpdate(&theScene, ITU_EVENT_MOUSELONGPRESS, 1, lastx, lasty);
                mouseDownTick = 0;
            }
            result |= ituSceneUpdate(&theScene, ITU_EVENT_TIMER, 0, 0, 0);
            //printf("%d\n", result);
            
         #ifndef _WIN32            
            if (inVideoState && !result)
            {
            	uint32_t timeOut = 0;
            	
            	while (itv_get_new_video() == false && timeOut < 100)
            	{          	
				      SDL_Delay(1);
				      timeOut++;     
				}
				if (timeOut != 100)
				    result |= true;
            }
        #endif           
        
        #ifndef _WIN32
            if (result)
        #endif
            {
                ituSceneDraw(&theScene, screenSurf);
                ituFlip(screenSurf);
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

        delay = periodPerFrame - (SDL_GetTicks() - tick);
        //printf("scene loop delay=%d\n", delay);
        if (delay > 0)
            SDL_Delay(delay);
        else
            sched_yield();
    }
#ifdef CFG_BROADCAST_CLIENT_ENABLE    
    close(sockfd);
#endif
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
