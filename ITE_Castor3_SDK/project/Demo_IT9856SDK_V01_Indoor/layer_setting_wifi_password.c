#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"

extern unsigned char *py_ime(unsigned char input_py_val[], int *len);

static ITULayer      *settingWiFiSsidLayer;
static ITUTextBox    *settingWiFiPasswordTextBox;
static ITUKeyboard   *settingWiFiPasswordKeyboard;
static ITUBackground *settingWiFiPasswordEnUpperBackground;
static ITUBackground *settingWiFiPasswordEnLowerBackground;
static ITUBackground *settingWiFiPasswordChsBackground;
static ITUBackground *settingWiFiPasswordSymbolBackground;
static ITUButton     *settingWiFiPasswordChs1Button;
static ITUButton     *settingWiFiPasswordChs2Button;
static ITUButton     *settingWiFiPasswordChs3Button;
static ITUButton     *settingWiFiPasswordChs4Button;
static ITUButton     *settingWiFiPasswordChs5Button;
static ITUButton     *settingWiFiPasswordChs6Button;
static ITUButton     *settingWiFiPasswordChs7Button;
static ITUButton     *settingWiFiPasswordChs8Button;
static ITUButton     *settingWiFiPasswordChs9Button;
static ITUButton     *settingWiFiPasswordChs0Button;
static ITUButton     *settingWiFiPasswordSymbolButton;
static ITUButton     *settingWiFiPasswordEnUpperButton;
static ITUButton     *settingWiFiPasswordEnLowerButton;
static ITUButton     *settingWiFiPasswordChsButton;

static int           settingWiFiPasswordChsIndex;
static unsigned char *settingWiFiPasswordChsString;
static int           settingWiFiPasswordChsStringIndex;
static int           gAPSecurityMode; 
ITUButton            **settingWiFiPasswordButtons[] =
{
    &settingWiFiPasswordChs1Button,
    &settingWiFiPasswordChs2Button,
    &settingWiFiPasswordChs3Button,
    &settingWiFiPasswordChs4Button,
    &settingWiFiPasswordChs5Button,
    &settingWiFiPasswordChs6Button,
    &settingWiFiPasswordChs7Button,
    &settingWiFiPasswordChs8Button,
    &settingWiFiPasswordChs9Button,
    &settingWiFiPasswordChs0Button,
};

void _ClearWiFiPasswordButtonString()
{
    int count = sizeof(settingWiFiPasswordButtons) / sizeof(settingWiFiPasswordButtons[0]);
    int i;
    for (i = 0; i < count; ++i)
        ituButtonSetString(*settingWiFiPasswordButtons[i], NULL);
}

void _SetWiFiPasswordButtonString(int i, char* string)
{
    int count = sizeof(settingWiFiPasswordButtons) / sizeof(settingWiFiPasswordButtons[0]);

    if ((!(i & 1)) && ((i >> 1) < count))   // make sure i == 0, 2, 4, ...., 18
    { 
        ITUButton *btn = *settingWiFiPasswordButtons[i >> 1];
        ituButtonSetString(btn, string);
    }
}

bool SettingWiFiPasswordEnterButtonOnPress(ITUWidget *widget, char *param)
{
    char buf[512];
    char security[3];

    strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));
    memset(theConfig.password, 0, sizeof(theConfig.password));
    sprintf(security, "%d", gAPSecurityMode);
#ifdef CFG_NET_WIFI
    wifiMgr_clientMode_disconnect();
#endif
    if (gAPSecurityMode > 0)
    {
        strcpy(theConfig.password, buf);
    }
    else if (gAPSecurityMode == 0)
    {
    }
    strcpy(theConfig.secumode, security);
    ConfigSave();
    printf("SettingWiFiPasswordEnterButtonOnPress  %s  \n", buf);
    if (1)
    {
#ifdef CFG_NET_WIFI
        wifiMgr_clientMode_connect_ap(theConfig.ssid, theConfig.password, security);
#endif
        ituLayerGoto(settingWiFiSsidLayer);
    }
    else
    {
        ituTextBoxSetString(settingWiFiPasswordTextBox, NULL);
    }
    return true;
}

bool SettingWiFiPasswordBackButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));
        buf[settingWiFiPasswordChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox *)settingWiFiPasswordKeyboard->target, buf);
        settingWiFiPasswordChsIndex      = -1;
        _ClearWiFiPasswordButtonString();
    }
    else
        ituTextBoxBack((ITUTextBox *)settingWiFiPasswordKeyboard->target);
    return true;
}

bool SettingWiFiPasswordEnUpperButtonOnPress(ITUWidget *widget, char *param)
{
    settingWiFiPasswordChsString = NULL;
    return true;
}

bool SettingWiFiPasswordChsButtonOnPress(ITUWidget *widget, char *param)
{
    _ClearWiFiPasswordButtonString();
    return true;
}

bool SettingWiFiPasswordPageUpButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsString)
    {
        if (settingWiFiPasswordChsStringIndex >= 20)
        {
            int i, count;

            settingWiFiPasswordChsStringIndex -= 20;

            count                              = strlen(settingWiFiPasswordChsString) - settingWiFiPasswordChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char    buf[64];
                int     len;

                wchar_t wc = settingWiFiPasswordChsString[settingWiFiPasswordChsStringIndex + i] | ((wchar_t)settingWiFiPasswordChsString[settingWiFiPasswordChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    buf[len] = '\0';
                    _SetWiFiPasswordButtonString(i, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                _SetWiFiPasswordButtonString(i, NULL);
            }
        }
    }
    return true;
}

bool SettingWiFiPasswordPageDownButtonOnPress(ITUWidget *widget, char *param)
{
    if (settingWiFiPasswordChsString)
    {
        int count = strlen(settingWiFiPasswordChsString);
        if (settingWiFiPasswordChsStringIndex + 20 <= count)
        {
            int i;

            settingWiFiPasswordChsStringIndex += 20;

            count                              = strlen(settingWiFiPasswordChsString) - settingWiFiPasswordChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char    buf[64];
                int     len;

                wchar_t wc = settingWiFiPasswordChsString[settingWiFiPasswordChsStringIndex + i] | ((wchar_t)settingWiFiPasswordChsString[settingWiFiPasswordChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    buf[len] = '\0';
                    _SetWiFiPasswordButtonString(i, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                _SetWiFiPasswordButtonString(i, NULL);
            }
        }
    }
    return true;
}

bool SettingWiFiPasswordChsCharButtonOnPress(ITUWidget *widget, char *param)
{
    ITUButton *btn   = (ITUButton *) widget;
    char      *input = ituTextGetString(&btn->text);
    char      buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char *str  = ituTextGetString(settingWiFiPasswordKeyboard->target);
        int  count = str ? strlen(str) : 0;

        if (count < settingWiFiPasswordTextBox->maxLen)
        {
            unsigned char *imestr;
            wchar_t       wc;
            int           len, i;

            if (settingWiFiPasswordChsIndex == -1)
            {
                settingWiFiPasswordChsIndex = count;
            }
            if (count - settingWiFiPasswordChsIndex < 6)
                ituTextBoxInput((ITUTextBox *)settingWiFiPasswordKeyboard->target, input);

            str    = ituTextGetString(settingWiFiPasswordKeyboard->target);
            imestr = py_ime(&str[settingWiFiPasswordChsIndex], &count);
            if (imestr)
            {
                if (count > 20)
                    count = 20;
                else if (count % 2)
                    count--;

                _ClearWiFiPasswordButtonString();

                for (i = 0; i < count; i += 2)
                {
                    wc  = imestr[i] | ((wchar_t)imestr[i + 1] << 8);
                    len = wctomb(buf, wc);
                    if (len > 0)
                    {
                        buf[len] = '\0';
                        _SetWiFiPasswordButtonString(i, buf);
                    }
                }

                if (count > 0)
                {
                    settingWiFiPasswordChsString      = imestr;
                    settingWiFiPasswordChsStringIndex = 0;
                }
            }
        }
    }
    else
    {
        if (settingWiFiPasswordChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(settingWiFiPasswordKeyboard->target));
            buf[settingWiFiPasswordChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox *)settingWiFiPasswordKeyboard->target, buf);
            settingWiFiPasswordChsIndex      = -1;
            _ClearWiFiPasswordButtonString();
        }
    }
    return true;
}

int SettingWiFiPasswordSetData(char *ssid, int securityMode)
{
    strcpy(theConfig.ssid, ssid);
    gAPSecurityMode = securityMode;
    printf("SettingWiFiPasswordSetData %s %d \n", ssid, securityMode);
    return 0;
}

bool SettingWiFiPasswordOnEnter(ITUWidget *widget, char *param)
{
    if (!settingWiFiSsidLayer)
    {
        settingWiFiSsidLayer                 = ituSceneFindWidget(&theScene, "settingWiFiSsidLayer");
        assert(settingWiFiSsidLayer);

        settingWiFiPasswordTextBox           = ituSceneFindWidget(&theScene, "settingWiFiPasswordTextBox");
        assert(settingWiFiPasswordTextBox);

        settingWiFiPasswordKeyboard          = ituSceneFindWidget(&theScene, "settingWiFiPasswordKeyboard");
        assert(settingWiFiPasswordKeyboard);
        settingWiFiPasswordKeyboard->target  = (ITUWidget *)settingWiFiPasswordTextBox;

        settingWiFiPasswordEnUpperBackground = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnUpperBackground");
        assert(settingWiFiPasswordEnUpperBackground);

        settingWiFiPasswordEnLowerBackground = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnLowerBackground");
        assert(settingWiFiPasswordEnLowerBackground);

        settingWiFiPasswordChsBackground     = ituSceneFindWidget(&theScene, "settingWiFiPasswordChsBackground");
        assert(settingWiFiPasswordChsBackground);

        settingWiFiPasswordSymbolBackground  = ituSceneFindWidget(&theScene, "settingWiFiPasswordSymbolBackground");
        assert(settingWiFiPasswordSymbolBackground);

        settingWiFiPasswordSymbolButton      = ituSceneFindWidget(&theScene, "settingWiFiPasswordSymbolButton");
        assert(settingWiFiPasswordSymbolButton);

        settingWiFiPasswordEnUpperButton     = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnUpperButton");
        assert(settingWiFiPasswordEnUpperButton);

        settingWiFiPasswordEnLowerButton     = ituSceneFindWidget(&theScene, "settingWiFiPasswordEnLowerButton");
        assert(settingWiFiPasswordEnLowerButton);

        settingWiFiPasswordChsButton         = ituSceneFindWidget(&theScene, "settingWiFiPasswordChsButton");
        assert(settingWiFiPasswordChsButton);

        settingWiFiPasswordChs1Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs1Button");
        assert(settingWiFiPasswordChs1Button);

        settingWiFiPasswordChs2Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs2Button");
        assert(settingWiFiPasswordChs2Button);

        settingWiFiPasswordChs3Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs3Button");
        assert(settingWiFiPasswordChs3Button);

        settingWiFiPasswordChs4Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs4Button");
        assert(settingWiFiPasswordChs4Button);

        settingWiFiPasswordChs5Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs5Button");
        assert(settingWiFiPasswordChs5Button);

        settingWiFiPasswordChs6Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs6Button");
        assert(settingWiFiPasswordChs6Button);

        settingWiFiPasswordChs7Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs7Button");
        assert(settingWiFiPasswordChs7Button);

        settingWiFiPasswordChs8Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs8Button");
        assert(settingWiFiPasswordChs8Button);

        settingWiFiPasswordChs9Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs9Button");
        assert(settingWiFiPasswordChs9Button);

        settingWiFiPasswordChs0Button        = ituSceneFindWidget(&theScene, "settingWiFiPasswordChs0Button");
        assert(settingWiFiPasswordChs0Button);
    }

    ituWidgetSetVisible(settingWiFiPasswordEnUpperBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordEnLowerBackground, true);
    ituWidgetSetVisible(settingWiFiPasswordChsBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordSymbolBackground, false);
    ituWidgetSetVisible(settingWiFiPasswordEnUpperButton, false);
    ituWidgetSetVisible(settingWiFiPasswordEnLowerButton, true);
    ituWidgetSetVisible(settingWiFiPasswordChsButton, false);
    _ClearWiFiPasswordButtonString();

    settingWiFiPasswordChsIndex       = -1;
    settingWiFiPasswordChsString      = NULL;
    settingWiFiPasswordChsStringIndex = 0;

    return true;
}

void SettingWiFiPasswordReset(void)
{
    settingWiFiSsidLayer = NULL;
}