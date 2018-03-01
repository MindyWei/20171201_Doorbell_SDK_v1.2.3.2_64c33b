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
#include "ite/ith.h"
#include "iniparser/iniparser.h"

#define MAX_COMMAND_QUEUE_SIZE  8
#define INI_FILENAME "doorbell_outdoor.ini"
#define CFG_CAMERA_TIMEOUT (35 * 1000 / 33) // 35 sec

static QuitValue quitValue;
static int currCallID;
static char currRemoteAddr[16];
static bool callReceived, callConnected, hint_sound;
static int currCallingCount, currCameraCount, pre_count = 0;
static bool calling_flag = false;
static bool callend_flag = true;
static int camera_timeout;
#ifdef CFG_LOOP_DIALING
    #define CFG_LOOP_DIALING_DELAY (10 * 1000 / 33) // 10 sec
    static int loopDialingDelay;
#endif

#ifdef _WIN32
static SDL_Window *window;
#endif

// command
typedef enum
{
    CMD_NONE,
    CMD_CALL_CONNECTED,
    CMD_CALL_RECEIVED,
    CMD_CALL_END,
    CMD_CALL_ERROR
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

static void SceneCallConnected(char* addr, int id)
{
    Command cmd;

    if (commandQueue == -1)
        return;

	calling_flag = false;
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

static int SceneCallAllowCallWaiting(char* ip)
{
    int result = 0;

    if (!callReceived && !callConnected)
    {
        result = 1;
        goto end;
    }

end:
    printf("%s(%s): %d\n", __FUNCTION__, ip, result);
    return result;
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

void SceneInit(void)
{
    struct mq_attr attr;

#ifdef _WIN32
    window = SDL_CreateWindow("Doorbell Outdoor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, 0);
    if (!window)
    {
        printf("Couldn't create window: %s\n", SDL_GetError());
        return;
    }
#endif // _WIN32

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_COMMAND_QUEUE_SIZE;
    attr.mq_msgsize = sizeof(Command);

    commandQueue = mq_open("scene", O_CREAT | O_NONBLOCK, 0644, &attr);
    assert(commandQueue != -1);

    currCallID          = 0;
    currRemoteAddr[0]   = '\0';
    callReceived        = false;
    callConnected       = false;
    currCallingCount    = -1;
    currCameraCount     = 0;
	camera_timeout     = 0;

    linphonec_call_end_notify_callback                  = SceneCallEnd;
    linphonec_call_imcoming_received_notify_callback    = SceneCallReceived;
    linphonec_call_connected_notify_callback            = SceneCallConnected;
    linphonec_call_error_notify_callback                = SceneCallError;
    linphonec_call_allow_call_waiting_callback          = SceneCallAllowCallWaiting;
	linphonec_call_is_camera_viewing_callback           = SceneCameraIsViewing;
}

void SceneExit(void)
{
    mq_close(commandQueue);
    commandQueue = -1;
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
#ifdef CFG_LOOP_DIALING
    loopDialingDelay = 0;
#endif

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

static void CallConnected(char* addr, int id)
{
    puts(__FUNCTION__);
    calling_flag = false;
	callend_flag = false;
	hint_sound = false;

    strcpy(currRemoteAddr, addr);

    callConnected = CallingConnectedOnCustom();//ture
    currCallID = id;

    if (!callReceived)
    {
        LinphoneTerminateOthers(id);
        EventWriteCalloutLog(addr ? addr : currRemoteAddr, EVENT_CALLOUT_START);
    }
}

static void CallReceived(char* username, char* addr, int id)
{
    printf("%s(): %d\n", __FUNCTION__, id);
    calling_flag = false;

    if (username && strcmp(username, "camera") == 0)
    {
#ifdef CFG_EMERGENCY_DOORCAMERA 
        DeviceInfo info;
        AddressBookGetDeviceInfo(&info, addr);
        if (info.type == DEVICE_MANAGER)
            LinphoneAnswer();
#endif
        currCameraCount++;
		camera_timeout = 0;
    }
    else
    {
        currCallID = id;
        callReceived = true;
    }
}

static void CallEnd(char* addr, int id)
{
    printf("%s(%s, %d): %d\n", __FUNCTION__, addr, id, currCallID);
#ifdef CFG_LOOP_DIALING
    loopDialingDelay = 0;
#endif
    calling_flag = false;
	callend_flag = true;

    if (id == currCallID || (currCallID == 0 && !callConnected))
    {
        if (!callReceived)
        {
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);
        }
        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        linphoneCastor3.calling_ip_count = 0;
        callReceived = false;
        callConnected = false;
    }

    if (currCameraCount > 0)
        currCameraCount--;
	if(currCameraCount == 0)
		camera_timeout = 0;
}

static void CallError(char* addr, int id, int code)
{
    printf("%s(%s, %d, %d): %d\n", __FUNCTION__, addr, id, code, currCallingCount);
#ifdef CFG_LOOP_DIALING
    loopDialingDelay = 0;
#endif

    if (currCallingCount == -1)
    {
        if (linphoneCastor3.calling_ip_count > 0)
            currCallingCount = linphoneCastor3.calling_ip_count;
        else
            currCallingCount = 1;
    }
	if(code >= 600)
		currCallingCount = 1;
	
	//if (currCallingCount == 1)
	//{
	//	callend_flag = true;
	//	calling_flag = false;
	//}
	
    if (--currCallingCount == 0 || code >= 600 || code == 486)
    {
        printf("CallError: %d\n", code);

        if (!callReceived)
        {
            EventWriteCalloutLog(addr, callConnected ? EVENT_CALLOUT_END : EVENT_CALLOUT_NOACK);
        }

        if (linphoneCastor3.calling_ip_count > 0)
            LinphoneTerminateOthers(id);

        currCallID = 0;
        currRemoteAddr[0] = '\0';
        currCallingCount = -1;
        linphoneCastor3.calling_ip_count = 0;
        callReceived = false;
        callConnected = false;
        callend_flag = true;
		calling_flag = false;

        if (currCameraCount > 0)
            currCameraCount--;
		if (hint_sound)
    		LinphonePlayHintSound(HINT_SOUND_REDIAL);
    }
#ifdef CFG_EMERGENCY_DOORCAMERA        
        CallingCall(NULL);
#endif
}

static void ProcessCommand(void)
{
    Command cmd;

    while (mq_receive(commandQueue, (char*)&cmd, sizeof(Command), 0) > 0)
    {
        switch (cmd.id)
        {
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
        }
    }
}

static bool CheckQuitValue(void)
{
    if (quitValue)
    {
        if (quitValue == QUIT_UPGRADE_ADDRESSBOOK || quitValue == QUIT_UPGRADE_CARDLIST)
        {
            static bool upgrading = false;

            if (upgrading)
            {
                if (UpgradeIsFinished())
                {
                    SceneQuit(QUIT_NONE);
                    upgrading = false;
                }
            }
            else
            {
                UpgradeProcess(quitValue);
                upgrading = true;
            }
        }
        else
            return true;
    }
    return false;
}

int SceneRun(void)
{
    SDL_Event ev;
    int tick, delay, i;
    int count_to_check = 0;
	dictionary* cfgPrivateIni;
	char room_num[8];

    //---light on green led
    ithGpioSet(20);
    ithGpioSetOut(20);
    ithGpioSetMode(20,ITH_GPIO_MODE0);
    //------
    //get room number
    cfgPrivateIni = iniparser_load(CFG_PRIVATE_DRIVE ":/" INI_FILENAME);
	if (!cfgPrivateIni)
	    sprintf(room_num, "%s", "101");
	else
		sprintf(room_num, "%s%s", iniparser_getstring(cfgPrivateIni, "doorbell:floor", ""), iniparser_getstring(cfgPrivateIni, "doorbell:room", ""));
	//printf("############set room %s\n", room_num);
    /* Watch keystrokes */
    for (;;)
    {
        if (CheckQuitValue())
            break;

        ProcessCommand();

        tick = SDL_GetTicks();

#ifdef CFG_LOOP_DIALING
        if(calling_flag == false && callend_flag == true && (++loopDialingDelay >= CFG_LOOP_DIALING_DELAY))
        {

            calling_flag = true;
            callend_flag = false;
            count_to_check = 0;      
#ifdef CFG_EMERGENCY_DOORCAMERA
            printf("call admin\n");
            hint_sound = false;
            CallingPollingIndexReset();
            CallingCall(NULL);	
#else
            printf("call indoor\n");
            hint_sound = true;
            CallingCall(room_num);	  
#endif
		
		}
#endif	
		if(calling_flag||callConnected)
        {
			if (count_to_check%50 == 0)
				CallingOnTimer();
			count_to_check++;
			pre_count++;
        }
        else
            usleep(1000);

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

        while (SDL_PollEvent(&ev))
        {
        	if(calling_flag)
	        {
				if (count_to_check%50 == 0)
					CallingOnTimer();
				count_to_check++;
				pre_count++;
	        }
	        else
	            usleep(1000);
            switch (ev.type)
            {
            case SDL_KEYDOWN:
                if (currCameraCount > 0)
                {
                    LinphoneTerminate();
                    SceneHangUp();
                    currCameraCount = 0;
                }
                switch (ev.key.keysym.sym)
                {
                case SDLK_UP:
					if(calling_flag && pre_count > 0 && pre_count <= 20)
					{
						pre_count = 1;
						break;
					}	
                    if(calling_flag == false && callend_flag == true)
			        {
			            if (currCameraCount > 0 && !callReceived)
			            {
			                LinphoneTerminate();
			                SceneHangUp();
			                currCameraCount = 0;
			            }

			            if (callReceived)
			            {
			                if (!callConnected)
			                {
			                    puts("answer call");
								count_to_check = 1;
			                    LinphoneAnswer();
			                }
			            }
						else
						{
			            	calling_flag = true;
							callend_flag = false;
			            	count_to_check = 0;
							pre_count = 0;
#ifdef CFG_EMERGENCY_DOORCAMERA
                            printf("call admin\n");
         		        	hint_sound = false;
                            CallingPollingIndexReset();
                            CallingCall(NULL);	
#else
                            printf("call indoor\n");
			            	hint_sound = true;
                            CallingCall(room_num);	  
#endif
						}	
			        }
					else if(count_to_check)
					{
						LinphoneTerminate();
			            SceneHangUp();
						callend_flag = true;
						calling_flag = false;
						hint_sound = false;
			            currCameraCount = 0;
						count_to_check = 0;
					}
                    break;

                case SDLK_DOWN:
                    puts("hang up");
                    LinphoneTerminate();
                    SceneHangUp();
                    break;

                case SDLK_LEFT:
                    puts("SDLK_LEFT");
                    break;

                case SDLK_RIGHT:
                    puts("SDLK_RIGHT");
                    break;

            #ifdef _WIN32
                case SDLK_f:
                    SnapshotTake();
                    break;
            #endif // _WIN32        
                }
                break;

            case SDL_KEYUP:
                break;
            }
        }

        delay = 33 - (SDL_GetTicks() - tick);
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

void SceneFlagReset(void)
{
    callend_flag = true;
	calling_flag = false;
    if(hint_sound)
        LinphonePlayHintSound(HINT_SOUND_REDIAL);
	hint_sound = false;
	currCameraCount = 0;
}

#ifdef CFG_EMERGENCY_DOORCAMERA
void sceneFlagdail(void)
{
    callend_flag = false;
    calling_flag = true;
}
#endif