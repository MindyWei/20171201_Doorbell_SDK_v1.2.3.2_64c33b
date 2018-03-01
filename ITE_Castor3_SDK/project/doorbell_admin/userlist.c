#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "libxml/xpath.h"
#include "ite/itc.h"
#include "ite/itp.h"
#include "doorbell.h"

static xmlDocPtr ulDoc;
static pthread_mutex_t ulMutex;
static char ulUserID[64];
static char ulUserPassword[64];

bool UserListCheckUser(char* id, char* pw)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128], *idEnc, *pwEnc;
    bool result = false;

    if (!id || !pw)
        return result;

    idEnc = itcUrlEncode(id);
    pwEnc = itcUrlEncode(pw);

    strcpy(buf, "/UserList/user[@id='");
    strcat(buf, idEnc);
    strcat(buf, "' and @pw='");
    strcat(buf, pwEnc);
    strcat(buf, "']");

    free(idEnc);
    free(pwEnc);

    pthread_mutex_lock(&ulMutex);

    xpathCtx = xmlXPathNewContext(ulDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            printf("user %s check success!\n", id);
            strcpy(ulUserID, id);
            strcpy(ulUserPassword, pw);
            result = true;
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
    }
    
    pthread_mutex_unlock(&ulMutex);

    return result;
}

void UserListInit(void)
{
    pthread_mutex_init(&ulMutex, NULL);

	ulDoc = xmlParseFile(CFG_PRIVATE_DRIVE ":/userlist.xml");
    assert(ulDoc);
}

void UserListExit(void)
{
    if (ulDoc)
    {
        xmlFreeDoc(ulDoc);
        ulDoc = NULL;
    }
    pthread_mutex_destroy(&ulMutex);
}

int UserListGetVersion(void)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    int result = 0;

    pthread_mutex_lock(&ulMutex);

    xpathCtx = xmlXPathNewContext(ulDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/UserList/@ver", xpathCtx);
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
    pthread_mutex_unlock(&ulMutex);

    return result;
}

void UserListReload(void)
{
    pthread_mutex_lock(&ulMutex);

    if (ulDoc)
    {
        xmlFreeDoc(ulDoc);
        ulDoc = NULL;
    }

    ulDoc = xmlParseFile(CFG_PRIVATE_DRIVE ":/userlist.xml");
    assert(ulDoc);

    pthread_mutex_unlock(&ulMutex);
}

char* UserListCurrentUserID(void)
{
    return ulUserID;
}

char* UserListCurrentUserPassword(void)
{
    return ulUserPassword;
}

int UserListGetUserIDArray(char idArray[][64], int count)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    int ret = 0;

    assert(idArray);

    xpathCtx = xmlXPathNewContext(ulDoc);
    if (!xpathCtx)
        return ret;
    
    pthread_mutex_lock(&ulMutex);

    xpathObj = xmlXPathEvalExpression(BAD_CAST "/UserList/user", xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        int i, len;

        len = xpathObj->nodesetval->nodeNr;
        for (i = 0; i < len && i < count; i++)
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[i];
            xmlAttr* attribute = node->properties;
            char* id = idArray[i];
            char* idDec;

            while (attribute && attribute->name && attribute->children)
            {
                xmlChar* value = NULL;

                if (strcmp(attribute->name, "id") == 0)
                {
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    idDec = itcUrlDecode(value);
                    strcpy(id, idDec);
                    free(idDec);
                    xmlFree(value);
                }
                attribute = attribute->next;
            }
        }
        ret = i;
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    pthread_mutex_unlock(&ulMutex);
    return ret;
}

void UserListDeleteUser(char* id)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128], *idEnc;

    assert(id);

    xpathCtx = xmlXPathNewContext(ulDoc);
    if (!xpathCtx)
        return;

    idEnc = itcUrlEncode(id);
    sprintf(buf, "/UserList/user[@id='%s']", idEnc);
    free(idEnc);

    pthread_mutex_lock(&ulMutex);

    xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    {
        xmlNode* node = xpathObj->nodesetval->nodeTab[0];
        xmlUnlinkNode(node);
        xmlFreeNode(node);
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);

    xmlSaveFormatFileEnc(CFG_PRIVATE_DRIVE ":/userlist.xml", ulDoc, "UTF-8", 1);

#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
    ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
#endif
    pthread_mutex_unlock(&ulMutex);
}

bool UserListAddUser(char* id, char* pw)
{
    xmlXPathContext* xpathCtx;
    xmlXPathObject* xpathObj;
    char buf[128], *idEnc, *pwEnc;
    bool result = false;

    if (!id || !pw)
        return result;

    idEnc = itcUrlEncode(id);
    pwEnc = itcUrlEncode(pw);

    strcpy(buf, "/UserList/user[@id='");
    strcat(buf, idEnc);
    strcat(buf, "']");

    pthread_mutex_lock(&ulMutex);

    xpathCtx = xmlXPathNewContext(ulDoc);
    if (xpathCtx)
    {
        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            result = false;
            xmlXPathFreeObject(xpathObj);
            xmlXPathFreeContext(xpathCtx);
            goto end;
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "/UserList", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNodePtr parent = xpathObj->nodesetval->nodeTab[0];
            xmlNodePtr node = xmlNewChild(parent, NULL, BAD_CAST "user", NULL);

            xmlSetProp(node, "id", idEnc);
            xmlSetProp(node, "pw", pwEnc);
        }
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);

        xmlSaveFormatFileEnc(CFG_PRIVATE_DRIVE ":/userlist.xml", ulDoc, "UTF-8", 1);

    #if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
        ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
    #endif
        result = true;
    }
end:
    pthread_mutex_unlock(&ulMutex);
    free(idEnc);
    free(pwEnc);

    return result;
}
