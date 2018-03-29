#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "linphone/linphone_castor3.h"
#include "doorbell.h"
#include "scene.h"

#define BUFSIZE 8

ServerInfo theOutdoorServerInfo;

static ServerInfo serverInfo;
static char* remoteOpenDoorIP;
static sem_t remoteOpenDoorSem;
static pthread_t remoteOpenDoorTask;
static bool remoteCallElevatorTasking;
static bool remoteGetWeatherTasking;
static bool remoteSetDontDisturbConfigTasking;
static bool remoteGetDontDisturbConfigTasking;
static bool remoteGetOutdoorInfoTasking;
static bool remoteSetExtCallingStateTasking;
static bool remoteQuit;

static void* RemoteOpenDoorTask(void* arg);

static void GetOutdoorInfoCallback(ServerInfo* info)
{
    memcpy(&theOutdoorServerInfo ,info, sizeof(ServerInfo));
}

void RemoteInit(void)
{
    pthread_attr_t attr;
    struct sched_param param;

    remoteOpenDoorIP = NULL;
    sem_init(&remoteOpenDoorSem, 0, 0);

    pthread_attr_init(&attr);
    param.sched_priority = 2;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&remoteOpenDoorTask, &attr, RemoteOpenDoorTask, NULL);

    remoteCallElevatorTasking = false;
    remoteGetWeatherTasking = false;
    remoteSetDontDisturbConfigTasking = false;
    remoteGetDontDisturbConfigTasking = false;
    remoteGetOutdoorInfoTasking = false;
    remoteSetExtCallingStateTasking = false;
    remoteQuit = false;

    memset(&theOutdoorServerInfo, 0, sizeof(ServerInfo));
    if (theOutdoorInfo.ip[0])
        RemoteGetOutdoorInfo(theOutdoorInfo.ip, GetOutdoorInfoCallback);
}

void RemoteExit(void)
{
    remoteQuit = true;
    sem_post(&remoteOpenDoorSem);

    pthread_join(remoteOpenDoorTask, NULL);
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
    char url[128];

    for (;;)
    {
        int retry = 2;

        sem_wait(&remoteOpenDoorSem);

        if (remoteQuit)
            break;

        if (!remoteOpenDoorIP)
            continue;

        curl = curl_easy_init();
        if (!curl)
        {
            printf("curl_easy_init() fail.\n");
            goto end;
        }

        sprintf(url, "http://%s:%d/open_door?ro=%s-%s-%s-%s-%s-%s", remoteOpenDoorIP, CFG_WEBSERVER_PORT,
            theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

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

        remoteOpenDoorIP = NULL;

        if (remoteQuit)
            break;
    }
    return NULL;
}

void RemoteOpenDoor(char* ip)
{
    if (!NetworkIsReady() || remoteOpenDoorIP)
        return;

    remoteOpenDoorIP = ip;
    sem_post(&remoteOpenDoorSem);
}

bool RemoteCheckCard(char* cardnum)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    Buffer buf;
    char url[128];
    bool result = false;

    if (!cardnum || !NetworkServerIsReady() || theCenterInfo.ip[0] == '\0')
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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
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

static void* RemoteInvokeCallElevatorTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char url[128];
    bool up = (bool) arg;

    remoteCallElevatorTasking = true;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, "http://%s:%s/call_elevator?ro=%s-%s-%s-%s-%s-%s&dir=%s",
        theEntranceInfo.ip, 
        CFG_WEBSERVER_PORT_STR, 
        theDeviceInfo.area, 
        theDeviceInfo.building, 
        theDeviceInfo.unit, 
        theDeviceInfo.floor, 
        theDeviceInfo.room, 
        theDeviceInfo.ext,
        up ? "up" : "down");

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

    remoteCallElevatorTasking = false;

    return NULL;
}

void RemoteCallElevator(bool up)
{
    if (!NetworkIsReady() || remoteCallElevatorTasking)
        return;

    CreateWorkerThread(RemoteInvokeCallElevatorTask, (void*)up);
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

    if (!NetworkServerIsReady() || theCenterInfo.ip[0] == '\0')
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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
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

int RemoteSetOutdoorSetting(char* ip, char* area, char* building, char* unit, char* floor, char* room, char* ext)
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

static void* RemoteGetWeatherTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    RemoteGetWeatherCallback func;
    Memory mem;
    char url[128];

    mem.memory = NULL;

    if (!NetworkServerIsReady() || theCenterInfo.ip[0] == '\0')
        goto end;

    remoteGetWeatherTasking = true;

    func = (RemoteGetWeatherCallback)arg;

    mem.memory = malloc(1);
    if (!mem.memory)
        goto end;

    mem.size = 0;

    sprintf(url,
        "http://%s/doorbell/weather",
        theCenterInfo.ip);

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
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

    func((char*)mem.memory, mem.size);

end:
    if (curl)
        curl_easy_cleanup(curl);

    free(mem.memory);

    remoteGetWeatherTasking = false;

    return NULL;
}

void RemoteGetWeather(RemoteGetWeatherCallback func)
{
    if (!NetworkServerIsReady() || remoteGetWeatherTasking)
        return;

    CreateWorkerThread(RemoteGetWeatherTask, func);
}

static void* RemoteSetDontDisturbConfigTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    int len, i;
    DeviceInfo* infos;
    char url[128];

    remoteSetDontDisturbConfigTasking = true;

    len = (int)((uint32_t*)arg)[0];
    infos = (DeviceInfo*)((uint32_t*)arg)[1];

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    for (i = 0; i < len; i++)
    {
        if (strcmp(infos[i].ext, "01") == 0)
            continue;

        if (theConfig.do_not_disturb_mode == DONOT_DISTURB_TIME)
        {
            sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/do_not_disturb?mode=%d&start_hour=%d&start_min=%d&end_hour=%d&end_min=%d",
                infos[i].ip, 
                theConfig.do_not_disturb_mode, 
                theConfig.do_not_disturb_start_hour, 
                theConfig.do_not_disturb_start_min, 
                theConfig.do_not_disturb_end_hour, 
                theConfig.do_not_disturb_end_min);
        }
        else
        {
            sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/do_not_disturb?mode=%d", infos[i].ip, theConfig.do_not_disturb_mode);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            printf("curl_easy_perform() fail: %d\n", res);
            continue;
        }
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    remoteSetDontDisturbConfigTasking = false;

    return NULL;
}

void RemoteSetDontDisturbConfig(void)
{
    static uint32_t args[2];
    static DeviceInfo infos[LINPHONE_CASTOR3_MAX_IP_COUNT];
    int len;

    if (!NetworkIsReady() /* || remoteSetDontDisturbConfigTasking */)
        return;

    // set do not disturb config to ext. except 01
    len = AddressBookGetDeviceInfoArray(infos, LINPHONE_CASTOR3_MAX_IP_COUNT, theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room);
    if (len > 0)
    {
        args[0] = len;
        args[1] = (uint32_t)infos;

        CreateWorkerThread(RemoteSetDontDisturbConfigTask, args);
    }
}

static void* RemoteGetDontDisturbConfigTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char* ip;
    Memory mem;
    char url[128];
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;

    remoteGetDontDisturbConfigTasking = true;

    ip = (char*)arg;

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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
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

    // do_not_disturb
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/DeviceInfo/do_not_disturb", xpathCtx);
    if (xpathObj)
    {
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlAttr* attribute = node->properties;
            xmlChar* str;

            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "mode") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    theConfig.do_not_disturb_mode = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "start_hour") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    theConfig.do_not_disturb_start_hour = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "start_min") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    theConfig.do_not_disturb_start_min = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "end_hour") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    theConfig.do_not_disturb_end_hour = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "end_min") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    theConfig.do_not_disturb_end_min = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
            SceneUpdateIndicators();
        }
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    if (xpathObj)
        xmlXPathFreeObject(xpathObj);

    if (xpathCtx)
        xmlXPathFreeContext(xpathCtx);

    if (doc)
        xmlFreeDoc(doc);

    free(mem.memory);
    free(ip);

    remoteGetDontDisturbConfigTasking = false;

    return NULL;
}

void RemoteGetDontDisturbConfig(void)
{
    char* ip;

    if (!NetworkIsReady() || remoteGetDontDisturbConfigTasking)
        return;

    // get do not disturb config from ext 01
    ip = AddressBookGetDeviceIP(theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, "01");
    if (ip)
    {
        CreateWorkerThread(RemoteGetDontDisturbConfigTask, ip);
    }
}

static void* RemoteGetOutdoorInfoTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    RemoteGetOutdoorInfoCallback func;
    Memory mem;
    char* ip;
    char url[128];
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    ServerInfo outdoorInfo;

    remoteGetOutdoorInfoTasking = true;

    ip = (char*)((uint32_t*)arg)[0];
    func = (RemoteGetOutdoorInfoCallback)((uint32_t*)arg)[1];

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

    memset(&outdoorInfo, 0, sizeof (ServerInfo));

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
                    outdoorInfo.addressBookVersion = atoi(str);
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
                    strcpy(outdoorInfo.firmwareVersion, str);
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
                    outdoorInfo.ringLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "play_lev") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    outdoorInfo.playLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "rec_lev") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    outdoorInfo.recLevel = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "dial_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    outdoorInfo.dialTime = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "voicememo_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    outdoorInfo.voiceMemoTime = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(attribute->name, "calling_time") == 0)
                {
                    str = xmlNodeListGetString(node->doc, attribute->children, 1);
                    outdoorInfo.callingTime = atoi(str);
                    xmlFree(str);
                }
                attribute = attribute->next;
            }
        }
    }

    func(&outdoorInfo);

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

    remoteGetOutdoorInfoTasking = false;

    return NULL;
}

void RemoteGetOutdoorInfo(char* ip, RemoteGetOutdoorInfoCallback func)
{
    static uint32_t args[2];

    if (!NetworkIsReady() || remoteGetOutdoorInfoTasking)
        return;

    args[0] = (uint32_t)ip;
    args[1] = (uint32_t)func;

    CreateWorkerThread(RemoteGetOutdoorInfoTask, args);
}

static void* RemoteSetOutdoorConfigTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    char url[256];
    char* ip;
    int ringLevel, playLevel, recLevel, dialTime, voiceMemoTime, callingTime;
    int retry = 2;

    ip = (char*)((uint32_t*)arg)[0];
    ringLevel = (int)((uint32_t*)arg)[1];
    playLevel = (int)((uint32_t*)arg)[2];
    recLevel = (int)((uint32_t*)arg)[3];
    dialTime = (int)((uint32_t*)arg)[4];
    voiceMemoTime = (int)((uint32_t*)arg)[5];
    callingTime = (int)((uint32_t*)arg)[6];

    if (remoteQuit)
        goto end;

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/dev/info.cgi?action=setting&ring_lev=%d&play_lev=%d&rec_lev=%d&dial_time=%d&voicememo_time=%d&calling_time=%d",
        ip, ringLevel, playLevel, recLevel, dialTime, voiceMemoTime, callingTime);

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

void RemoteSetOutdoorConfig(char* ip, int ringLevel, int playLevel, int recLevel, int dialTime, int voiceMemoTime, int callingTime)
{
    static uint32_t args[7];

    if (!NetworkIsReady())
        return;

    args[0] = (uint32_t)ip;
    args[1] = (uint32_t)ringLevel;
    args[2] = (uint32_t)playLevel;
    args[3] = (uint32_t)recLevel;
    args[4] = (uint32_t)dialTime;
    args[5] = (uint32_t)voiceMemoTime;
    args[6] = (uint32_t)callingTime;

    CreateWorkerThread(RemoteSetOutdoorConfigTask, args);
}

static void* RemoteSetExtCallingStateTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    int len, i, ext_calling;
    DeviceInfo* infos;
    char url[128];

    remoteSetExtCallingStateTasking = true;

    len = (int)((uint32_t*)arg)[0];
    infos = (DeviceInfo*)((uint32_t*)arg)[1];
    ext_calling = (int)((uint32_t*)arg)[2];

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    for (i = 0; i < len; i++)
    {
        if (strcmp(infos[i].ext, theDeviceInfo.ext) == 0)
            continue;

        printf("notify ext %s\n", infos[i].ext);
        sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/notify_ext_calling?mode=%d", infos[i].ip, ext_calling);

        curl_easy_setopt(curl, CURLOPT_URL, url);

        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            printf("curl_easy_perform() fail: %d\n", res);
            continue;
        }
    }

end:
    if (curl)
        curl_easy_cleanup(curl);

    remoteSetExtCallingStateTasking = false;

    return NULL;
}

void RemoteSetExtCallingState(int ext_calling)
{
    static uint32_t args[3];
    static DeviceInfo infos[LINPHONE_CASTOR3_MAX_IP_COUNT];
    int len;

    if (!NetworkIsReady())
        return;

    //notify other ext, one indoor still on calling, do not receive call.
    len = AddressBookGetDeviceInfoArray(infos, LINPHONE_CASTOR3_MAX_IP_COUNT, theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room);
    if (len > 0)
    {

        args[0] = len;
        args[1] = (uint32_t)infos;
        args[2] = ext_calling;

		CreateWorkerThread(RemoteSetExtCallingStateTask, args);
    }
}
