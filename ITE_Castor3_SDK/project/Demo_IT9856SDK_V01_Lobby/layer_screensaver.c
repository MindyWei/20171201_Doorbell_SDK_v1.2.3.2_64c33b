#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include "scene.h"
#include "doorbell.h"
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "mediastreamer2/mediastream.h"
#include "castor3player.h"
#include "ite/itp.h"


static ITULayer* userLayer;
static ITUIcon* screenSaverIcon;
static ITUAnalogClock* screenSaverAnalogClock;
static ITUBackground* screensaverVideoBackground;

#define MAX_AD_COUNT 64
#define FILEPATH_MAX 512

typedef struct
{
    uint8_t* memory;
    size_t size;
} ADVINFO_Memory;

typedef struct
{
    char *filename;
    FILE *stream;
} FTPFile;

typedef struct
{
    char path[FILEPATH_MAX];
} AD;

typedef struct
{
    char checksum[40];
    int count;
    char btm[10];
    char etm[10];
    int no;
    AD ad[MAX_AD_COUNT];

} ADVInfo;

char Adv_Checksum[40] = {0};
int Adv_Count = 0;
char FTP_DOWNLOAD_DIR[FILEPATH_MAX] = {0};
pthread_t adv_tid;
static ADVInfo advInfo;
static int adv_index = 0;
static bool b_StreamEOF = false;
static bool b_StreamDisconnect = false;
static bool b_AllowPlayingADV;
static bool b_DownloadADVFinished;
static bool b_ForceStopDownload = false;
static MTAL_SPEC mtal_spec = {0};

static char* GetSDPath(void)
{
    ITPDriveStatus* driveStatusTable;
    ITPDriveStatus* driveStatus = NULL;
    int i;

    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        //if (driveStatus->avail && driveStatus->disk >= ITP_DISK_MSC00 && driveStatus->disk <= ITP_DISK_MSC17)
        if (driveStatus->avail && driveStatus->disk >= ITP_DISK_SD0 && driveStatus->disk <= ITP_DISK_SD1)
        {
            return driveStatus->name;
        }
    }
    return NULL;
}

static void DeleteAllFilesFromDir(char *dir)
{
    DIR *pdir = NULL;
    char path[FILEPATH_MAX];
    // Open the directory
    pdir = opendir(dir);
    if(pdir)
    {
        struct dirent *pent;
        chdir(dir);
        while ((pent = readdir(pdir)) != NULL)
        {
            if ((strcmp(pent->d_name, ".") == 0) || (strcmp(pent->d_name, "..") == 0))
            {
                continue;
            }

            strcpy(path, dir);
            strcat(path, "/");
            strcat(path, pent->d_name);
            remove(path);
        }
    }

    if (pdir)
    {
        closedir(pdir);
    }
}


static size_t WriteADVMemoryData(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    ADVINFO_Memory* mem = (ADVINFO_Memory*)data;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


static int GetADVInfo(void)
{
    CURL* curl = NULL;
    CURLcode res = -1;
    char url[128];
    ADVINFO_Memory mem;
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    xmlNodeSetPtr nodeset = NULL;
    int i;

    mem.memory = NULL;

    if (theCenterInfo.ip[0] == '\0')
    {
        res = -1;
        goto end;
    }

    mem.memory = malloc(1);
    if (!mem.memory)
    {
        res = -1;
        goto end;
    }

    mem.size = 0;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        res = -1;
        goto end;
    }

    sprintf(url, "http://%s/doorbell/get_adinfo?ro=%s-%s-%s-%s-%s-%s",
        theCenterInfo.ip, theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteADVMemoryData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        res = -1;
        goto end;
    }

    doc = xmlParseMemory(mem.memory, mem.size);
    if (!doc)
    {
        printf("xmlParseMemory() fail\n");
        res = -1;
        goto end;
    }

    xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx)
    {
        printf("xmlXPathNewContext() fail\n");
        res = -1;
        goto end;
    }

    res = 0;
    memset(&advInfo, 0, sizeof (ADVInfo));

    //cfg
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/advertisement/cfg", xpathCtx);
    if (xpathObj)
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlChar* str;
        str = xmlGetProp(node,(const xmlChar*)"checksum");
        if(str)
            strcpy(advInfo.checksum, str);
        str = xmlGetProp(node,(const xmlChar*)"count");
        advInfo.count = atoi(str);
        str = xmlGetProp(node,(const xmlChar*)"btm");
        strcpy(advInfo.btm, str);
        str = xmlGetProp(node,(const xmlChar*)"etm");
        strcpy(advInfo.etm, str);
        xmlFree(str);        
    }

    //ad
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/advertisement/ad", xpathCtx);
    if (xpathObj)
    {
        xmlChar* str;
        nodeset = xpathObj->nodesetval;  
        for(i = 0; i < nodeset->nodeNr; i++)
        {
            str = xmlGetProp(nodeset->nodeTab[i],(const xmlChar*)"no");
            advInfo.no = atoi(str);
            str = xmlGetProp(nodeset->nodeTab[i],(const xmlChar*)"path");
            strcpy(advInfo.ad[advInfo.no - 1].path, str);
            xmlFree(str);
        }
    }
    
end:
    if (xpathObj)
        xmlXPathFreeObject(xpathObj);
    
    if (xpathCtx)
        xmlXPathFreeContext(xpathCtx);

    if (doc)
        xmlFreeDoc(doc);

    free(mem.memory);

    if (curl)
        curl_easy_cleanup(curl);

    return res;
}

static size_t WriteADVFile(void *buffer, size_t size, size_t nmemb, void *stream)
{
    FTPFile *out=(FTPFile *)stream;
    if(out && !out->stream)
    {
        /* open file for writing */
        out->stream=fopen(out->filename, "wb");
        if(!out->stream)
        {
            printf("Open file fail\n");
            return -1; /* failure, can't open file to write */ 
        }    
    }
    return fwrite(buffer, size, nmemb, out->stream);
}

static int DownloadADVFile(char *url, char *file_path)
{
    CURL *curl;
    CURLM *curlm;
    CURLcode res = -1;
    int still_running = 0;

    FTPFile ftpfile;
    ftpfile.filename = file_path;
    ftpfile.stream = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        res = -1;
        goto end;
    }
#if 1
    curlm = curl_multi_init();
    if (!curlm)
    {
        printf("curl_multi_init() fail.\n");
        res = -1;
        goto end;
    }
#endif
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteADVFile);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

#if 1
    curl_multi_add_handle(curlm, curl);
    do{
        res = curl_multi_perform(curlm, &still_running);
        if (CURLE_OK != res)
        {
            printf("curl_multi_perform() fail: %d\n", res);
            break;
        }
        usleep(1000);
    }while(still_running!=0 && !b_ForceStopDownload);
#endif
    //res = curl_easy_perform(curl);
    
    if (CURLE_OK != res || b_ForceStopDownload)
    {
        printf("Download file fail: %d\n",res);
        res = -1;
        goto end;
    }
       
end:
    if (curl)
        curl_easy_cleanup(curl);
   
    if (curlm)
        curl_multi_cleanup(curlm);
 
    if (ftpfile.stream)
        fclose(ftpfile.stream);
    
    curl_global_cleanup();
    return res;
}

static void *ADVDownload_main(void *arg)
{
    char *dir_path = (char *)arg;
    int i;
    int b_Failed = 0;
    
    for(i=0; i<advInfo.count; i++)
    {
        char outfile[FILEPATH_MAX] = {0};
        char ftp_url[FILEPATH_MAX] = {0};
        sprintf(outfile,"%s/%d.mkv", dir_path, i+1);
        sprintf(ftp_url,"ftp://%s/%s", theCenterInfo.ip, advInfo.ad[i].path);
        printf("FTP download url: %s, outfile = %s\n", ftp_url, outfile);
        if(DownloadADVFile(ftp_url, outfile) == -1)
        {
            b_Failed = -1;
            break;
        }
    }
    if(b_Failed == -1)
    {
        printf("FTP download file fail\n");
        Adv_Count = 0;
        memset(Adv_Checksum, 0, sizeof(Adv_Checksum));
        b_DownloadADVFinished = false;
    }
    else
    {
        printf("FTP download finished\n");
        b_DownloadADVFinished = true;
    }
    pthread_exit(NULL);
}

static int ADVStartDownload(char *dir)
{
    int rc;
    //char temp[FILEPATH_MAX];
    //strcpy(temp, dir);
    printf("ADVStartDownload...\n");
    b_ForceStopDownload = false;

#ifndef WIN32    
    adv_tid = 0;
#endif
    rc = pthread_create(&adv_tid, NULL, ADVDownload_main, (void *)dir);
    if(rc)
    {
        printf("Create thread fail...\n");
        return -1;
    }

    return 0;
}

static int ADVStopDownload()
{
#ifndef WIN32
    if(adv_tid)
#endif        
	{
	    printf("ADVStopDownload...\n");
	    b_ForceStopDownload = true;
		pthread_join(adv_tid, NULL);
#ifndef WIN32        
		adv_tid = 0;
#endif
	}
    return 0;
}

#if defined(CFG_RTSP_STREAMING)   
void EventHandler(RTSPCLIENT_EVENT nEventID, void *arg)
{
    int level;
    switch (nEventID)
    {
    case STREAM_EOF:
        printf("Receive Stream EOF\n");
        b_StreamEOF = true;
        adv_index++;
        if(adv_index == advInfo.count)
            adv_index = 0;    
        break;
    case STREAM_ERROR:
        printf("Receive Stream Error\n");
        b_StreamEOF = true;
        break;
    case STREAM_CONNECT_FAIL:
        printf("Receive Stream Connect Fail\n");
        b_StreamDisconnect = true;
        break;
    case STREAM_SET_VOLUME:
        //printf("Reset volume when open audio codec\n");
        level = LinphoneGetVoiceLevel();
        LinphoneSetVoiceLevel(level);
        break;
        
    default:
        break;
    }
}
#elif defined(CFG_DOWNLOAD_TO_SD_CARD)
void EventHandler(PLAYER_EVENT nEventID, void *arg)
{
    switch(nEventID)
    {
    case PLAYER_EVENT_EOF:
        printf("File EOF\n");
        b_StreamEOF = true;
        adv_index++;
        if(adv_index == advInfo.count)
            adv_index = 0;
        break;
    case PLAYER_EVENT_OPEN_FILE_FAIL:
        printf("Open file fail\n");
        b_StreamEOF = true;
        adv_index++;
        if(adv_index == advInfo.count)
            adv_index = 0;
        break;
    case PLAYER_EVENT_UNSUPPORT_FILE:
        printf("File not support\n");
        b_StreamEOF = true;
        adv_index++;
        if(adv_index == advInfo.count)
            adv_index = 0;
        break;
    default:
        break;
    }
}
#endif 

bool ScreenSaverKeyOnPress(ITUWidget* widget, char* param)
{
    ScreenSaverRefresh();
	ituLayerGoto(userLayer);
	return true;
}

bool ScreenSaverOnTimer(ITUWidget* widget, char* param)
{
#if defined(CFG_RTSP_STREAMING)
    if (theConfig.screensaver_type == SCREENSAVER_ADV && b_AllowPlayingADV)
    {
        if(b_StreamDisconnect)
        {
            b_StreamDisconnect = false;
            ScreenSaverRefresh();
	        ituLayerGoto(userLayer);
        }
        if(b_StreamEOF)
        {
            b_StreamEOF = false;
#ifdef CFG_RTSP_CLIENT_ENABLE            
            stopRTSPClient();
#endif
            usleep(500000);
            printf("Play Next Stream\n");
#ifdef CFG_RTSP_CLIENT_ENABLE            
            startRTSPClient(theCenterInfo.ip, 3232, advInfo.ad[adv_index].path, EventHandler);
#endif
        }
    }
#elif defined(CFG_DOWNLOAD_TO_SD_CARD)
    if (theConfig.screensaver_type == SCREENSAVER_ADV && b_AllowPlayingADV && b_DownloadADVFinished)
    {
        char file_path[FILEPATH_MAX] = {0};
        if(ituWidgetIsVisible(screenSaverAnalogClock))
        {
            ituWidgetSetVisible(screenSaverAnalogClock, false);
            //start playing adv
            ituWidgetSetVisible(screensaverVideoBackground, true);
            b_StreamEOF = false;
            adv_index = 0;
            sprintf(file_path,"%s/%d.mkv", FTP_DOWNLOAD_DIR, adv_index+1);
            strcpy(mtal_spec.srcname, file_path);
            mtal_spec.vol_level = LinphoneGetVoiceLevel();
            mtal_pb_select_file(&mtal_spec);
            mtal_pb_play();
        }
        else
        {
            if(b_StreamEOF)
            {
                b_StreamEOF = false;
                mtal_pb_stop();
                sprintf(file_path,"%s/%d.mkv", FTP_DOWNLOAD_DIR, adv_index+1);
                strcpy(mtal_spec.srcname, file_path);
                mtal_spec.vol_level = LinphoneGetVoiceLevel();
                mtal_pb_select_file(&mtal_spec);
                mtal_pb_play();    
            }
        }
    }
#endif     
    return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void screensaverVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
}
#endif

bool ScreenSaverOnEnter(ITUWidget* widget, char* param)
{
    if (!userLayer)
    {
        userLayer = ituSceneFindWidget(&theScene, "userLayer");
        assert(userLayer);

        screenSaverIcon = ituSceneFindWidget(&theScene, "screenSaverIcon");
        assert(screenSaverIcon);

        screenSaverAnalogClock = ituSceneFindWidget(&theScene, "screenSaverAnalogClock");
        assert(screenSaverAnalogClock);

        screensaverVideoBackground = ituSceneFindWidget(&theScene, "screensaverVideoBackground");
        assert(screensaverVideoBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(screensaverVideoBackground, screensaverVideoBackgroundDraw);
#endif        
    }
    ituWidgetSetVisible(screenSaverIcon, false);
    ituWidgetSetVisible(screenSaverAnalogClock, false);
    ituWidgetSetVisible(screensaverVideoBackground, false);

    if(theConfig.screensaver_type == SCREENSAVER_ADV)
    {
        struct timeval tv;
        struct tm* tm;
        char ctime[10];

        b_AllowPlayingADV = true;
        b_DownloadADVFinished = false;
        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);
        sprintf(ctime, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
        printf("Current time = %s\n", ctime);
#if defined(CFG_RTSP_STREAMING)
        if(GetADVInfo() == -1)
        {
            b_AllowPlayingADV = false;
        }
        else if(strlen(advInfo.btm) && strlen(advInfo.etm))
        {
            if(strcmp(ctime, advInfo.btm) < 0 || strcmp(ctime, advInfo.etm) > 0)
            {
                b_AllowPlayingADV = false;
            }
        }
        printf("Count = %d, Btm = %s, Etm = %s, First Path = %s\n", advInfo.count, advInfo.btm, advInfo.etm, advInfo.ad[0].path);
#elif defined(CFG_DOWNLOAD_TO_SD_CARD)
        if(GetADVInfo() == -1)
        {
            if(Adv_Count == 0)
                b_AllowPlayingADV = false;
            else
                b_DownloadADVFinished = true;
        }
        else
        {
            if(!strlen(advInfo.checksum))
            {
                if(Adv_Count == 0)
                    b_AllowPlayingADV = false;
                else
                    b_DownloadADVFinished = true;
            }   
            else
            {
                if(strlen(advInfo.btm) && strlen(advInfo.etm))
                {
                    if(strcmp(ctime, advInfo.btm) < 0 || strcmp(ctime, advInfo.etm) > 0)
                    {
                        b_AllowPlayingADV = false;
                    }
                }

                printf("Checksum = %s, Count = %d, Btm = %s, Etm = %s, First Path = %s\n", advInfo.checksum, advInfo.count, advInfo.btm, advInfo.etm, advInfo.ad[0].path);
                //if(Adv_Checksum != advInfo.checksum) //checksum is changed, need re-downloading files.            
                if(strlen(Adv_Checksum) == 0 || strcmp(Adv_Checksum, advInfo.checksum) != 0)
                {
                    printf("Checksum is modified\n");
                    //char buf[FILEPATH_MAX];
                    char *path = GetSDPath();
                        
                    //Adv_Checksum = advInfo.checksum;
                    strcpy(Adv_Checksum, advInfo.checksum);
                    Adv_Count = advInfo.count;
                    if(path)
                    {
                        //strcpy(buf, path);
                        //strcat(buf, "adv");
                        strcpy(FTP_DOWNLOAD_DIR, path);
                        strcat(FTP_DOWNLOAD_DIR, "adv");
                        mkdir(FTP_DOWNLOAD_DIR, S_IRWXU);
                        printf("FTP_download_dir = %s\n", FTP_DOWNLOAD_DIR);
                        DeleteAllFilesFromDir(FTP_DOWNLOAD_DIR);
                        //start download files
                        ADVStartDownload(FTP_DOWNLOAD_DIR);
                    }
                    else
                    {
                        printf("No detect SD card...\n");
                        Adv_Count = 0;
                        memset(Adv_Checksum, 0, sizeof(Adv_Checksum));
                        b_AllowPlayingADV = false;
                    }    
                }
                else
                    b_DownloadADVFinished = true;
            }
        }
#endif        
    }
    
    switch (theConfig.screensaver_type)
    {
    case SCREENSAVER_CLOCK:
        ituWidgetSetVisible(screenSaverAnalogClock, true);
        break;

    case SCREENSAVER_PHOTO:
        {
            // try to load screensaver jpeg file if exists
            FILE* f = fopen(CFG_PUBLIC_DRIVE ":/screensaver.jpg", "rb");
            if (f)
            {
                uint8_t* data;
                int size;

                fseek(f, 0, SEEK_END);
                size = ftell(f);
                fseek(f, 0, SEEK_SET);

                data = malloc(size);
                if (data)
                {
                    size = fread(data, 1, size, f);
                    ituIconLoadJpegData(screenSaverIcon, data, size);
                    free(data);
                }
                fclose(f);
            }
            ituWidgetSetVisible(screenSaverIcon, true);
        }
        break;

    case SCREENSAVER_ADV:
        {
#if defined(CFG_RTSP_STREAMING)            
            if(b_AllowPlayingADV)
            {
                ituWidgetSetVisible(screensaverVideoBackground, true);
                castor3snd_deinit_for_video_memo_play();
                b_StreamEOF = false;
                b_StreamDisconnect = false;
                adv_index = 0;
                LinphoneStartADVPlay();
#ifdef CFG_RTSP_CLIENT_ENABLE
                SetRTSPClientMode(ADV_MODE);
                startRTSPClient(theCenterInfo.ip, 3232, advInfo.ad[adv_index].path, EventHandler);
#endif
            }
            else
                ituWidgetSetVisible(screenSaverAnalogClock, true);
#elif defined(CFG_DOWNLOAD_TO_SD_CARD)
            if(b_AllowPlayingADV) //init player
            {
                castor3snd_deinit_for_video_memo_play();
                mtal_pb_init(EventHandler);
            }
            
            if(b_AllowPlayingADV && b_DownloadADVFinished)
            {
                char file_path[FILEPATH_MAX] = {0};
                //start playing adv
                ituWidgetSetVisible(screensaverVideoBackground, true);
                b_StreamEOF = false;
                adv_index = 0;
                sprintf(file_path,"%s/%d.mkv", FTP_DOWNLOAD_DIR, adv_index+1);
                strcpy(mtal_spec.srcname, file_path);
                mtal_spec.vol_level = LinphoneGetVoiceLevel();
                mtal_pb_select_file(&mtal_spec);
                mtal_pb_play();
            }
            else
                ituWidgetSetVisible(screenSaverAnalogClock, true);            
#endif            
        }
        break;
    case SCREENSAVER_POST:
        //ituWidgetSetVisible(screensaverVideoBackground, true);
        // TODO: IMPLEMENT
        break;
    }
	return true;
}

bool ScreenSaverOnLeave(ITUWidget* widget, char* param)
{
#if defined(CFG_RTSP_STREAMING)
    if (theConfig.screensaver_type == SCREENSAVER_ADV && b_AllowPlayingADV)
    {
        b_StreamEOF = false;
        b_StreamDisconnect = false;
        LinphoneStopADVPlay();
    }
#elif defined(CFG_DOWNLOAD_TO_SD_CARD)
    if (theConfig.screensaver_type == SCREENSAVER_ADV && b_AllowPlayingADV)
    {
        if(!b_DownloadADVFinished)
        {
            ADVStopDownload();
        }
        else
        {
            mtal_pb_stop();
            b_StreamEOF = false;
        }
        mtal_pb_exit();
        castor3snd_reinit_for_video_memo_play();
    }
#endif    
    return true;
}

void ScreenSaverReset(void)
{
    userLayer = NULL;
}
