#include <stdio.h>
#include <string.h>
#include "ite/ith.h"
#include "doorbell.h"

// need to install VS90SP1-KB980263-x86.exe for vs2008
#pragma execution_character_set("utf-8")

#define STR_CAMARA "Camera"
#define STR_CAMERA_CHS "单元探头"
#define STR_CAMERA_CHT "單元探頭"

static const char* stringAddressArray[] =
{
    "Area",
    "Building",
    "Unit",
    "Floor",
    "Room",
    "Ext",
};

static const char* stringAddressArrayChs[] =
{
    "区",
    "栋",
    "单元",
    "层",
    "室",
    "分机",
};

static const char* stringAddressArrayCht[] =
{
    "區",
    "棟",
    "單元",
    "層",
    "室",
    "分機",
};

static const char* stringCallLogArray[] =
{
    "",
    "Missed",
    "Received",
    "Sent",
};

static const char* stringCallLogArrayChs[] =
{
    "",
    "未接",
    "已接",
    "已拨",
};

static const char* stringCallLogArrayCht[] =
{
    "",
    "未接",
    "已接",
    "已撥",
};

static const char* stringGuardEventArray[] =
{
    "Warning",
    "Open Door",
};

static const char* stringGuardEventArrayChs[] =
{
    "警报",
    "开门",
};

static const char* stringGuardEventArrayCht[] =
{
    "警報",
    "開門",
};

static const char* stringGuardTypeArray[] =
{
    "Emergency Alarm",
    "Infrared Alarm",
    "Door Alarm",
    "Window Alarm",
    "Smoke Alarm",
    "Gas Alarm",
    "Area Alarm",
    "Rob Alarm",
    "Open door Alarm",
};

static const char* stringGuardTypeArrayChs[] =
{
    "紧急异常",
    "红外异常",
    "门磁异常",
    "窗感异常",
    "烟感异常",
    "瓦斯异常",
    "周界异常",
    "劫持异常",
    "开门异常",
};

static const char* stringGuardTypeArrayCht[] =
{
    "緊急異常",
    "紅外異常",
    "門磁異常",
    "窗感異常",
    "煙感異常",
    "瓦斯異常",
    "周界異常",
    "劫持異常",
    "開門異常",
};

static const char* stringGuardStateArray[] =
{
    "Unhandled",
    "Handled",
};

static const char* stringGuardStateArrayChs[] =
{
    "未处理",
    "已处理",
};

static const char* stringGuardStateArrayCht[] =
{
    "未處理",
    "已處理",
};

static const char* stringSystemInfoArray[] =
{
    "Device Type",
    "Address",
    "Serial",
    "Local IP",
    "Outdoor IP",
    "Entrance IP",
    "Server IP",
    "Firmware Version",
    "Hardware Version",
    "Address Book Version",
    "Working Mode",
    "Outdoor Address Book Version",
};

static const char* stringSystemInfoArrayChs[] =
{
    "设备类型",
    "地址",
    "序列号",
    "本机IP",
    "门外机",
    "入口机",
    "服務器IP",
    "软件版本信息",
    "硬件版本信息",
    "地址簿版本",
    "本机工作模式",
    "门铃机地址簿",
};

static const char* stringSystemInfoArrayCht[] =
{
    "設備類型",
    "地址",
    "序列號",
    "本機IP",
    "門外機",
    "入口機",
    "服務器IP",
    "軟件版本信息",
    "硬件版本信息",
    "地址簿版本",
    "本機工作模式",
    "門鈴機地址簿",
};

static const char* stringDeviceTypes[] =
{
    "Control Server",
    "Door Camera",
    "Indoor Phone",
    "Labby Phone",
    "Wall",
    "Indoor Phone",
    "Administrator Unit",
    "Indoor Phone with SD card",
    "Mobile Phone",
    "Intercom",
    "IP Camera",
};

static const char* stringDeviceTypesChs[] =
{
    "管理中心",
    "门口机",
    "单元门口机",
    "栋门口机",
    "围墙机",
    "室内机",
    "管理中心机",
    "带SD卡的室内机",
    "手机",
    "紧急对讲机",
    "IP Camera",
};

static const char* stringDeviceTypesCht[] =
{
    "管理中心",
    "門口機",
    "單元門口機",
    "棟門口機",
    "圍牆機",
    "室內機",
    "管理中心機",
    "帶SD卡的室內機",
    "手機",
    "緊急對講機",
    "IP Camera",
};

static const char* stringWorkModeArray[] =
{
    "Normal",
    "Engineer",
};

static const char* stringWorkModeArrayChs[] =
{
    "正常模式",
    "工程模式",
};

static const char* stringWorkModeArrayCht[] =
{
    "正常模式",
    "工程模式",
};

char* StringGetRoomAddress(char* area, char* building, char* unit, char* floor, char* room, char* ext)
{
    char buf[64];

    if (theConfig.lang == LANG_CHS)
    {
        strcpy(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[0]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[1]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[2]);
        strcat(buf, " ");
        strcat(buf, floor);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[3]);
        strcat(buf, " ");
        strcat(buf, room);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[4]);

        if (ext)
        {
            strcat(buf, " ");
            strcat(buf, ext);
            strcat(buf, stringAddressArrayChs[5]);
        }
    }
    else if (theConfig.lang == LANG_CHT)
    {
        strcpy(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[0]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[1]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[2]);
        strcat(buf, " ");
        strcat(buf, floor);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[3]);
        strcat(buf, " ");
        strcat(buf, room);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[4]);

        if (ext)
        {
            strcat(buf, " ");
            strcat(buf, ext);
            strcat(buf, stringAddressArrayCht[5]);
        }
    }
    else
    {
        strcpy(buf, stringAddressArray[0]);
        strcat(buf, " ");
        strcat(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[1]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[2]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[3]);
        strcat(buf, " ");
        strcat(buf, floor);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[4]);
        strcat(buf, " ");
        strcat(buf, room);

        if (ext)
        {
            strcat(buf, " ");
            strcat(buf, stringAddressArray[5]);
            strcat(buf, " ");
            strcat(buf, ext);
        }
    }
    return strdup(buf);
}

char* StringGetCameraPosition(char* area, char* building, char* unit)
{
    char buf[64];

    if (theConfig.lang == LANG_CHS)
    {
        strcpy(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[0]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[1]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayChs[2]);
        strcat(buf, " " STR_CAMERA_CHS);
    }
    else if (theConfig.lang == LANG_CHT)
    {
        strcpy(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[0]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[1]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " ");
        strcat(buf, stringAddressArrayCht[2]);
        strcat(buf, " " STR_CAMERA_CHT);
    }
    else
    {
        strcpy(buf, stringAddressArray[0]);
        strcat(buf, " ");
        strcat(buf, area);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[1]);
        strcat(buf, " ");
        strcat(buf, building);
        strcat(buf, " ");
        strcat(buf, stringAddressArray[2]);
        strcat(buf, " ");
        strcat(buf, unit);
        strcat(buf, " " STR_CAMARA);
    }
    return strdup(buf);
}

const char* StringGetSystemInfoArray(int* count)
{
    if (theConfig.lang == LANG_CHS)
    {
        if (count)
            *count = ITH_COUNT_OF(stringSystemInfoArrayChs);

        return (const char*)stringSystemInfoArrayChs;
    }
    else if (theConfig.lang == LANG_CHT)
    {
        if (count)
            *count = ITH_COUNT_OF(stringSystemInfoArrayCht);

        return (const char*)stringSystemInfoArrayCht;
    }
    else
    {
        if (count)
            *count = ITH_COUNT_OF(stringSystemInfoArray);

        return (const char*)stringSystemInfoArray;
    }
}

const char* StringGetDeviceType(DeviceType type)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringDeviceTypesChs[type];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringDeviceTypesCht[type];
    }
    else
    {
        return stringDeviceTypes[type];
    }
}

const char* StringGetCallLogType(CallLogType type)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringCallLogArrayChs[type];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringCallLogArrayCht[type];
    }
    else
    {
        return stringCallLogArray[type];
    }
}

const char* StringGetGuardEvent(GuardEvent ev)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringGuardEventArrayChs[ev];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringGuardEventArrayCht[ev];
    }
    else
    {
        return stringGuardEventArray[ev];
    }
}

const char* StringGetGuardType(GuardType type)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringGuardTypeArrayChs[type];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringGuardTypeArrayCht[type];
    }
    else
    {
        return stringGuardTypeArray[type];
    }
}

const char* StringGetGuardState(GuardState state)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringGuardStateArrayChs[state];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringGuardStateArrayCht[state];
    }
    else
    {
        return stringGuardStateArray[state];
    }
}

const char* StringGetWorkMode(bool engineer)
{
    if (theConfig.lang == LANG_CHS)
    {
        return stringWorkModeArrayChs[engineer ? 1 : 0];
    }
    else if (theConfig.lang == LANG_CHT)
    {
        return stringWorkModeArrayCht[engineer ? 1 : 0];
    }
    else
    {
        return stringWorkModeArray[engineer ? 1 : 0];
    }
}
