#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ite/itp.h"
#include "platform.h"
#include "microhttpd.h"
#include "libxml/xpath.h"
#include "ite/ug.h"
#include "doorbell.h"
#include "scene.h"

#define PAGE "<html><head><meta http-equiv=\"pragma\" content=\"no-cache\"/><meta http-equiv=\"expires\" content=\"0\"/><meta http-equiv=\"refresh\" content=\"0; URL=/setting.html\"/></head></html>"

struct MHD_Daemon* webServerDaemon;
static uint8_t* filebuf = NULL;
static uint32_t filesize = 0;

static int
iterate_post (void *cls,
	       enum MHD_ValueKind kind,
	       const char *key,
	       const char *filename,
	       const char *content_type,
	       const char *transfer_encoding,
	       const char *data, uint64_t off, size_t size)
{
    struct MHD_Connection* connection = cls;

    if (0 != strcmp (key, "filename"))
        return MHD_NO;

    if (filebuf == NULL)
    {
        const char *clen = MHD_lookup_connection_value (connection,
                                      MHD_HEADER_KIND,
                                      MHD_HTTP_HEADER_CONTENT_LENGTH);

        printf("filesize %s\n", clen);

        filesize = 0;
        filebuf = malloc(strtoul (clen, NULL, 10));
        if (!filebuf)
        {
            printf("alloc filebuf fail\n");
            return MHD_NO;
        }
    }

    memcpy(filebuf + off, data, size);
    filesize += size;
    printf("recv %lu bytes\n", filesize);

    return MHD_YES;
}

static ssize_t
file_reader (void *cls, uint64_t pos, char *buf, size_t max)
{
  FILE *file = cls;

  (void) fseek (file, pos, SEEK_SET);
  return fread (buf, 1, max, file);
}

static void
file_free_callback (void *cls)
{
  FILE *file = cls;
  fclose (file);
}

static int
ahc_echo (void *cls,
          struct MHD_Connection *connection,
          const char *url,
          const char *method,
          const char *version,
          const char *upload_data, size_t *upload_data_size, void **ptr)
{
    static int aptr;
    static struct MHD_PostProcessor* pp;

    struct MHD_Response *response;
    int ret;

    if ((0 != strcmp (method, MHD_HTTP_METHOD_GET)) && (0 != strcmp (method, MHD_HTTP_METHOD_POST)))
        return MHD_NO;              /* unexpected method */

    if (NULL == *ptr)
    {
        if ((0 == strcmp (method, MHD_HTTP_METHOD_POST)) && (0 == strcmp(url, "/dev/info.cgi")))
        {
            free(filebuf);
            filebuf = NULL;
            filesize = 0;

            pp = MHD_create_post_processor (connection, 1024, &iterate_post, connection);
            if (NULL == pp)
            {
                printf ("Failed to setup post processor for '%s'\n", url);
                return MHD_NO; /* internal error */
            }
            *ptr = pp;
            return MHD_YES;
        }

        /* do never respond on first call */
        *ptr = &aptr;
        return MHD_YES;
    }

    if (strcmp(url, "/dev/info.cgi") == 0)
    {
        if (0 == strcmp (method, MHD_HTTP_METHOD_GET))
        {
            const char*        val;

            *ptr = NULL;                  /* reset when done */

            val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "action");
            if (val == NULL)
                return MHD_NO;

            if (strcmp(val, "reboot") == 0)
            {
                printf("REBOOT!");
                sleep(2);
                exit(0);
            }

            if (SceneGetQuitValue() == QUIT_UPGRADE_WEB)
                return MHD_NO;

            if (strcmp(val, "setting") == 0)
            {
                const char* str;
                bool savePrivate = false;

                // address
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "area");
                if (str && strcmp(theConfig.area, str))
                {
                    strncpy(theConfig.area, str, sizeof (theConfig.area) - 1);
                    savePrivate = true;
                }

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "building");
                if (str && strcmp(theConfig.building, str))
                {
                    strncpy(theConfig.building, str, sizeof (theConfig.building) - 1);
                    savePrivate = true;
                }

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "unit");
                if (str && strcmp(theConfig.unit, str))
                {
                    strncpy(theConfig.unit, str, sizeof (theConfig.unit) - 1);
                    savePrivate = true;
                }

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "floor");
                if (str && strcmp(theConfig.floor, str))
                {
                    strncpy(theConfig.floor, str, sizeof (theConfig.floor) - 1);
                    savePrivate = true;
                }

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "room");
                if (str && strcmp(theConfig.room, str))
                {
                    strncpy(theConfig.room, str, sizeof (theConfig.room) - 1);
                    savePrivate = true;
                }

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ext");
                if (str && strcmp(theConfig.ext, str))
                {
                    strncpy(theConfig.ext, str, sizeof (theConfig.ext) - 1);
                    savePrivate = true;
                }

                // rec_lev
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "rec_lev");
                if (str)
                {
                    LinphoneSetMicLevel(atoi(str));
                }

                // ring_lev
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ring_lev");
                if (str)
                {
                    LinphoneSetRingLevel(atoi(str));
                }

                // play_lev
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "play_lev");
                if (str)
                {
                    LinphoneSetVoiceLevel(atoi(str));
                }

                // dial_time
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "dial_time");
                if (str)
                {
                    theConfig.dial_time = atoi(str);
                }

                // voicememo_time
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "voicememo_time");
                if (str)
                {
                    theConfig.voicememo_time = atoi(str);
                }

                // calling_time
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "calling_time");
                if (str)
                {
                    theConfig.calling_time = atoi(str);
                }

                // mgsalarm_time
                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "mgsalarm_time");
                if (str)
                {
                    theConfig.mgsalarm_time = atoi(str);
                }

                if (savePrivate)
                {
                    ConfigSaveAll();

                    // reset network
                    SceneQuit(QUIT_RESET_NETWORK);
                }
                else
                    ConfigSave();

                response = MHD_create_response_from_buffer (strlen (PAGE), (void *) PAGE, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
            else if (strcmp(val, "reset_factory") == 0)
            {
                SceneQuit(QUIT_RESET_FACTORY);

                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
            else if (strcmp(val, "upgrade") == 0)
            {
                puts("ready to upgrade.");

				ret = MHD_HTTP_INTERNAL_SERVER_ERROR;

                // upgrade firmware
                if (filesize > 0 && filebuf)
                {
                    ITCArrayStream arrayStream;

                    itcArrayStreamOpen(&arrayStream, filebuf, filesize);

                    if (ugCheckCrc(&arrayStream.stream, NULL))
                    {
                        puts("Upgrade failed.");
                    }
                    else
                    {
                        UpgradeSetStream(&arrayStream.stream);
                        SceneQuit(QUIT_UPGRADE_WEB);

                        while (!UpgradeIsFinished())
                            sleep(1);

                        ret = UpgradeGetResult();
                        if (ret)
                        {
                            printf("Upgrade failed: %d\n", ret);
							ret = MHD_HTTP_INTERNAL_SERVER_ERROR;
                        }
                        else
                        {
                            puts("Upgrade finished.");
                            ret = MHD_HTTP_OK;
						#if defined(CFG_NOR_ENABLE) && CFG_NOR_CACHE_SIZE > 0
							ioctl(ITP_DEVICE_NOR, ITP_IOCTL_FLUSH, NULL);
						#endif
                        }
                    }
                }
                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, ret, response);
                MHD_destroy_response (response);

                return ret;
            }
            else if (strcmp(val, "macaddr") == 0)
            {
                const char* str;
                char macaddr[6];

                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr0");
                if (str)
                {
                    macaddr[0] = strtol(str, NULL, 16);

                    str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr1");
                    if (str)
                    {
                        macaddr[1] = strtol(str, NULL, 16);

                        str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr2");
                        if (str)
                        {
                            macaddr[2] = strtol(str, NULL, 16);

                            str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr3");
                            if (str)
                            {
                                macaddr[3] = strtol(str, NULL, 16);

                                str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr4");
                                if (str)
                                {
                                    macaddr[4] = strtol(str, NULL, 16);

                                    str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "macaddr5");
                                    if (str)
                                    {
                                        macaddr[5] = strtol(str, NULL, 16);
                                        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_WIRTE_MAC, macaddr);

                                        response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                                        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                                        MHD_destroy_response (response);

                                        return ret;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (strcmp(val, "upgrade_fw") == 0)
            {
                const char* val;

                val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");
                if (val == NULL)
                    return MHD_NO;

                UpgradeSetUrl((char*)val);
                SceneQuit(QUIT_UPGRADE_FIRMWARE);

                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
            else if (strcmp(val, "upgrade_res") == 0)
            {
                const char* val;

                val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");
                if (val == NULL)
                    return MHD_NO;

                UpgradeSetUrl((char*)val);
                SceneQuit(QUIT_UPGRADE_RESOURCE);

                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
            else if (strcmp(val, "upgrade_addressbook") == 0)
            {
                const char* val;

                val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");
                if (val == NULL)
                    return MHD_NO;

                UpgradeSetUrl((char*)val);
                SceneQuit(QUIT_UPGRADE_ADDRESSBOOK);

                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
        #ifdef CFG_CARDLIST_ENABLE
            else if (strcmp(val, "upgrade_cardlist") == 0)
            {
                const char* val;

                val = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "url");
                if (val == NULL)
                    return MHD_NO;

                UpgradeSetUrl((char*)val);
                SceneQuit(QUIT_UPGRADE_CARDLIST);

                response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
        #endif // CFG_CARDLIST_ENABLE
            return MHD_NO;
        }
        else if (0 == strcmp (method, MHD_HTTP_METHOD_POST))
        {
            if (0 != *upload_data_size)
            {
                /* evaluate POST data */
                MHD_post_process (pp, upload_data, *upload_data_size);
                *upload_data_size = 0;

                return MHD_YES;
            }
            else
            {
                *ptr = NULL;                  /* reset when done */

                puts("upload finish.");

                /* done with POST data, serve response */
                MHD_destroy_post_processor (pp);
                pp = NULL;

                response = MHD_create_response_from_buffer (strlen (PAGE), (void *) PAGE, MHD_RESPMEM_PERSISTENT);
                ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                MHD_destroy_response (response);

                return ret;
            }
        }
        *ptr = NULL;                  /* reset when done */
        return MHD_NO;
    }

    if (SceneGetQuitValue() == QUIT_UPGRADE_WEB)
        return MHD_NO;

    if (!strcmp(url, "/setting.html") || !strcmp(url, "/"))
    {
        xmlDocPtr       doc;
        xmlXPathContext *xpathCtx;
        xmlXPathObject  *xpathObj;
        xmlChar         *xmlbuff;
        int             buffersize;
        char            buf[32];
        #define MAX_DEVICE_INFO 100
        DeviceInfo infos[MAX_DEVICE_INFO];

        *ptr = NULL;                  /* reset when done */

        doc      = xmlParseFile(CFG_PRIVATE_DRIVE ":/web/setting.html");
        xpathCtx = xmlXPathNewContext(doc);

		//device type
		xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='device_type']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text;
            sprintf(buf, "%d", theDeviceInfo.type);
            text = xmlNewText(BAD_CAST buf);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);


		// address
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='address']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text;
            sprintf(buf, "%s-%s-%s-%s-%s-%s", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);
            text = xmlNewText(BAD_CAST buf);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

        // macaddr
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='macaddr']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text;

        #ifdef CFG_NET_WIFI
            ITPWifiInfo wifiInfo;

            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &wifiInfo);

            sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
                (uint8_t)wifiInfo.hardwareAddress[0],
                (uint8_t)wifiInfo.hardwareAddress[1],
                (uint8_t)wifiInfo.hardwareAddress[2],
                (uint8_t)wifiInfo.hardwareAddress[3],
                (uint8_t)wifiInfo.hardwareAddress[4],
                (uint8_t)wifiInfo.hardwareAddress[5]);

        #else
            ITPEthernetInfo ethInfo;

            ethInfo.index = 0;
            ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &ethInfo);

            sprintf(buf, "%02x%02x%02x%02x%02x%02x",
                    ethInfo.hardwareAddress[0],
                    ethInfo.hardwareAddress[1],
                    ethInfo.hardwareAddress[2],
                    ethInfo.hardwareAddress[3],
                    ethInfo.hardwareAddress[4],
                    ethInfo.hardwareAddress[5]);
        #endif // CFG_NET_WIFI

            text = xmlNewText(BAD_CAST buf);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

		// outdoor_ip
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='outdoor_ip']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST theDeviceInfo.ip);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

		// outdoor_addressbook_ver
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='outdoor_addressbook_ver']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
			char* ptr = AddressBookGetVersion();
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST ptr);
            xmlAddChild(node, text);
            free(ptr);
        }
        xmlXPathFreeObject(xpathObj);


        // ipaddr
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='ipaddr']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST theDeviceInfo.ip);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

        // addressbook_ver
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='addressbook_ver']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            char* ptr = AddressBookGetVersion();
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST ptr);
            xmlAddChild(node, text);
            free(ptr);
        }
        xmlXPathFreeObject(xpathObj);

#ifdef SIP_SERVER_TEST
		// sip_account
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='sip_account']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST CFG_REGISTER_ACCOUNT);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

		// sip_password
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='sip_password']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST CFG_REGISTER_DOMAIN);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);

		// sip_domain
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='sip_domain']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNode* text = xmlNewText(BAD_CAST CFG_REGISTER_PWD);
            xmlAddChild(node, text);
        }
        xmlXPathFreeObject(xpathObj);
#endif

        // indoor_info
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='indoor_info']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            int count = AddressBookGetDeviceInfoArray(infos, MAX_DEVICE_INFO, theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, NULL, NULL);
            if (count > 0)
            {
                xmlNode *firstNode, *child, *text, *node;
                DeviceInfo* info;
                int i;

                firstNode = xpathObj->nodesetval->nodeTab[0];

                for (i = 1; i < count; i++)
                {
                    node = xmlDocCopyNodeList(doc, firstNode);
                    if (!node)
                        break;

                    info = &infos[i];

                    child = xmlFirstElementChild(node);

                    sprintf(buf, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext);
                    text = xmlNewText(BAD_CAST buf);

                    xmlAddChild(child, text);

                    child = xmlNextElementSibling(child);
                    text = xmlNewText(BAD_CAST info->ip);
                    xmlAddChild(child, text);

                    xmlAddSibling(firstNode, node);
                }

                child = xmlFirstElementChild(firstNode);
                info = &infos[0];

                sprintf(buf, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext);
                text = xmlNewText(BAD_CAST buf);

                xmlAddChild(child, text);

                child = xmlNextElementSibling(child);
                text = xmlNewText(BAD_CAST info->ip);
                xmlAddChild(child, text);
            }
        }
        xmlXPathFreeObject(xpathObj);

        // manager_info
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='manager_info']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            int count = AddressBookGetManagerInfoArray(infos, MAX_DEVICE_INFO);
            if (count > 0)
            {
                xmlNode *firstNode, *child, *text, *node;
                DeviceInfo* info;
                int i;

                firstNode = xpathObj->nodesetval->nodeTab[0];

                for (i = 1; i < count; i++)
                {
                    node = xmlDocCopyNodeList(doc, firstNode);
                    if (!node)
                        break;

                    info = &infos[i];

                    child = xmlFirstElementChild(node);

                    sprintf(buf, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext);
                    text = xmlNewText(BAD_CAST buf);

                    xmlAddChild(child, text);

                    child = xmlNextElementSibling(child);
                    text = xmlNewText(BAD_CAST info->ip);
                    xmlAddChild(child, text);

                    xmlAddSibling(firstNode, node);
                }

                child = xmlFirstElementChild(firstNode);
                info = &infos[0];

                sprintf(buf, "%s-%s-%s-%s-%s-%s", info->area, info->building, info->unit, info->floor, info->room, info->ext);
                text = xmlNewText(BAD_CAST buf);

                xmlAddChild(child, text);

                child = xmlNextElementSibling(child);
                text = xmlNewText(BAD_CAST info->ip);
                xmlAddChild(child, text);
            }
        }
        xmlXPathFreeObject(xpathObj);

        // address
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='area']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.area);
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='building']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.building);
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='unit']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.unit);
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='floor']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.floor);
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='room']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.room);
        }
        xmlXPathFreeObject(xpathObj);

        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='ext']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            xmlNewProp(node, BAD_CAST "value", BAD_CAST theConfig.ext);
        }
        xmlXPathFreeObject(xpathObj);

        // rec_lev
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='rec_lev']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            sprintf(buf, "%d", LinphoneGetMicLevel());
            xmlNewProp(node, BAD_CAST "value", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);

        // ring_lev
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='ring_lev']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            sprintf(buf, "%d", LinphoneGetRingLevel());
            xmlNewProp(node, BAD_CAST "value", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);

        // play_lev
        xpathObj = xmlXPathEvalExpression(BAD_CAST "//*[@id='play_lev']", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            sprintf(buf, "%d", LinphoneGetVoiceLevel());
            xmlNewProp(node, BAD_CAST "value", BAD_CAST buf);
        }
        xmlXPathFreeObject(xpathObj);

        // output
        xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

        response = MHD_create_response_from_buffer (buffersize, xmlbuff, MHD_RESPMEM_MUST_FREE);
        if (response == NULL)
        {
            xmlFree (xmlbuff);
            xmlXPathFreeContext(xpathCtx);
            xmlFreeDoc(doc);
            return MHD_NO;
        }
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);

        xmlXPathFreeContext(xpathCtx);
        xmlFreeDoc(doc);

        return ret;
    }
    else if (strcmp(url, "/open_door") == 0)
    {
        const char* ptr;

        ptr = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ro");
        if (ptr)
        {
            char* area, *saveptr;

            printf("open_door: %s\n", ptr);

            area = strtok_r((char*)ptr, "-", &saveptr);
            if (area)
            {
                char* building = strtok_r(NULL, "-", &saveptr);
                if (building)
                {
                    char* unit = strtok_r(NULL, "-", &saveptr);
                    if (unit)
                    {
                        char* floor = strtok_r(NULL, "-", &saveptr);
                        if (floor)
                        {
                            char* room = strtok_r(NULL, "-", &saveptr);
                            if (room)
                            {
                                char* ext = strtok_r(NULL, "-", &saveptr);
                                if (ext)
                                {
                                    if (!strcmp(area, theCenterInfo.area) &&
                                        !strcmp(building, theCenterInfo.building) &&
                                        !strcmp(unit, theCenterInfo.unit) &&
                                        !strcmp(floor, theCenterInfo.floor) &&
                                        !strcmp(room, theCenterInfo.room) &&
                                        !strcmp(ext, theCenterInfo.ext))
                                    {
                                        // is center
                                        time_t t = SnapshotTake();
                                        PeripheralOpenDoor();
                                        LinphonePlayHintSound(HINT_SOUND_OPENDOOR);
                                        EventWriteOpenDoorLog(EVENT_OPENDOOR_REMOTE, &theCenterInfo, true, NULL, &t);
                                    }
                                    else if (!strcmp(area, theDeviceInfo.area) &&
                                        !strcmp(building, theDeviceInfo.building) &&
                                        !strcmp(unit, theDeviceInfo.unit))
                                    {
                                        // is the same building
                                        DeviceInfo info;
                                        time_t t;

                                        strcpy(info.area, area);
                                        strcpy(info.building, building);
                                        strcpy(info.unit, unit);
                                        strcpy(info.floor, floor);
                                        strcpy(info.room, room);
                                        strcpy(info.ext, ext);

                                        t = SnapshotTake();
                                        PeripheralOpenDoor();
                                        LinphonePlayHintSound(HINT_SOUND_OPENDOOR);
                                        EventWriteOpenDoorLog(EVENT_OPENDOOR_REMOTE, &info, true, NULL, &t);
                                    }
                                    else
                                    {
                                        // otherwise
                                        DeviceInfo info;
                                        char* ip = AddressBookGetDeviceIP(area, building, unit, floor, room, ext);

                                        if (ip)
                                        {
                                            AddressBookGetDeviceInfo(&info, ip);
                                            free(ip);

                                            if (info.type == DEVICE_MANAGER)
                                            {
                                                time_t t = SnapshotTake();
                                                PeripheralOpenDoor();
                                                LinphonePlayHintSound(HINT_SOUND_OPENDOOR);
                                                EventWriteOpenDoorLog(EVENT_OPENDOOR_REMOTE, &info, true, NULL, &t);
                                            }
                                            else
                                            {
                                                EventWriteOpenDoorLog(EVENT_OPENDOOR_REMOTE, &info, false, NULL, NULL);
                                            }
                                        }
                                        else
                                        {
                                            strcpy(info.area, area);
                                            strcpy(info.building, building);
                                            strcpy(info.unit, unit);
                                            strcpy(info.floor, floor);
                                            strcpy(info.room, room);
                                            strcpy(info.ext, ext);

                                            EventWriteOpenDoorLog(EVENT_OPENDOOR_REMOTE, &info, false, NULL, NULL);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);

        return ret;
    }
    else if (strcmp(url, "/get_info") == 0)
    {
        int size = 0;
        char* buf = UpgradeGetDeviceInfo(&size);

        response = MHD_create_response_from_buffer (size, buf, MHD_RESPMEM_MUST_FREE);
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);

        return ret;
    }
    else if (strcmp(url, "/call_elevator") == 0)
    {
        const char* str;

        str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ro");
        if (str)
        {
            char* area, *saveptr;

            printf("call_elevator: %s\n", str);

            area = strtok_r((char*)str, "-", &saveptr);
            if (area)
            {
                char* building = strtok_r(NULL, "-", &saveptr);
                if (building)
                {
                    char* unit = strtok_r(NULL, "-", &saveptr);
                    if (unit)
                    {
                        char* floor = strtok_r(NULL, "-", &saveptr);
                        if (floor)
                        {
                            str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "dir");
                            if (str)
                            {
                                bool up = strcmp(str, "up") == 0 ? true : false;
                                PeripheralCallElevator(floor, up);
                            }
                        }
                    }
                }
            }
        }
        response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);

        return ret;
    }
    else if (strcmp(url, "/photo") == 0)
    {
        const char* ptr;

        ptr = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ro");
        if (ptr)
        {
            char* area, *saveptr;

            printf("photo: %s\n", ptr);

            area = strtok_r((char*)ptr, "-", &saveptr);
            if (area)
            {
                char* building = strtok_r(NULL, "-", &saveptr);
                if (building)
                {
                    char* unit = strtok_r(NULL, "-", &saveptr);
                    if (unit)
                    {
                        char* floor = strtok_r(NULL, "-", &saveptr);
                        if (floor)
                        {
                            char* room = strtok_r(NULL, "-", &saveptr);
                            if (room)
                            {
                                char* ext = strtok_r(NULL, "-", &saveptr);
                                if (ext)
                                {
                                    if (!strcmp(area, theDeviceInfo.area) &&
                                        !strcmp(building, theDeviceInfo.building) &&
                                        !strcmp(unit, theDeviceInfo.unit) &&
                                        !strcmp(floor, theDeviceInfo.floor) &&
                                        !strcmp(room, theDeviceInfo.room) &&
                                        !strcmp(ext, theDeviceInfo.ext))
                                    {
                                        int size;
                                        uint8_t* buf;
                                        ptr = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "time");
                                        buf = SnapshotGet((char*)ptr, &size);
                                        if (buf)
                                        {
                                            response = MHD_create_response_from_buffer (size, buf, MHD_RESPMEM_PERSISTENT);
                                            ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                                            MHD_destroy_response (response);
                                            return ret;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        response = MHD_create_response_from_buffer (0, NULL, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
        MHD_destroy_response (response);

        return ret;
    }
    else if (strncmp(url, "/public/", 8) == 0)
    {
        const char* str;
        char* saveptr;

        str = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "ro");
        if (str)
        {
            char* area = strtok_r((char*)str, "-", &saveptr);
            if (area)
            {
                char* building = strtok_r(NULL, "-", &saveptr);
                if (building)
                {
                    char* unit = strtok_r(NULL, "-", &saveptr);
                    if (unit)
                    {
                        char* floor = strtok_r(NULL, "-", &saveptr);
                        if (floor)
                        {
                            char* room = strtok_r(NULL, "-", &saveptr);
                            if (room)
                            {
                                char* ext = strtok_r(NULL, "-", &saveptr);
                                if (ext)
                                {
                                    if (!strcmp(area, theDeviceInfo.area) &&
                                        !strcmp(building, theDeviceInfo.building) &&
                                        !strcmp(unit, theDeviceInfo.unit) &&
                                        !strcmp(floor, theDeviceInfo.floor) &&
                                        !strcmp(room, theDeviceInfo.room) &&
                                        !strcmp(ext, theDeviceInfo.ext))
                                    {
                                        struct stat buf;
                                        FILE *file;
                                        char fullpath[PATH_MAX];

                                        strcpy(fullpath, CFG_PUBLIC_DRIVE ":");
                                        strcat(fullpath, strrchr(url, '/'));

                                        if (stat (fullpath, &buf))
                                        {
                                            *ptr = NULL;                  /* reset when done */
                                            return MHD_NO;
                                        }

                                        file = fopen (fullpath, "rb");
                                        if (file == NULL)
                                        {
                                            *ptr = NULL;                  /* reset when done */
                                            return MHD_NO;
                                        }
                                        else
                                        {
                                            response = MHD_create_response_from_callback (buf.st_size, 32 * 1024,     /* 32k PAGE_NOT_FOUND size */
                                                                                    &file_reader, file,
                                                                                    &file_free_callback);
                                            if (response == NULL)
	                                        {
	                                            fclose (file);
	                                            return MHD_NO;
	                                        }
                                            ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
                                            MHD_destroy_response (response);

                                            return ret;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return MHD_NO;
    }
    else
    {
        struct stat buf;
        FILE *file;
        char fullpath[PATH_MAX];

        strcpy(fullpath, CFG_PRIVATE_DRIVE ":/web");
        strcat(fullpath, strrchr(url, '/'));

        if (stat (fullpath, &buf))
        {
            *ptr = NULL;                  /* reset when done */
            return MHD_NO;
        }

        file = fopen (fullpath, "rb");
        if (file == NULL)
        {
            *ptr = NULL;                  /* reset when done */
            return MHD_NO;
        }
        else
        {
            response = MHD_create_response_from_callback (buf.st_size, 32 * 1024,     /* 32k PAGE_NOT_FOUND size */
                                                    &file_reader, file,
                                                    &file_free_callback);
            if (response == NULL)
	        {
	            fclose (file);
	            return MHD_NO;
	        }
            ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
            MHD_destroy_response (response);

            return ret;
        }
    }
}

void WebServerInit(void)
{
    printf("Start Web Server...\n");

    webServerDaemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, CFG_WEBSERVER_PORT, NULL, NULL,
                         &ahc_echo, NULL,
                         NULL, MHD_OPTION_END);
    if (webServerDaemon == NULL)
        printf("Start Web Server fail\n");
}

void WebServerExit(void)
{
    printf("Stop Web Server...\n");

    MHD_stop_daemon(webServerDaemon);
}
