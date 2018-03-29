#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "libxml/xpath.h"
#include "doorbell.h"

static xmlDocPtr clDoc;
static pthread_mutex_t clMutex;
static bool clQuit;

static bool CardListCheckNumber(char* cardnum)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[64];
    bool result = false;

    if (!cardnum)
        return result;

    strcpy(buf, "/CardList/dev[@ro='");
    strcat(buf, theDeviceInfo.area);
    strcat(buf, "-");
    strcat(buf, theDeviceInfo.building);
    strcat(buf, "-");
    strcat(buf, theDeviceInfo.unit);
    strcat(buf, "-");
    strcat(buf, theDeviceInfo.floor);
    strcat(buf, "-");
    strcat(buf, theDeviceInfo.room);
    if (theDeviceInfo.ext[0])
    {
        strcat(buf, "-");
        strcat(buf, theDeviceInfo.ext);
    }

    strcat(buf, "']/card");

    pthread_mutex_lock(&clMutex);

    xpathCtx = xmlXPathNewContext(clDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            int i, len;

            len = xpathObj->nodesetval->nodeNr;
            for (i = 0; i < len; i++)
            {
                xmlNode* node = xpathObj->nodesetval->nodeTab[i];
                xmlChar* value = xmlNodeListGetString(node->doc, node->children, 1);
                if (value)
                {
                    if (strcmp(cardnum, value) == 0)
                    {
                        printf("cardnum %s check success!\n", cardnum);
                        result = true;
                        xmlFree(value);
                        break;
                    }
                    xmlFree(value);
                }
            }
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
    }
    
    pthread_mutex_unlock(&clMutex);

    return result;
}

static void* CardListCheckTask(void* arg)
{
    while (!clQuit)
    {
        char* cardnum;
        
        if (!PeripheralDoorIsOpened())
        {
            cardnum = PeripheralReadCard();
            if (cardnum)
            {
                if (CardListCheckNumber(cardnum) || RemoteCheckCard(cardnum))
                {
                    PeripheralOpenDoor();
                    EventWriteOpenDoorLog(EVENT_OPENDOOR_CARD, NULL, true, cardnum);
                }
                else
                {
                    EventWriteOpenDoorLog(EVENT_OPENDOOR_CARD, NULL, false, cardnum);
                }
            }
        }
        usleep(300000);
    }

    // quit
    if (clDoc)
    {
        xmlFreeDoc(clDoc);
        clDoc = NULL;
    }
    pthread_mutex_destroy(&clMutex);
    return NULL;
}

void CardListInit(void)
{
    pthread_t task;
    pthread_attr_t attr;

    pthread_mutex_init(&clMutex, NULL);

	clDoc = xmlParseFile(CFG_PRIVATE_DRIVE ":/cardlist.xml");
    assert(clDoc);

    clQuit = false;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, CardListCheckTask, NULL);
}

void CardListExit(void)
{
    clQuit = true;
}

int CardListGetVersion(void)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    int result = 0;

    pthread_mutex_lock(&clMutex);

    xpathCtx = xmlXPathNewContext(clDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/CardList/@ver", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlChar* value = xmlNodeListGetString(node->doc, node->children, 1);
            if (value)
            {
                result = atoi(value);
                xmlFree(value);
            }
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
    }    
    pthread_mutex_unlock(&clMutex);

    return result;
}

void CardListReload(void)
{
    pthread_mutex_lock(&clMutex);

    if (clDoc)
    {
        xmlFreeDoc(clDoc);
        clDoc = NULL;
    }

    clDoc = xmlParseFile(CFG_PRIVATE_DRIVE ":/cardlist.xml");
    assert(clDoc);

    pthread_mutex_unlock(&clMutex);
}
