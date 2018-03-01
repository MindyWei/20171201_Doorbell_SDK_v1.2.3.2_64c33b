#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "doorbell.h"

#define BUFSIZE 8

void RemoteInit(void)
{
    // DO NOTHING
}

typedef struct
{
    uint8_t buf[BUFSIZE];
    size_t size;
} Buffer;

typedef struct
{
    uint8_t* memory;
    size_t size;
} Memory;

static size_t WriteBufferData(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    Buffer* buf = (Buffer*)data;

    if (buf->size + realsize > BUFSIZE)
    {
        printf("out of buffer: %d\n", buf->size + realsize - BUFSIZE);
        realsize = BUFSIZE - buf->size;
    }

    memcpy(&(buf->buf[buf->size]), ptr, realsize);
    buf->size += realsize;
    buf->buf[buf->size] = 0;

    return realsize;
}

static size_t WriteMemoryData(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    Memory* mem = (Memory*)data;

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
bool RemoteCheckCard(char* cardnum)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    Buffer buf;
    char url[128];
    bool result = false;

    if (!cardnum|| !NetworkIsReady() || theCenterInfo.ip[0] == '\0')
        goto end;

    buf.size = 0;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, "http://%s/check_ic?ic=%s&ro=%s-%s-%s-%s-%s-%s", theCenterInfo.ip, cardnum,
        theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteBufferData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }

    if (strcmp(buf.buf, "OK") == 0)
        result = true;

end:
    if (curl)
        curl_easy_cleanup(curl);

    return result;
}
