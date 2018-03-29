#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/ug.h"
#include "doorbell.h"

#define URL_LEN 256

static char upgradeUrl[URL_LEN];
static bool upgradeIsFinished;
static int upgradeResult;
static ITCArrayStream arrayStream;
static ITCFileStream fileStream;
static ITCStream* upgradeStream;
static char pkgFilePath[PATH_MAX];

struct MemoryStruct
{
  char *memory;
  size_t size;
};

int UpgradeInit(void)
{
    int ret = 0;

#ifdef CFG_CHECK_FILES_CRC_ON_BOOTING

    ret = ugCheckFilesCrc(CFG_PUBLIC_DRIVE ":", CFG_PUBLIC_DRIVE ":/ite_crc.dat");
    if (ret)
    {
        printf("check public file crc result: %d\n", ret);
        ret = QUIT_RESET_FACTORY;
        goto end;
    }
end:
#endif // CFG_CHECK_FILES_CRC_ON_BOOTING

    upgradeStream = NULL;
    upgradeIsFinished = false;
    upgradeResult = 0;
    return ret;
}

static void UpgradeLcdConsoleEnable(void)
{
#ifdef CFG_LCD_ENABLE
    // switch to lcd console
    itpRegisterDevice(ITP_DEVICE_STD, &itpDeviceLcdConsole);
    itpRegisterDevice(ITP_DEVICE_LCDCONSOLE, &itpDeviceLcdConsole);
    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_LCDCONSOLE, ITP_IOCTL_CLEAR, NULL);
#endif
}

static int UpgradeResetFactory(void)
{
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_UNMOUNT, (void*)ITP_DISK_NOR);
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORCE_MOUNT, (void*)ITP_DISK_NOR);
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORMAT, (void*)1);
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORMAT, (void*)2);
    ugResetFactory();
    return 0;
}

static int UpgradeResetTempDrive(void)
{
    int ret = 0;

#ifdef CFG_WATCHDOG_ENABLE
    ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_DISABLE, NULL);
#endif
    puts("Unmounting...");
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_UNMOUNT, (void*)ITP_DISK_NOR);
    puts("Mounting...");
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORCE_MOUNT, (void*)ITP_DISK_NOR);
    puts("Formating...");
    ioctl(ITP_DEVICE_FAT, ITP_IOCTL_FORMAT, (void*)2);
    
#ifdef CFG_WATCHDOG_ENABLE
    ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_ENABLE, NULL);
#endif
    
    puts("Finished.");

    return ret;
}

static ITCStream* OpenUsbPackage(char* path)
{
    ITPDriveStatus* driveStatusTable;
    ITPDriveStatus* driveStatus = NULL;
    int i;

    // try to find the package drive
    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && driveStatus->disk >= ITP_DISK_MSC00 && driveStatus->disk <= ITP_DISK_MSC17)
        {
            char buf[PATH_MAX], *ptr, *saveptr;

            // get file path from list
            strcpy(buf, path);
            ptr = strtok_r(buf, " ", &saveptr);
            do
            {
                strcpy(pkgFilePath, driveStatus->name);
                strcat(pkgFilePath, ptr);

                if (itcFileStreamOpen(&fileStream, pkgFilePath, false) == 0)
                {
                    printf("found package file %s\n", pkgFilePath);
                        return &fileStream.stream;
                }
                else
                {
                    printf("try to fopen(%s) fail:0x%X\n", pkgFilePath, errno);
                }
            }
            while ((ptr = strtok_r(NULL, " ", &saveptr)) != NULL);
        }
    }
    printf("cannot find package file.\n");
    return NULL;
}

static size_t throw_away(void *ptr, size_t size, size_t nmemb, void *data)
{
    (void)ptr;
    (void)data;
    /* we are not interested in the headers itself,
     so we only return the size we would have saved ... */
    return (size_t)(size * nmemb);
}

static int GetPackageSize(char* ftpurl)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    double filesize = 0.0;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_URL, ftpurl);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, throw_away);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0L);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 15L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }

    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &filesize);
    if ((CURLE_OK == res) && (filesize > 0.0))
    {
        printf("filesize: %0.0f bytes\n", filesize);
    }
    else
    {
        printf("curl_easy_getinfo(CURLINFO_CONTENT_LENGTH_DOWNLOAD) fail: %d, filesize: %0.0f bytes\n", res, filesize);
        filesize = 0.0;
        goto end;
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    return (int)filesize;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    assert(mem->memory);

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;

    return realsize;
}

static ITCStream* DownloadPackage(char* ftpurl, int filesize)
{
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(filesize);    /* will be grown as needed by the realloc above */ 
    chunk.size = 0;                     /* no data at this point */ 

    /* init the curl session */ 
    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto error;
    }

    /* specify URL to get */ 
    curl_easy_setopt(curl, CURLOPT_URL, ftpurl);

    /* send all data to this function  */ 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */ 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */ 
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    curl_easy_setopt(curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 15L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    /* get it! */
    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto error;
    }
    else
    {
        printf("%lu bytes retrieved\n", (long)chunk.size);
    }

    curl_easy_cleanup(curl);

    itcArrayStreamOpen(&arrayStream, chunk.memory, chunk.size);

    return &arrayStream.stream;

error:
    if (curl)
        curl_easy_cleanup(curl);

    free(chunk.memory);

    return NULL;
}

static ITCStream* OpenFtpPackage(char* path)
{
    int filesize;
    ITCStream* fwStream = NULL;
    int retry = 10;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    printf("ftp url: %s\n", path);

    // get file size first
    while (retry-- >= 0)
    {
        filesize = GetPackageSize(path);
        if (filesize > 0)
            break;
    }

    // download firmware
    while (retry-- >= 0)
    {
        fwStream = DownloadPackage(path, filesize);
        if (fwStream)
            break;
    };

    curl_global_cleanup();
    return fwStream;
}

static int UpgradePackage(void)
{
    int ret = 0;
    ITCStream* fwStream = NULL;

    if (upgradeUrl[0] == '\0')
    {
        // open from USB drive
        fwStream = OpenUsbPackage(CFG_UPGRADE_FILENAME_LIST);
        if (!fwStream)
        {
            ret = -1;
            printf("packages unavailable: %s\n", CFG_UPGRADE_FILENAME_LIST);
            return ret;
        }
    }
    else
    {
        // download from ftp server
        fwStream = OpenFtpPackage(upgradeUrl);
        if (!fwStream)
        {
            ret = -1;
            printf("remote package unavailable: %s\n", upgradeUrl);
            return ret;
        }
    }

    ret = ugCheckCrc(fwStream, NULL);
    if (ret)
    {
        printf("check crc fail: %d.\n", ret);
        return ret;
    }

    ret = ugUpgradePackage(fwStream);
    if (ret)
    {
        printf("upgrade fail: %d.\n", ret);
        return ret;
    }

#ifdef CFG_UPGRADE_DELETE_PKGFILE_AFTER_FINISH
    if (upgradeUrl[0] == '\0')
    {
        remove(pkgFilePath);
    }
#endif

    printf("upgrade success!\n");
    
#if defined(CFG_UPGRADE_DELAY_AFTER_FINISH) && CFG_UPGRADE_DELAY_AFTER_FINISH > 0
    sleep(CFG_UPGRADE_DELAY_AFTER_FINISH);
#endif    
        
    return 0;
}

void UpgradeSetFileCrc(char* filepath)
{
    if (strncmp(filepath, CFG_PUBLIC_DRIVE, 1) == 0)
        ugSetFileCrc(&filepath[2], CFG_PUBLIC_DRIVE ":", CFG_PUBLIC_DRIVE ":/ite_crc.dat");
}

static void UpgradeFilesCrc(void)
{
    ugUpgradeFilesCrc(CFG_PUBLIC_DRIVE ":", CFG_PUBLIC_DRIVE ":/ite_crc.dat");
}

char* UpgradeGetDeviceInfo(int* size)
{
#ifdef CFG_CARDLIST_ENABLE
    const char* docStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?><DeviceInfo><firmware/><addressbook/><cardlist/></DeviceInfo>";
#else
    const char* docStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?><DeviceInfo><firmware/><addressbook/></DeviceInfo>";
#endif
    xmlDocPtr doc;
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    xmlChar* xmlbuff = NULL;

    doc = xmlParseMemory(docStr, strlen(docStr));
    if (!doc)
        goto end;

    xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx)
        goto end;

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/firmware", xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
        char buf[128];

        sprintf(buf, "%s.%s.%s.%s.%s", CFG_VERSION_MAJOR_STR, CFG_VERSION_MINOR_STR, CFG_VERSION_PATCH_STR, CFG_VERSION_CUSTOM_STR, CFG_VERSION_TWEAK_STR);
        xmlSetProp(node, "version", buf);

        xmlXPathFreeObject(xpathObj);
    }

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/addressbook", xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
        char* ptr = AddressBookGetVersion();

        xmlSetProp(node, "version", ptr);
        xmlFree(ptr);

        xmlXPathFreeObject(xpathObj);
    }

#ifdef CFG_CARDLIST_ENABLE
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/cardlist", xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
        int ver = CardListGetVersion();
        char buf[32];

        sprintf(buf, "%d", ver);
        xmlSetProp(node, "version", buf);

        xmlXPathFreeObject(xpathObj);
    }
#endif // CFG_CARDLIST_ENABLE

    xmlDocDumpFormatMemory(doc, &xmlbuff, size, 1);

end:
    if (xpathCtx)
        xmlXPathFreeContext(xpathCtx);

    if (doc)
        xmlFreeDoc(doc);

    return xmlbuff;
}

void UpgradeSetUrl(char* url)
{
    if (url)
    {
        strncpy(upgradeUrl, url, URL_LEN - 1);
        upgradeUrl[URL_LEN - 1] = '\0';
    }
    else
        upgradeUrl[0] = '\0';
}

void UpgradeSetStream(void* stream)
{
    upgradeIsFinished = false;
    upgradeResult = 0;
    upgradeStream = stream;
}

static void* UpgradeProcessTask(void* arg)
{
    int ret, code = (int) arg;

    if(code == QUIT_UPGRADE_ADDRESSBOOK) {
        /*remove old addressbook#.ucl*/
        RemoveAddressbook();
    }

    ret = UpgradePackage();
    if (ret)
        goto end;

    UpgradeFilesCrc();

    if (code == QUIT_UPGRADE_ADDRESSBOOK)
    {
        AddressBookInit();
        DeviceInfoInit();
    }
#ifdef CFG_CARDLIST_ENABLE
    else if (code == QUIT_UPGRADE_CARDLIST)
    {
        CardListReload();
    }
#endif // CFG_CARDLIST_ENABLE
end:

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif

    upgradeIsFinished = true;
    return NULL;
}

int UpgradeProcess(int code)
{
    int ret = 0;

    if (code == QUIT_RESET_FACTORY)
    {
        UpgradeLcdConsoleEnable();
        ret = UpgradeResetFactory();
    }
    else if (code == QUIT_RESET_TEMP_DRIVE)
    {
        UpgradeLcdConsoleEnable();
        ret = UpgradeResetTempDrive();
    }
    else if (code == QUIT_UPGRADE_FIRMWARE)
    {
		if(strstr(upgradeUrl, "address")) {
            /*remove old addressbook#.ucl*/
            RemoveAddressbook();
        }
        UpgradeLcdConsoleEnable();
        ret = UpgradePackage();
    }
    else if (code == QUIT_UPGRADE_RESOURCE)
    {
        UpgradeLcdConsoleEnable();
        ret = UpgradePackage();
        if (ret == 0)
            UpgradeFilesCrc();
    }
    else if (code == QUIT_UPGRADE_ADDRESSBOOK || code == QUIT_UPGRADE_CARDLIST)
    {
        pthread_t task;
        pthread_attr_t attr;
        struct sched_param param;

        upgradeIsFinished = false;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        param.sched_priority = sched_get_priority_min(0) + 1;
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&task, &attr, UpgradeProcessTask, (void*)code);
    }
    else if (code == QUIT_UPGRADE_WEB)
    {
    #ifdef CFG_WATCHDOG_ENABLE
        ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_DISABLE, NULL);
    #endif
        upgradeResult = ugUpgradePackage(upgradeStream);
        upgradeIsFinished = true;

    #ifdef CFG_WATCHDOG_ENABLE
        ioctl(ITP_DEVICE_WATCHDOG, ITP_IOCTL_ENABLE, NULL);
    #endif

        for (;;)
            sleep(UINT_MAX);
    }

    if (code == QUIT_RESET_FACTORY || code == QUIT_RESET_TEMP_DRIVE || code == QUIT_UPGRADE_FIRMWARE || code == QUIT_UPGRADE_RESOURCE || code == QUIT_RESET_NETWORK)
    {
    #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
    #endif
    }
    return ret;
}

bool UpgradeIsFinished(void)
{
    return upgradeIsFinished;
}

int UpgradeGetResult(void)
{
    return upgradeResult;
}
