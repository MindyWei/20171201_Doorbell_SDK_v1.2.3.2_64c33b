#include <sys/ioctl.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "curl/curl.h"
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "doorbell.h"

#define MAX_EVENT_COUNT 999

static const char* evDocStr = "<?xml version=\"1.0\" encoding=\"utf-8\"?><EventList></EventList>";

static xmlDocPtr evDoc;
static pthread_mutex_t evMutex;
static bool evQuit, evHasRegisterEvent;
static int evItemCount;

static void* EventUploadTask(void* arg)
{
    while (!evQuit)
    {
        if (NetworkIsReady())
        {
            if (evItemCount > 0)
            {
                CURL* curl = NULL;
                CURLcode res = CURLE_OK;
                struct curl_httppost *formpost = NULL;
                struct curl_httppost *lastptr = NULL;
                DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
                char url[128];
                xmlChar* xmlbuff = NULL;
                int i, count, buffersize, okCount, itemCount;

                pthread_mutex_lock(&evMutex);
                itemCount = evItemCount;
                xmlDocDumpFormatMemory(evDoc, &xmlbuff, &buffersize, 1);
                pthread_mutex_unlock(&evMutex);
                //puts(xmlbuff);

                curl_formadd(&formpost, &lastptr,
                    CURLFORM_COPYNAME, "upload_event",
                    CURLFORM_COPYCONTENTS, xmlbuff,
                    CURLFORM_END);

                count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);
                okCount = 0;

                for (i = 0; i < count; i++)
                {
                    curl = curl_easy_init();
                    if (curl)
                    {
                        sprintf(url, "http://%s:" CFG_WEBSERVER_PORT_STR "/doorbell/upload_event", managerInfoArray[i].ip);

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

                if (theCenterInfo.ip[0] != '\0' && (NetworkServerIsReady() || evHasRegisterEvent))
                {
                    curl = curl_easy_init();
                    if (curl)
                    {
                        struct curl_slist* slist;

                        slist = curl_slist_append(NULL, "Content-Type: text/xml; charset=utf-8");
                        if (slist)
                        {
                            sprintf(url, "http://%s/doorbell/upload_event", theCenterInfo.ip);

                            curl_easy_setopt(curl, CURLOPT_URL, url);
                            curl_easy_setopt(curl, CURLOPT_POST, 1);
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (char*)xmlbuff);
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, buffersize);
                            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
                            curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
                            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

                        #ifndef NDEBUG
                            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
                        #endif

                            res = curl_easy_perform(curl);
                            if (CURLE_OK == res)
                            {
                                okCount++;
                                evHasRegisterEvent = false;
                            }
                            curl_slist_free_all(slist);
                        }
                        curl_easy_cleanup(curl);
                    }
                }
                xmlFree(xmlbuff);

                if (okCount > 0)
                {
                    xmlXPathContext* xpathCtx;
                    xmlXPathObject* xpathObj;

                    // clear sent items
                    pthread_mutex_lock(&evMutex);
                    xpathCtx = xmlXPathNewContext(evDoc);
                    if (xpathCtx)
                    {
                        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
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
                    evItemCount -= itemCount;
                    pthread_mutex_unlock(&evMutex);
                }
            }
        }
        sleep(5);
    }

    // quit
    if (evDoc)
    {
        xmlFreeDoc(evDoc);
        evDoc = NULL;
    }
    pthread_mutex_destroy(&evMutex);
    return NULL;
}

void EventInit(void)
{
    pthread_t task;
    pthread_attr_t attr;

    evQuit              = false;
    evHasRegisterEvent  = false;
    evItemCount = 0;

    pthread_mutex_init(&evMutex, NULL);

    evDoc = xmlParseMemory(evDocStr, strlen(evDocStr));
    assert(evDoc);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, EventUploadTask, NULL);
}

void EventExit(void)
{
    evQuit = true;
}

void EventWriteCalloutLog(char* ip, EventCalloutAction action)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;

    if (evQuit || evItemCount > MAX_EVENT_COUNT)
        return;

    pthread_mutex_lock(&evMutex);

    xpathCtx = xmlXPathNewContext(evDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            char buf[32];
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "packet", NULL);

            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

            xmlSetProp(node, "addr", buf);

            sprintf(buf, "%d", theDeviceInfo.type);
            xmlSetProp(node, "type", buf);

            xmlSetProp(node, "event", "callout");
        }
        xmlXPathFreeObject(xpathObj);
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='callout'][last()]", xpathCtx);

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            struct timeval tv;
            struct tm* timeinfo;
            char buf[32];
            DeviceInfo info;

            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "item", NULL);

            // time
            gettimeofday(&tv, NULL);
            timeinfo = localtime((const time_t*)&tv.tv_sec);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
            xmlNewProp(node, BAD_CAST "time", BAD_CAST buf);

            // to
            AddressBookGetDeviceInfo(&info, ip);
            sprintf(buf, "%s-%s-%s-%s-%s", info.area, info.building, info.unit, info.floor, info.room);
            xmlNewProp(node, BAD_CAST "to", BAD_CAST buf);

            // type
            sprintf(buf, "%d", info.type);
            xmlNewProp(node, BAD_CAST "type", BAD_CAST buf);

            // action
            switch (action)
            {
            case EVENT_CALLOUT_START:
                strcpy(buf, "start");
                break;

            case EVENT_CALLOUT_END:
                strcpy(buf, "end");
                break;

            case EVENT_CALLOUT_NOACK:
                strcpy(buf, "noack");
                break;
            }
            xmlNewProp(node, BAD_CAST "action", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        evItemCount++;
    }
    pthread_mutex_unlock(&evMutex);
}

void EventWriteGuardLog(GuardSensor sensor, GuardStatus status, time_t timestamp)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;

    if (evQuit || evItemCount > MAX_EVENT_COUNT)
        return;

    pthread_mutex_lock(&evMutex);

    xpathCtx = xmlXPathNewContext(evDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            char buf[32];
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "packet", NULL);

            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

            xmlSetProp(node, "addr", buf);

            sprintf(buf, "%d", theDeviceInfo.type);
            xmlSetProp(node, "type", buf);

            xmlSetProp(node, "event", "warn");
        }
        xmlXPathFreeObject(xpathObj);
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='warn'][last()]", xpathCtx);

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            struct tm* timeinfo;
            char buf[32];

            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "item", NULL);

            // time
            timeinfo = localtime((const time_t*)&timestamp);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
            xmlNewProp(node, BAD_CAST "time", BAD_CAST buf);

            // type
            switch (sensor)
            {
            case GUARD_EMERGENCY:
                strcpy(buf, "emergency");
                break;

            case GUARD_INFRARED:
                strcpy(buf, "infrared");
                break;

            case GUARD_DOOR:
                strcpy(buf, "door");
                break;

            case GUARD_WINDOW:
                strcpy(buf, "window");
                break;

            case GUARD_SMOKE:
                strcpy(buf, "smoke");
                break;

            case GUARD_GAS:
                strcpy(buf, "gas");
                break;

            case GUARD_AREA:
                strcpy(buf, "area");
                break;

            case GUARD_ROB:
                strcpy(buf, "rob");
                break;
            }
            xmlNewProp(node, BAD_CAST "type", BAD_CAST buf);

            // channel
            sprintf(buf, "%d", sensor);
            xmlNewProp(node, BAD_CAST "chanel", BAD_CAST buf);

            // action
            switch (status)
            {
            case GUARD_ENABLE:
                strcpy(buf, "enable");
                break;

            case GUARD_DISABLE:
                strcpy(buf, "disable");
                break;

            case GUARD_ALARM:
                strcpy(buf, "trig");
                break;

            case GUARD_UNALARM:
                strcpy(buf, "unalarm");
                break;
            }
            xmlNewProp(node, BAD_CAST "action", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        evItemCount++;
    }
    pthread_mutex_unlock(&evMutex);
}

void EventWriteRegisterDevice(uint8_t hardwareAddress[])
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;
    char buf[32];

    if (evQuit || evItemCount > MAX_EVENT_COUNT || theCenterInfo.ip[0] == '\0')
        return;

    pthread_mutex_lock(&evMutex);

    xpathCtx = xmlXPathNewContext(evDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "packet", NULL);

            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

            xmlSetProp(node, "addr", buf);

            sprintf(buf, "%d", theDeviceInfo.type);
            xmlSetProp(node, "type", buf);

            xmlSetProp(node, "event", "register-device");
        }
        xmlXPathFreeObject(xpathObj);
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='register-device'][last()]", xpathCtx);

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "dev", NULL);

            // ip
            xmlNewProp(node, BAD_CAST "ip", BAD_CAST theDeviceInfo.ip);

            // mac
            sprintf(buf, "%02x%02x%02x%02x%02x%02x",
                hardwareAddress[0],
                hardwareAddress[1],
                hardwareAddress[2],
                hardwareAddress[3],
                hardwareAddress[4],
                hardwareAddress[5]);
            xmlNewProp(node, BAD_CAST "mc", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);

        evHasRegisterEvent  = true;
        evItemCount++;
    }
    pthread_mutex_unlock(&evMutex);
}

void EventWriteOpenDoorLog(EventOpenDoorMode mode, DeviceInfo* info, bool verified, char* card)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;

    if (evQuit || evItemCount > MAX_EVENT_COUNT)
        return;

    pthread_mutex_lock(&evMutex);

    xpathCtx = xmlXPathNewContext(evDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            char buf[32];
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "packet", NULL);

            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

            xmlSetProp(node, "addr", buf);

            sprintf(buf, "%d", theDeviceInfo.type);
            xmlSetProp(node, "type", buf);

            xmlSetProp(node, "event", "open-door");
        }
        xmlXPathFreeObject(xpathObj);
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='open-door'][last()]", xpathCtx);

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            struct tm* timeinfo;
            struct timeval tv;
            char buf[32];

            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "item", NULL);

            // time
            gettimeofday(&tv, NULL);
            timeinfo = localtime((const time_t*)&tv.tv_sec);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
            xmlNewProp(node, BAD_CAST "time", BAD_CAST buf);

            // mode
            switch (mode)
            {
            case EVENT_OPENDOOR_REMOTE:
                strcpy(buf, "remote");
                break;

            case EVENT_OPENDOOR_PASSWORD:
                strcpy(buf, "pwd");
                break;

            case EVENT_OPENDOOR_CARD:
                strcpy(buf, "card");
                break;
            }
            xmlNewProp(node, BAD_CAST "mode", BAD_CAST buf);

            // ro
            if (info)
            {
                sprintf(buf, "%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room);
                xmlNewProp(node, BAD_CAST "ro", BAD_CAST buf);
            }

            // verified
            sprintf(buf, "%s", verified ? "true" : "false");
            xmlNewProp(node, BAD_CAST "verified", BAD_CAST buf);

            // card
            if (card)
            {
                xmlNewProp(node, BAD_CAST "card", BAD_CAST card);
            }
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);

        evItemCount++;
    }
    pthread_mutex_unlock(&evMutex);
}

void EventWriteSmsLog(char* ip)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char* ret = NULL;

    if (evQuit || evItemCount > MAX_EVENT_COUNT)
        return;

    pthread_mutex_lock(&evMutex);

    xpathCtx = xmlXPathNewContext(evDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            char buf[32];
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "packet", NULL);

            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);

            xmlSetProp(node, "addr", buf);

            sprintf(buf, "%d", theDeviceInfo.type);
            xmlSetProp(node, "type", buf);

            xmlSetProp(node, "event", "sms");
        }
        xmlXPathFreeObject(xpathObj);
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/EventList/packet[@event='sms'][last()]", xpathCtx);

        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            struct tm* timeinfo;
            struct timeval tv;
            char buf[32];
            DeviceInfo info;

            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "item", NULL);

            // time
            gettimeofday(&tv, NULL);
            timeinfo = localtime((const time_t*)&tv.tv_sec);
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
            xmlNewProp(node, BAD_CAST "time", BAD_CAST buf);

            // from
            AddressBookGetDeviceInfo(&info, ip);
            sprintf(buf, "%s-%s-%s-%s-%s-%s", info.area, info.building, info.unit, info.floor, info.room, info.ext);
            xmlNewProp(node, BAD_CAST "from", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);

        evItemCount++;
    }
    pthread_mutex_unlock(&evMutex);
}
