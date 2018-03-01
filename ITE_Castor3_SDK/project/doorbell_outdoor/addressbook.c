#include <sys/ioctl.h>
#include <sys/socket.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "libxml/xpath.h"
#include "ite/itp.h"
#include "ucl/ucl.h"
#include "doorbell.h"

#define MAX_ADDRESSBOOK 24

typedef struct
{
    uint8_t room[6];
    uint8_t type;
    uint8_t padding;
    uint32_t ip;
    uint32_t group;
} ABDevice;

static int abDeviceCount;
static ABDevice* abDevices;
static char abVersion[32];
static char abGateway[32];
static char abNetMask[32];

static DeviceType _ToDeviceType(xmlChar* value)
{
    switch (*value)
    {
    case '0':
        return DEVICE_CENTER;

    case '1':
        if (value[1] == '0')
            return DEVICE_IPCAM;
        else
            return DEVICE_OUTDOOR;

    case '2':
        return DEVICE_ENTRANCE;

    case '3':
        return DEVICE_BUILDING;

    case '4':
        return DEVICE_WALL;

    case '5':
        return DEVICE_INDOOR;

    case '6':
        return DEVICE_MANAGER;

    case '7':
        return DEVICE_INDOOR2;

    case '8':
        return DEVICE_MOBILE;

    case '9':
        return DEVICE_INTERCOM;
    }

    return DEVICE_UNKNOWN;
}

static void GetDeviceInfo(DeviceInfo* info, ABDevice* dev)
{
    char* ip;

    info->type = dev->type;

    sprintf(info->area, "%02d", dev->room[0]);
    sprintf(info->building, "%02d", dev->room[1]);
    sprintf(info->unit, "%02d", dev->room[2]);
    sprintf(info->floor, "%02d", dev->room[3]);
    sprintf(info->room, "%02d", dev->room[4]);
    sprintf(info->ext, "%02d", dev->room[5]);

    info->alias[0] = '\0';

    ip = ipaddr_ntoa((const ip_addr_t *)&dev->ip);
    strcpy(info->ip, ip);
}

static uint32_t DcpsGetSize( uint8_t *Src, uint32_t SrcLen )
{
	uint32_t DcpsSize=0;
	uint32_t i=0;
	uint32_t out_len=0;
	uint32_t in_len=0;

	while(i<SrcLen)
	{
		out_len = ((Src[i+3]) | (Src[i+2]<<8) | (Src[i+1]<<16) | (Src[i]<<24));
        i=i+4;
        in_len = ((Src[i+3]) | (Src[i+2]<<8) | (Src[i+1]<<16) | (Src[i]<<24));
        i=i+4;

        if (out_len == 0)
			break;
		   
        if( in_len < out_len)	DcpsSize += out_len;
        else					DcpsSize += in_len;	

		i += in_len;
	}
	return	DcpsSize;	
}

static ucl_uint32 xread32(unsigned char* b)
{
    ucl_uint32 v;

    v  = (ucl_uint32) b[3] <<  0;
    v |= (ucl_uint32) b[2] <<  8;
    v |= (ucl_uint32) b[1] << 16;
    v |= (ucl_uint32) b[0] << 24;
    return v;
}

static int SoftwareDecompress(uint8_t*outbuf, uint8_t* inbuf)
{
    ucl_uint block_size = xread32(inbuf + 14);
    ucl_bytep in = inbuf + 18;
    ucl_bytep out = outbuf;
    int ret;

    ret = ucl_init();
    assert(ret == UCL_E_OK);

    for (;;)
    {
        ucl_uint in_len;
        ucl_uint out_len;

        /* read uncompressed size */
        out_len = xread32(in);
        in += 4;

        /* exit if last block (EOF marker) */
        if (out_len == 0)
            break;

        /* read compressed size */
        in_len = xread32(in);
        in += 4;

        /* sanity check of the size values */
        if (in_len > block_size || out_len > block_size ||
            in_len == 0 || in_len > out_len)
        {
            printf("block size error - data corrupted\n");
            goto error;
        }

        if (in_len < out_len)
        {
            /* decompress - use safe decompressor as data might be corrupted */
            ucl_uint new_len = out_len;

            ret = ucl_nrv2e_decompress_8(in,in_len,out,&new_len,NULL);

            if (ret != UCL_E_OK || new_len != out_len)
            {
                printf("compressed data violation: error %d (%ld/%ld/%ld)\n", ret, (long) in_len, (long) out_len, (long) new_len);
                goto error;
            }
            out += out_len;
        }
        else
        {
            /* write original (incompressible) block */
            memcpy(out, in, in_len);
            out += in_len;
        }
        in += in_len;
    }
    return 0;

error:
    return -1;
}

static void Parse_UCL2MEM(char *ucl_filename)
{
    char *filepath[64];
    FILE *f = NULL;
    uint8_t *xmlbuf = NULL, *filebuf = NULL;
    int readsize, compressedSize, xmlsize, ret;
    struct stat sb;
    xmlDocPtr doc = NULL;
    xmlXPathContext* xpathCtx = NULL;
    xmlXPathObject* xpathObj = NULL;
    xmlChar* value = NULL;
    char buf[128];

    sprintf(filepath, CFG_PRIVATE_DRIVE":/%s", ucl_filename);

    f = fopen(filepath, "rb");
    if (!f)
    {
        printf("file open %s fail\n", filepath);
        goto error;
    }

    if (fstat(fileno(f), &sb) == -1)
    {
        printf("get file size fail\n");
        goto error;
    }

    filebuf = malloc(sb.st_size);
    if (!filebuf) {
        goto error;
    }

    readsize = fread(filebuf, 1, sb.st_size, f);
    assert(readsize == sb.st_size);

    fclose(f);
    f = NULL;

    compressedSize = sb.st_size - 18;
    xmlsize = DcpsGetSize(filebuf + 18, compressedSize);

    xmlbuf = malloc(xmlsize + xmlsize / 8 + 256);
    if (!xmlbuf) {
        goto error;
    }

#ifdef CFG_DCPS_ENABLE
    // hardware decompress
    compressedSize -= 8;
    ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_INIT, NULL);
    ret = write(ITP_DEVICE_DECOMPRESS, filebuf + 18, compressedSize);
    assert(ret == compressedSize);
    ret = read(ITP_DEVICE_DECOMPRESS, xmlbuf, xmlsize);
    assert(ret == xmlsize);
    ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_EXIT, NULL);

#else
    // software decompress
    ret = SoftwareDecompress(xmlbuf, filebuf);
    assert(ret == 0);

#endif // CFG_DCPS_ENABLE

    free(filebuf);
    filebuf = NULL;

    doc = xmlParseMemory(xmlbuf, xmlsize);
    if (!doc)
    {
        printf("parse xml fail\n");
        goto error;
    }
    free(xmlbuf);
    xmlbuf = NULL;

    xpathCtx = xmlXPathNewContext(doc);
    if (!xpathCtx)
    {
        printf("create xml context fail\n");
        goto error;
    }

    // get devices
    xpathObj = xmlXPathEvalExpression(BAD_CAST "/AddrList/dev", xpathCtx);
    if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
    { //realloc
        int tmp_abDeviceCount = abDeviceCount;
        int k = 0, j = 0;
        ABDevice* tmp_abDevices = NULL;

        abDeviceCount = xpathObj->nodesetval->nodeNr + tmp_abDeviceCount;

        if(abDevices == NULL) {
            abDevices = malloc(abDeviceCount * sizeof(ABDevice));
        }
        else {
            tmp_abDevices = realloc(abDevices, abDeviceCount * sizeof(ABDevice));
            abDevices = tmp_abDevices;
        }
        if (!abDevices) {
            printf("abDevices malloc/realloc error, abDeviceCount=%d\n", abDeviceCount);
            goto error;
        }

        for(j = tmp_abDeviceCount; j < abDeviceCount; j++)
        {
            ABDevice* dev = &abDevices[j];
            xmlNode* node = xpathObj->nodesetval->nodeTab[k];
            xmlAttr* attribute = node->properties;
            while (attribute && attribute->name && attribute->children)
            {
                if (strcmp(attribute->name, "ty") == 0)
                {
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    dev->type = _ToDeviceType(value);
                    xmlFree(value);
                }
                else if (strcmp(attribute->name, "ro") == 0)
                {
                    int area, building, unit, floor, room, ext;
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    sscanf(value, "%d-%d-%d-%d-%d-%d", &area, &building, &unit, &floor, &room, &ext);
                    dev->room[0] = (uint8_t)area;
                    dev->room[1] = (uint8_t)building;
                    dev->room[2] = (uint8_t)unit;
                    dev->room[3] = (uint8_t)floor;
                    dev->room[4] = (uint8_t)room;
                    dev->room[5] = (uint8_t)ext;
                    xmlFree(value);
                }
#if 0
                else if (strcmp(attribute->name, "alias") == 0)
                {
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    strcpy(dev->alias, value);
                    xmlFree(value);
                }
#endif
                else if (strcmp(attribute->name, "ip") == 0)
                {
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    dev->ip = ipaddr_addr(value);
                    xmlFree(value);
                }
                else if (strcmp(attribute->name, "group") == 0)
                {
                    value = xmlNodeListGetString(node->doc, attribute->children, 1);
                    dev->group = ipaddr_addr(value);
                    xmlFree(value);
                }
                attribute = attribute->next;
            }
            k++;
        }

    }
    xmlXPathFreeObject(xpathObj);

    // get version
    if(abVersion[0] == '\0') {
        xpathObj = xmlXPathEvalExpression(BAD_CAST "/AddrList/@ver", xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            value = xmlNodeListGetString(doc, node->children, 1);
            strcpy(abVersion, value);
            xmlFree(value);
        }
        xmlXPathFreeObject(xpathObj);
    }

    // get gateway
    if(abGateway[0] == '\0') {
        sprintf(buf, "/AddrList//dev[@ro='%s-%s-%s-%s-%s-%s']/@gw", theConfig.area, theConfig.building, theConfig.unit, theConfig.floor, theConfig.room, theConfig.ext);
        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            value = xmlNodeListGetString(doc, node->children, 1);
            strcpy(abGateway, value);
            xmlFree(value);
        }
        xmlXPathFreeObject(xpathObj);
    }

    // get netmask
    if(abNetMask[0] == '\0') {
        sprintf(buf, "/AddrList//dev[@ro='%s-%s-%s-%s-%s-%s']/@sm", theConfig.area, theConfig.building, theConfig.unit, theConfig.floor, theConfig.room, theConfig.ext);
        xpathObj = xmlXPathEvalExpression(BAD_CAST buf, xpathCtx);
        if (!xmlXPathNodeSetIsEmpty(xpathObj->nodesetval))
        {
            xmlNode* node = xpathObj->nodesetval->nodeTab[0];
            value = xmlNodeListGetString(doc, node->children, 1);
            strcpy(abNetMask, value);
            xmlFree(value);
        }
        xmlXPathFreeObject(xpathObj);
    }


    xmlXPathFreeContext(xpathCtx);
    xmlFreeDoc(doc);

    return;

error:
    if (abDevices)
    {
        free(abDevices);
        abDevices = NULL;
    }
    abDeviceCount = 0;

    if (xpathObj)
        xmlXPathFreeObject(xpathObj);

    if (xpathCtx)
        xmlXPathFreeContext(xpathCtx);

    if (doc)
        xmlFreeDoc(doc);

    if (xmlbuf)
        free(xmlbuf);

    if (filebuf)
        free(filebuf);

    if (f)
        fclose(f);
}

void RemoveAddressbook(void)
{
    struct stat sb;
    char *filepath[64];
    int i = 0;

    for(i = 1; i <= MAX_ADDRESSBOOK; i++) {
        sprintf(filepath, CFG_PRIVATE_DRIVE":/addressbook%d.ucl", i);
        if(stat(filepath, &sb) == 0) {
            remove(filepath);
        }
        else {
            break;
        }
    }
}

static int CountAddressbook(void)
{
    struct stat sb;
    char *filepath[64];
    int i=0;

    for(i= 1; i <= MAX_ADDRESSBOOK; i++) {
        sprintf(filepath, CFG_PRIVATE_DRIVE":/addressbook%d.ucl", i);
        if(stat(filepath, &sb) != 0) {
            break;
        }
    }

    return i-1;
}


void AddressBookInit(void)
{
    int k;
    char ucl_filename[32];
    int addressbook_count = 0;

    AddressBookExit();

    abVersion[0] = abGateway[0] = abNetMask[0] = '\0';

    Parse_UCL2MEM("addressbook.ucl");

    /*Big Addressbook Support*/
    addressbook_count = CountAddressbook();
    for(k = 1;k <= addressbook_count; k++) {
        sprintf(ucl_filename, "addressbook%d.ucl", k);
        Parse_UCL2MEM(ucl_filename);
    }
}

void AddressBookExit(void)
{
    if (abDevices)
    {
        free(abDevices);
        abDevices = NULL;
    }
    abDeviceCount = 0;
}

char* AddressBookGetVersion(void)
{
    if (abVersion[0] == '\0')
        return NULL;

    return strdup(abVersion);
}

void AddressBookGetCenterInfo(DeviceInfo* info)
{
    int i;

    memset(info, 0, sizeof (DeviceInfo));

    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_CENTER)
        {
            GetDeviceInfo(info, dev);
            return;
        }
    }
}

void AddressBookGetDeviceInfo(DeviceInfo* info, char* ip)
{
    uint32_t ipaddr = ipaddr_addr(ip);
    int i;

    memset(info, 0, sizeof (DeviceInfo));

    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->ip == ipaddr)
        {
            GetDeviceInfo(info, dev);
            return;
        }
    }
}

void AddressBookGetEntranceInfo(DeviceInfo* info, DeviceInfo* deviceInfo)
{
    int area, building, unit, i;

    memset(info, 0, sizeof (DeviceInfo));

    area = atoi(deviceInfo->area);
    building = atoi(deviceInfo->building);
    unit = atoi(deviceInfo->unit);

    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_ENTRANCE && dev->room[0] == area && dev->room[1] == building && dev->room[2] == unit)
        {
            GetDeviceInfo(info, dev);
            return;
        }
    }
}

void AddressBookGetOutdoorInfo(DeviceInfo* info, DeviceInfo* deviceInfo)
{
    int area, building, unit, floor, room, i;

    memset(info, 0, sizeof (DeviceInfo));

    area = atoi(deviceInfo->area);
    building = atoi(deviceInfo->building);
    unit = atoi(deviceInfo->unit);
    floor = atoi(deviceInfo->floor);
    room = atoi(deviceInfo->room);

    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_OUTDOOR && dev->room[0] == area && dev->room[1] == building && dev->room[2] == unit && dev->room[3] == floor && dev->room[4] == room)
        {
            GetDeviceInfo(info, dev);
            return;
        }
    }
}

char* AddressBookGetDeviceIP(char* area, char* building, char* unit, char* floor, char* room, char* ext)
{
    int area_i, building_i, unit_i, floor_i, room_i, i;
    char* ip;

    area_i = atoi(area);
    building_i = atoi(building);
    unit_i = atoi(unit);
    floor_i = atoi(floor);
    room_i = atoi(room);

    if (ext)
    {
        int ext_i = atoi(ext);

        for (i = 0; i < abDeviceCount; i++)
        {
            ABDevice* dev = &abDevices[i];

            if (dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i && dev->room[3] == floor_i && dev->room[4] == room_i && dev->room[5] == ext_i)
            {
                ip = ipaddr_ntoa((const ip_addr_t *)&dev->ip);
                return strdup(ip);
            }
        }
    }
    else
    {
        for (i = 0; i < abDeviceCount; i++)
        {
            ABDevice* dev = &abDevices[i];

            if (dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i && dev->room[3] == floor_i && dev->room[4] == room_i)
            {
                ip = ipaddr_ntoa((const ip_addr_t *)&dev->ip);
                return strdup(ip);
            }
        }
    }
    return NULL;
}

int AddressBookGetDeviceInfoArray(DeviceInfo infoArray[], int count, char* area, char* building, char* unit, char* floor, char* room)
{
    int area_i, building_i, unit_i, floor_i, room_i, i, j;

    assert(infoArray);

    area_i = atoi(area);
    building_i = atoi(building);
    unit_i = atoi(unit);

    j = 0;
    if (floor && room)
    {
        floor_i = atoi(floor);
        room_i = atoi(room);

        for (i = 0; i < abDeviceCount; i++)
        {
            ABDevice* dev = &abDevices[i];

            if ((dev->type == DEVICE_INDOOR || dev->type == DEVICE_INDOOR2 || dev->type == DEVICE_MOBILE) && dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i && dev->room[3] == floor_i && dev->room[4] == room_i)
            {
                DeviceInfo* info = &infoArray[j];
                GetDeviceInfo(info, dev);
                j++;

                if (j >= count)
                    return j;
            }
        }
    }
    else
    {
        for (i = 0; i < abDeviceCount; i++)
        {
            ABDevice* dev = &abDevices[i];

            if ((dev->type == DEVICE_INDOOR || dev->type == DEVICE_INDOOR2 || dev->type == DEVICE_MOBILE) && dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i)
            {
                DeviceInfo* info = &infoArray[j];
                GetDeviceInfo(info, dev);
                j++;

                if (j >= count)
                    return j;
            }
        }
    }
    return j;
}

int AddressBookGetCameraInfoArray(DeviceInfo infoArray[], int count)
{
    int i, j;

    assert(infoArray);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_OUTDOOR || dev->type == DEVICE_ENTRANCE || dev->type == DEVICE_BUILDING || dev->type == DEVICE_WALL || dev->type == DEVICE_INTERCOM || dev->type == DEVICE_IPCAM)
        {
            DeviceInfo* info = &infoArray[j];
            GetDeviceInfo(info, dev);
            j++;

            if (j >= count)
                return j;
        }
    }
    return j;
}

int AddressBookGetLobbyInfoArray(DeviceInfo infoArray[], int count)
{
    int i, j;

    assert(infoArray);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_OUTDOOR || dev->type == DEVICE_ENTRANCE)
        {
            DeviceInfo* info = &infoArray[j];
            GetDeviceInfo(info, dev);
            j++;

            if (j >= count)
                return j;
        }
    }
    return j;
}

int AddressBookGetManagerInfoArray(DeviceInfo infoArray[], int count)
{
    int i, j;

    assert(infoArray);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_MANAGER)
        {
            DeviceInfo* info = &infoArray[j];
            GetDeviceInfo(info, dev);
            j++;

            if (j >= count)
                return j;
        }
    }
    return j;
}

int AddressBookGetIPCameraInfoArray(DeviceInfo infoArray[], int count)
{
    int i, j;

    assert(infoArray);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_IPCAM)
        {
            DeviceInfo* info = &infoArray[j];
            GetDeviceInfo(info, dev);
            j++;

            if (j >= count)
                return j;
        }
    }
    return j;
}

char* AddressBookGetGateway(DeviceInfo* info)
{
    if (abGateway[0] == '\0')
        return NULL;

    return strdup(abGateway);
}

char* AddressBookGetNetMask(DeviceInfo* info)
{
    if (abNetMask[0] == '\0')
        return NULL;

    return strdup(abNetMask);
}

bool AddressBookIsEntranceExist(DeviceType type, char* pos, char* ext)
{
    char area[4], building[4], unit[4];
    int area_i, building_i, unit_i, ext_i, i, j;

    if (!pos || !ext || strlen(pos) != 6 || strlen(ext) != 2)
        return false;

    strncpy(area, &pos[0], 2);
    area[2] = '\0';
    strncpy(building, &pos[2], 2);
    building[2] = '\0';
    strncpy(unit, &pos[4], 2);
    unit[2] = '\0';

    area_i = atoi(area);
    building_i = atoi(building);
    unit_i = atoi(unit);
    ext_i = atoi(ext);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == type && dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i && dev->room[5] == ext_i)
        {
            return true;
        }
    }
    return false;
}

int AddressBookGetOutdoorInfoArray(DeviceInfo infoArray[], int count, char* area, char* building, char* unit, char* floor, char* room)
{
    int area_i, building_i, unit_i, floor_i, room_i, i, j;

    assert(infoArray);

    area_i = atoi(area);
    building_i = atoi(building);
    unit_i = atoi(unit);
    floor_i = atoi(floor);
    room_i = atoi(room);

    j = 0;
    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->type == DEVICE_OUTDOOR && dev->room[0] == area_i && dev->room[1] == building_i && dev->room[2] == unit_i && dev->room[3] == floor_i && dev->room[4] == room_i)
        {
            DeviceInfo* info = &infoArray[j];
            GetDeviceInfo(info, dev);
            j++;

            if (j >= count)
                return j;
        }
    }
    return j;
}

#ifdef ENABLE_VIDEO_MULTICAST
char* AddressBookGetMulticastGroup(char* ipaddr)
{
    uint32_t ip = ipaddr_addr(ipaddr);
    int i;

    for (i = 0; i < abDeviceCount; i++)
    {
        ABDevice* dev = &abDevices[i];

        if (dev->ip == ip)
        {
            char* buf = ipaddr_ntoa((const ip_addr_t *)&dev->group);
            return strdup(buf);
        }
    }
    return NULL;
}
#endif

#if 0
void AddressBookExport(void)
{
    FILE* f = NULL;
    uint8_t* filebuf = NULL;
    uint8_t* xmlbuf = NULL;
    ITPDriveStatus* driveStatusTable;
    ITPDriveStatus* driveStatus = NULL;
    int i;

    ioctl(ITP_DEVICE_DRIVE, ITP_IOCTL_GET_TABLE, &driveStatusTable);

    for (i = ITP_MAX_DRIVE - 1; i >= 0; i--)
    {
        driveStatus = &driveStatusTable[i];
        if (driveStatus->avail && 
            (driveStatus->disk >= ITP_DISK_MSC00 && driveStatus->disk <= ITP_DISK_MSC17) ||
            (driveStatus->disk >= ITP_DISK_SD0 && driveStatus->disk <= ITP_DISK_SD1))
        {
            char buf[PATH_MAX];
            struct stat sb;
            int readsize, compressedSize, xmlsize, ret;

            strcpy(buf, driveStatus->name);
            strcat(buf, "addressbook.xml");

            f = fopen(CFG_PRIVATE_DRIVE ":/addressbook.ucl", "rb");
            if (!f)
            {
                printf("file open %s fail\n", CFG_PRIVATE_DRIVE ":/addressbook.ucl");
                goto error;
            }

            if (fstat(fileno(f), &sb) == -1)
            {
                printf("get file size fail\n");
                goto error;
            }
             
            filebuf = malloc(sb.st_size);
            if (!filebuf)
                goto error;

            readsize = fread(filebuf, 1, sb.st_size, f);
            assert(readsize == sb.st_size);

            fclose(f);
            f = NULL;

            compressedSize = sb.st_size - 18;
            xmlsize = DcpsGetSize(filebuf + 18, compressedSize);
            
            xmlbuf = malloc(xmlsize + xmlsize / 8 + 256);
            if (!xmlbuf)
                goto error;

        #ifdef CFG_DCPS_ENABLE
            // hardware decompress
            ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_INIT, NULL);
            ret = write(ITP_DEVICE_DECOMPRESS, filebuf + 18, compressedSize);
            assert(ret == compressedSize);
            ret = read(ITP_DEVICE_DECOMPRESS, xmlbuf, xmlsize);
            assert(ret == xmlsize);
            ioctl(ITP_DEVICE_DECOMPRESS, ITP_IOCTL_EXIT, NULL);

        #else
            // software decompress
            ret = SoftwareDecompress(xmlbuf, filebuf);
            assert(ret == 0);

        #endif // CFG_DCPS_ENABLE

            free(filebuf);
            filebuf = NULL;

            f = fopen(buf, "wb");
            if (!f)
            {
                printf("file open %s fail\n", buf);
                goto error;
            }

            ret = fwrite(xmlbuf, 1, xmlsize, f);
            assert(ret == xmlsize);

            free(xmlbuf);

            fclose(f);
            f = NULL;

            printf("save to %s\n", buf);
            return;
        }
    }
    printf("cannot find USB or SD disk.\n");
    return;

error:
    if (xmlbuf)
        free(xmlbuf);

    if (filebuf)
        free(filebuf);

    if (f)
        fclose(f);
}
#endif
