#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "curl/curl.h"
#include "ite/itp.h"
#include "doorbell.h"
#include "thread_helper.h"

#define MAX_FILE_SIZE 6000000

static char qrCodeUrl[128];
static bool qrCodeTasking;

typedef struct
{
    uint8_t* memory;
    size_t size;
} Memory;

static size_t WriteData(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    Memory* mem = (Memory*)data;

    if(mem->size >=MAX_FILE_SIZE)
    {
        goto end;        
    }
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
end:
    mem->size += realsize;
    return realsize;
}

void QRCodeInit(void)
{
    qrCodeUrl[0] = '\0';
    qrCodeTasking = false;
}

void QRCodeExit(void)
{
}

static void QRCodeInitUrl(void)
{
    strcpy(qrCodeUrl, "http://");
    strcat(qrCodeUrl, theCenterInfo.ip);
    strcat(qrCodeUrl, "/doorbell/mobile_qrcode?ro=");
    strcat(qrCodeUrl, theDeviceInfo.area);
    strcat(qrCodeUrl, "-");
    strcat(qrCodeUrl, theDeviceInfo.building);
    strcat(qrCodeUrl, "-");
    strcat(qrCodeUrl, theDeviceInfo.unit);
    strcat(qrCodeUrl, "-");
    strcat(qrCodeUrl, theDeviceInfo.floor);
    strcat(qrCodeUrl, "-");
    strcat(qrCodeUrl, theDeviceInfo.room);
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(qrCodeUrl, "-");
        strcat(qrCodeUrl, theDeviceInfo.ext);
    }
    strcat(qrCodeUrl, "&user=");
}

static void* QRCodeRegisterTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    QRCodeRegisterCallback func;
    Memory mem;
    uint32_t* args = (uint32_t*)arg;
    char* user = NULL;
    char url[256];

    qrCodeTasking = true;

    user = (char*)args[0];
    func = (QRCodeRegisterCallback)args[1];

    strcpy(url, qrCodeUrl);
    strcat(url, user);

	printf("QRCodeRegisterTask %s \n",url);

    mem.memory = malloc(1);
    if (!mem.memory)
        goto end;

    mem.size = 0;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }

    func(mem.memory, mem.size);

end:
    free(user);

    if (curl)
        curl_easy_cleanup(curl);

    qrCodeTasking = false;

    return NULL;
}

void QRCodeRegister(char* user, QRCodeRegisterCallback func)
{
    static uint32_t args[2];

//    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || qrCodeTasking)
//        return;

    if (qrCodeUrl[0] == '\0')
        QRCodeInitUrl();

    args[0] = (uint32_t)strdup(user);
    args[1] = (uint32_t)func;

    CreateWorkerThread(QRCodeRegisterTask, args);
}
