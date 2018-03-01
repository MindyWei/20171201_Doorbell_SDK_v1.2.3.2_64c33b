#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include "ite/audio.h"
#include "main_processor_message_queue.h"
#include "audio_mgr.h"
#include "doorbell.h"

MMP_INT
smtkAudioMgrPlayNetwork(
    SMTK_AUDIO_PARAM_NETWORK* pNetwork);

static void *AL_Local_player_http_handle = NULL;

#define AUDIOLINK_LOCAL_PLAYER_BUFFER_LEN            (64 * 1024)

void AudioInit(void)
{
    smtkAudioMgrInitialize();
}

void AudioExit(void)
{
    // TODO: IMPLEMENT
}

int AudioPlay(char* filename)
{
    void *local_player_http_handle = NULL;

    SMTK_AUDIO_PARAM_NETWORK audiomgr_local;

    int nResult = 0;
    // close handler (if any)
    if (AL_Local_player_http_handle) {
        fclose(AL_Local_player_http_handle);            
        AL_Local_player_http_handle = NULL;
    }

    if (filename)
        AL_Local_player_http_handle = fopen(filename, "rb");

    if (!AL_Local_player_http_handle) {
        printf("[Main]%s() L#%ld: fopen error \r\n", __FUNCTION__, __LINE__);
        return -1;
    }
    printf("[Main]%s() L#%ld:  %s success \r\n", __FUNCTION__, __LINE__,filename);
    audiomgr_local.pHandle = AL_Local_player_http_handle;
    audiomgr_local.LocalRead = fread;
    audiomgr_local.nReadLength = AUDIOLINK_LOCAL_PLAYER_BUFFER_LEN;
    audiomgr_local.nType = SMTK_AUDIO_WAV;
    audiomgr_local.bSeek = 0;
    audiomgr_local.nM4A = 0;
    audiomgr_local.bLocalPlay = 1;
#ifdef __OPENRTOS__
    smtkAudioMgrQuickStop();
#endif
    nResult = smtkAudioMgrPlayNetwork(&audiomgr_local);
    return 0;
}

void AudioStop(void)
{
    smtkAudioMgrQuickStop();
}

void AudioPlayKeySound(void)
{
    char filepath[PATH_MAX];

    i2s_deinit_DAC();

    smtkAudioMgrSetVolume(theConfig.keylevel);

    strcpy(filepath, CFG_PRIVATE_DRIVE ":/rings/");
    strcat(filepath, theConfig.keysound);
    AudioPlay(filepath);
}

void AudioSetKeyLevel(int level)
{
    if (level <= 0 || level > 100)
        return;

    theConfig.keylevel = level;
}

void AudioMute(void)
{
    smtkAudioMgrMuteOn();
}

void AudioUnMute(void)
{
    smtkAudioMgrMuteOff();
}

bool AudioIsMuted(void)
{
    return smtkAudioMgrIsMuteOn() ? true : false;
}
