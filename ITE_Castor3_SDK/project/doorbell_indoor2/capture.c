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
#include "mediastreamer2/msfilewriter.h" 

#define SNAPSHOT_PATH CFG_TEMP_DRIVE ":/capture"
#define CAPTURE_IMAGE_EXT  ".jpg"
#define CAPTURE_VIDEO_EXT  ".mkv"
#define CAPTURE_AUDIO_EXT  ".mp3"
#define MAX_CAPTURE_COUNT    99

pthread_mutex_t    streaming_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t    capture_mutex   = PTHREAD_MUTEX_INITIALIZER;
bool IPCamStreaming = false, IPCamSnapshotFlag = false;
bool captureRecording = false;
bool captureRunSDCardPath = false;
bool captureDeletePic = false, captureSnapshotFlag = false;
static bool captureInited, captureQuit, capturePlayFinished ,captureSnapShotSuccess , ipCamSnapShotSuccess;
static int  captureCount, captureUnplayCount;
static char captureRecordFileName[PATH_MAX];
static char CaptureSnapshotEntryname[PATH_MAX], IPCamSnapshotEntryname[PATH_MAX];
static CaptureEntry captureArray[MAX_CAPTURE_COUNT];
static char gSaveaddr[20]={0};
static time_t gSaveT;


static int CaptureCompare(const void *pa, const void *pb, const void *config)
{
    struct dirent* dira = (struct dirent*) pa;
    struct dirent* dirb = (struct dirent*) pb;
    char *ta, *tb;
    assert(pa);
    assert(pb);

    if ((dira->d_type == DT_DIR) && (dirb->d_type != DT_DIR))
        return -1;

    if ((dira->d_type != DT_DIR) && (dirb->d_type == DT_DIR))
        return 1;

    // comare by time
    ta = strchr(dira->d_name, '_');
    tb = strchr(dirb->d_name, '_');

    return strcmp(ta, tb);
}
static void* CaptureDeleteTask(void* arg)
{
    int i;
    CaptureEntry* entry;
    while(!captureQuit)
    {
        if(captureRecording)
        {
            if(captureCount >= MAX_CAPTURE_COUNT || !CaptureCheckDiskSpace(512*1024*theConfig.calling_time, MEDIA_VIDEO))
            {
                if((captureCount - 1) >= 0)
                {
                    pthread_mutex_lock(&capture_mutex);
		            for(i=captureCount-1; i>=0; i--)
                    {
                        entry = &captureArray[captureCount - 1 - i];
                        if(entry->type == MEDIA_VIDEO) 
                        {
                            break;
                        }
                    }
		            CaptureDeleteEntry(i);
                    pthread_mutex_unlock(&capture_mutex);
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
                }
                else
                {
                    printf("Capture out of memory!!\n");
                    return NULL;
                }
            }
        }
        sleep(1);
    }
    return NULL;
}

static void* CaptureSnapshotDeleteTask(void* arg)
{
    int i;
    CaptureEntry* entry;
	while(!captureQuit)
	{
		if(captureDeletePic)
		{
			if(captureCount >= MAX_CAPTURE_COUNT || !CaptureCheckDiskSpace(512*1024 + 256*1024, MEDIA_IMAGE))
		    {
		        if((captureCount - 1) >= 0)
		        {
		            pthread_mutex_lock(&capture_mutex);
		            for(i=captureCount-1; i>=0; i--)
                    {
                        entry = &captureArray[captureCount - 1 - i];
                        if(entry->type == MEDIA_IMAGE) 
                        {
                            break;
                        }
                    }
		            CaptureSnapshotDeleteEntry(i);
                    pthread_mutex_unlock(&capture_mutex);
                    
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
                    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
					captureDeletePic = false;

					//for the case of out of disk space and retry again.
					if(captureSnapshotFlag)
					{
						CaptureSnapshot(gSaveaddr,gSaveT); 
						captureSnapshotFlag = false;
					}else if(IPCamSnapshotFlag)
					{
						IPCamSnapshot(gSaveaddr,gSaveT); 
						IPCamSnapshotFlag = false;
					}
		        }
                else
		        {
		             printf("Snapshot out of memory!!\n");
					 return NULL;
		        }
		    }
		}
		sleep(1);
	}
    return NULL;
}

static void* CaptureCreateTask(void* arg)
{
    DIR *pdir = NULL;
    DIR *pdir1 = NULL;
    struct dirent* val;
    void* rbTree;
    char* capturePath;

    rbTree = rbinit(CaptureCompare, NULL);
    if (rbTree == NULL)
    {
	    puts("insufficient memory");
	    goto end;
    }

    if (captureQuit)
        goto end;

    capturePath = CaptureGetPath();

    if (capturePath)
    {
        mkdir(capturePath, S_IRWXU);

        // Open the directory
        pdir = opendir(capturePath);
        if (pdir)
        {
            struct dirent *pent;

            if (captureQuit)
                goto end;

            // Make this our current directory
            chdir(capturePath);

            // List the contents of the directory
            while ((pent = readdir(pdir)) != NULL)
            {
                struct dirent *dir, *ret;
                char* pch;

                if (captureQuit)
                    goto end;

                if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
                    continue;

                pch = strrchr(pent->d_name, '.');
                if (!pch)
                    continue;

                if (strcmp(pch, CAPTURE_IMAGE_EXT) && strcmp(pch, CAPTURE_VIDEO_EXT) && strcmp(pch, CAPTURE_AUDIO_EXT))
                    continue;

                dir = malloc(sizeof (struct dirent));
	            if (dir == NULL)
	            {
                    printf("out of memory: %d\n", sizeof(struct dirent));
		            goto end;
	            }

                if (captureQuit)
                    goto end;

                memcpy(dir, pent, sizeof (struct dirent));

		        ret = (struct dirent*) rbsearch((void *)dir, rbTree);
		        if(ret == NULL)
		        {
                    printf("out of memory: %d\n", sizeof(struct dirent));
                    free(dir);
		            goto end;
		        }

                if (captureQuit)
                    goto end;
            }
        }
        else
        {
            printf("opendir(%s) failed\n", capturePath);
	        goto end;
        }
    }

  if (captureRunSDCardPath)
    {
        mkdir(SNAPSHOT_PATH, S_IRWXU);

        // Open the directory
        pdir1 = opendir(SNAPSHOT_PATH);
        if (pdir1)
        {
            struct dirent *pent;

            if (captureQuit)
                goto end;

            // Make this our current directory
            chdir(SNAPSHOT_PATH);

            // List the contents of the directory
            while ((pent = readdir(pdir1)) != NULL)
            {
                struct dirent *dir, *ret;
                char* pch;

                if (captureQuit)
                    goto end;

                if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
                    continue;

                pch = strrchr(pent->d_name, '.');
                if (!pch)
                    continue;

                if (strcmp(pch, CAPTURE_IMAGE_EXT) && strcmp(pch, CAPTURE_VIDEO_EXT) && strcmp(pch, CAPTURE_AUDIO_EXT))
                    continue;

                dir = malloc(sizeof (struct dirent));
	            if (dir == NULL)
	            {
                    printf("out of memory: %d\n", sizeof(struct dirent));
		            goto end;
	            }

                if (captureQuit)
                    goto end;

                memcpy(dir, pent, sizeof (struct dirent));

		        ret = (struct dirent*) rbsearch((void *)dir, rbTree);
		        if(ret == NULL)
		        {
                    printf("out of memory: %d\n", sizeof(struct dirent));
                    free(dir);
		            goto end;
		        }

                if (captureQuit)
                    goto end;
            }
        }
        else
        {
            printf("opendir(%s) failed\n", SNAPSHOT_PATH);
	        goto end;
        }
    }

    captureCount = captureUnplayCount = 0;

    for (val = (struct dirent*) rblookup(RB_LUFIRST, NULL, rbTree); 
         val != NULL; 
         val = (struct dirent*)rblookup(RB_LUNEXT, val, rbTree))
    {
        char buf[PATH_MAX], name[PATH_MAX], *pch;
        struct stat st;
        struct tm tm;
        DeviceInfo info;
        int len;
        CaptureEntry* entry = &captureArray[captureCount];

        if (captureQuit)
            goto end;

        entry->type = MEDIA_TEXT;
        strncpy(entry->filename, val->d_name, CAPUTRE_MAX_FILENAME_SIZE);

        // decode file name
        strcpy(buf, val->d_name);
        pch = strrchr(buf, '.');
        if (pch)
        {
            if (stricmp(pch, CAPTURE_IMAGE_EXT) == 0)
                entry->type = MEDIA_IMAGE;
            else if (stricmp(pch, CAPTURE_VIDEO_EXT) == 0)
                entry->type = MEDIA_VIDEO;
            else if (stricmp(pch, CAPTURE_AUDIO_EXT) == 0)
                entry->type = MEDIA_AUDIO;

            *pch = '\0';
        }
        pch = strchr(buf, '_');
        *pch = '\0';

        AddressBookGetDeviceInfo(&info, buf);
        strcpy(name, info.building);
        strcat(name, StringGetDeviceType(info.type));
        strcat(name, " ");
        len = strlen(name);

        pch++;
        strptime(pch, "%Y-%m-%d_%H-%M-%S", &tm);
        strftime(&name[len], sizeof(name) - len,"%Y-%m-%d/%H:%M", &tm);

        strncpy(entry->name, name, CAPUTRE_MAX_NAME_SIZE);
        entry->time = mktime(&tm);

        if (entry->sec == 0 && (entry->type == MEDIA_VIDEO || entry->type == MEDIA_AUDIO))
        {
            if (capturePath)
            {
                strcpy(buf, capturePath);
                strcat(buf, "/");
                strcat(buf, val->d_name);
                stat(buf, &st);
                entry->sec = VideoRecordGetTimeLength(buf);
            }    
        }else if(entry->type == MEDIA_IMAGE)
        {
            strcpy(buf, SNAPSHOT_PATH);
            strcat(buf, "/");
            strcat(buf, val->d_name);
            stat(buf, &st);
        }

        if (st.st_mode & S_IEXEC)
            entry->played = true;
        else
            captureUnplayCount++;

        if (captureCount < MAX_CAPTURE_COUNT)
        {
            captureCount++;
        }
        else
        {
            break;
        }
    }
    captureInited = true;

end:
    // Close the directory
    if (pdir)
        closedir(pdir);

    if(pdir1) 
        closedir(pdir1);

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

void CaptureInit(void)
{
    pthread_t task;
	pthread_t task1;
    pthread_t task2;
    pthread_attr_t attr;

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        if (!StorageGetDrivePath(STORAGE_SD))
            return;
    }

    captureInited = captureQuit = captureRecording = capturePlayFinished = captureSnapShotSuccess = captureRunSDCardPath = ipCamSnapShotSuccess  = false;
    captureCount = captureUnplayCount = 0;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, CaptureCreateTask, NULL);
    pthread_create(&task1, &attr, CaptureSnapshotDeleteTask, NULL);
    pthread_create(&task2, &attr, CaptureDeleteTask, NULL);
}

void CaptureExit(void)
{
    captureQuit = true;
    captureInited = false;
}

int CaptureGetCount(void)
{
    if (captureInited && !captureQuit)
        return captureCount;

    return 0;
}

int CaptureGetUnplayCount(void)
{
    if (captureInited && !captureQuit)
        return captureUnplayCount;

    return 0;
}

CaptureEntry* CaptureGetEntry(int index)
{
    int i;
    CaptureEntry* entry;
    char buf[PATH_MAX];

    if (!captureInited || captureQuit)
        return NULL;

    if (captureCount == 0 || index >= captureCount)
        return NULL;

    assert(captureCount <= MAX_CAPTURE_COUNT);
    assert(index < captureCount);

    i = captureCount - 1 - index;

    entry = &captureArray[i];
    if (entry->sec == 0 && (entry->type == MEDIA_VIDEO || entry->type == MEDIA_AUDIO))
    {
        char* capturePath = CaptureGetPath();
        if (capturePath)
        {
            strcpy(buf, capturePath);
            strcat(buf, "/");
            strcat(buf, entry->filename);
            entry->sec = VideoRecordGetTimeLength(buf);
        }
    }
    return entry;
}

static void CaptureDeleteFile(int index)
{
    char buf[PATH_MAX];
    CaptureEntry* entry = &captureArray[index];
    char* capturePath = CaptureGetPath();

    if (!capturePath)
        return;

    if (!entry->played && captureUnplayCount > 0)
        captureUnplayCount--;

    strcpy(buf, capturePath);
    strcat(buf, "/");
    strcat(buf, entry->filename);
    remove(buf);
}

static void CaptureSnapshotDeleteFile(int index)
{
    char buf[PATH_MAX];
    CaptureEntry* entry = &captureArray[index];

    if (!entry->played && captureUnplayCount > 0)
        captureUnplayCount--;

    strcpy(buf, SNAPSHOT_PATH);
    strcat(buf, "/");
    strcat(buf, entry->filename);
    remove(buf);
}

void CaptureDeleteEntry(int index)
{
    int i, endIndex;

    if (!captureInited || captureQuit)
        return;

    if (captureCount == 0 || index >= captureCount)
        return;
    
    assert(captureCount <= MAX_CAPTURE_COUNT);
    assert(index < captureCount);

    i = captureCount - 1 - index;
    endIndex = captureCount - 1;

    assert(i <= endIndex);

    CaptureDeleteFile(i);

    memmove(&captureArray[i], &captureArray[i + 1], sizeof(CaptureEntry) * (endIndex - i));

    captureCount--;
    assert(captureCount >= 0);
   
}


void CaptureSnapshotDeleteEntry(int index)
{
    int i, endIndex;

    if (!captureInited || captureQuit)
        return;

    if (captureCount == 0 || index >= captureCount)
        return;

    assert(captureCount <= MAX_CAPTURE_COUNT);
    assert(index < captureCount);

    i = captureCount - 1 - index;
    endIndex = captureCount - 1;

    assert(i <= endIndex);
    CaptureSnapshotDeleteFile(i);

    memmove(&captureArray[i], &captureArray[i + 1], sizeof(CaptureEntry) * (endIndex - i));


    captureCount--;
    assert(captureCount >= 0);
    
}


bool CaptureIsPlayFinished(void)
{
    return capturePlayFinished;
}

static void CapturePlayFinished(void)
{
    capturePlayFinished = true;
}

int CaptureStartPlay(int index)
{
    char buf[128];
    CaptureEntry* entry;
    char* capturePath = CaptureGetPath();

    if (!captureInited || captureQuit || !capturePath)
        return -1;  

    pthread_mutex_lock(&capture_mutex);
    entry = CaptureGetEntry(index);

    if (!entry->played)
    {
        entry->played = true;
        if (captureUnplayCount > 0)
            captureUnplayCount--;
    }
    if(entry->type == MEDIA_IMAGE)
    {
        strcpy(buf, SNAPSHOT_PATH);
        strcat(buf, "/");
        strcat(buf, entry->filename);
        chmod(buf, S_IRWXU);
    }else
    {
        strcpy(buf, capturePath);
        strcat(buf, "/");
        strcat(buf, entry->filename);
        chmod(buf, S_IRWXU);
    }
#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    if (entry->type == MEDIA_VIDEO || entry->type == MEDIA_AUDIO)
    {
        capturePlayFinished = false;
        pthread_mutex_unlock(&capture_mutex);
        return VideoRecordStartPlay(buf, CapturePlayFinished);
    }
    else
    {
        pthread_mutex_unlock(&capture_mutex);
        return 0;
    }
}

void CaptureStopPlay(void)
{
    VideoRecordStopPlay();
    capturePlayFinished = false;
}

bool CaptureIsRecording(void)
{
    return captureRecording;
}

int CaptureStartRecord(MediaType type, char* addr)
{
    time_t rawtime;
    struct tm* timeinfo;
    char filepath[PATH_MAX];
    int len, ret = 0;
    char* capturePath = CaptureGetPath();

    pthread_mutex_lock(&capture_mutex);
    if (!captureInited || captureQuit || !capturePath)
    {
        pthread_mutex_unlock(&capture_mutex);  
        return -1;
    }
    
    len = strlen(addr);
    strcpy(captureRecordFileName, addr);
    captureRecordFileName[len] = '_';
    len++;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(&captureRecordFileName[len], sizeof(captureRecordFileName) - len,"%Y-%m-%d_%H-%M-%S", timeinfo);

    switch (type)
    {
    case MEDIA_IMAGE:
        strcat(captureRecordFileName, CAPTURE_IMAGE_EXT);
        break;

    case MEDIA_VIDEO:
        strcat(captureRecordFileName, CAPTURE_VIDEO_EXT);
        break;

    case MEDIA_AUDIO:
        strcat(captureRecordFileName, CAPTURE_AUDIO_EXT);
        break;

    default:
        strcat(captureRecordFileName, CAPTURE_IMAGE_EXT);
        break;
    }
    strcpy(filepath, capturePath);
    strcat(filepath, "/");
    strcat(filepath, captureRecordFileName);

    //remove(filepath);   //YC mark for 9850

    ret = VideoRecordStartRecord(filepath);
    //while(!captureRecording) usleep(1000);   //YC mark for 9850
    captureRecording = true;
    
    pthread_mutex_unlock(&capture_mutex);
    return ret;
}

int CaptureStopRecord(void)
{
    CaptureEntry* entry;
    int ret = 0;
    char* capturePath = CaptureGetPath();
    pthread_mutex_lock(&capture_mutex);
    if (!captureInited || captureQuit || !captureRecording)
    {
        pthread_mutex_unlock(&capture_mutex);
        return -1;
    }
    ret = VideoRecordStopRecord();
    captureRecording = false;
    //while(captureRecording) usleep(1000);
    
    if (ret == 0 && capturePath)
    {
        struct tm tm;
        char* pch;
        DeviceInfo info;
        char buf[PATH_MAX];
        int len;

        entry = &captureArray[captureCount];

        strncpy(entry->filename, captureRecordFileName, CAPUTRE_MAX_FILENAME_SIZE);
        entry->type = MEDIA_TEXT;
        pch = strrchr(captureRecordFileName, '.');
        if (pch)
        {
            if (stricmp(pch, CAPTURE_IMAGE_EXT) == 0)
                entry->type = MEDIA_IMAGE;
            else if (stricmp(pch, CAPTURE_VIDEO_EXT) == 0)
                entry->type = MEDIA_VIDEO;
            else if (stricmp(pch, CAPTURE_AUDIO_EXT) == 0)
                entry->type = MEDIA_AUDIO;

            *pch = '\0';
        }

        pch = strchr(captureRecordFileName, '_');
        *pch = '\0';
        AddressBookGetDeviceInfo(&info, captureRecordFileName);
        strcpy(buf, info.building);
        strcat(buf, StringGetDeviceType(info.type));
        strcat(buf, " ");
        len = strlen(buf);

        pch++;
        strptime(pch, "%Y-%m-%d_%H-%M-%S", &tm);
        strftime(&buf[len], sizeof(buf) - len,"%Y-%m-%d/%H:%M", &tm);

        strncpy(entry->name, buf, CAPUTRE_MAX_NAME_SIZE);
        entry->time = mktime(&tm);

        strcpy(buf, capturePath);
        strcat(buf, "/");
        strcat(buf, entry->filename);
        entry->sec = 0;
        entry->played = false;

        captureCount++;
        assert(captureCount <= MAX_CAPTURE_COUNT);

        captureUnplayCount++;
    }
    pthread_mutex_unlock(&capture_mutex);
    return ret;
}

static void CaptureSnapShotSuccess(void *arg)
{    
    DeviceInfo info;
    CaptureEntry* entry;
    int i ,len , name_len, ipaddr_len;
    char buf[PATH_MAX];
    char *filenamefullpath = NULL;
    char *filename = NULL;
    char *tmpStr1 ,*tmpStr2=NULL;
    pthread_mutex_lock(&capture_mutex);
    filenamefullpath = (char*)arg;

    captureSnapShotSuccess = true;
    if(captureSnapShotSuccess)  
    {
        filename = strrchr(filenamefullpath, '/');

        entry = &captureArray[captureCount];
        tmpStr1 = strchr(filenamefullpath, '_');
        tmpStr2 = strrchr(filenamefullpath, '-');
        strncpy(&buf[len], (tmpStr1+1) , ((tmpStr2-1)-tmpStr1));
        strncpy(entry->filename, (filename+1), CAPUTRE_MAX_FILENAME_SIZE);
		entry->type = MEDIA_IMAGE;
		strcpy(entry->name,CaptureSnapshotEntryname);
		entry->sec = 0;  
		entry->played = false;

        captureCount++;
        assert(captureCount <= MAX_CAPTURE_COUNT);
        captureUnplayCount++;

		captureSnapShotSuccess = false;
    }
    pthread_mutex_unlock(&capture_mutex);
}

int CaptureSnapshotStart(char* filepath)
{
    captureSnapShotSuccess = false;
    FileWriter_callback = CaptureSnapShotSuccess;   
    LinphoneSnapshot(filepath);
    return 0;
}

int CaptureSnapshot(char* addr, time_t t)
{
    time_t rawtime;
    struct tm* timeinfo;
    char buf[PATH_MAX];
    DeviceInfo info;
    CaptureEntry* entry;
    int len, ret = 0;
    //char* capturePath = CaptureGetPath();

    if (!captureInited || captureQuit )
    {
        return -1;
    }

	if(captureCount >= MAX_CAPTURE_COUNT || !CaptureCheckDiskSpace(512*1024 + 256*1024, MEDIA_IMAGE))
	{
		captureDeletePic = true;
		printf("SnapShot Memory full, please wait!!\n");
		strncpy(gSaveaddr, addr, sizeof(gSaveaddr));
		gSaveT =  t;
		captureSnapshotFlag = true;
        return ret;
	}

	pthread_mutex_lock(&capture_mutex);
    // composite entry's name
    AddressBookGetDeviceInfo(&info, addr);
    strcpy(buf, info.building);
    strcat(buf, StringGetDeviceType(info.type));
    strcat(buf, " ");
    len = strlen(buf);

    entry = &captureArray[captureCount];

    if (t)
    {
        timeinfo = localtime(&t);
        entry->time = t;
    }
    else
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        entry->time = rawtime;
    }
    strftime(&buf[len], sizeof(buf) - len, "%Y-%m-%d/%H:%M", timeinfo);   
    entry->type = MEDIA_IMAGE;
    strncpy(entry->name, buf, CAPUTRE_MAX_NAME_SIZE);
    strcpy(CaptureSnapshotEntryname, entry->name);
    entry->sec = 0;  
    entry->played = false;

    // composite entry's filename
    strcpy(buf, addr);
    len = strlen(buf);
    buf[len] = '_';
    len++;

    strftime(&buf[len], sizeof(buf) - len, "%Y-%m-%d_%H-%M-%S", timeinfo);
    strcat(buf, CAPTURE_IMAGE_EXT);
    strncpy(entry->filename, buf, CAPUTRE_MAX_FILENAME_SIZE);

    strcpy(buf, SNAPSHOT_PATH);
    strcat(buf, "/");
    strcat(buf, entry->filename);

    CaptureSnapshotStart(buf);
    pthread_mutex_unlock(&capture_mutex);
    return ret;
}

char* CaptureGetSnapshotFileNameByTime(char* time)
{
    int i, count = CaptureGetCount();
    struct tm tm;
    time_t t;

    strptime(time, "%c", &tm);
    t = mktime(&tm);

    for (i = 0; i < count; i++)
    {
        CaptureEntry* entry = CaptureGetEntry(i);
        if (entry && entry->type == MEDIA_IMAGE && entry->time == t)
        {
            return entry->filename;
        }
    }
    return NULL;
}

char* CaptureGetPath(void)
{
    static char capturePath[PATH_MAX];

    if (theDeviceInfo.type == DEVICE_INDOOR2)
    {
        char* path = UpgradeGetSDPath();
        if (path)
        {
            captureRunSDCardPath = true;
            strcpy(capturePath, path);
            strcat(capturePath, "capture");

            return capturePath;
        }
    }
    else
    {
        strcpy(capturePath, CFG_TEMP_DRIVE ":/capture");
        return capturePath;
    }
    return NULL;

}

bool CaptureCheckDiskSpace(int max_file_size, MediaType media_type)
{
    struct statvfs info;

    // check capacity
    if(media_type == MEDIA_VIDEO)
    {
        char* capturePath = CaptureGetPath();
        if (statvfs(capturePath, &info) == 0)
        {
            uint64_t avail = info.f_bfree * info.f_bsize;

            if (avail > max_file_size) //the free size that needs much more bigger than nor cache size.
                return true;

            printf("Out of disk space: %llu\n", avail);
        }
    }
    else if(media_type == MEDIA_IMAGE)
    {
        if (statvfs(SNAPSHOT_PATH, &info) == 0)
        {
            uint64_t avail = info.f_bfree * info.f_bsize;

            if (avail > max_file_size) //the free size that needs much more bigger than nor cache size.
                return true;

            printf("Out of disk space: %llu\n", avail);
        }
    }    
    return false;
}

int IPCamStartStreaming(void)
{
    int ret = 0;
    pthread_mutex_lock(&streaming_mutex);
    if(IPCamStreaming)
    {
        pthread_mutex_unlock(&streaming_mutex);
        return -1;
    }

    ret = IPCamStreamStartStreaming();
    while(!IPCamStreaming) usleep(1000);
    pthread_mutex_unlock(&streaming_mutex);
    return ret;
}

int IPCamStopStreaming(void)
{
    int ret = 0;
    pthread_mutex_lock(&streaming_mutex);
    if(!IPCamStreaming)
    {
        pthread_mutex_unlock(&streaming_mutex);
        return -1;
    }

    ret = IPCamStreamStopStreaming();
    while(IPCamStreaming) usleep(1000);
    pthread_mutex_unlock(&streaming_mutex);
    return ret;
}

bool IPCamIsRecording(void)
{
    return captureRecording;
}

int IPCamStartRecord(MediaType type, char* addr)
{
    time_t rawtime;
    struct tm* timeinfo;
    char filepath[PATH_MAX];
    int len, ret = 0;
    char* capturePath = CaptureGetPath();

    pthread_mutex_lock(&capture_mutex);
    if (!captureInited || captureQuit || !capturePath || !IPCamStreaming)
    {
        pthread_mutex_unlock(&capture_mutex);
        return -1;
    }    

    len = strlen(addr);
    strcpy(captureRecordFileName, addr);
    captureRecordFileName[len] = '_';
    len++;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(&captureRecordFileName[len], sizeof(captureRecordFileName) - len,"%Y-%m-%d_%H-%M-%S", timeinfo);

    switch (type)
    {
    case MEDIA_IMAGE:
        strcat(captureRecordFileName, CAPTURE_IMAGE_EXT);
        break;

    case MEDIA_VIDEO:
        strcat(captureRecordFileName, CAPTURE_VIDEO_EXT);
        break;

    case MEDIA_AUDIO:
        strcat(captureRecordFileName, CAPTURE_AUDIO_EXT);
        break;

    default:
        strcat(captureRecordFileName, CAPTURE_IMAGE_EXT);
        break;
    }
    strcpy(filepath, capturePath);
    strcat(filepath, "/");
    strcat(filepath, captureRecordFileName);

    //remove(filepath);
    ret = IPCamRecordStartRecord(filepath);

    captureRecording = true;
    pthread_mutex_unlock(&capture_mutex);
    return ret;       
}

int IPCamStopRecord(void)
{
    CaptureEntry* entry;
    int ret = 0;
    char* capturePath = CaptureGetPath();
    pthread_mutex_lock(&capture_mutex);
    if (!captureInited || captureQuit || !captureRecording)
    {
        pthread_mutex_unlock(&capture_mutex);    
        return -1;
    }
    ret = IPCamRecordStopRecord();
    captureRecording = false;
    if (ret == 0 && capturePath)
    {
        struct tm tm;
        char* pch;
        DeviceInfo info;
        char buf[PATH_MAX];
        int len;

        entry = &captureArray[captureCount];

        strncpy(entry->filename, captureRecordFileName, CAPUTRE_MAX_FILENAME_SIZE);
        entry->type = MEDIA_TEXT;
        pch = strrchr(captureRecordFileName, '.');
        if (pch)
        {
            if (stricmp(pch, CAPTURE_IMAGE_EXT) == 0)
                entry->type = MEDIA_IMAGE;
            else if (stricmp(pch, CAPTURE_VIDEO_EXT) == 0)
                entry->type = MEDIA_VIDEO;
            else if (stricmp(pch, CAPTURE_AUDIO_EXT) == 0)
                entry->type = MEDIA_AUDIO;

            *pch = '\0';
        }

        pch = strchr(captureRecordFileName, '_');
        *pch = '\0';
        AddressBookGetDeviceInfo(&info, captureRecordFileName);
        strcpy(buf, info.building);
        strcat(buf, StringGetDeviceType(info.type));
        strcat(buf, " ");
        len = strlen(buf);

        pch++;
        strptime(pch, "%Y-%m-%d_%H-%M-%S", &tm);
        strftime(&buf[len], sizeof(buf) - len,"%Y-%m-%d/%H:%M", &tm);

        strncpy(entry->name, buf, CAPUTRE_MAX_NAME_SIZE);
        entry->time = mktime(&tm);

        strcpy(buf, capturePath);
        strcat(buf, "/");
        strcat(buf, entry->filename);
        entry->sec = 0;
        entry->played = false;

        captureCount++;
        assert(captureCount <= MAX_CAPTURE_COUNT);

        captureUnplayCount++;
    }
    pthread_mutex_unlock(&capture_mutex);
    return ret;
}

static void IPCamSnapShotSuccess(void *arg)
{    
    DeviceInfo info;
    CaptureEntry* entry;
    int i ,len , name_len, ipaddr_len;
    char buf[PATH_MAX];
    char *filenamefullpath = NULL;
    char *filename = NULL;
    char *tmpStr1 ,*tmpStr2=NULL;
    pthread_mutex_lock(&capture_mutex);
    filenamefullpath = (char*)arg;

    ipCamSnapShotSuccess = true;
    if(ipCamSnapShotSuccess)  
    {
        filename = strrchr(filenamefullpath, '/');

        entry = &captureArray[captureCount];
        tmpStr1 = strchr(filenamefullpath, '_');
        tmpStr2 = strrchr(filenamefullpath, '-');
        strncpy(&buf[len], (tmpStr1+1) , ((tmpStr2-1)-tmpStr1));
        strncpy(entry->filename, (filename+1), CAPUTRE_MAX_FILENAME_SIZE);
		entry->type = MEDIA_IMAGE;
		strcpy(entry->name,IPCamSnapshotEntryname);
		entry->sec = 0;  
		entry->played = false;

        captureCount++;
        assert(captureCount <= MAX_CAPTURE_COUNT);
        captureUnplayCount++;
		
		ipCamSnapShotSuccess = false;
    }
    pthread_mutex_unlock(&capture_mutex);
}


int IPCamSnapshotStart(char* filepath)
{
    ipCamSnapShotSuccess = false;
    FileWriter_callback = IPCamSnapShotSuccess;
    LinphoneIPCamSnapshot(filepath);
    return 0;
}

int IPCamSnapshot(char* addr, time_t t)
{
    time_t rawtime;
    struct tm* timeinfo;
    char buf[PATH_MAX];
    DeviceInfo info;
    CaptureEntry* entry;
    int len, ret = 0;
    //char* capturePath = CaptureGetPath();

    if (!captureInited || captureQuit || !IPCamStreaming)
        return -1;

    if(captureCount >= MAX_CAPTURE_COUNT || !CaptureCheckDiskSpace(512*1024 + 256*1024, MEDIA_IMAGE))
	{
		captureDeletePic = true;
		printf("SnapShot Memory full, please wait!!\n");
		strncpy(gSaveaddr, addr, sizeof(gSaveaddr));
		gSaveT =  t;
		IPCamSnapshotFlag = true;
        return ret;
	}

    pthread_mutex_lock(&capture_mutex);
    // composite entry's name
    AddressBookGetDeviceInfo(&info, addr);
    strcpy(buf, info.building);
    strcat(buf, StringGetDeviceType(info.type));
    strcat(buf, " ");
    len = strlen(buf);

    entry = &captureArray[captureCount];

    if (t)
    {
        timeinfo = localtime(&t);
        entry->time = t;
    }
    else
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        entry->time = rawtime;
    }
    strftime(&buf[len], sizeof(buf) - len, "%Y-%m-%d/%H:%M", timeinfo);

    entry->type = MEDIA_IMAGE;
    strncpy(IPCamSnapshotEntryname, buf, CAPUTRE_MAX_NAME_SIZE);
    entry->sec = 0;
    entry->played = false;

    // composite entry's filename
    strcpy(buf, addr);
    len = strlen(buf);
    buf[len] = '_';
    len++;

    strftime(&buf[len], sizeof(buf) - len, "%Y-%m-%d_%H-%M-%S", timeinfo);
    strcat(buf, CAPTURE_IMAGE_EXT);
    strncpy(entry->filename, buf, CAPUTRE_MAX_FILENAME_SIZE);
    
    strcpy(buf, SNAPSHOT_PATH);
    strcat(buf, "/");
    strcat(buf, entry->filename);

    IPCamSnapshotStart(buf);
    pthread_mutex_unlock(&capture_mutex);
    return ret;
}


