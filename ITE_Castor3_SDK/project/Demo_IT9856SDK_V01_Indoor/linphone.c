#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "isp/mmp_isp.h"
#include "ite/itp.h"
#include "i2s/i2s.h"
#include "linphone/linphone_castor3.h"
#include "doorbell.h"
#include "scene.h"

#define LINPHONE_STACK_SIZE (80000)
#ifdef CFG_DUAL_REGISTER_SIP_SERVER
#define SIP_REGISTER_TIMEOUT_MSEC     (10 * 60 * 1000)   //10min
#define REGISTER_1_IS_MAIN 1
#define REGISTER_2_IS_MAIN 2
#endif

static char lpVideoMsgUri[128];
static bool lpInDoNotDisturb;
static bool lpInMute;
static bool lpInited;
static bool lpQuit;
static bool lpWarnSoundPlaying;
static bool lpKeySoundPaused;
static bool lpSettingMode;

static void LinphoneInitialized(void)
{
#ifdef CFG_REGISTER_SIP_SERVER
    LinphoneRegister();
#endif
    SceneUpdateIndicators();
}

static void* LinphoneTask(void* arg)
{
    static char* args[] =
    {
        "linphonec",
        "-D",
    #ifndef NDEBUG
        "-d",
        "6"
    #endif
    };
    struct mq_attr attr;

    linphonec_camera_enabled                = 0;
    linphonec_initialized_notify_callback   = LinphoneInitialized;

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    lp_config_sync_notify_callback = ConfigUpdateCrc;
#endif

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = PROMPT_MAX_LEN;

    linphoneCastor3.mq = mq_open("linphone", O_CREAT, 0644, &attr);
    assert(linphoneCastor3.mq != -1);

    lpInited = true;
    return (void*) linphonec_main(ITH_COUNT_OF(args), args);
}

#ifdef CFG_DUAL_REGISTER_SIP_SERVER
void LinphoneCreateRegTask(void)
{
	pthread_t reg_task;
    pthread_attr_t reg_attr;

	pthread_attr_init(&reg_attr);
	pthread_attr_setstacksize(&reg_attr, LINPHONE_STACK_SIZE);
	pthread_create(&reg_task, &reg_attr, LinphoneRegTask, NULL);
}

void* LinphoneRegTask(void)
{
    unsigned long	ms = 0;
	linphoneCastor3.DualSIP.sip_register_order = REGISTER_1_IS_MAIN; //default  sip registration order, test only

	LinphoneRegisterAll(linphoneCastor3.DualSIP.sip_register_order);

	while(linphoneCastor3.DualSIP.sip_account_1 || linphoneCastor3.DualSIP.sip_account_2){
		sleep(1);
		ms += 1000;

		if (ms >= SIP_REGISTER_TIMEOUT_MSEC){
			LinphoneRegisterAll(linphoneCastor3.DualSIP.sip_register_order);
			ms = 0;
		}
	}

	printf("SIP accounts are all registration failed.\n");

	return;
}

void LinphoneAccount_1(void)
{
    char prompt[PROMPT_MAX_LEN];

	sprintf(prompt, "register sip:%s@%s sip:%s %s",
		CFG_REGISTER_ACCOUNT, CFG_REGISTER_DOMAIN, CFG_REGISTER_DOMAIN, CFG_REGISTER_PWD);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

	//Reference LinphoneRegistrationState at linphonecore.h
	while(linphoneCastor3.DualSIP.sip_register_status!= Linphone_RegistrationOk){
		usleep(100000);
	}

	if (linphoneCastor3.DualSIP.sip_register_status == Linphone_RegistrationOk)
		linphoneCastor3.DualSIP.sip_account_1 = 1; //Already registration success:1

	linphoneCastor3.DualSIP.sip_register_status = Linphone_RegistrationNone; //reset status to LinphoneRegistrationNone.
}
void LinphoneAccount_2(void)
{
    char prompt[PROMPT_MAX_LEN];

	sprintf(prompt, "register sip:%s@%s sip:%s %s",
		CFG_REGISTER_ACCOUNT_2, CFG_REGISTER_DOMAIN_2, CFG_REGISTER_DOMAIN_2, CFG_REGISTER_PWD_2);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

	while(linphoneCastor3.DualSIP.sip_register_status!= Linphone_RegistrationOk){
		usleep(100000);
	}

	if (linphoneCastor3.DualSIP.sip_register_status == Linphone_RegistrationOk)
		linphoneCastor3.DualSIP.sip_account_2 = 1;

	linphoneCastor3.DualSIP.sip_register_status = Linphone_RegistrationNone;
}

void LinphoneRegisterAll(int priority)
{
	switch (priority){
		case REGISTER_1_IS_MAIN:
			LinphoneAccount_2();
			LinphoneAccount_1();

			break;

		case REGISTER_2_IS_MAIN:
			LinphoneAccount_1();
			LinphoneAccount_2();

			break;
	}
}
#endif

static char* LinphonecCallGetAddr(char* ip)
{
    DeviceInfo info;
    char buf[64];

    AddressBookGetDeviceInfo(&info, ip);

    if (info.type == DEVICE_INDOOR || info.type == DEVICE_INDOOR2)
        sprintf(buf, "%s-%s-%s-%s-%s", info.area, info.building, info.unit, info.floor, info.room);
    else
        sprintf(buf, "%s-%s-%s-%s-%s-%s", info.area, info.building, info.unit, info.floor, info.room, info.ext);

#ifdef CFG_DBG_RMALLOC
    return Rstrdup(buf, RM_FILE_POS);
#else
    return strdup(buf);
#endif
}

void LinphoneInit(void)
{
    pthread_t task;
    pthread_attr_t attr;

    lpVideoMsgUri[0] = '\0';

    linphoneCastor3.key_level = theConfig.keylevel;
    linphoneCastor3.warn_level = theConfig.warnlevel;
    linphonec_call_get_addr_callback = LinphonecCallGetAddr;

    lpInited = false;
    lpQuit = false;
    lpInDoNotDisturb = false;
    lpInMute = false;
    lpWarnSoundPlaying = false;
    lpKeySoundPaused = false;
    lpSettingMode = false;

    pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, LINPHONE_STACK_SIZE);

    pthread_create(&task, &attr, LinphoneTask, NULL);

#ifdef CFG_DUAL_REGISTER_SIP_SERVER
	usleep(500000);

	LinphoneCreateRegTask();
#endif
}

void LinphoneExit(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    lpQuit = true;

    strcpy(prompt, "quit");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    while (!linphoneCastor3.quit)
        usleep(100000);

    mq_close(linphoneCastor3.mq);
}

int LinphoneGetVoiceLevel(void)
{
    return linphoneCastor3.play_level;
}

void LinphoneSetVoiceLevel(int level)
{
    char prompt[PROMPT_MAX_LEN];

    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.play_level = level;

    if(lpSettingMode){
        i2s_set_direct_volperc(level);
        lpSettingMode = false;
        return;
    }

    strcpy(prompt, "castor3-set-play-level");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

int LinphoneGetMicLevel(void)
{
    return linphoneCastor3.rec_level;
}

void LinphoneSetMicLevel(int level)
{
    char prompt[PROMPT_MAX_LEN];

    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.rec_level = level;

    if(lpSettingMode){
        i2s_ADC_set_direct_volstep(level);
        lpSettingMode = false;
        return;
    }

    strcpy(prompt, "castor3-set-rec-level");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphonePlayRingSound(int arg)
{
    char prompt[PROMPT_MAX_LEN];

    if (theConfig.ringsound[0] == '\0' || !lpInited || lpWarnSoundPlaying)
        return;

    strcpy(prompt, "castor3-play-ring " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, theConfig.ringsound);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

int LinphoneGetRingLevel(void)
{
    return linphoneCastor3.ring_level;
}

void LinphoneSetRingLevel(int level, bool active)
{
    char prompt[PROMPT_MAX_LEN];

    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.ring_level = level;

    if(lpSettingMode){
        if(active) i2s_set_direct_volperc(level); //derect set volume level
        lpSettingMode = false;
        return;
    }

    strcpy(prompt, "castor3-set-ring-level");
    if (active)
        strcat(prompt, " active");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);//send to command queue set volume level
}

void LinphoneSetRingSound(char* file)
{
    char prompt[PROMPT_MAX_LEN];

    if (file[0] == '\0' || !lpInited)
        return;

    strncpy(theConfig.ringsound, file, sizeof (theConfig.ringsound) - 1);

    strcpy(prompt, "castor3-set-ring " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, file);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneSetKeyLevel(int level)
{
    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.key_level = level;
    theConfig.keylevel = level;
}

void LinphonePlayKeySound(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (theConfig.keysound[0] == '\0' || !lpInited || lpWarnSoundPlaying || lpKeySoundPaused)
        return;

    strcpy(prompt, "castor3-play-keysound " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, theConfig.keysound);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphonePauseKeySound(void)
{
    lpKeySoundPaused = true;
}

void LinphoneResumeKeySound(void)
{
    lpKeySoundPaused = false;
}

int LinphoneGetWarnLevel(void)
{
    return linphoneCastor3.warn_level;
}

void LinphoneSetWarnLevel(int level)
{
    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.warn_level = level;
    theConfig.warnlevel = level;
}

void LinphonePlayWarnSound(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-play-warnsound " CFG_PRIVATE_DRIVE ":/rings/warn.wav");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    lpWarnSoundPlaying = true;
}

void LinphoneCall(char* ip, bool video, bool earlyMedia)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "call sip:toto@");
    strcat(prompt, ip);

    if (!video)
        strcat(prompt, " --audio-only");

    if (earlyMedia)
        strcat(prompt, " --early-media");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneWatchCamera(char* ip)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-watch-camera sip:camera@");
    strcat(prompt, ip);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopCamera(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-camera");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneAnswer(int id)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    if (id > 0)
        sprintf(prompt, "answer %d", id);
    else
        strcpy(prompt, "answer");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneMuteMic(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "mute");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneUnMute(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "unmute");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneTerminate(int id)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    if (id)
        sprintf(prompt, "terminate %d", id);
    else
        strcpy(prompt, "terminate all");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneAddFriend(DeviceInfo* info, bool blacklist, char* comment)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    if (info->type == DEVICE_INDOOR || info->type == DEVICE_INDOOR2)
        sprintf(linphoneCastor3.friend_addr, "%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ip);
    else
        sprintf(linphoneCastor3.friend_addr, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext, info->ip);

    sprintf(linphoneCastor3.friend_sip, "sip:%s", info->ip);
    linphoneCastor3.friend_comment = comment;

    strcpy(prompt, "castor3-add-friend ");
    strcat(prompt, blacklist ? "1" : "0");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneUpdateFriend(char* addr, bool blacklist, char* comment)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    linphoneCastor3.friend_comment = comment;

    strcpy(prompt, "castor3-update-friend ");
    strcat(prompt, addr);
    strcat(prompt, blacklist ? " 1" : " 0");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneDeleteFriend(char* addr)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-delete-friend ");
    strcat(prompt, addr);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneClearFriends(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-clear-friends");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneClearMissedLogs(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-clear-call-logs-missed");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneClearReceivedLogs(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-clear-call-logs-received");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneClearSentLogs(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-clear-call-logs-sent");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneDeleteCallLog(int refkey)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    sprintf(prompt, "castor3-delete-call-log %d", refkey);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneSetWindowID(void* widget)
{
#ifdef _WIN32
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    sprintf(prompt, "vwindow id 0x%X", widget);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
#else
    int x, y, width, height;

    ituWidgetGetGlobalPosition(widget, &x, &y);
    width = ituWidgetGetWidth(widget);
    height = ituWidgetGetHeight(widget);

    printf("video window: x=%d y=%d w=%d h=%d\n", x, y, width, height);
    itv_set_video_window(x, y, width, height);
#endif // _WIN32
}

static LinphoneInitVideoMessageUri(void)
{
    // zzz <sip:xx-xx-xx-xx-xx@yyy.yyy.yyy.yyy>
    strcpy(lpVideoMsgUri, "%d <sip:");
    strcat(lpVideoMsgUri, theDeviceInfo.area);
    strcat(lpVideoMsgUri, "-");
    strcat(lpVideoMsgUri, theDeviceInfo.building);
    strcat(lpVideoMsgUri, "-");
    strcat(lpVideoMsgUri, theDeviceInfo.unit);
    strcat(lpVideoMsgUri, "-");
    strcat(lpVideoMsgUri, theDeviceInfo.floor);
    strcat(lpVideoMsgUri, "-");
    strcat(lpVideoMsgUri, theDeviceInfo.room);
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(lpVideoMsgUri, "-");
        strcat(lpVideoMsgUri, theDeviceInfo.ext);
    }
    strcat(lpVideoMsgUri, "@");
    strcat(lpVideoMsgUri, theDeviceInfo.ip);
    strcat(lpVideoMsgUri, ">");
}

void LinphonePlayVideoMessage(char* ip, int id)
{
    char prompt[PROMPT_MAX_LEN];

    if (!NetworkServerIsReady() || !lpInited)
        return;

    if (lpVideoMsgUri[0] == '\0')
        LinphoneInitVideoMessageUri();

    sprintf(linphoneCastor3.video_msg_uri, lpVideoMsgUri, id);

    strcpy(prompt, "castor3-play-video-msg sip:toto@");
    strcat(prompt, ip);
    strcat(prompt, " --play-video-msg");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneCallDevices(DeviceInfo infoArray[], int count, bool video, bool earlyMedia)
{
    char prompt[PROMPT_MAX_LEN];
    int i;

    if (!lpInited)
        return;

    if (count > LINPHONE_CASTOR3_MAX_IP_COUNT)
        count = LINPHONE_CASTOR3_MAX_IP_COUNT;

    for (i = 0; i < count; i++)
    {
        strcpy(linphoneCastor3.calling_ip_array[i], infoArray[i].ip);
    }
    linphoneCastor3.calling_ip_count = count;

    strcpy(prompt, "castor3-call");

    if (!video)
        strcat(prompt, " --audio-only");

    if (earlyMedia)
        strcat(prompt, " --early-media");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneTerminateOthers(long id)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    sprintf(prompt, "castor3-terminate-others %d", id);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopSound(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-sound");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
    lpWarnSoundPlaying = false;
}

void LinphoneEnableDoNotDisturb(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-set-do-not-disturb on");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    lpInDoNotDisturb = true;
}

void LinphoneDisableDoNotDisturb(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-set-do-not-disturb off");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    lpInDoNotDisturb = false;
}

int LinphoneCheckDoNotDisturb(void)
{
    time_t rawtime;
    struct tm* timeinfo;
    int currMins, startMins, endMins;

    if (!lpInited)
        return 0;

    if (theConfig.do_not_disturb_mode != DONOT_DISTURB_TIME)
        return 0;

    if (theConfig.do_not_disturb_start_hour == theConfig.do_not_disturb_end_hour &&
        theConfig.do_not_disturb_start_min == theConfig.do_not_disturb_end_min)
        return 0;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    currMins = timeinfo->tm_hour * 60 + timeinfo->tm_min;
    startMins = theConfig.do_not_disturb_start_hour * 60 + theConfig.do_not_disturb_start_min;
    endMins = theConfig.do_not_disturb_end_hour * 60 + theConfig.do_not_disturb_end_min;

    if (lpInDoNotDisturb == false)
    {
        if (startMins < endMins)
        {
            if (currMins >= startMins && currMins < endMins)
            {
                LinphoneEnableDoNotDisturb();
                return 1;
            }
        }
        else
        {
            if (currMins >= startMins || currMins < endMins)
            {
                LinphoneEnableDoNotDisturb();
                return 1;
            }
        }
    }
    else
    {
        if (startMins < endMins)
        {
            if (currMins < startMins || currMins >= endMins)
            {
                LinphoneDisableDoNotDisturb();
                return -1;
            }
        }
        else
        {
            if (currMins < startMins && currMins >= endMins)
            {
                LinphoneDisableDoNotDisturb();
                return -1;
            }
        }
    }
    return 0;
}

bool LinphoneIsInDoNotDisturb(void)
{
    return lpInDoNotDisturb;
}

void LinphoneEnableBusy(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-set-presence busy");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneDisableBusy(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-set-presence online");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneRedirect(char* ip)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "redirect sip:toto@");
    strcat(prompt, ip);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneSnapshot(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "snapshot ");
    strcat(prompt, filepath);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartVoiceMemoRecord(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-voice-memo-record ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopVoiceMemoRecord(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    linphoneCastor3.sync = 0;
    strcpy(prompt, "castor3-stop-voice-memo-record");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    while (linphoneCastor3.sync == 0)
        usleep(100000);
}

void LinphoneStartVoiceMemoPlay(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-voice-memo-play ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopVoiceMemoPlay(void)
{
    char prompt[PROMPT_MAX_LEN];
	int  timeout = 3;

    if (!lpInited)
        return;
	linphoneCastor3.sync = 0;
    strcpy(prompt, "castor3-stop-voice-memo-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
	
	while (linphoneCastor3.sync == 0)
	{
		if (timeout < 0)
			break;
        usleep(100000);
		timeout--;
	}
	
}

void LinphoneStartPlayRec(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;
    LinphoneSetMicLevel(70);
    LinphoneSetRingLevel(70,true);
    strcpy(prompt, "castor3-start-playrec ");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopPlayRec(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-playrec ");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}


void LinphoneRegister(void)
{
    char prompt[PROMPT_MAX_LEN];

	if (theConfig.dhcp){
		while (!theConfig.dhcp_ipaddr_writen){
			sleep(1);
			printf("###Wait SIP registeration...\n");
		}
	}else{
		while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL)){
			sleep(1);
			printf("###Wait SIP registeration...\n");
		}
	}

	if (!theConfig.dhcp){
    if (!lpInited || strcmp(theDeviceInfo.ext, "01") || theCenterInfo.ip[0] == '\0')
        return;
	}

#ifdef CFG_SIP_SERVER_TEST
    if (theConfig.dhcp)
		sprintf(prompt, "register sip:%s@%s sip:%s %s",
			CFG_REGISTER_ACCOUNT, theConfig.dhcp_ipaddr, CFG_REGISTER_DOMAIN, CFG_REGISTER_PWD);
	else
	sprintf(prompt, "register sip:%s@%s sip:%s %s",
		CFG_REGISTER_ACCOUNT, theDeviceInfo.ip, CFG_REGISTER_DOMAIN, CFG_REGISTER_PWD);
#else
    sprintf(prompt, "register sip:%s%s%s%s%s%s@%s sip:%s",
        theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext, theDeviceInfo.ip, theCenterInfo.ip);
#endif

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartIPCamStreaming(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-ipcam-streaming");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopIPCamStreaming(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-ipcam-streaming");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartIPCamRecord(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-ipcam-record ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopIPCamRecord(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-ipcam-record");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneIPCamSnapshot(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-ipcam-snapshot ");
    strcat(prompt, filepath);

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartVideoMemoRecord(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-video-memo-record ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopVideoMemoRecord(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-video-memo-record");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartAudioStreamRecord(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-audiostream-memo-record ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopAudioStreamRecord(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-audiostream-memo-record");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartVideoMemoPlay(char* filepath)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-video-memo-play ");
    strcat(prompt, filepath);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopVideoMemoPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-video-memo-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartMediaVideoPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-media-video-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopMediaVideoPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-media-video-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartMediaAudioPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-media-audio-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopMediaAudioPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-media-audio-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneSettingMode(void)
{
    lpSettingMode = true;//skip mq_send direct setting driver
}

bool LinphoneGetWarnSoundPlaying(void)
{
    return lpWarnSoundPlaying;
}