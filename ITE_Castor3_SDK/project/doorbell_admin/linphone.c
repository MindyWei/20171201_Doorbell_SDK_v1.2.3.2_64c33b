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

static char lpVideoMsgUri[128];
static bool lpInDoNotDisturb;
static bool lpInMute;
static bool lpInited;
static bool lpQuit;
static bool lpWarnSoundPlaying;
static bool lpSettingMode;

static void LinphoneInitialized(void)
{
#ifdef CFG_REGISTER_SIP_SERVER
    LinphoneRegister();
#endif
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

    linphonec_camera_enabled = 0;
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

static char* LinphonecCallGetAddr(char* ip)
{
    DeviceInfo info;
    char buf[64];
    AddressBookGetDeviceInfo(&info, ip);
    sprintf(buf, "%s-%s-%s-%s-%s", info.area, info.building, info.unit, info.floor, info.room);
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
    linphonec_call_get_addr_callback = LinphonecCallGetAddr;
    
    lpInited = false;
    lpQuit = false;
    lpInDoNotDisturb = false;
    lpInMute = false;
    lpWarnSoundPlaying = false;
    lpSettingMode = false;

    pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, LINPHONE_STACK_SIZE);

    pthread_create(&task, &attr, LinphoneTask, NULL);
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
        lpSettingMode = false;
        return;
    }

    strcpy(prompt, "castor3-set-ring-level");
    if (active)
        strcat(prompt, " active");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
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

    if (theConfig.keysound[0] == '\0' || !lpInited || lpWarnSoundPlaying)
        return;

    strcpy(prompt, "castor3-play-keysound " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, theConfig.keysound);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
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

void LinphoneBindIPCamEnable(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (theConfig.ipcam_ipaddr[0] == '\0' || !lpInited)
        return;

    strcpy(prompt, "castor3-bind-ipcam ");
    strcat(prompt, theConfig.ipcam_ipaddr);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneBindIPCamDisable(void)
{
    char prompt[PROMPT_MAX_LEN];

    strcpy(prompt, "castor3-cancel-bind-ipcam");
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

void LinphoneUnMuteMic(void)
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

    //if (info->ext[0] == '\0')
        sprintf(linphoneCastor3.friend_addr, "%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ip);
    //else
    //    sprintf(linphoneCastor3.friend_addr, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext, info->ip);

    sprintf(linphoneCastor3.friend_sip, "sip:%s", info->ip);
    linphoneCastor3.friend_comment = comment;

    strcpy(prompt, "castor3-add-friend ");
    strcat(prompt, blacklist ? "1" : "0");
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

void LinphoneCallDevices(DeviceInfo infoArray[], int count, bool video, bool earlyMedia, bool videoFromIPCam)
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

    if (videoFromIPCam)    
        strcat(prompt, " --video-from-ipcam");
    
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

bool LinphoneIsInDoNotDisturb(void)
{
    return lpInDoNotDisturb;
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

void LinphoneTransfer(char* ip)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "transfer sip:toto@");
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

void LinphoneRegister(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited || theCenterInfo.ip[0] == '\0')
        return;

#ifdef CFG_SIP_SERVER_TEST
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

void LinphoneRecordVideoMessage(DeviceInfo* info, bool videoFromIPCam)
{
    char prompt[PROMPT_MAX_LEN];

    if (!NetworkIsReady() || !lpInited)
        return;

    if (info->type == DEVICE_INDOOR2)
    {
        strcpy(prompt, "call sip:voicememo@");
        strcat(prompt, info->ip);
        strcat(prompt, " --early-media");

        if (videoFromIPCam)    
            strcat(prompt, " --video-from-ipcam");
    }
    else
    {
        if (theCenterInfo.ip[0] == '\0')
            return;

        if (lpVideoMsgUri[0] == '\0')
            LinphoneInitVideoMessageUri();

        sprintf(linphoneCastor3.video_msg_uri, lpVideoMsgUri, info->area, info->building, info->unit, info->floor, info->room);

        strcpy(prompt, "castor3-record-video-msg sip:toto@");
        strcat(prompt, theCenterInfo.ip);
    }
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneSettingMode(void)
{
    lpSettingMode = true;//skip mq_send direct setting driver
}

