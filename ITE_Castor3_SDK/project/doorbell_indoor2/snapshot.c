#include <sys/time.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "doorbell.h"
#include "ite_encoder.h"

#define MAX_SNAPSHOT_COUNT 10
#define STREAM_BUF_SIZE 150 * 1024; 

static Snapshot snapshotArray[MAX_SNAPSHOT_COUNT];
static int snapshotIndex;

void SnapshotInit(void)
{
    snapshotIndex = 0;

    // TODO: IMPLEMENT
}

void SnapshotExit(void)
{
    int i;

    for (i = 0; i < MAX_SNAPSHOT_COUNT; i++)
    {
        Snapshot* snapshot = &snapshotArray[i];

        if (snapshot->buffer)
        {
            free(snapshot->buffer);
            snapshot->buffer = NULL;
        }
    }

    // TODO: IMPLEMENT
}

time_t SnapshotTake(void)
{
    struct timeval tv;
    struct tm* timeinfo;
    Snapshot* snapshot = &snapshotArray[snapshotIndex];
    JPEG_ENCODE_PARAMETER enPara;

    if (snapshot->buffer)
    {
        free(snapshot->buffer);
        snapshot->buffer = NULL;
    }

    // JPEG Encoding   
    enPara.quality = 85;
    enPara.strmBuf = malloc(STREAM_BUF_SIZE);
    enPara.strmBuf_size = STREAM_BUF_SIZE;
   
    if (enPara.strmBuf)
    {
        JPEGEncodeFrame(&enPara);
        
        snapshot->size = enPara.enSize;
        snapshot->buffer = enPara.strmBuf;
        
        ithPrintf("JPEG Encoding Done Size(%d)\n", enPara.enSize);
    }
        
    gettimeofday(&tv, NULL);
    timeinfo = localtime((const time_t*)&tv.tv_sec);
    strftime(snapshot->time, sizeof(snapshot->time),"%Y-%m-%d %H:%M:%S", timeinfo);

    EventWriteSnapshotLog(snapshot->time);

    if (++snapshotIndex >= MAX_SNAPSHOT_COUNT)
        snapshotIndex = 0;

    return tv.tv_sec;
}

uint8_t* SnapshotGet(char* time, int* size)
{
    int i;

    for (i = 0; i < MAX_SNAPSHOT_COUNT; i++)
    {
        Snapshot* snapshot = &snapshotArray[i];

        if (strcmp(time, snapshot->time) == 0)
        {
            *size = snapshot->size;
            return snapshot->buffer;
        }
    }
    return NULL;
}