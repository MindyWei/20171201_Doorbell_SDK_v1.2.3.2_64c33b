#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite/itp.h"
#include "lwip/ip.h"
#include "scene.h"
#include "doorbell.h"

static ITUScrollListBox* settingSystemItemScrollListBox;
static ITUScrollListBox* settingSystemMessageScrollListBox;

static int settingSystemInfoCount;
static const char** settingSystemInfoArray;

bool SettingSystemItemScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;

    if (settingSystemInfoCount == 0)
        settingSystemInfoArray = (const char**)StringGetSystemInfoArray(&settingSystemInfoCount);

    entryCount = settingSystemInfoCount;
    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        const char* s = settingSystemInfoArray[j];
        ituTextSetString(scrolltext, s);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingSystemMessageScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;
#ifdef CFG_NET_WIFI
	ITPWifiInfo netInfo;
	char* ip;
#endif

    if (settingSystemInfoCount == 0)
        settingSystemInfoArray = (const char**)StringGetSystemInfoArray(&settingSystemInfoCount);

    entryCount = settingSystemInfoCount;
    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        char* ptr;
        char buf[8];

        switch (j)
        {
        case 0: // Device Type
            ptr = (char*)StringGetDeviceType(theDeviceInfo.type);
            break;

        case 1: // Address
            ptr = StringGetRoomAddress(theDeviceInfo.area, theDeviceInfo.building, theDeviceInfo.unit, theDeviceInfo.floor, theDeviceInfo.room, theDeviceInfo.ext);
            break;

        case 2: // Serial
            ptr = "********";
            break;

        case 3: // Local IP
        #ifdef CFG_NET_WIFI
            ioctl(ITP_DEVICE_WIFI, ITP_IOCTL_GET_INFO, &netInfo);
    		ip = ipaddr_ntoa((const ip_addr_t*)&netInfo.address);
			printf("###ip: %s\n", ip);
			strcpy(theConfig.dhcp_ipaddr, ip);
			ptr = theConfig.dhcp_ipaddr;
		#else
            ptr = theDeviceInfo.ip;
		#endif
            break;

        case 4: // Outdoor IP
            ptr = theOutdoorInfo.ip;
            break;

        case 5: // Entrance IP
            ptr = theEntranceInfo.ip;
            break;

        case 6: // Server IP
            ptr = theConfig.gw;
            break;

        case 7: // Firmware Version
            ptr = CFG_VERSION_MAJOR_STR "." CFG_VERSION_MINOR_STR "." CFG_VERSION_PATCH_STR "." CFG_VERSION_CUSTOM_STR "." CFG_VERSION_TWEAK_STR;
            break;

        case 8: // Hardware Version
            ptr = CFG_HW_VERSION;
            break;

        case 9: // Address Book Version
            ptr = AddressBookGetVersion();
            break;

        case 10: // Working Mode
            ptr = (char*)StringGetWorkMode(DeviceInfoIsInEngineerMode());
            break;

        case 11: // Outdoor Address Book Version
            sprintf(buf, "%d", theOutdoorServerInfo.addressBookVersion);
            ptr = buf;
            break;
        }
        ituTextSetString(scrolltext, ptr);

        if (j == 1 || j == 9)
            free(ptr);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool SettingSysInfoOnEnter(ITUWidget* widget, char* param)
{
    if (!settingSystemItemScrollListBox)
    {
        settingSystemItemScrollListBox = ituSceneFindWidget(&theScene, "settingSystemItemScrollListBox");
        assert(settingSystemItemScrollListBox);

        settingSystemMessageScrollListBox = ituSceneFindWidget(&theScene, "settingSystemMessageScrollListBox");
        assert(settingSystemMessageScrollListBox);
    }
    return true;
}

void SettingSysInfoReset(void)
{
    settingSystemItemScrollListBox = NULL;
}
