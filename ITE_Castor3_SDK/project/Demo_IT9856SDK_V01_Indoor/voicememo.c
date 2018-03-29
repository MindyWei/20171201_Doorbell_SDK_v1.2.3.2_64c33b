#include <sys/stat.h>
#include <sys/statvfs.h>
#include <assert.h>
#include <dirent.h>
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "ite/itp.h"
#include "redblack/redblack.h"
#include "doorbell.h"
#include "thread_helper.h"

#define VOICEMEMO_PATH CFG_TEMP_DRIVE ":/voicememo"
#define VOICEMEMO_EXT  ".svm"
#define MAX_VOICEMEMO_COUNT    99

static bool voiceMemoInited, voiceMemoQuit, voiceMemoRecording;
static int voiceMemoIndex, voiceMemoCount, voiceMemoUnplayCount;
static char voiceMemoRecordFileName[128];
static VoiceMemoEntry voiceMemoArray[MAX_VOICEMEMO_COUNT];

static int VoiceMemoCompare(const void *pa, const void *pb, const void *config)
{
    struct dirent* dira = (struct dirent*) pa;
    struct dirent* dirb = (struct dirent*) pb;
    assert(pa);
    assert(pb);

    if ((dira->d_type == DT_DIR) && (dirb->d_type != DT_DIR))
        return -1;

    if ((dira->d_type != DT_DIR) && (dirb->d_type == DT_DIR))
        return 1;

    return strcmp(dira->d_name, dirb->d_name);
}

static void* VoiceMemoCreateTask(void* arg)
{
    DIR *pdir = NULL;
    struct dirent* val;
    void* rbTree;
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    mkdir(VOICEMEMO_PATH, S_IRWXU);

    rbTree = rbinit(VoiceMemoCompare, NULL);
    if (rbTree == NULL)
    {
	    puts("insufficient memory");
	    goto end;
    }

    if (voiceMemoQuit)
        goto end;

    // Open the directory
    pdir = opendir(VOICEMEMO_PATH);
    if (pdir)
    {
        struct dirent *pent;

        if (voiceMemoQuit)
            goto end;

        // Make this our current directory
        chdir(VOICEMEMO_PATH);

        // List the contents of the directory
        while ((pent = readdir(pdir)) != NULL)
        {
            struct dirent *dir, *ret;
            char* pch;

            if (voiceMemoQuit)
                goto end;

            if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
                continue;

            pch = strrchr(pent->d_name, '.');
            if (!pch)
                continue;

            if (strcmp(pch, VOICEMEMO_EXT) != 0)
                continue;

            dir = malloc(sizeof (struct dirent));
	        if (dir == NULL)
	        {
                printf("out of memory: %d\n", sizeof(struct dirent));
		        goto end;
	        }

            if (voiceMemoQuit)
                goto end;

            memcpy(dir, pent, sizeof (struct dirent));

		    ret = (struct dirent*) rbsearch((void *)dir, rbTree);
		    if(ret == NULL)
		    {
                printf("out of memory: %d\n", sizeof(struct dirent));
                free(dir);
		        goto end;
		    }

            if (voiceMemoQuit)
                goto end;
        }
    }
    else
    {
        printf("opendir(%s) failed\n", VOICEMEMO_PATH);
	    goto end;
    }

    voiceMemoCount = voiceMemoUnplayCount = 0;

    for (val = (struct dirent*) rblookup(RB_LUFIRST, NULL, rbTree); 
         val != NULL; 
         val = (struct dirent*)rblookup(RB_LUNEXT, val, rbTree))
    {
        char buf[128], *pch;
        struct stat st;
        VoiceMemoEntry* entry = &voiceMemoArray[voiceMemoCount];
        int year, month, day, index;

        if (voiceMemoQuit)
            goto end;

        if (sscanf(val->d_name, "%d-%d-%d-%d." VOICEMEMO_EXT, &year, &month, &day, &index) == 4)
        {
            if (year == timeinfo->tm_year + 1900 && month == timeinfo->tm_mon + 1 && day == timeinfo->tm_mday)
            {
                if (index + 1> voiceMemoIndex)
                    voiceMemoIndex = index + 1;
            }
        }

        strcpy(buf, val->d_name);
        pch = strrchr(buf, '.');
        if (pch)
            *pch = '\0';

        strncpy(entry->name, buf, VOICEMEMO_MAX_NAME_SIZE);
        strcpy(buf, VOICEMEMO_PATH);
        strcat(buf, "/");
        strcat(buf, val->d_name);
        stat(buf, &st);

        entry->time = st.st_mtime;
        entry->sec = AudioRecordGetTimeLength(buf);

        if (st.st_mode & S_IEXEC)
            entry->played = true;
        else
            voiceMemoUnplayCount++;

        if (voiceMemoCount < MAX_VOICEMEMO_COUNT)
            voiceMemoCount++;
        else
            break;
    }
    voiceMemoInited = true;

end:
    // Close the directory
    if (pdir)
        closedir(pdir);

    if (rbTree)
    {
        RBLIST *rblist;

	    if ((rblist = rbopenlist(rbTree)) != NULL)
	    {
            void* val;
	        while ((val = (void*) rbreadlist(rblist)))
		        free(val);

	        rbcloselist(rblist);
	    }
        rbdestroy(rbTree);
        rbTree = NULL;
    }
    return NULL;
}

void VoiceMemoInit(void)
{
    voiceMemoInited = voiceMemoQuit = voiceMemoRecording = false;
    voiceMemoIndex = voiceMemoCount = voiceMemoUnplayCount = 0;

    CreateWorkerThread(VoiceMemoCreateTask, NULL);
}

void VoiceMemoExit(void)
{
    voiceMemoQuit = true;
    voiceMemoInited = false;
}

int VoiceMemoGetCount(void)
{
    if (voiceMemoInited && !voiceMemoQuit)
        return voiceMemoCount;

    return -1;
}

int VoiceMemoGetUnplayCount(void)
{
    if (voiceMemoInited && !voiceMemoQuit)
        return voiceMemoUnplayCount;

    return -1;
}

VoiceMemoEntry* VoiceMemoGetEntry(int index)
{
    int i;

    if (!voiceMemoInited || voiceMemoQuit)
        return NULL;

    if (voiceMemoCount == 0 || index >= voiceMemoCount)
        return NULL;

    assert(voiceMemoCount <= MAX_VOICEMEMO_COUNT);
    assert(index < voiceMemoCount);

    i = voiceMemoCount - 1 - index;
    return &voiceMemoArray[i];
}

static void VoiceMemoDeleteFile(int index)
{
    char buf[128];
    VoiceMemoEntry* entry = &voiceMemoArray[index];

    if (!entry->played && voiceMemoUnplayCount > 0)
        voiceMemoUnplayCount--;

    strcpy(buf, VOICEMEMO_PATH);
    strcat(buf, "/");
    strcat(buf, entry->name);
    strcat(buf, VOICEMEMO_EXT);
    remove(buf);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
}

void VoiceMemoDeleteEntry(int index)
{
    int i, endIndex;

    if (!voiceMemoInited || voiceMemoQuit)
        return;

    if (voiceMemoCount == 0 || index >= voiceMemoCount)
        return;

    assert(voiceMemoCount <= MAX_VOICEMEMO_COUNT);
    assert(index < voiceMemoCount);

    i = voiceMemoCount - 1 - index;
    endIndex = voiceMemoCount - 1;

    assert(i <= endIndex);

    VoiceMemoDeleteFile(i);

    memmove(&voiceMemoArray[i], &voiceMemoArray[i + 1], sizeof(VoiceMemoEntry) * (endIndex - i));

    voiceMemoCount--;
    assert(voiceMemoCount >= 0);
}

int VoiceMemoStartPlay(int index)
{
    char buf[PATH_MAX];
    VoiceMemoEntry* entry;

    if (!voiceMemoInited || voiceMemoQuit)
        return -1;

    entry = VoiceMemoGetEntry(index);

    if (!entry->played)
    {
        entry->played = true;
        if (voiceMemoUnplayCount > 0)
            voiceMemoUnplayCount--;
    }
    strcpy(buf, VOICEMEMO_PATH);
    strcat(buf, "/");
    strcat(buf, entry->name);
    strcat(buf, VOICEMEMO_EXT);
    chmod(buf, S_IRWXU);
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    return AudioRecordStartPlay(buf);
}

void VoiceMemoStopPlay(void)
{
    AudioRecordStopPlay();
}

int VoiceMemoStartRecord(void)
{
    time_t rawtime;
    struct tm* timeinfo;
    char buf[PATH_MAX];
    int ret;

    if (!voiceMemoInited || voiceMemoQuit || !VoiceMemoCheckDiskSpace())
        return -1;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf, sizeof(voiceMemoRecordFileName),"%Y-%m-%d-%%d", timeinfo);
    sprintf(voiceMemoRecordFileName, buf, voiceMemoIndex);

    strcpy(buf, VOICEMEMO_PATH);
    strcat(buf, "/");
    strcat(buf, voiceMemoRecordFileName);
    strcat(buf, VOICEMEMO_EXT);

    remove(buf);

    ret = AudioRecordStartRecord(buf);
    if (ret == 0)
        voiceMemoRecording = true;

    return ret;
}

int VoiceMemoStopRecord(void)
{
    int ret;

    if (!voiceMemoInited || voiceMemoQuit || !voiceMemoRecording)
        return -1;

    ret = AudioRecordStopRecord();
    if (ret == 0)
    {
        VoiceMemoEntry* entry;
        struct stat st;
        char buf[PATH_MAX], buf2[PATH_MAX];
        struct tm* timeinfo;

        if (voiceMemoCount >= MAX_VOICEMEMO_COUNT)
            VoiceMemoDeleteEntry(voiceMemoCount - 1);

        entry = &voiceMemoArray[voiceMemoCount];

        strcpy(buf, VOICEMEMO_PATH);
        strcat(buf, "/");
        strcat(buf, voiceMemoRecordFileName);
        strcat(buf, VOICEMEMO_EXT);
        stat(buf, &st);

        entry->time = st.st_mtime;
        entry->sec = AudioRecordGetTimeLength(buf);
        entry->played = false;

        timeinfo = localtime(&st.st_mtime);
        strftime(buf2, sizeof(voiceMemoRecordFileName),"%Y-%m-%d-%%d", timeinfo);
        sprintf(voiceMemoRecordFileName, buf2, voiceMemoIndex++);

        strncpy(entry->name, voiceMemoRecordFileName, VOICEMEMO_MAX_NAME_SIZE);

        strcpy(buf2, VOICEMEMO_PATH);
        strcat(buf2, "/");
        strcat(buf2, voiceMemoRecordFileName);
        strcat(buf2, VOICEMEMO_EXT);

        if (strcmp(buf, buf2))
            rename(buf, buf2);

        voiceMemoCount++;
        assert(voiceMemoCount <= MAX_VOICEMEMO_COUNT);

        voiceMemoUnplayCount++;
    }
    voiceMemoRecording = false;
    return ret;
}

bool VoiceMemoCheckDiskSpace(void)
{
    struct statvfs info;

    // check capacity
    if (statvfs(VOICEMEMO_PATH, &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;

        if (avail > 32 * 1024)
            return true;

        printf("Out of disk space: %llu\n", avail);
    }
    return false;
}
