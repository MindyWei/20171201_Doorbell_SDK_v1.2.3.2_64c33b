#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "linphone/linphone_castor3.h"
#include "doorbell.h"

#define BUFSIZE 8

static ServerInfo serverInfo;
static bool remoteOpenDoorTasking;
static bool remoteGetIntercomInfoTasking;
static bool remoteQuit;

void RemoteInit(void)
{
    remoteOpenDoorTasking = false;
    remoteGetIntercomInfoTasking = false;
    remoteQuit = false;
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

static void* RemoteOpenDoorTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char* ip = (char*)arg;
    char url[128];

    remoteOpenDoorTasking = true;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, "http://%s:%d/open_door?ro=%s-%s-%s-%s-%s-%s", ip, CFG_WEBSERVER_PORT,
        theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    remoteOpenDoorTasking = false;

    return NULL;
}

void RemoteOpenDoor(char* ip)
{
    pthread_t task;
    pthread_attr_t attr;
    struct sched_param param;

    if (!NetworkIsReady() || remoteOpenDoorTasking)
        return;

    pthread_attr_init(&attr);
    param.sched_priority = 2;
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, RemoteOpenDoorTask, (void*)ip);
}

void RemoteExit(void)
{
    remoteQuit = true;
}

ServerInfo* RemoteGetServerInfo(void)
{
    CURL* curl = NULL;
    CURLcode res = -1;
    char url[128];
    Memory mem;
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;

    mem.memory = NULL;

    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady())
        goto end;

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

    sprintf(url, "http://%s/doorbell/get_info?ro=%s-%s-%s-%s-%s-%s",
        theCenterInfo.ip, theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryData);
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

    memset(&serverInfo, 0, sizeof (ServerInfo));

    // firmware
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/firmware", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.firmwareUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    strcpy(serverInfo.firmwareVersion, str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // addressbook
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/addressbook", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.addressBookUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    serverInfo.addressBookVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // screen saver
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/screensaver", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.screensaverUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    serverInfo.screensaverVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // cardlist
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/cardlist", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.cardListUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    serverInfo.cardListVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // setting
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/setting", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.settingUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    serverInfo.settingVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // advertisement
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/ServerInfo/advertisement", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            str = xmlNodeListGetString(doc, node->children, 1);
            strcpy(serverInfo.advertisementUrl, str);
            xmlFree(str);

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    serverInfo.advertisementVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
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

    if (CURLE_OK != res)
        return NULL;

    return &serverInfo;
}

static void* RemoteGetIntercomInfoTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    RemoteGetIntercomInfoCallback func;
    Memory mem;
    char* ip;
    char url[128];
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    ServerInfo intercomInfo;

    remoteGetIntercomInfoTasking = true;

    ip = (char*)((uint32_t*)arg)[0];
    func = (RemoteGetIntercomInfoCallback)((uint32_t*)arg)[1];

    mem.memory = malloc(1);
    if (!mem.memory)
        goto end;

    mem.size = 0;

    sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/get_info", ip);

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryData);
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

    memset(&intercomInfo, 0, sizeof (ServerInfo));

    // addressbook
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/addressbook", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.addressBookVersion = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // firmware
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/firmware", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "version") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    strcpy(intercomInfo.firmwareVersion, str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    // level
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/config", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "ring_lev") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.ringLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "play_lev") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.playLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "rec_lev") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.recLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "dial_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.dialTime = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "calling_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.callingTime = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "mgsalarm_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    intercomInfo.guardTime = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    func(&intercomInfo);

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

    remoteGetIntercomInfoTasking = false;

    return NULL;
}

void RemoteGetIntercomInfo(char* ip, RemoteGetIntercomInfoCallback func)
{
    pthread_t task;
    pthread_attr_t attr;
    static uint32_t args[2];

    if (!NetworkIsReady() || remoteGetIntercomInfoTasking)
        return;

    args[0] = (uint32_t)ip;
    args[1] = (uint32_t)func;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, RemoteGetIntercomInfoTask, args);
}

int RemoteSetIntercomSetting(char* ip, char* area, char* building, char* unit, char* floor, char* room, char* ext)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char url[256];

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url,
        "http://%s:" CFG_WEBSERVER_PORT_STR "/dev/info.cgi?action=setting&area=%s&building=%s&unit=%s&floor=%s&room=%s&ext=%s",
        ip, area, building, unit, floor, room, ext);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);
    if (CURLE_OK != res)
    {
        printf("curl_easy_perform() fail: %d\n", res);
        goto end;
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    return res;
}

static void* RemoteSetIntercomConfigTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char url[256];
    char* ip;
    int ringLevel, playLevel, recLevel, dialTime, callingTime, guardTime;
    int retry = 2;

    ip = (char*)((uint32_t*)arg)[0];
    ringLevel = (int)((uint32_t*)arg)[1];
    playLevel = (int)((uint32_t*)arg)[2];
    recLevel = (int)((uint32_t*)arg)[3];
    dialTime = (int)((uint32_t*)arg)[4];
    callingTime = (int)((uint32_t*)arg)[5];
    guardTime = (int)((uint32_t*)arg)[6];

    if (remoteQuit)
        goto end;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/dev/info.cgi?action=setting&ring_lev=%d&play_lev=%d&rec_lev=%d&dial_time=%d&calling_time=%d&mgsalarm_time=%d",
        ip, ringLevel, playLevel, recLevel, dialTime, callingTime, guardTime);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    while (--retry > 0)
    {
        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            printf("curl_easy_perform() fail: %d\n", res);
            if (remoteQuit)
                break;
        }
        else
            break;
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    return NULL;
}

void RemoteSetIntercomConfig(char* ip, int ringLevel, int playLevel, int recLevel, int dialTime, int callingTime, int guardTime)
{
    pthread_t task;
    pthread_attr_t attr;
    static uint32_t args[7];

    if (!NetworkIsReady())
        return;

    args[0] = (uint32_t)ip;
    args[1] = (uint32_t)ringLevel;
    args[2] = (uint32_t)playLevel;
    args[3] = (uint32_t)recLevel;
    args[4] = (uint32_t)dialTime;
    args[5] = (uint32_t)callingTime;
    args[6] = (uint32_t)guardTime;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, RemoteSetIntercomConfigTask, args);
}
