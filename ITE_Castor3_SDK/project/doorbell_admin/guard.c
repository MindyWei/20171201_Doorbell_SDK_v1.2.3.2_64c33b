#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "doorbell.h"

#define MAX_GUARD_LOG_COUNT    999
#define GUARD_LOG_PATH CFG_TEMP_DRIVE ":/guardlog.dat"
#define MAX_GUARD_LOG_SYNC_COUNT 99

static pthread_mutex_t guardLogMutex;
static int guardLogCount;
static bool guardLogQuit;
static GuardLog guardLogArray[MAX_GUARD_LOG_COUNT];

static const char* guardLogSyncDocStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?><GuardLogSyncList></GuardLogSyncList>";
static xmlDocPtr guardLogSyncDoc;
static int guardLogSyncItemCount;
static int guardUnhandledLogCount;

static int guardLogIndoorCount;
static char guardLogIndoorIPArray[MAX_GUARD_LOG_SYNC_COUNT][16];

static void GetIndoorIPArray(void)
{
    xmlXPathContext* xpathCtx;

    guardLogIndoorCount = 0;

    xpathCtx = xmlXPathNewContext(guardLogSyncDoc);
    if (xpathCtx)
    {
        xmlXPathObject* xpathObj;

        xpathObj = xmlXPathEvalExpression(BAD_CAST "/GuardLogSyncList/item", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            int i, len;

            len = xpathObj->nodesetval->nodeNr;
            for (i = 0; i < len; i++)
            {
                xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                xmlAttr* attribute = node->properties;
                char* ip;
                char area[4], building[4], unit[4], floor[4], room[4], ext[4];

                area[0] = building[0] = unit[0] = floor[0] = room[0] = ext[0] = '\0';

                while (attribute && attribute->name && attribute->children)
                {
                    xmlChar* value = NULL;

                    if (strcmp(attribute->name, "addr") == 0)
                    {
                        char *token, *saveptr;

                        value = xmlNodeListGetString(node->doc, attribute->children, 1);
                        token = strtok_r(value, "-", &saveptr);
                        if (token)
                        {
                            area[0] = token[0];
                            area[1] = token[1];
                            area[2] = '\0';

                            token = strtok_r(NULL, "-", &saveptr);
                            if (token)
                            {
                                building[0] = token[0];
                                building[1] = token[1];
                                building[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    unit[0] = token[0];
                                    unit[1] = token[1];
                                    unit[2] = '\0';

                                    token = strtok_r(NULL, "-", &saveptr);
                                    if (token)
                                    {
                                        floor[0] = token[0];
                                        floor[1] = token[1];
                                        floor[2] = '\0';

                                        token = strtok_r(NULL, "-", &saveptr);
                                        if (token)
                                        {
                                            room[0] = token[0];
                                            room[1] = token[1];
                                            room[2] = '\0';

                                            token = strtok_r(NULL, "-", &saveptr);
                                            if (token)
                                            {
                                                ext[0] = token[0];
                                                ext[1] = token[1];
                                                ext[2] = '\0';
                                            }
                                        }

                                    }
                                }
                            }
                        }
                    }
                    xmlFree(value);

                    attribute = attribute->next;
                }
                ip = AddressBookGetDeviceIP(area, building, unit, floor, room, ext);
                if (ip)
                {
                    int j;
                    
                    for (j = 0; j < guardLogIndoorCount; j++)
                    {
                        if (strcmp(guardLogIndoorIPArray[j], ip) == 0)
                            break;
                    }
                    if (j == guardLogIndoorCount)
                    {
                        strcpy(guardLogIndoorIPArray[guardLogIndoorCount], ip);
                        guardLogIndoorCount++;
                    }
                    free(ip);

                    if (guardLogIndoorCount >= MAX_GUARD_LOG_SYNC_COUNT)
                        break;
                }
            }
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
    }
}

static void* GuardLogSyncTask(void* arg)
{
    while (!guardLogQuit)
    {
        if (guardLogSyncItemCount > 0 && NetworkIsReady())
        {
            CURL* curl = NULL;
            CURLcode res = CURLE_OK;
            struct curl_httppost *formpost = NULL;
            struct curl_httppost *lastptr = NULL;
            DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
            char url[128];
            xmlChar* xmlbuff = NULL;
            int i, count, buffersize, okCount, itemCount;

            pthread_mutex_lock(&guardLogMutex);

            itemCount = guardLogSyncItemCount;
            GetIndoorIPArray();
            xmlDocDumpFormatMemory(guardLogSyncDoc, &xmlbuff, &buffersize, 1);

            pthread_mutex_unlock(&guardLogMutex);
            //puts(xmlbuff);

            curl_formadd(&formpost, &lastptr,
                CURLFORM_COPYNAME, "guardlog_sync",
                CURLFORM_COPYCONTENTS, xmlbuff,
                CURLFORM_END);

            for (i = 0; i < guardLogIndoorCount; i++)
            {
                curl = curl_easy_init();
                if (curl)
                {
                    sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/doorbell/guardlog_sync", guardLogIndoorIPArray[i]);

                    curl_easy_setopt(curl, CURLOPT_URL, url);
                    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
                    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

                #ifndef NDEBUG
                    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                #endif

                    res = curl_easy_perform(curl);

                    curl_easy_cleanup(curl);
                }
            }

            count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);
            okCount = 0;

            for (i = 0; i < count; i++)
            {
                if (strcmp(managerInfoArray[i].ip, theDeviceInfo.ip) == 0)
                    continue;

                curl = curl_easy_init();
                if (curl)
                {
                    sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/doorbell/guardlog_sync", managerInfoArray[i].ip);

                    curl_easy_setopt(curl, CURLOPT_URL, url);
                    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
                    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

                #ifndef NDEBUG
                    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                #endif

                    res = curl_easy_perform(curl);
                    if (CURLE_OK == res)
                        okCount++;

                    curl_easy_cleanup(curl);
                }
            }
            curl_formfree(formpost);

            if (okCount > 0 || count <= 1)
            {
                xmlXPathContext* xpathCtx;
                xmlXPathObject* xpathObj;

                // clear sent items
                pthread_mutex_lock(&guardLogMutex);
                xpathCtx = xmlXPathNewContext(guardLogSyncDoc);
                if (xpathCtx)
                {
                    xpathObj = xmlXPathEvalExpression(BAD_CAST "/GuardLogSyncList", xpathCtx);
                    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
                    {
                        int count = 0;
                        xmlNodePtr p = xpathObj->nodesetval->nodeTab[0];
                        for (p = p->children; p; )
                        {
                            xmlNodePtr node = p;
                            p = p->next;
                            xmlUnlinkNode(node);
                            xmlFreeNode(node);

                            if (++count >= itemCount)
                                break;
                        }
                    }
                    xmlXPathFreeObject(xpathObj);
                    xmlXPathFreeContext(xpathCtx);
                }
                guardLogSyncItemCount -= itemCount;
                pthread_mutex_unlock(&guardLogMutex);
            }
        }
        sleep(5);
    }

    // quit
    if (guardLogSyncDoc)
    {
        xmlFreeDoc(guardLogSyncDoc);
        guardLogSyncDoc = NULL;
    }
    pthread_mutex_destroy(&guardLogMutex);

    return NULL;
}

static void GuardWriteLog(GuardLog* log)
{
    GuardLog* entry;

    pthread_mutex_lock(&guardLogMutex);

    if (guardLogCount >= MAX_GUARD_LOG_COUNT)
        GuardDeleteLog(guardLogCount - 1);

    entry = &guardLogArray[guardLogCount];

    memcpy(entry, log, sizeof (GuardLog));

    guardLogCount++;
    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);

    if (log->state == GUARD_UNHANDLED)
    {
        if (guardUnhandledLogCount < MAX_GUARD_LOG_COUNT)
            guardUnhandledLogCount++;
    }
    pthread_mutex_unlock(&guardLogMutex);
}

static void GuardReplaceLog(GuardLog* log)
{
    int i;

    pthread_mutex_lock(&guardLogMutex);

    for (i = 0; i < guardLogCount; i++)
    {
        GuardLog* entry = &guardLogArray[i];

        if (entry->timestamp == log->timestamp &&
            entry->ev == log->ev &&
            entry->type == log->type &&
            strcmp(entry->area, log->area) == 0 &&
            strcmp(entry->building, log->building) == 0 &&
            strcmp(entry->unit, log->unit) == 0 &&
            strcmp(entry->floor, log->floor) == 0 &&
            strcmp(entry->room, log->room) == 0 &&
            strcmp(entry->ext, log->ext) == 0)
        {
            if (entry->state == GUARD_UNHANDLED && log->state == GUARD_HANDLED)
            {
                if (guardUnhandledLogCount > 0)
                    guardUnhandledLogCount--;
            }
            else if (entry->state == GUARD_HANDLED && log->state == GUARD_UNHANDLED)
            {
                if (guardUnhandledLogCount < MAX_GUARD_LOG_COUNT)
                    guardUnhandledLogCount++;
            }

            memcpy(entry, log, sizeof (GuardLog));
            break;
        }
    }
    pthread_mutex_unlock(&guardLogMutex);
}

void GuardInit(void)
{
    pthread_t task;
    pthread_attr_t attr;
    pthread_mutexattr_t mattr; 

    guardLogQuit = false;
    guardLogCount = guardLogSyncItemCount = guardUnhandledLogCount = 0;

    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&guardLogMutex, &mattr);

    guardLogSyncDoc = xmlParseMemory(guardLogSyncDocStr, strlen(guardLogSyncDocStr));
    assert(guardLogSyncDoc);

    GuardLoadLog();

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, GuardLogSyncTask, NULL);
}

void GuardExit(void)
{
    guardLogQuit = true;
}

int GuardGetLogCount(void)
{
    return guardLogCount;
}

GuardLog* GuardGetLog(int index)
{
    int i;

    if (guardLogCount == 0 || index >= guardLogCount)
        return NULL;

    pthread_mutex_lock(&guardLogMutex);

    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);
    assert(index < guardLogCount);

    i = guardLogCount - 1 - index;

    pthread_mutex_unlock(&guardLogMutex);

    return &guardLogArray[i];
}

void GuardDeleteLog(int index)
{
    int i, endIndex;

    if (guardLogCount == 0 || index >= guardLogCount)
        return;

    pthread_mutex_lock(&guardLogMutex);

    assert(guardLogCount <= MAX_GUARD_LOG_COUNT);
    assert(index < guardLogCount);

    i = guardLogCount - 1 - index;
    endIndex = guardLogCount - 1;

    assert(i <= endIndex);

    if (guardLogArray[i].state == GUARD_UNHANDLED)
    {
        if (guardUnhandledLogCount > 0)
            guardUnhandledLogCount--;
    }

    memmove(&guardLogArray[i], &guardLogArray[i + 1], sizeof(GuardLog) * (endIndex - i));

    guardLogCount--;
    assert(guardLogCount >= 0);

    pthread_mutex_unlock(&guardLogMutex);
}

void GuardLoadLog(void)
{
    struct stat st;
    FILE* f;
    int i;

    if (stat(CFG_TEMP_DRIVE ":/", &st) != 0)
        return;

    pthread_mutex_lock(&guardLogMutex);

    f = fopen(GUARD_LOG_PATH, "rb");
    if (f)
    {
        guardLogCount = fread(guardLogArray, sizeof(GuardLog), MAX_GUARD_LOG_COUNT, f);
        fclose(f);

        guardUnhandledLogCount = 0;
        for (i = 0; i < guardLogCount; i++)
        {
            if (guardLogArray[i].state == GUARD_UNHANDLED)
                guardUnhandledLogCount++;
        }
    }

    pthread_mutex_unlock(&guardLogMutex);
}

void GuardSaveLog(void)
{
    struct stat st;
    FILE* f;

    if (stat(CFG_TEMP_DRIVE ":/", &st) != 0)
        return;

    pthread_mutex_lock(&guardLogMutex);

    f = fopen(GUARD_LOG_PATH, "wb");
    if (f)
    {
        fwrite(guardLogArray, sizeof(GuardLog), guardLogCount, f);
        fclose(f);
    }

    pthread_mutex_unlock(&guardLogMutex);
}

void GuardParseLog(char* xml, int size)
{
    xmlDocPtr doc;

    pthread_mutex_lock(&guardLogMutex);

    doc = xmlParseMemory(xml, size);
    if (doc)
    {
        xmlXPathContext* xpathCtx = xmlXPathNewContext(doc);
        if (xpathCtx)
        {
            GuardLog log;
            xmlXPathObject* xpathObj;
            
            // warn
            memset(&log, 0, sizeof (GuardLog));

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='warn']/@addr", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                xmlNode* node = xpathObj->nodesetval->nodeTab[0];
                xmlChar* value = xmlNodeListGetString(node->doc, node->children, 1);
                char *saveptr, *token = strtok_r(value, "-", &saveptr);
                if (token)
                {
                    log.area[0] = token[0];
                    log.area[1] = token[1];
                    log.area[2] = '\0';

                    token = strtok_r(NULL, "-", &saveptr);
                    if (token)
                    {
                        log.building[0] = token[0];
                        log.building[1] = token[1];
                        log.building[2] = '\0';

                        token = strtok_r(NULL, "-", &saveptr);
                        if (token)
                        {
                            log.unit[0] = token[0];
                            log.unit[1] = token[1];
                            log.unit[2] = '\0';

                            token = strtok_r(NULL, "-", &saveptr);
                            if (token)
                            {
                                log.floor[0] = token[0];
                                log.floor[1] = token[1];
                                log.floor[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    log.room[0] = token[0];
                                    log.room[1] = token[1];
                                    log.room[2] = '\0';

                                    token = strtok_r(NULL, "-", &saveptr);
                                    if (token)
                                    {
                                        log.ext[0] = token[0];
                                        log.ext[1] = token[1];
                                        log.ext[2] = '\0';
                                    }
                                }

                            }
                        }
                    }
                }
                xmlFree(value);
            }
            xmlXPathFreeObject(xpathObj);

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='warn']/item", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                int i, len;

                len = xpathObj->nodesetval->nodeNr;
                for (i = 0; i < len; i++)
                {
                    xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                    xmlAttr* attribute = node->properties;
                    bool isAlarm = false;

                    log.ev = GUARD_EVENT_WARN;

                    while (attribute && attribute->name && attribute->children)
                    {
                        xmlChar* value = NULL;

                        if (strcmp(attribute->name, "time") == 0)
                        {
                            struct tm tm;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            strptime(value, "%Y-%m-%d %H:%M:%S", &tm);

                            log.timestamp = (unsigned long)mktime(&tm);
                        }
                        else if (strcmp(attribute->name, "type") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            
                            if (strcmp(value, "emergency") == 0)
                                log.type = GUARD_TYPE_EMERGENCY;
                            else if (strcmp(value, "infrared") == 0)
                                log.type = GUARD_TYPE_INFRARED;
                            else if (strcmp(value, "door") == 0)
                                log.type = GUARD_TYPE_DOOR;
                            else if (strcmp(value, "window") == 0)
                                log.type = GUARD_TYPE_WINDOW;
                            else if (strcmp(value, "smoke") == 0)
                                log.type = GUARD_TYPE_SMOKE;
                            else if (strcmp(value, "gas") == 0)
                                log.type = GUARD_TYPE_GAS;
                            else if (strcmp(value, "area") == 0)
                                log.type = GUARD_TYPE_AREA;
                            else if (strcmp(value, "rob") == 0)
                                log.type = GUARD_TYPE_ROB;
                            else if (strcmp(value, "opendoor") == 0)
                                log.type = GUARD_TYPE_OPENDOOR;
                        }
                        else if (strcmp(attribute->name, "action") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            if (strcmp(value, "trig") == 0)
                                isAlarm = true;
                        }
                        xmlFree(value);

                        attribute = attribute->next;
                    }
                    if (isAlarm)
                        GuardWriteLog(&log);
                }
            }
            xmlXPathFreeObject(xpathObj);

            // open-door
            memset(&log, 0, sizeof (GuardLog));

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='open-door']/@addr", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                xmlNode* node = xpathObj->nodesetval->nodeTab[0];
                xmlChar* value = xmlNodeListGetString(node->doc, node->children, 1);
                char *saveptr, *token = strtok_r(value, "-", &saveptr);
                if (token)
                {
                    log.area[0] = token[0];
                    log.area[1] = token[1];
                    log.area[2] = '\0';

                    token = strtok_r(NULL, "-", &saveptr);
                    if (token)
                    {
                        log.building[0] = token[0];
                        log.building[1] = token[1];
                        log.building[2] = '\0';

                        token = strtok_r(NULL, "-", &saveptr);
                        if (token)
                        {
                            log.unit[0] = token[0];
                            log.unit[1] = token[1];
                            log.unit[2] = '\0';

                            token = strtok_r(NULL, "-", &saveptr);
                            if (token)
                            {
                                log.floor[0] = token[0];
                                log.floor[1] = token[1];
                                log.floor[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    log.room[0] = token[0];
                                    log.room[1] = token[1];
                                    log.room[2] = '\0';

                                    token = strtok_r(NULL, "-", &saveptr);
                                    if (token)
                                    {
                                        log.ext[0] = token[0];
                                        log.ext[1] = token[1];
                                        log.ext[2] = '\0';
                                    }
                                }

                            }
                        }
                    }
                }
                xmlFree(value);
            }
            xmlXPathFreeObject(xpathObj);

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='open-door']/item", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                int i, len;

                len = xpathObj->nodesetval->nodeNr;
                for (i = 0; i < len; i++)
                {
                    xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                    xmlAttr* attribute = node->properties;
                    bool isAlarm = false;

                    log.ev = GUARD_EVENT_OPENDOOR;
                    log.type = GUARD_TYPE_OPENDOOR;

                    while (attribute && attribute->name && attribute->children)
                    {
                        xmlChar* value = NULL;

                        if (strcmp(attribute->name, "time") == 0)
                        {
                            struct tm tm;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            strptime(value, "%Y-%m-%d %H:%M:%S", &tm);

                            log.timestamp = (unsigned long)mktime(&tm);
                        }
                        else if (strcmp(attribute->name, "verified") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            if (strcmp(value, "false") == 0)
                                isAlarm = true;
                        }
                        xmlFree(value);

                        attribute = attribute->next;
                    }
                    if (isAlarm)
                        GuardWriteLog(&log);
                }
            }
            xmlXPathFreeObject(xpathObj);

            xmlXPathFreeContext(xpathCtx);
        }
        xmlFreeDoc(doc);
    }
    pthread_mutex_unlock(&guardLogMutex);
}

void GuardSendSyncLog(GuardLog* log)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;

    if (guardLogQuit || guardLogSyncItemCount >= MAX_GUARD_LOG_SYNC_COUNT)
        return;

    pthread_mutex_lock(&guardLogMutex);

    xpathCtx = xmlXPathNewContext(guardLogSyncDoc);
    if (xpathCtx)
    {
        char buf[256], buf2[32], buf3[16], buf4[16];
        struct tm* timeinfo;

        timeinfo = localtime((const time_t*)&log->timestamp);
        strftime(buf2, sizeof(buf2), "%Y-%m-%d %H:%M:%S", timeinfo);

        // event
        switch (log->ev)
        {
        case GUARD_EVENT_WARN:
            strcpy(buf3, "warn");
            break;

        case GUARD_EVENT_OPENDOOR:
            strcpy(buf3, "open-door");
            break;
        }

        // type
        switch (log->type)
        {
        case GUARD_TYPE_EMERGENCY:
            strcpy(buf4, "emergency");
            break;

        case GUARD_TYPE_INFRARED:
            strcpy(buf4, "infrared");
            break;

        case GUARD_TYPE_DOOR:
            strcpy(buf4, "door");
            break;

        case GUARD_TYPE_WINDOW:
            strcpy(buf4, "window");
            break;

        case GUARD_TYPE_SMOKE:
            strcpy(buf4, "smoke");
            break;

        case GUARD_TYPE_GAS:
            strcpy(buf4, "gas");
            break;

        case GUARD_TYPE_AREA:
            strcpy(buf4, "area");
            break;

        case GUARD_TYPE_ROB:
            strcpy(buf4, "rob");
            break;

        case GUARD_TYPE_OPENDOOR:
            strcpy(buf4, "opendoor");
            break;
        }

        sprintf(buf, "/GuardLogSyncList/item[@addr='%s-%s-%s-%s-%s-%s' and @time='%s' and @event='%s' and type='%s']",
            log->area, log->building, log->unit, log->floor, log->room, log->ext,
            buf2, buf3, buf4);

        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlXPathFreeObject(xpathObj);
            xpathObj = xmlXPathEvalExpression(BAD_CAST "/GuardLogSyncList", xpathCtx);
        }

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "item", NULL);

            // addr
            sprintf(buf, "%s-%s-%s-%s-%s-%s", log->area, log->building, log->unit, log->floor, log->room, log->ext);
            xmlSetProp(node, "addr", buf);

            // time
            timeinfo = localtime((const time_t*)&log->timestamp);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
            xmlNewProp(node, BAD_CAST "time", BAD_CAST buf);

            // event
            switch (log->ev)
            {
            case GUARD_EVENT_WARN:
                strcpy(buf, "warn");
                break;

            case GUARD_EVENT_OPENDOOR:
                strcpy(buf, "open-door");
                break;
            }
            xmlNewProp(node, BAD_CAST "event", BAD_CAST buf);

            // state
            sprintf(buf, "%d", log->state);
            xmlNewProp(node, BAD_CAST "state", BAD_CAST buf);

            // type
            switch (log->type)
            {
            case GUARD_TYPE_EMERGENCY:
                strcpy(buf, "emergency");
                break;

            case GUARD_TYPE_INFRARED:
                strcpy(buf, "infrared");
                break;

            case GUARD_TYPE_DOOR:
                strcpy(buf, "door");
                break;

            case GUARD_TYPE_WINDOW:
                strcpy(buf, "window");
                break;

            case GUARD_TYPE_SMOKE:
                strcpy(buf, "smoke");
                break;

            case GUARD_TYPE_GAS:
                strcpy(buf, "gas");
                break;

            case GUARD_TYPE_AREA:
                strcpy(buf, "area");
                break;

            case GUARD_TYPE_ROB:
                strcpy(buf, "rob");
                break;

            case GUARD_TYPE_OPENDOOR:
                strcpy(buf, "opendoor");
                break;
            }
            xmlNewProp(node, BAD_CAST "type", BAD_CAST buf);

            // handler
            xmlNewProp(node, BAD_CAST "handler", BAD_CAST log->handler);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);

        guardLogSyncItemCount++;
    }
    pthread_mutex_unlock(&guardLogMutex);
}

void GuardReceiveSyncLog(char* xml, int size)
{
    xmlDocPtr doc;

    pthread_mutex_lock(&guardLogMutex);

    doc = xmlParseMemory(xml, size);
    if (doc)
    {
        xmlXPathContext* xpathCtx = xmlXPathNewContext(doc);
        if (xpathCtx)
        {
            xmlXPathObject* xpathObj;

            xpathObj = xmlXPathEvalExpression(BAD_CAST "/GuardLogSyncList/item", xpathCtx);
            if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
            {
                int i, len;

                len = xpathObj->nodesetval->nodeNr;
                for (i = 0; i < len; i++)
                {
                    GuardLog log;
                    xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                    xmlAttr* attribute = node->properties;

                    memset(&log, 0, sizeof (GuardLog));

                    while (attribute && attribute->name && attribute->children)
                    {
                        xmlChar* value = NULL;

                        if (strcmp(attribute->name, "addr") == 0)
                        {
                            char *token, *saveptr;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            token = strtok_r(value, "-", &saveptr);
                            if (token)
                            {
                                log.area[0] = token[0];
                                log.area[1] = token[1];
                                log.area[2] = '\0';

                                token = strtok_r(NULL, "-", &saveptr);
                                if (token)
                                {
                                    log.building[0] = token[0];
                                    log.building[1] = token[1];
                                    log.building[2] = '\0';

                                    token = strtok_r(NULL, "-", &saveptr);
                                    if (token)
                                    {
                                        log.unit[0] = token[0];
                                        log.unit[1] = token[1];
                                        log.unit[2] = '\0';

                                        token = strtok_r(NULL, "-", &saveptr);
                                        if (token)
                                        {
                                            log.floor[0] = token[0];
                                            log.floor[1] = token[1];
                                            log.floor[2] = '\0';

                                            token = strtok_r(NULL, "-", &saveptr);
                                            if (token)
                                            {
                                                log.room[0] = token[0];
                                                log.room[1] = token[1];
                                                log.room[2] = '\0';

                                                token = strtok_r(NULL, "-", &saveptr);
                                                if (token)
                                                {
                                                    log.ext[0] = token[0];
                                                    log.ext[1] = token[1];
                                                    log.ext[2] = '\0';
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                        else if (strcmp(attribute->name, "time") == 0)
                        {
                            struct tm tm;

                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            strptime(value, "%Y-%m-%d %H:%M:%S", &tm);

                            log.timestamp = (unsigned long)mktime(&tm);
                        }
                        else if (strcmp(attribute->name, "event") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            
                            if (strcmp(value, "warn") == 0)
                                log.ev = GUARD_EVENT_WARN;
                            else if (strcmp(value, "open-door") == 0)
                                log.ev = GUARD_EVENT_OPENDOOR;
                        }
                        else if (strcmp(attribute->name, "type") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);

                            if (strcmp(value, "emergency") == 0)
                                log.type = GUARD_TYPE_EMERGENCY;
                            else if (strcmp(value, "infrared") == 0)
                                log.type = GUARD_TYPE_INFRARED;
                            else if (strcmp(value, "door") == 0)
                                log.type = GUARD_TYPE_DOOR;
                            else if (strcmp(value, "window") == 0)
                                log.type = GUARD_TYPE_WINDOW;
                            else if (strcmp(value, "smoke") == 0)
                                log.type = GUARD_TYPE_SMOKE;
                            else if (strcmp(value, "gas") == 0)
                                log.type = GUARD_TYPE_GAS;
                            else if (strcmp(value, "area") == 0)
                                log.type = GUARD_TYPE_AREA;
                            else if (strcmp(value, "rob") == 0)
                                log.type = GUARD_TYPE_ROB;
                            else if (strcmp(value, "opendoor") == 0)
                                log.type = GUARD_TYPE_OPENDOOR;
                        }
                        else if (strcmp(attribute->name, "state") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            log.state = atoi(value);
                        }
                        else if (strcmp(attribute->name, "handler") == 0)
                        {
                            value = xmlNodeListGetString(node->doc, attribute->children, 1);
                            strcpy(log.handler, value);
                        }
                        xmlFree(value);

                        attribute = attribute->next;
                    }
                    GuardReplaceLog(&log);
                }
            }
            xmlXPathFreeObject(xpathObj);
            xmlXPathFreeContext(xpathCtx);
        }
        xmlFreeDoc(doc);
    }
    pthread_mutex_unlock(&guardLogMutex);
}

int GuardGetUnhandledLogCount(void)
{
    return guardUnhandledLogCount;
}

void GuardHandleLog(int index, char* user)
{
    GuardLog* log = GuardGetLog(index);
    if (log)
    {
        log->state = GUARD_HANDLED;
        strcpy(log->handler, user);

        if (guardUnhandledLogCount > 0)
            guardUnhandledLogCount--;
    }
}
