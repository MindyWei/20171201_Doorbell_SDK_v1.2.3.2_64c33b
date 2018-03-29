#include <sys/stat.h>
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

#define VIDEOMEMO_VIDEO_EXT  ".mkv"
#define VIDEOMEMO_AUDIO_EXT  ".mp3"
#define MAX_VIDEOMEMO_COUNT    99

static bool videoMemoInited, videoMemoQuit, videoMemoRecording, videoMemoPlayFinished;
static int videoMemoIndex, videoMemoCount, videoMemoUnplayCount;
static char videoMemoRecordFileName[PATH_MAX];
static VideoMemoEntry videoMemoArray[MAX_VIDEOMEMO_COUNT];

static int VideoMemoCompare(const void *pa, const void *pb, const void *config)
{
    struct dirent* dira = (struct dirent*) pa;
    struct dirent* dirb = (struct dirent*) pb;
    struct stat sta, stb;
    char buf[PATH_MAX];
    char* videoMemoPath = VideoMemoGetPath();
    assert(pa);
    assert(pb);

    if (!videoMemoPath)
        return -1;

    if ((dira->d_type == DT_DIR) && (dirb->d_type != DT_DIR))
        return -1;

    if ((dira->d_type != DT_DIR) && (dirb->d_type == DT_DIR))
        return 1;

    // comare by time
    strcpy(buf, videoMemoPath);
    strcat(buf, "/");
    strcat(buf, dira->d_name);
    stat(buf, &sta);

    strcpy(buf, videoMemoPath);
    strcat(buf, "/");
    strcat(buf, dirb->d_name);
    stat(buf, &stb);

    if (sta.st_mtime < stb.st_mtime)
        return -1;
    else if (sta.st_mtime > stb.st_mtime)
        return 1;
    else
        return 0;
}

static void* VideoMemoCreateTask(void* arg)
{
    DIR *pdir = NULL;
    struct dirent* val;
    void* rbTree;
    char* videoMemoPath;

    rbTree = rbinit(VideoMemoCompare, NULL);
    if (rbTree == NULL)
    {
	    puts("insufficient memory");
	    goto end;
    }

    if (videoMemoQuit)
        goto end;

    videoMemoPath = VideoMemoGetPath();
    if (videoMemoPath)
    {
        mkdir(videoMemoPath, S_IRWXU);

        // Open the directory
        pdir = opendir(videoMemoPath);
        if (pdir)
        {
            struct dirent *pent;

            if (videoMemoQuit)
                goto end;

            // Make this our current directory
            chdir(videoMemoPath);

            // List the contents of the directory
            while ((pent = readdir(pdir)) != NULL)
            {
                struct dirent *dir, *ret;
                char* pch;

                if (videoMemoQuit)
                    goto end;

                if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
                    continue;

                pch = strrchr(pent->d_name, '.');
                if (!pch)
                    continue;

                if (strcmp(pch, VIDEOMEMO_VIDEO_EXT) && strcmp(pch, VIDEOMEMO_AUDIO_EXT))
                    continue;

                dir = malloc(sizeof (struct dirent));
	            if (dir == NULL)
	            {
                    printf("out of memory: %d\n", sizeof(struct dirent));
		            goto end;
	            }

                if (videoMemoQuit)
                    goto end;

                memcpy(dir, pent, sizeof (struct dirent));

		        ret = (struct dirent*) rbsearch((void *)dir, rbTree);
		        if(ret == NULL)
		        {
                    printf("out of memory: %d\n", sizeof(struct dirent));
                    free(dir);
		            goto end;
		        }

                if (videoMemoQuit)
                    goto end;
            }
        }
        else
        {
            printf("opendir(%s) failed\n", videoMemoPath);
	        goto end;
        }
    }
    videoMemoCount = videoMemoUnplayCount = 0;

    for (val = (struct dirent*) rblookup(RB_LUFIRST, NULL, rbTree); 
         val != NULL; 
         val = (struct dirent*)rblookup(RB_LUNEXT, val, rbTree))
    {
        char buf[PATH_MAX], *pch;
        struct stat st;
        DeviceInfo info;
        VideoMemoEntry* entry = &videoMemoArray[videoMemoCount];
        char* ptr;

        if (videoMemoQuit)
            goto end;

        entry->type = MEDIA_TEXT;
        strncpy(entry->filename, val->d_name, VIDEOMEMO_MAX_FILENAME_SIZE);

        // decode file name
        strcpy(buf, val->d_name);
        pch = strrchr(buf, '.');
        if (pch)
        {
            if (stricmp(pch, VIDEOMEMO_VIDEO_EXT) == 0)
                entry->type = MEDIA_VIDEO;
            else if (stricmp(pch, VIDEOMEMO_AUDIO_EXT) == 0)
                entry->type = MEDIA_AUDIO;

            *pch = '\0';
        }
        pch = strrchr(buf, '-');
        if (pch)
        {
            int index = atoi(pch + 1);

            if (index + 1 > videoMemoIndex)
                videoMemoIndex = index + 1;

            *pch = '\0';
        }

        AddressBookGetDeviceInfo(&info, buf);

        ptr = StringGetRoomAddress(info.area, info.building, info.unit, info.floor, info.room, NULL);
        if (!ptr)
            continue;

        strncpy(entry->name, ptr, CAPUTRE_MAX_NAME_SIZE);
        free(ptr);

        strcpy(buf, videoMemoPath);
        strcat(buf, "/");
        strcat(buf, val->d_name);
        stat(buf, &st);

        entry->time = st.st_mtime;
        entry->sec = VideoRecordGetTimeLength(buf);

        if (st.st_mode & S_IEXEC)
            entry->played = true;
        else
            videoMemoUnplayCount++;

        if (videoMemoCount < MAX_VIDEOMEMO_COUNT)
            videoMemoCount++;
        else
            break;
    }
    videoMemoInited = true;

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

void VideoMemoInit(void)
{
    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        if (!StorageGetDrivePath(STORAGE_SD))
            return;
    }

    videoMemoInited = videoMemoQuit = videoMemoRecording = videoMemoPlayFinished = false;
    videoMemoIndex = videoMemoCount = videoMemoUnplayCount = 0;

    CreateWorkerThread(VideoMemoCreateTask, NULL);
}

void VideoMemoExit(void)
{
    videoMemoQuit = true;
    videoMemoInited = false;
}

int VideoMemoGetCount(void)
{
    if (videoMemoInited && !videoMemoQuit)
        return videoMemoCount;

    return 0;
}

int VideoMemoGetUnplayCount(void)
{
    if (videoMemoInited && !videoMemoQuit)
        return videoMemoUnplayCount;

    return 0;
}

VideoMemoEntry* VideoMemoGetEntry(int index)
{
    int i;
    VideoMemoEntry* entry;
    char buf[PATH_MAX];

    if (!videoMemoInited || videoMemoQuit)
        return NULL;

    if (videoMemoCount == 0 || index >= videoMemoCount)
        return NULL;

    assert(videoMemoCount <= MAX_VIDEOMEMO_COUNT);
    assert(index < videoMemoCount);

    i = videoMemoCount - 1 - index;
    entry = &videoMemoArray[i];

    if (entry->sec == 0)
    {
        char* videoMemoPath = VideoMemoGetPath();
        if (videoMemoPath)
        {
            strcpy(buf, videoMemoPath);
            strcat(buf, "/");
            strcat(buf, entry->filename);
            entry->sec = VideoRecordGetTimeLength(buf);
        }
    }
    return entry;
}

static void VideoMemoDeleteFile(int index)
{
    char buf[PATH_MAX];
    VideoMemoEntry* entry = &videoMemoArray[index];
    char* videoMemoPath = VideoMemoGetPath();

    if (!videoMemoPath)
        return;

    if (!entry->played && videoMemoUnplayCount > 0)
        videoMemoUnplayCount--;

    strcpy(buf, videoMemoPath);
    strcat(buf, "/");
    strcat(buf, entry->filename);

    remove(buf);
}

void VideoMemoDeleteEntry(int index)
{
    int i, endIndex;

    if (!videoMemoInited || videoMemoQuit)
        return;

    if (videoMemoCount == 0 || index >= videoMemoCount)
        return;

    assert(videoMemoCount <= MAX_VIDEOMEMO_COUNT);
    assert(index < videoMemoCount);

    i = videoMemoCount - 1 - index;
    endIndex = videoMemoCount - 1;

    assert(i <= endIndex);

    VideoMemoDeleteFile(i);

    memmove(&videoMemoArray[i], &videoMemoArray[i + 1], sizeof(VideoMemoEntry) * (endIndex - i));

    videoMemoCount--;
    assert(videoMemoCount >= 0);
}

bool VideoMemoIsPlayFinished(void)
{
    return videoMemoPlayFinished;
}

static void VideoMemoPlayFinished(void)
{
    videoMemoPlayFinished = true;
}

int VideoMemoStartPlay(int index)
{
    char buf[PATH_MAX];
    VideoMemoEntry* entry;
    char* videoMemoPath = VideoMemoGetPath();

    if (!videoMemoInited || videoMemoQuit || !videoMemoPath)
        return -1;

    entry = VideoMemoGetEntry(index);
    if (!entry->played)
    {
        entry->played = true;
        if (videoMemoUnplayCount > 0)
            videoMemoUnplayCount--;
    }
    strcpy(buf, videoMemoPath);
    strcat(buf, "/");
    strcat(buf, entry->filename);
    chmod(buf, S_IRWXU);
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
    videoMemoPlayFinished = false;
    return VideoRecordStartPlay(buf, VideoMemoPlayFinished);
}

void VideoMemoStopPlay(void)
{
    VideoRecordStopPlay();
    videoMemoPlayFinished = false;
}

int VideoMemoStartRecord(MediaType type, char* addr)
{
    char filepath[PATH_MAX];
    int ret = 0;
    char* videoMemoPath = VideoMemoGetPath();

    if (!videoMemoInited || videoMemoQuit || !videoMemoPath)
        return -1;

    sprintf(videoMemoRecordFileName, "%s-%d", addr, videoMemoIndex++);

    switch (type)
    {
    case MEDIA_VIDEO:
        strcat(videoMemoRecordFileName, VIDEOMEMO_VIDEO_EXT);
        break;

    case MEDIA_AUDIO:
        strcat(videoMemoRecordFileName, VIDEOMEMO_AUDIO_EXT);
        break;

    default:
        return -1;
        break;
    }
    strcpy(filepath, videoMemoPath);
    strcat(filepath, "/");
    strcat(filepath, videoMemoRecordFileName);

    ret = VideoRecordStartRecord(filepath);
    if (ret == 0)
        videoMemoRecording = true;

    return ret;
}

int VideoMemoStopRecord(void)
{
    VideoMemoEntry* entry;
    int ret = 0;
    char* ptr;
    char* videoMemoPath = VideoMemoGetPath();

    if (!videoMemoInited || videoMemoQuit || !videoMemoRecording)
        return -1;

    ret = VideoRecordStopRecord();
    if (ret == 0 && videoMemoPath)
    {
        struct stat st;
        char* pch;
        DeviceInfo info;
        char buf[PATH_MAX];

        while(videoMemoCount >= MAX_VIDEOMEMO_COUNT || !VideoMemoCheckDiskSpace(512*1024*theConfig.calling_time))
        {
            if((videoMemoCount - 1) >= 0)
            {
                VideoMemoDeleteEntry(videoMemoCount - 1);
            }else
            {
                 printf("out of memory!!\n");
                 return -1;
            }
        }

        entry = &videoMemoArray[videoMemoCount];

        strncpy(entry->filename, videoMemoRecordFileName, VIDEOMEMO_MAX_FILENAME_SIZE);
        entry->type = MEDIA_TEXT;
        pch = strrchr(videoMemoRecordFileName, '.');
        if (pch)
        {
            if (stricmp(pch, VIDEOMEMO_VIDEO_EXT) == 0)
                entry->type = MEDIA_VIDEO;
            else if (stricmp(pch, VIDEOMEMO_AUDIO_EXT) == 0)
                entry->type = MEDIA_AUDIO;

            *pch = '\0';
        }

        pch = strrchr(videoMemoRecordFileName, '-');
        *pch = '\0';
        AddressBookGetDeviceInfo(&info, videoMemoRecordFileName);

        ptr = StringGetRoomAddress(info.area, info.building, info.unit, info.floor, info.room, NULL);
        if (!ptr)
            return -1;

        strncpy(entry->name, ptr, CAPUTRE_MAX_NAME_SIZE);
        free(ptr);

        strcpy(buf, videoMemoPath);
        strcat(buf, "/");
        strcat(buf, entry->filename);
        stat(buf, &st);
        entry->time = st.st_mtime;
        entry->sec = 0;
        entry->played = false;

        videoMemoCount++;
        assert(videoMemoCount <= MAX_VIDEOMEMO_COUNT);

        videoMemoUnplayCount++;
    }
    videoMemoRecording = false;
    return ret;
}

char* VideoMemoGetPath(void)
{
    static char videoMemoPath[PATH_MAX];

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        char* path = UpgradeGetSDPath();
        if (path)
        {
            strcpy(videoMemoPath, path);
            strcat(videoMemoPath, "videomemo");

            return videoMemoPath;
        }
    }
    else
    {
        strcpy(videoMemoPath, CFG_TEMP_DRIVE ":/videomemo");
        return videoMemoPath;
    }
    return NULL;
}

bool VideoMemoCheckDiskSpace(int max_file_size)
{
    struct statvfs info;
    char* videoMemoPath = VideoMemoGetPath();

    // check capacity
    if (statvfs(videoMemoPath, &info) == 0)
    {
        uint64_t avail = info.f_bfree * info.f_bsize;

        if (avail > max_file_size)
            return true;

        printf("Out of disk space: %llu\n", avail);
    }
    return false;
}

