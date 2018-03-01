#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include "thread_helper.h"
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "doorbell.h"

#define MAX_MESSAGE_COUNT   999
#define MAX_FILE_SIZE 6000000

static char msgTextListUrl[128];
static char msgTextDeleteUrl[128];
static bool msgTextListTasking;
static bool msgTextDeleteTasking;
static bool msgTextContentTasking;
static char msgVideoListUrl[128];
static char msgVideoDeleteUrl[128];
static bool msgVideoListTasking;
static bool msgVideoDeleteTasking;

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

void MessageInit(void)
{
    msgTextListUrl[0] = '\0';
    msgTextListTasking = false;
    msgTextDeleteTasking = false;
    msgTextContentTasking = false;

    msgVideoListUrl[0] = '\0';
    msgVideoListTasking = false;
    msgVideoDeleteTasking = false;
}

void MessageExit(void)
{
}

static void MessageInitUrl(void)
{
    // get text list
    strcpy(msgTextListUrl, "http://");
    strcat(msgTextListUrl, theCenterInfo.ip);
    strcat(msgTextListUrl, "/doorbell/message_text_list?ro=");
    strcat(msgTextListUrl, theDeviceInfo.area);
    strcat(msgTextListUrl, "-");
    strcat(msgTextListUrl, theDeviceInfo.building);
    strcat(msgTextListUrl, "-");
    strcat(msgTextListUrl, theDeviceInfo.unit);
    strcat(msgTextListUrl, "-");
    strcat(msgTextListUrl, theDeviceInfo.floor);
    strcat(msgTextListUrl, "-");
    strcat(msgTextListUrl, theDeviceInfo.room);
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(msgTextListUrl, "-");
        strcat(msgTextListUrl, theDeviceInfo.ext);
    }

    // delete text
    strcpy(msgTextDeleteUrl, "http://");
    strcat(msgTextDeleteUrl, theCenterInfo.ip);
    strcat(msgTextDeleteUrl, "/doorbell/message_text_delete?ro=");
    strcat(msgTextDeleteUrl, theDeviceInfo.area);
    strcat(msgTextDeleteUrl, "-");
    strcat(msgTextDeleteUrl, theDeviceInfo.building);
    strcat(msgTextDeleteUrl, "-");
    strcat(msgTextDeleteUrl, theDeviceInfo.unit);
    strcat(msgTextDeleteUrl, "-");
    strcat(msgTextDeleteUrl, theDeviceInfo.floor);
    strcat(msgTextDeleteUrl, "-");
    strcat(msgTextDeleteUrl, theDeviceInfo.room);
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(msgTextDeleteUrl, "-");
        strcat(msgTextDeleteUrl, theDeviceInfo.ext);
    }
    strcat(msgTextDeleteUrl, "&id=%d");

    // get video list
    strcpy(msgVideoListUrl, "http://");
    strcat(msgVideoListUrl, theCenterInfo.ip);
    strcat(msgVideoListUrl, "/doorbell/message_video_list?ro=");
    strcat(msgVideoListUrl, theDeviceInfo.area);
    strcat(msgVideoListUrl, "-");
    strcat(msgVideoListUrl, theDeviceInfo.building);
    strcat(msgVideoListUrl, "-");
    strcat(msgVideoListUrl, theDeviceInfo.unit);
    strcat(msgVideoListUrl, "-");
    strcat(msgVideoListUrl, theDeviceInfo.floor);
    strcat(msgVideoListUrl, "-");
    strcat(msgVideoListUrl, theDeviceInfo.room);
    
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(msgVideoListUrl, "-");
        strcat(msgVideoListUrl, theDeviceInfo.ext);
    }

    // delete video
    strcpy(msgVideoDeleteUrl, "http://");
    strcat(msgVideoDeleteUrl, theCenterInfo.ip);
    strcat(msgVideoDeleteUrl, "/doorbell/message_video_delete?ro=");
    strcat(msgVideoDeleteUrl, theDeviceInfo.area);
    strcat(msgVideoDeleteUrl, "-");
    strcat(msgVideoDeleteUrl, theDeviceInfo.building);
    strcat(msgVideoDeleteUrl, "-");
    strcat(msgVideoDeleteUrl, theDeviceInfo.unit);
    strcat(msgVideoDeleteUrl, "-");
    strcat(msgVideoDeleteUrl, theDeviceInfo.floor);
    strcat(msgVideoDeleteUrl, "-");
    strcat(msgVideoDeleteUrl, theDeviceInfo.room);
    if (theDeviceInfo.ext[0] != '\0')
    {
        strcat(msgVideoDeleteUrl, "-");
        strcat(msgVideoDeleteUrl, theDeviceInfo.ext);
    }
    strcat(msgVideoDeleteUrl, "&id=%d");
}

static void* MessageTextListGetTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    MessageListGetCallback func = (MessageListGetCallback)arg;
    Memory mem;
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    Message* messages = NULL;
    int i, len = 0;

    msgTextListTasking = true;

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

    printf("url: %s\n", msgTextListUrl);
    curl_easy_setopt(curl, CURLOPT_URL, msgTextListUrl);
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

    doc = xmlParseMemory(mem.memory, mem.size);
    if (!doc)
    {
        printf("xmlParseMemory() fail\n");
        goto end;
    }

    xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx)
    {
        printf("xmlXPathNewContext() fail\n");
        goto end;
    }

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/MessageTextList/message", xpathCtx);
    if (!xpathObj)
    {
        printf("xmlXPathEvalExpression() fail\n");
        goto end;
    }

    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        len = xpathObj->nodesetval->nodeNr;
        if (len == 0)
            goto end;

        messages = malloc(sizeof (Message) * len);
        if (!messages)
            goto end;

        for (i = 0; i < len && i < MAX_MESSAGE_COUNT; i++)
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[i];
            xmlNode* child = node->children;
            Message* msg = &messages[i];

            while (child != NULL)
            {
                if (strcmp(child->name, "id") == 0)
                {
                    xmlChar* str = xmlNodeListGetString(doc, child->children, 1);
                    msg->id = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(child->name, "topic") == 0)
                {
                    msg->topic = xmlNodeListGetString(doc, child->children, 1);
                }
                else if (strcmp(child->name, "time") == 0)
                {
                    msg->time = xmlNodeListGetString(doc, child->children, 1);
                }
                else if (strcmp(child->name, "image") == 0)
                {
                    msg->image = xmlNodeListGetString(doc, child->children, 1);
                }
                else if (strcmp(child->name, "read") == 0)
                {
                    xmlChar* str = xmlNodeListGetString(doc, child->children, 1);
                    msg->read = strcmp(str, "true") == 0 ? 1 : 0;
                    xmlFree(str);
                }
                child = child->next;
            }
        }

        func(messages, len);
    }
    else
    {
        func(NULL, 0);
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

    msgTextListTasking = false;

    return NULL;
}

void MessageTextListGet(MessageListGetCallback func)
{
    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || msgTextListTasking)
        return;

    if (msgTextListUrl[0] == '\0')
        MessageInitUrl();

    CreateWorkerThread(MessageTextListGetTask, func);
}

static void* MessageTextDeleteTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    MessageNotifyCallback func;
    int id;
    uint32_t* args = (uint32_t*)arg;
    char url[128];

    msgTextDeleteTasking = true;

    id = (int)args[0];
    func = (MessageNotifyCallback)args[1];

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, msgTextDeleteUrl, id);
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

    func();

end:
    if (curl)
        curl_easy_cleanup(curl);

    msgTextDeleteTasking = false;

    return NULL;
}

void MessageTextDelete(int id, MessageNotifyCallback func)
{
    static uint32_t args[2];

    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || msgTextDeleteTasking)
        return;

    if (msgTextDeleteUrl[0] == '\0')
        MessageInitUrl();

    args[0] = (uint32_t)id;
    args[1] = (uint32_t)func;

    CreateWorkerThread(MessageTextDeleteTask, args);
}

static void* MessageTextContentGetTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    MessageTextContentGetCallback func;
    Memory mem;
    uint32_t* args = (uint32_t*)arg;
    xmlChar* imageUrl = NULL;

    msgTextContentTasking = true;

    imageUrl = (xmlChar*)args[0];
    func = (MessageTextContentGetCallback)args[1];

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

    curl_easy_setopt(curl, CURLOPT_URL, imageUrl);
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
    free(imageUrl);

    if (curl)
        curl_easy_cleanup(curl);

    msgTextContentTasking = false;

    return NULL;
}

void MessageTextContentGet(char* image, MessageTextContentGetCallback func)
{
    static uint32_t args[2];

    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || msgTextContentTasking)
        return;

    args[0] = (uint32_t)strdup(image);
    args[1] = (uint32_t)func;

    CreateWorkerThread(MessageTextContentGetTask, args);
}

static void* MessageVideoListGetTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    MessageListGetCallback func = (MessageListGetCallback)arg;
    Memory mem;
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    Message* messages = NULL;
    int i, len = 0, retry = 3;

    msgVideoListTasking = true;

    mem.memory = malloc(1);
    if (!mem.memory)
        goto end;

    while (retry-- >= 0)
    {
        mem.size = 0;

        curl = curl_easy_init();
        if (!curl)
        {
            printf("curl_easy_init() fail.\n");
            goto end;
        }

        printf("url: %s\n", msgVideoListUrl);
        curl_easy_setopt(curl, CURLOPT_URL, msgVideoListUrl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    #ifndef NDEBUG
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    #endif

        res = curl_easy_perform(curl);
        if (CURLE_OK != res)
        {
            printf("curl_easy_perform() fail: %d\n", res);
            curl_easy_cleanup(curl);
            curl = NULL;
        }
        else
            break;
    }
    if (retry <= 0)
        goto end;

    doc = xmlParseMemory(mem.memory, mem.size);
    if (!doc)
    {
        printf("xmlParseMemory() fail\n");
        goto end;
    }

    xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx)
    {
        printf("xmlXPathNewContext() fail\n");
        goto end;
    }

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/MessageVideoList/message", xpathCtx);
    if (!xpathObj)
    {
        printf("xmlXPathEvalExpression() fail\n");
        goto end;
    }

    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        len = xpathObj->nodesetval->nodeNr;
        if (len == 0)
            goto end;

        messages = calloc(1, sizeof (Message) * len);
        if (!messages)
            goto end;

        for (i = 0; i < len && i < MAX_MESSAGE_COUNT; i++)
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[i];
            xmlNode* child = node->children;
            Message* msg = &messages[i];

            while (child != NULL)
            {
                if (strcmp(child->name, "id") == 0)
                {
                    xmlChar* str = xmlNodeListGetString(doc, child->children, 1);
                    msg->id = atoi(str);
                    xmlFree(str);
                }
                else if (strcmp(child->name, "topic") == 0)
                {
                    msg->topic = xmlNodeListGetString(doc, child->children, 1);
                }
                else if (strcmp(child->name, "time") == 0)
                {
                    msg->time = xmlNodeListGetString(doc, child->children, 1);
                }
                else if (strcmp(child->name, "read") == 0)
                {
                    xmlChar* str = xmlNodeListGetString(doc, child->children, 1);
                    msg->read = strcmp(str, "true") == 0 ? 1 : 0;
                    xmlFree(str);
                }
                child = child->next;
            }
        }
        func(messages, len);
    }
    else
    {
        func(NULL, 0);
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

    msgVideoListTasking = false;

    return NULL;
}

void MessageVideoListGet(MessageListGetCallback func)
{
    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || msgVideoListTasking)
        return;

    if (msgVideoListUrl[0] == '\0')
        MessageInitUrl();

    CreateWorkerThread(MessageVideoListGetTask, func);
}

static void* MessageVideoDeleteTask(void* arg)
{
    CURL* curl = NULL;
    CURLcode res = CURLE_OK;
    MessageNotifyCallback func;
    int id;
    uint32_t* args = (uint32_t*)arg;
    char url[128];

    msgVideoDeleteTasking = true;

    id = (int)args[0];
    func = (MessageNotifyCallback)args[1];

    curl = curl_easy_init();
    if (!curl)
    {
        printf("curl_easy_init() fail.\n");
        goto end;
    }

    sprintf(url, msgVideoDeleteUrl, id);
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

    func();

end:
    if (curl)
        curl_easy_cleanup(curl);

    msgVideoDeleteTasking = false;

    return NULL;
}

void MessageVideoDelete(int id, MessageNotifyCallback func)
{
    static uint32_t args[2];

    if (theCenterInfo.ip[0] == '\0' || !NetworkServerIsReady() || msgVideoDeleteTasking)
        return;

    if (msgVideoDeleteUrl[0] == '\0')
        MessageInitUrl();

    args[0] = (uint32_t)id;
    args[1] = (uint32_t)func;

    CreateWorkerThread(MessageVideoDeleteTask, args);
}
