#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/audio.h"
#include "doorbell.h"
#include "castor3player.h"

void VideoRecordInit(void)
{
    // TODO: IMPLEMENT
}

void VideoRecordExit(void)
{
    // TODO: IMPLEMENT
}

int IPCamStreamStartStreaming(void)
{
    LinphoneStartIPCamStreaming();
    return 0;
}

int IPCamStreamStopStreaming(void)
{
    LinphoneStopIPCamStreaming();
    return 0;
}

int IPCamRecordStartRecord(char* filepath)
{
    LinphoneStartIPCamRecord(filepath);
    return 0;
} 

int IPCamRecordStopRecord(void)
{
    LinphoneStopIPCamRecord();
    return 0;
}

int VideoRecordStartRecord(char* filepath)
{
    LinphoneStartVideoMemoRecord(filepath);
    return 0;
}

int VideoRecordStopRecord(void)
{
    LinphoneStopVideoMemoRecord();
    return 0;
}

int VideoRecordGetTimeLength(char* filepath)
{
    int totaltime = 0;
    mtal_pb_get_total_duration_ext(&totaltime, filepath);
    
    return totaltime;
}

static VideoRecordPlayFinishedCallback videoRecodrdPlayFinishedCallback;

static void VideoRecordPlaybackEventCallback(int event_id, void *arg)
{
    if (!videoRecodrdPlayFinishedCallback)
        return;

    switch (event_id)
    {
    case 0:
    case 1:
    case 2:
        videoRecodrdPlayFinishedCallback();
        break;
    }
}

extern void (*linphonec_video_memo_playback_event_callback)(int event_id, void *arg);

int VideoRecordStartPlay(char* filepath, VideoRecordPlayFinishedCallback func)
{
    videoRecodrdPlayFinishedCallback = func;
    linphonec_video_memo_playback_event_callback = VideoRecordPlaybackEventCallback;
    LinphoneStartVideoMemoPlay(filepath);
    return 0;
}

void VideoRecordStopPlay(void)
{
    LinphoneStopVideoMemoPlay();
}
