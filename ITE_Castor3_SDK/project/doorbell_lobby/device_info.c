#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"

DeviceInfo theCenterInfo;
DeviceInfo theDeviceInfo;

void DeviceInfoInit(void)
{
    bool saveCfg = false;
    char* buf;

    AddressBookGetCenterInfo(&theCenterInfo);

    if (theConfig.local_pos[0] && theConfig.local_ext[0])
    {
        int len = strlen(theConfig.local_pos);

        theDeviceInfo.type = theConfig.local_type;

        if (len >= 2)
            strncpy(theDeviceInfo.area, &theConfig.local_pos[0], 2);
        else
            strcpy(theDeviceInfo.area, "00");

        if (len >= 4)
            strncpy(theDeviceInfo.building, &theConfig.local_pos[2], 2);
        else
            strcpy(theDeviceInfo.building, "00");

        if (len >= 6)
            strncpy(theDeviceInfo.unit, &theConfig.local_pos[4], 2);
        else
            strcpy(theDeviceInfo.unit, "00");

        strcpy(theDeviceInfo.floor, "00");
        strcpy(theDeviceInfo.room, "00");
        strcpy(theDeviceInfo.ext, theConfig.local_ext);
        strcpy(theDeviceInfo.ip, theConfig.ipaddr);

        buf = AddressBookGetDeviceIP(theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);
        if (buf)
        {
            if (!theConfig.dhcp && strcmp(buf, theConfig.ipaddr))
            {
                printf("reset %s-%s-%s-%s-%s-%s ip to %s...\n", theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext, buf);
                strcpy(theConfig.ipaddr, buf);
                saveCfg = true;
            }
            free(buf);
        }
    }
    else if (!theConfig.dhcp)
    {
        AddressBookGetDeviceInfo(&theDeviceInfo, theConfig.ipaddr);
    }

    if (theDeviceInfo.type == DEVICE_CENTER)
        theDeviceInfo.type = DEVICE_ENTRANCE;

    buf = AddressBookGetGateway(&theDeviceInfo);
    if (buf)
    {
        if (!theConfig.dhcp && strcmp(buf, theConfig.gw))
        {
            printf("reset gateway to %s...\n", buf);
            strcpy(theConfig.gw, buf);
            saveCfg = true;
        }
        free(buf);
    }

    buf = AddressBookGetNetMask(&theDeviceInfo);
    if (buf)
    {
        if (!theConfig.dhcp && strcmp(buf, theConfig.netmask))
        {
            printf("reset netmask to %s...\n", buf);
            strcpy(theConfig.netmask, buf);
            saveCfg = true;
        }
        free(buf);
    }

#ifdef ENABLE_VIDEO_MULTICAST
	if (theConfig.ipaddr[0])
	{
		buf = AddressBookGetMulticastGroup(theConfig.ipaddr);
		if (buf)
		{
			printf("set multicast group %s\n", buf);
			rtp_session_set_multicast_group(buf);
			free(buf);
		}
	}
#endif	

    if (saveCfg)
        ConfigSave();
}

void DeviceInfoInitByDhcp(char* ip)
{
    if (theConfig.local_pos[0] && theConfig.local_ext[0])
    {
        int len = strlen(theConfig.local_pos);

        theDeviceInfo.type = theConfig.local_type;

        if (len >= 2)
            strncpy(theDeviceInfo.area, &theConfig.local_pos[0], 2);
        else
            strcpy(theDeviceInfo.area, "00");

        if (len >= 4)
            strncpy(theDeviceInfo.building, &theConfig.local_pos[2], 2);
        else
            strcpy(theDeviceInfo.building, "00");

        if (len >= 6)
            strncpy(theDeviceInfo.unit, &theConfig.local_pos[4], 2);
        else
            strcpy(theDeviceInfo.unit, "00");

        strcpy(theDeviceInfo.floor, "00");
        strcpy(theDeviceInfo.room, "00");
        strcpy(theDeviceInfo.ext, theConfig.local_ext);
        strcpy(theDeviceInfo.ip, ip);
    }
    else
        AddressBookGetDeviceInfo(&theDeviceInfo, ip);

    if (theDeviceInfo.type == DEVICE_CENTER)
        theDeviceInfo.type = DEVICE_ENTRANCE;
}
