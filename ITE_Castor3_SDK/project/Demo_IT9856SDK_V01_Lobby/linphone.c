#include <sys/ioctl.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ite/ith.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "linphone/linphone_castor3.h"

#define LINPHONE_STACK_SIZE (80000)

static char lpVideoMsgUri[128];
static bool lpInited, lpQuit, lpKeySoundEnabled;
static bool lpWarnSoundPlaying;

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
        "-C",
    #ifndef NDEBUG
        "-d",
        "6"
    #endif
    };
    struct mq_attr attr;

    linphonec_initialized_notify_callback   = LinphoneInitialized;

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING
    lp_config_sync_notify_callback = ConfigUpdateCrc;
#endif

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = PROMPT_MAX_LEN;

    linphoneCastor3.mq = mq_open("linphone", O_CREAT, 0644, &attr);
    assert(linphoneCastor3.mq != -1);

    lpInited = lpKeySoundEnabled = true;
    return (void*) linphonec_main(ITH_COUNT_OF(args), args);
}

void LinphoneInit(void)
{
    pthread_t task;
    pthread_attr_t attr;

    linphoneCastor3.key_level = theConfig.keylevel;
    lpInited = false;
    lpQuit = false;
    lpWarnSoundPlaying = false;

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

    strcpy(prompt, "castor3-set-rec-level");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphonePlayRingSound(void)
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

void LinphoneSetRingLevel(int level)
{
    char prompt[PROMPT_MAX_LEN];

    if (level < 0 || level > 100 || !lpInited)
        return;

    linphoneCastor3.ring_level = level;

    strcpy(prompt, "castor3-set-ring-level");
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

int LinphoneGetKeyLevel(void)
{
    return linphoneCastor3.key_level;
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

    if (theConfig.keysound[0] == '\0' || !lpInited || !lpKeySoundEnabled || lpWarnSoundPlaying)
        return;

    strcpy(prompt, "castor3-play-keysound " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, theConfig.keysound);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneEnableKeySound(void)
{
    lpKeySoundEnabled = true;
}

void LinphoneDisableKeySound(void)
{
    lpKeySoundEnabled = false;
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

    strcpy(prompt, "castor3-play-warnsound " CFG_PRIVATE_DRIVE ":/rings/");
    strcat(prompt, theConfig.warnsound);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);

    lpWarnSoundPlaying = true;
}

void LinphonePlayHintSound(HintSound snd)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited || lpWarnSoundPlaying)
        return;

    strcpy(prompt, "castor3-play-voicesound " CFG_PRIVATE_DRIVE ":/rings/");

    switch (snd)
    {
    case HINT_SOUND_OPENDOOR:
        strcat(prompt, "opendoor.wav 1");
        break;

    case HINT_SOUND_VOICEMEMO:
        strcat(prompt, "voicememo.wav 7");
        break;

    case HINT_SOUND_ROOM_NOT_EXIST:
        strcat(prompt, "noexist.wav 4");
        break;

    case HINT_SOUND_REDIAL:
        strcat(prompt, "redial.wav 3");
        break;

    default:
        return;
    }
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphonePlayNumberKeySound(char num)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited || lpWarnSoundPlaying)
        return;

    sprintf(prompt, "castor3-play-keysound " CFG_PRIVATE_DRIVE ":/rings/%c.wav", num);
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneCall(char* ip, bool video)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "call sip:toto@");
    strcat(prompt, ip);

    if (!video)
        strcat(prompt, " --audio-only");

    strcat(prompt, " --early-media");

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneAnswer(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "answer");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneTerminate(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "terminate all");
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

    linphoneCastor3.calling_ip_count = 0;
    for (i = 0; i < count; i++)
    {
        if (infoArray[i].type == DEVICE_INDOOR || infoArray[i].type == DEVICE_INDOOR2)
        {
            strcpy(linphoneCastor3.calling_ip_array[linphoneCastor3.calling_ip_count], infoArray[i].ip);
            linphoneCastor3.calling_ip_count++;
        }
    }

    strcpy(prompt, "castor3-call");

    if (!video)
        strcat(prompt, " --audio-only");

    if (earlyMedia)
        strcat(prompt, " --early-media");

#ifdef CFG_DUAL_STREAM
	strcat(prompt, " --call-mobile");
#endif

#if !defined(ENABLE_VIDEO_MULTICAST)
    strcat(prompt, " --other-only-ring");
#endif

#ifdef CFG_CALL_SIP_SERVER
    sprintf(linphoneCastor3.sip_server_uri, "sip:%s%s%s%s%s%s@%s", infoArray[0].area, infoArray[0].building, infoArray[0].unit, infoArray[0].floor, infoArray[0].room, infoArray[0].ext, theCenterInfo.ip);
#else
    linphoneCastor3.sip_server_uri[0] = '\0';

#endif // CFG_CALL_SIP_SERVER

    linphoneCastor3.calling_mobile_count = 0;
    for (i = 0; i < count; i++)
    {
        if (infoArray[i].type == DEVICE_MOBILE)
        {
            strcpy(linphoneCastor3.calling_mobile_array[linphoneCastor3.calling_mobile_count], infoArray[i].ip);
            linphoneCastor3.calling_mobile_count++;
        }
    }
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

static LinphoneInitVideoMessageUri(void)
{
    // "zz-zz-zz-zz-zz" <sip:xx-xx-xx-xx-xx@yyy.yyy.yyy.yyy>
    strcpy(lpVideoMsgUri, "\"%s-%s-%s-%s-%s\" <sip:");
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

void LinphoneRecordVideoMessage(DeviceInfo* info)
{
    char prompt[PROMPT_MAX_LEN];

    if (!NetworkIsReady() || !lpInited)
        return;

    if (info->type == DEVICE_INDOOR2)
    {
        strcpy(prompt, "call sip:voicememo@");
        strcat(prompt, info->ip);
        strcat(prompt, " --early-media");
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
        strcat(prompt, " --audio_only_send");
    }
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneRegister(void)
{
    char prompt[PROMPT_MAX_LEN];

#if !defined(CFG_NET_ETHERNET_DHCP)
    if (!lpInited || theCenterInfo.ip[0] == '\0')
        return;
#endif

	while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL)){
		sleep(1);
	}

#ifdef CFG_SIP_SERVER_TEST
	sprintf(prompt, "register sip:%s@%s sip:%s %s",
		CFG_REGISTER_ACCOUNT, theDeviceInfo.ip, CFG_REGISTER_DOMAIN, CFG_REGISTER_PWD);
#else
    sprintf(prompt, "register sip:%s%s%s%s%s%s@%s sip:%s",
        theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext, theDeviceInfo.ip, theCenterInfo.ip);
#endif

    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStartADVPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-start-adv-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

void LinphoneStopADVPlay(void)
{
    char prompt[PROMPT_MAX_LEN];

    if (!lpInited)
        return;

    strcpy(prompt, "castor3-stop-adv-play");
    mq_send(linphoneCastor3.mq, prompt, PROMPT_MAX_LEN, 0);
}

