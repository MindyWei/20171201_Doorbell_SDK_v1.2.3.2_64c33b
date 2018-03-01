#include <sys/stat.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/audio.h"
#include "doorbell.h"

static int cur_mic_level;
static int cur_ring_level;
static int cur_warn_level;

void AudioRecordInit(void)
{
    // TODO: IMPLEMENT
}

void AudioRecordExit(void)
{
    // TODO: IMPLEMENT
}

int AudioRecordStartRecord(char* filepath)
{
    int set_mic_level, set_warn_level;
    cur_mic_level = LinphoneGetMicLevel();
    set_mic_level = (cur_mic_level+20)>100 ? 100 : (cur_mic_level+20);
    LinphoneSetMicLevel(set_mic_level);
    cur_ring_level = LinphoneGetRingLevel();
    cur_warn_level = LinphoneGetWarnLevel();
    set_warn_level = (cur_warn_level-20)<=0 ? 20 : (cur_warn_level-20);
    if(LinphoneGetWarnSoundPlaying())
        LinphoneSetRingLevel(set_warn_level, true);
    else
        LinphoneSetRingLevel(cur_ring_level, true);

    LinphoneStartVoiceMemoRecord(filepath);
    return 0;
}

int AudioRecordStopRecord(void)
{
    LinphoneStopVoiceMemoRecord();
    LinphoneSetMicLevel(cur_mic_level);
    LinphoneSetRingLevel(cur_ring_level, true);
    return 0;
}

int AudioStreamRecordStartRecord(char* filepath)
{
    LinphoneStartAudioStreamRecord(filepath);
    return 0;
}

int AudioStreamRecordStopRecord(void)
{
    LinphoneStopAudioStreamRecord();
    return 0;
}

int AudioRecordGetTimeLength(char* filepath)
{
    int time_length = 0;
    struct stat st;

    stat(filepath, &st);
    time_length = st.st_size/1000;
    return time_length;
}

int AudioRecordStartPlay(char* filepath)
{
    LinphoneStartVoiceMemoPlay(filepath);
    return 0;
}

void AudioRecordStopPlay(void)
{
    LinphoneStopVoiceMemoPlay();
}
