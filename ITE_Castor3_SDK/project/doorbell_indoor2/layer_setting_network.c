#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ite/itp.h"
#include "scene.h"
#include "doorbell.h"

static ITUSprite* settingNetworkIPAssignSprite;
static ITUText* settingNetworkIPAddrText;
static ITUText* settingNetworkNetmaskText;
static ITUText* settingNetworkDNSText;
static ITUText* settingNetworkMacText;
static ITUText* settingNetworkGatewayText;
static ITUBackground* settingNetworkGrayBackground;
static ITURadioBox* settingNetworkStaticIPRadioBox;
static ITURadioBox* settingNetworkDHCPRadioBox;
static ITUBackground* settingBackground;
static ITUBackground* settingNetworkBackground;
static ITUBackgroundButton* settingNetworkIPAssignBackgroundButton;
static ITUBackground* mainTopBackground;
static ITUBackgroundButton* settingNetworkIPInputBackgroundButton;	
static ITUTextBox* settingNetworkIPInput1TextBox;
static ITUTextBox* settingNetworkIPInput2TextBox;
static ITUTextBox* settingNetworkIPInput3TextBox;
static ITUTextBox* settingNetworkIPInput4TextBox;
static ITUBackgroundButton* settingNetworkMacInputBackgroundButton;	
static ITUTextBox* settingNetworkMacInputTextBox;

// status
static int settingNetworkIPAssignOld;
static char settingNetworkIPAddrOld[16];
static char settingNetworkNetmaskOld[16];
static char settingNetworkDnsOld[16];
static char settingNetworkMacOld[32];
static char settingNetworkGatewayOld[16];
static ITUText* activeSettingNetworkText;
static ITUTextBox* activeSettingNetworkIPInputTextBox;
static ITPEthernetInfo settingNetworkEthernetInfo;

bool SettingNetworkIPAssignRadioBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituRadioBoxIsChecked(settingNetworkStaticIPRadioBox))
    {
        theConfig.dhcp = 0;
        ituSpriteGoto(settingNetworkIPAssignSprite, 0);
    }
    else if (ituRadioBoxIsChecked(settingNetworkDHCPRadioBox))
    {
        theConfig.dhcp = 1;
        ituSpriteGoto(settingNetworkIPAssignSprite, 1);
    }
    return true;
}

bool SettingNetworkIPAddrButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[16];
    char *p, *saveptr;

    strcpy(buf, theConfig.ipaddr);

    activeSettingNetworkText = settingNetworkIPAddrText;
    activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    p = strtok_r(buf, ".", &saveptr);
    if (p)
    {
        ituTextBoxSetString(settingNetworkIPInput1TextBox, p);
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
        p = strtok_r(NULL, ".", &saveptr);
        if (p)
        {
            ituTextBoxSetString(settingNetworkIPInput2TextBox, p);
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
            p = strtok_r(NULL, ".", &saveptr);
            if (p)
            {
                ituTextBoxSetString(settingNetworkIPInput3TextBox, p);
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
                p = strtok_r(NULL, ".", &saveptr);
                if (p)
                {
                    ituTextBoxSetString(settingNetworkIPInput4TextBox, p);
                }
            }
        }
    }
    return true;
}

bool SettingNetworkNetmaskButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[16];
    char *p, *saveptr;

    strcpy(buf, theConfig.netmask);

    activeSettingNetworkText = settingNetworkNetmaskText;
    activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    p = strtok_r(buf, ".", &saveptr);
    if (p)
    {
        ituTextBoxSetString(settingNetworkIPInput1TextBox, p);
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
        p = strtok_r(NULL, ".", &saveptr);
        if (p)
        {
            ituTextBoxSetString(settingNetworkIPInput2TextBox, p);
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
            p = strtok_r(NULL, ".", &saveptr);
            if (p)
            {
                ituTextBoxSetString(settingNetworkIPInput3TextBox, p);
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
                p = strtok_r(NULL, ".", &saveptr);
                if (p)
                {
                    ituTextBoxSetString(settingNetworkIPInput4TextBox, p);
                }
            }
        }
    }
    return true;
}

bool SettingNetworkDNSButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[16];
    char *p, *saveptr;

    strcpy(buf, theConfig.dns);

    activeSettingNetworkText = settingNetworkDNSText;
    activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    p = strtok_r(buf, ".", &saveptr);
    if (p)
    {
        ituTextBoxSetString(settingNetworkIPInput1TextBox, p);
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
        p = strtok_r(NULL, ".", &saveptr);
        if (p)
        {
            ituTextBoxSetString(settingNetworkIPInput2TextBox, p);
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
            p = strtok_r(NULL, ".", &saveptr);
            if (p)
            {
                ituTextBoxSetString(settingNetworkIPInput3TextBox, p);
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
                p = strtok_r(NULL, ".", &saveptr);
                if (p)
                {
                    ituTextBoxSetString(settingNetworkIPInput4TextBox, p);
                }
            }
        }
    }
    return true;
}

bool SettingNetworkMacButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextBoxSetString(settingNetworkMacInputTextBox, ituTextGetString(settingNetworkMacText));
    return true;
}

bool SettingNetworkGatewayButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[16];
    char *p, *saveptr;

    strcpy(buf, theConfig.gw);

    activeSettingNetworkText = settingNetworkGatewayText;
    activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    p = strtok_r(buf, ".", &saveptr);
    if (p)
    {
        ituTextBoxSetString(settingNetworkIPInput1TextBox, p);
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
        p = strtok_r(NULL, ".", &saveptr);
        if (p)
        {
            ituTextBoxSetString(settingNetworkIPInput2TextBox, p);
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
            p = strtok_r(NULL, ".", &saveptr);
            if (p)
            {
                ituTextBoxSetString(settingNetworkIPInput3TextBox, p);
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
                p = strtok_r(NULL, ".", &saveptr);
                if (p)
                {
                    ituTextBoxSetString(settingNetworkIPInput4TextBox, p);
                }
            }
        }
    }
    return true;
}

bool SettingNetworkIPInputNumberButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = param;
    char* str = ituTextGetString(activeSettingNetworkIPInputTextBox);
    int count = str ? strlen(str) : 0;

    if (ituWidgetIsVisible(settingNetworkMacInputBackgroundButton))
    {
        ituTextBoxInput(settingNetworkMacInputTextBox, input);
    }
    else if (!ituWidgetIsVisible(settingNetworkIPInputBackgroundButton))
        return false;

    if (count >= activeSettingNetworkIPInputTextBox->maxLen)
    {
        if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput1TextBox)
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
        else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput2TextBox)
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
        else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput3TextBox)
            activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
    }
    else if (str)
    {
        char buf[8];
        int value;

        strcpy(buf, str);
        strcat(buf, input);
        value = atoi(buf);
        if (value > 255)
        {
            if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput1TextBox)
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
            else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput2TextBox)
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
            else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput3TextBox)
                activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
            else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput4TextBox)
                return true;
        }
    }
    ituTextBoxInput(activeSettingNetworkIPInputTextBox, input);
    return true;
}

bool SettingNetworkIPInputBackButtonOnPress(ITUWidget* widget, char* param)
{
    ituTextSetString(activeSettingNetworkIPInputTextBox, NULL);

    if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput4TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
    }
    else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput3TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
    }
    else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput2TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    }
    return true;
}

bool SettingNetworkIPInputClearButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingNetworkMacInputBackgroundButton))
    {
        ituTextBoxSetString(settingNetworkMacInputTextBox, NULL);
    }
    else if (!ituWidgetIsVisible(settingNetworkIPInputBackgroundButton))
        return false;

    ituTextBoxSetString(settingNetworkIPInput1TextBox, NULL);
    ituTextBoxSetString(settingNetworkIPInput2TextBox, NULL);
    ituTextBoxSetString(settingNetworkIPInput3TextBox, NULL);
    ituTextBoxSetString(settingNetworkIPInput4TextBox, NULL);
    activeSettingNetworkIPInputTextBox = settingNetworkIPInput1TextBox;
    return true;
}

bool SettingNetworkMacInputConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    char buf[32], mac[6];
    int i;

    strcpy(buf, ituTextBoxGetString(settingNetworkMacInputTextBox));
    for (i = 0; i < 6; i++)
    {
        char buf2[4];
        strncpy(buf2, &buf[i * 2], 2);
        buf2[3] = '\0';
        mac[i] = strtol(buf2, NULL, 16);
    }

    if (memcmp(settingNetworkEthernetInfo.hardwareAddress, mac, 6))
    {
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_WIRTE_MAC, mac);
    }
    ituWidgetSetVisible(settingNetworkGrayBackground, false);
    ituWidgetSetVisible(settingNetworkMacInputBackgroundButton, false);
    ituWidgetEnable(mainTopBackground);
    ituWidgetEnable(settingBackground);
    ituWidgetEnable(settingNetworkBackground);
    return true;
}

bool SettingNetworkIPInputConfirmButtonOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingNetworkMacInputBackgroundButton))
    {
        return SettingNetworkMacInputConfirmButtonOnPress(widget, param);
    }
    else if (!ituWidgetIsVisible(settingNetworkIPInputBackgroundButton))
        return false;

    if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput1TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput2TextBox;
    }
    else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput2TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput3TextBox;
    }
    else if (activeSettingNetworkIPInputTextBox == settingNetworkIPInput3TextBox)
    {
        activeSettingNetworkIPInputTextBox = settingNetworkIPInput4TextBox;
    }
    else
    {
        char buf[16];

        strcpy(buf, ituTextGetString(settingNetworkIPInput1TextBox));
        strcat(buf, ".");
        strcat(buf, ituTextGetString(settingNetworkIPInput2TextBox));
        strcat(buf, ".");
        strcat(buf, ituTextGetString(settingNetworkIPInput3TextBox));
        strcat(buf, ".");
        strcat(buf, ituTextGetString(settingNetworkIPInput4TextBox));

        ituTextSetString(activeSettingNetworkText, buf);

        if (activeSettingNetworkText == settingNetworkIPAddrText)
        {
            strcpy(theConfig.ipaddr, buf);
        }
        else if (activeSettingNetworkText == settingNetworkNetmaskText)
        {
            strcpy(theConfig.netmask, buf);
        }
        else if (activeSettingNetworkText == settingNetworkDNSText)
        {
            strcpy(theConfig.dns, buf);
        }
        else if (activeSettingNetworkText == settingNetworkGatewayText)
        {
            strcpy(theConfig.gw, buf);
        }
        ituWidgetSetVisible(settingNetworkGrayBackground, false);
        ituWidgetSetVisible(settingNetworkIPInputBackgroundButton, false);
        ituWidgetEnable(mainTopBackground);
        ituWidgetEnable(settingBackground);
        ituWidgetEnable(settingNetworkBackground);
        return true;
    }
    return false;
}

bool SettingNetworkOnEnter(ITUWidget* widget, char* param)
{
    if (!settingNetworkIPAssignSprite)
    {
        settingNetworkIPAssignSprite = ituSceneFindWidget(&theScene, "settingNetworkIPAssignSprite");
        assert(settingNetworkIPAssignSprite);

        settingNetworkIPAddrText = ituSceneFindWidget(&theScene, "settingNetworkIPAddrText");
        assert(settingNetworkIPAddrText);

        settingNetworkNetmaskText = ituSceneFindWidget(&theScene, "settingNetworkNetmaskText");
        assert(settingNetworkNetmaskText);

        settingNetworkDNSText = ituSceneFindWidget(&theScene, "settingNetworkDNSText");
        assert(settingNetworkDNSText);

        settingNetworkMacText = ituSceneFindWidget(&theScene, "settingNetworkMacText");
        assert(settingNetworkMacText);

        settingNetworkGatewayText = ituSceneFindWidget(&theScene, "settingNetworkGatewayText");
        assert(settingNetworkGatewayText);

        settingNetworkGrayBackground = ituSceneFindWidget(&theScene, "settingNetworkGrayBackground");
        assert(settingNetworkGrayBackground);

        settingNetworkStaticIPRadioBox = ituSceneFindWidget(&theScene, "settingNetworkStaticIPRadioBox");
        assert(settingNetworkStaticIPRadioBox);

        settingNetworkDHCPRadioBox = ituSceneFindWidget(&theScene, "settingNetworkDHCPRadioBox");
        assert(settingNetworkDHCPRadioBox);

        settingBackground = ituSceneFindWidget(&theScene, "settingBackground");
        assert(settingBackground);

        settingNetworkBackground = ituSceneFindWidget(&theScene, "settingNetworkBackground");
        assert(settingNetworkBackground);

        settingNetworkIPAssignBackgroundButton = ituSceneFindWidget(&theScene, "settingNetworkIPAssignBackgroundButton");
        assert(settingNetworkIPAssignBackgroundButton);

        mainTopBackground = ituSceneFindWidget(&theScene, "mainTopBackground");
        assert(mainTopBackground);

        settingNetworkIPInputBackgroundButton = ituSceneFindWidget(&theScene, "settingNetworkIPInputBackgroundButton");
        assert(settingNetworkIPInputBackgroundButton);

        settingNetworkIPInput1TextBox = ituSceneFindWidget(&theScene, "settingNetworkIPInput1TextBox");
        assert(settingNetworkIPInput1TextBox);

        settingNetworkIPInput2TextBox = ituSceneFindWidget(&theScene, "settingNetworkIPInput2TextBox");
        assert(settingNetworkIPInput2TextBox);

        settingNetworkIPInput3TextBox = ituSceneFindWidget(&theScene, "settingNetworkIPInput3TextBox");
        assert(settingNetworkIPInput3TextBox);

        settingNetworkIPInput4TextBox = ituSceneFindWidget(&theScene, "settingNetworkIPInput4TextBox");
        assert(settingNetworkIPInput4TextBox);

        settingNetworkMacInputBackgroundButton = ituSceneFindWidget(&theScene, "settingNetworkMacInputBackgroundButton");
        assert(settingNetworkMacInputBackgroundButton);

        settingNetworkMacInputTextBox = ituSceneFindWidget(&theScene, "settingNetworkMacInputTextBox");
        assert(settingNetworkMacInputTextBox);
    }

    // current settings
    if (theConfig.dhcp)
    {
        ituRadioBoxSetChecked(settingNetworkDHCPRadioBox, true);
        ituSpriteGoto(settingNetworkIPAssignSprite, 1);
    }
    else
    {
        ituRadioBoxSetChecked(settingNetworkStaticIPRadioBox, true);
        ituSpriteGoto(settingNetworkIPAssignSprite, 0);
    }
    settingNetworkIPAssignOld = theConfig.dhcp;

    ituTextSetString(settingNetworkIPAddrText, theConfig.ipaddr);
    strcpy(settingNetworkIPAddrOld, theConfig.ipaddr);

    ituTextSetString(settingNetworkNetmaskText, theConfig.netmask);
    strcpy(settingNetworkNetmaskOld, theConfig.netmask);

    ituTextSetString(settingNetworkDNSText, theConfig.dns);
    strcpy(settingNetworkDnsOld, theConfig.dns);

    settingNetworkEthernetInfo.index = 0;
    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &settingNetworkEthernetInfo);

    sprintf(settingNetworkMacOld, "%02x%02x%02x%02x%02x%02x", 
            settingNetworkEthernetInfo.hardwareAddress[0], 
            settingNetworkEthernetInfo.hardwareAddress[1], 
            settingNetworkEthernetInfo.hardwareAddress[2], 
            settingNetworkEthernetInfo.hardwareAddress[3], 
            settingNetworkEthernetInfo.hardwareAddress[4], 
            settingNetworkEthernetInfo.hardwareAddress[5]);
    ituTextSetString(settingNetworkMacText, settingNetworkMacOld);

    ituTextSetString(settingNetworkGatewayText, theConfig.gw);
    strcpy(settingNetworkGatewayOld, theConfig.gw);

    return true;
}

bool SettingNetworkOnLeave(ITUWidget* widget, char* param)
{
    bool toReboot = false;
    char* str = ituTextBoxGetString(settingNetworkMacInputTextBox);

    if (str == NULL)
        str = settingNetworkMacOld;

    if (settingNetworkIPAssignOld != theConfig.dhcp ||
        strcmp(settingNetworkIPAddrOld, theConfig.ipaddr) ||
        strcmp(settingNetworkNetmaskOld, theConfig.netmask) ||
        strcmp(settingNetworkDnsOld, theConfig.dns) ||
        strcmp(settingNetworkMacOld, str) ||
        strcmp(settingNetworkGatewayOld, theConfig.gw))
    {
        ConfigSave();

        if (theConfig.area[0] == '\0' && 
            theConfig.building[0] == '\0' && 
            theConfig.unit[0] == '\0' && 
            theConfig.floor[0] == '\0' && 
            theConfig.room[0] == '\0' && 
            theConfig.ext[0] == '\0')
        {
            // reset network
            SceneQuit(QUIT_RESET_NETWORK);
        }
    }
    return true;
}

void SettingNetworkReset(void)
{
    settingNetworkIPAssignSprite = NULL;
}
