#include <assert.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "linphone/linphone_castor3.h"
#include "doorbell.h"

#define MAX_CALLLOG_COUNT 999

static int callLogIndex, callLogCount;
static CallLogEntry callLogArray[MAX_CALLLOG_COUNT];
static int callLogCurrMissedCount;

static pthread_mutex_t clMutex;

static void CallLogAddCallback(int type, char* ip, char* time, int duration, int refkey, int init)
{
    CallLogEntry* entry;

    pthread_mutex_lock(&clMutex);

    if (callLogCount >= MAX_CALLLOG_COUNT)
        CallLogDeleteEntry(callLogCount - 1);

    entry = &callLogArray[callLogCount];

    entry->type = type;
    strcpy(entry->ip, ip);
    strcpy(entry->time, time);
    entry->duration = duration;
    entry->refkey = refkey;

    callLogCount++;
    if (type == CALLLOG_MISSED && !init)
        callLogCurrMissedCount++;

    pthread_mutex_unlock(&clMutex);
}

void CallLogInit(void)
{
    callLogIndex = callLogCount = 0;
    callLogCurrMissedCount = theConfig.missed_call_count;

    pthread_mutex_init(&clMutex, NULL);
    linphonec_call_log_add_callback = CallLogAddCallback;
}

void CallLogExit(void)
{
    pthread_mutex_destroy(&clMutex);
}

int CallLogGetCount(void)
{
    return callLogCount;
}

CallLogEntry* CallLogGetEntry(int index)
{
    int i;

    if (callLogCount == 0 || index >= callLogCount)
        return NULL;

    assert(callLogCount <= MAX_CALLLOG_COUNT);
    assert(index < callLogCount);

    i = callLogCount - 1 - index;
    return &callLogArray[i];
}

void CallLogDeleteEntry(int index)
{
    int i, endIndex;
    CallLogEntry* entry;

    if (callLogCount == 0 || index >= callLogCount)
        return;

    pthread_mutex_lock(&clMutex);

    assert(callLogCount <= MAX_CALLLOG_COUNT);
    assert(index < callLogCount);

    i = callLogCount - 1 - index;
    endIndex = callLogCount - 1;

    assert(i <= endIndex);

    entry = &callLogArray[i];
    LinphoneDeleteCallLog(entry->refkey);

    memmove(&callLogArray[i], &callLogArray[i + 1], sizeof(CallLogEntry) * (endIndex - i));

    callLogCount--;
    assert(callLogCount >= 0);

    if (entry->type == CALLLOG_MISSED && callLogCurrMissedCount > 0)
    {
        callLogCurrMissedCount--;
        assert(callLogCurrMissedCount >= 0);

    }
    pthread_mutex_unlock(&clMutex);
}

void CallLogClearEntries(void)
{
    pthread_mutex_lock(&clMutex);

    LinphoneClearMissedLogs();
    LinphoneClearReceivedLogs();
    LinphoneClearSentLogs();
    callLogIndex = callLogCount = callLogCurrMissedCount = 0;

    pthread_mutex_unlock(&clMutex);
}

int CallLogGetCurrMissedCount(void)
{
    return callLogCurrMissedCount;
}

void CallLogClearCurrMissedCount(void)
{
    callLogCurrMissedCount = 0;
}
