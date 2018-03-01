#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"

extern unsigned char* py_ime(unsigned char input_py_val[], int* len);

static ITULayer* mainMenuLayer;
static ITUTextBox* loginNameTextBox;
static ITUTextBox* loginPasswordTextBox;
static ITUKeyboard* loginKeyboard;
static ITUBackground* loginKeyboardEnUpperBackground;
static ITUBackground* loginKeyboardEnLowerBackground;
static ITUBackground* loginKeyboardChsBackground;
static ITUBackground* loginKeyboardSymbolBackground;
static ITUButton* loginKeyboardChs1Button;
static ITUButton* loginKeyboardChs2Button;
static ITUButton* loginKeyboardChs3Button;
static ITUButton* loginKeyboardChs4Button;
static ITUButton* loginKeyboardChs5Button;
static ITUButton* loginKeyboardChs6Button;
static ITUButton* loginKeyboardChs7Button;
static ITUButton* loginKeyboardChs8Button;
static ITUButton* loginKeyboardChs9Button;
static ITUButton* loginKeyboardChs0Button;
static ITUButton* loginKeyboardSymbolButton;
static ITUButton* loginKeyboardEnUpperButton;
static ITUButton* loginKeyboardEnLowerButton;
static ITUButton* loginKeyboardChsButton;
static ITUBackground* loginBackground;
static ITUBackground* loginErrorBackground;

static int loginKeyboardChsIndex;
static unsigned char* loginKeyboardChsString;
static int loginKeyboardChsStringIndex;

bool LoginKeyboardEnterButtonOnPress(ITUWidget* widget, char* param)
{
    char* s = ituTextGetString((ITUTextBox*)loginKeyboard->target);
    int count = s ? strlen(s) : 0;

    if (loginKeyboard->target == (ITUWidget*)loginNameTextBox)
    {
        loginKeyboard->target = (ITUWidget*)loginPasswordTextBox;
        ituFocusWidget(loginPasswordTextBox);
    }
    else
    {
    	char* id = ituTextGetString(loginNameTextBox);
	    char* pw = ituTextGetString(loginPasswordTextBox);

        if (UserListCheckUser(id, pw))
        {
            SceneEnableKeypad(true);
            ituLayerGoto(mainMenuLayer);
        }
        else
        {
            ituWidgetSetVisible(loginErrorBackground, true);
            ituWidgetDisable(loginBackground);
            loginKeyboard->target = (ITUWidget*)loginNameTextBox;
        }
	}
	return true;
}

bool LoginKeyboardBackButtonOnPress(ITUWidget* widget, char* param)
{
    if (loginKeyboardChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(loginKeyboard->target));
        buf[loginKeyboardChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox*)loginKeyboard->target, buf);
        loginKeyboardChsIndex = -1;
        ituButtonSetString(loginKeyboardChs1Button, NULL);
        ituButtonSetString(loginKeyboardChs2Button, NULL);
        ituButtonSetString(loginKeyboardChs3Button, NULL);
        ituButtonSetString(loginKeyboardChs4Button, NULL);
        ituButtonSetString(loginKeyboardChs5Button, NULL);
        ituButtonSetString(loginKeyboardChs6Button, NULL);
        ituButtonSetString(loginKeyboardChs7Button, NULL);
        ituButtonSetString(loginKeyboardChs8Button, NULL);
        ituButtonSetString(loginKeyboardChs9Button, NULL);
        ituButtonSetString(loginKeyboardChs0Button, NULL);
    }
    else
        ituTextBoxBack((ITUTextBox*)loginKeyboard->target);
	return true;
}

bool LoginKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param)
{
    loginKeyboardChsString = NULL;
	return true;
}

bool LoginKeyboardChsButtonOnPress(ITUWidget* widget, char* param)
{
    ituButtonSetString(loginKeyboardChs1Button, NULL);
    ituButtonSetString(loginKeyboardChs2Button, NULL);
    ituButtonSetString(loginKeyboardChs3Button, NULL);
    ituButtonSetString(loginKeyboardChs4Button, NULL);
    ituButtonSetString(loginKeyboardChs5Button, NULL);
    ituButtonSetString(loginKeyboardChs6Button, NULL);
    ituButtonSetString(loginKeyboardChs7Button, NULL);
    ituButtonSetString(loginKeyboardChs8Button, NULL);
    ituButtonSetString(loginKeyboardChs9Button, NULL);
    ituButtonSetString(loginKeyboardChs0Button, NULL);
	return true;
}

bool LoginKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param)
{
    if (loginKeyboardChsString)
    {
        if (loginKeyboardChsStringIndex >= 20)
        {
            int i, count;

            loginKeyboardChsStringIndex -= 20;

            count = strlen(loginKeyboardChsString) - loginKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = loginKeyboardChsString[loginKeyboardChsStringIndex + i] | ((wchar_t)loginKeyboardChsString[loginKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = loginKeyboardChs1Button;
                    else if (i == 2)
                        btn = loginKeyboardChs2Button;
                    else if (i == 4)
                        btn = loginKeyboardChs3Button;
                    else if (i == 6)
                        btn = loginKeyboardChs4Button;
                    else if (i == 8)
                        btn = loginKeyboardChs5Button;
                    else if (i == 10)
                        btn = loginKeyboardChs6Button;
                    else if (i == 12)
                        btn = loginKeyboardChs7Button;
                    else if (i == 14)
                        btn = loginKeyboardChs8Button;
                    else if (i == 16)
                        btn = loginKeyboardChs9Button;
                    else if (i == 18)
                        btn = loginKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = loginKeyboardChs1Button;
                else if (i == 2)
                    btn = loginKeyboardChs2Button;
                else if (i == 4)
                    btn = loginKeyboardChs3Button;
                else if (i == 6)
                    btn = loginKeyboardChs4Button;
                else if (i == 8)
                    btn = loginKeyboardChs5Button;
                else if (i == 10)
                    btn = loginKeyboardChs6Button;
                else if (i == 12)
                    btn = loginKeyboardChs7Button;
                else if (i == 14)
                    btn = loginKeyboardChs8Button;
                else if (i == 16)
                    btn = loginKeyboardChs9Button;
                else if (i == 18)
                    btn = loginKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool LoginKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param)
{
    if (loginKeyboardChsString)
    {
        int count = strlen(loginKeyboardChsString);
        if (loginKeyboardChsStringIndex + 20 <= count)
        {
            int i;

            loginKeyboardChsStringIndex += 20;

            count = strlen(loginKeyboardChsString) - loginKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = loginKeyboardChsString[loginKeyboardChsStringIndex + i] | ((wchar_t)loginKeyboardChsString[loginKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = loginKeyboardChs1Button;
                    else if (i == 2)
                        btn = loginKeyboardChs2Button;
                    else if (i == 4)
                        btn = loginKeyboardChs3Button;
                    else if (i == 6)
                        btn = loginKeyboardChs4Button;
                    else if (i == 8)
                        btn = loginKeyboardChs5Button;
                    else if (i == 10)
                        btn = loginKeyboardChs6Button;
                    else if (i == 12)
                        btn = loginKeyboardChs7Button;
                    else if (i == 14)
                        btn = loginKeyboardChs8Button;
                    else if (i == 16)
                        btn = loginKeyboardChs9Button;
                    else if (i == 18)
                        btn = loginKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = loginKeyboardChs1Button;
                else if (i == 2)
                    btn = loginKeyboardChs2Button;
                else if (i == 4)
                    btn = loginKeyboardChs3Button;
                else if (i == 6)
                    btn = loginKeyboardChs4Button;
                else if (i == 8)
                    btn = loginKeyboardChs5Button;
                else if (i == 10)
                    btn = loginKeyboardChs6Button;
                else if (i == 12)
                    btn = loginKeyboardChs7Button;
                else if (i == 14)
                    btn = loginKeyboardChs8Button;
                else if (i == 16)
                    btn = loginKeyboardChs9Button;
                else if (i == 18)
                    btn = loginKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool LoginKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = ituTextGetString(&btn->text);
    char buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char* str = ituTextBoxGetString((ITUTextBox*)loginKeyboard->target);
        int count = str ? strlen(str) : 0;

        if (count < loginNameTextBox->maxLen)
        {
            unsigned char* imestr;
            wchar_t wc;
            int len, i;

            if (loginKeyboardChsIndex == -1)
            {
                loginKeyboardChsIndex = count;
            }
            if (count - loginKeyboardChsIndex < 6)
                ituTextBoxInput((ITUTextBox*)loginKeyboard->target, input);

            str = ituTextGetString(loginKeyboard->target);
	        imestr = py_ime(&str[loginKeyboardChsIndex], &count);
            if (imestr)
            {
                if (count > 20)
                    count = 20;
                else if (count % 2)
                    count--;

                ituButtonSetString(loginKeyboardChs1Button, NULL);
                ituButtonSetString(loginKeyboardChs2Button, NULL);
                ituButtonSetString(loginKeyboardChs3Button, NULL);
                ituButtonSetString(loginKeyboardChs4Button, NULL);
                ituButtonSetString(loginKeyboardChs5Button, NULL);
                ituButtonSetString(loginKeyboardChs6Button, NULL);
                ituButtonSetString(loginKeyboardChs7Button, NULL);
                ituButtonSetString(loginKeyboardChs8Button, NULL);
                ituButtonSetString(loginKeyboardChs9Button, NULL);
                ituButtonSetString(loginKeyboardChs0Button, NULL);

                for (i = 0; i < count; i += 2)
                {
                    wc = imestr[i] | ((wchar_t)imestr[i + 1] << 8);
                    len = wctomb(buf, wc);
                    if (len > 0)
                    {
                        ITUButton* btn = NULL;
                        buf[len] = '\0';

                        if (i == 0)
                            btn = loginKeyboardChs1Button;
                        else if (i == 2)
                            btn = loginKeyboardChs2Button;
                        else if (i == 4)
                            btn = loginKeyboardChs3Button;
                        else if (i == 6)
                            btn = loginKeyboardChs4Button;
                        else if (i == 8)
                            btn = loginKeyboardChs5Button;
                        else if (i == 10)
                            btn = loginKeyboardChs6Button;
                        else if (i == 12)
                            btn = loginKeyboardChs7Button;
                        else if (i == 14)
                            btn = loginKeyboardChs8Button;
                        else if (i == 16)
                            btn = loginKeyboardChs9Button;
                        else if (i == 18)
                            btn = loginKeyboardChs0Button;

                        if (btn)
                            ituButtonSetString(btn, buf);
                    }
                }

                if (count > 0)
                {
                    loginKeyboardChsString = imestr;
                    loginKeyboardChsStringIndex = 0;
                }
            }
        }
    }
    else
    {
        if (loginKeyboardChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(loginKeyboard->target));
            buf[loginKeyboardChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox*)loginKeyboard->target, buf);
            loginKeyboardChsIndex = -1;
            ituButtonSetString(loginKeyboardChs1Button, NULL);
            ituButtonSetString(loginKeyboardChs2Button, NULL);
            ituButtonSetString(loginKeyboardChs3Button, NULL);
            ituButtonSetString(loginKeyboardChs4Button, NULL);
            ituButtonSetString(loginKeyboardChs5Button, NULL);
            ituButtonSetString(loginKeyboardChs6Button, NULL);
            ituButtonSetString(loginKeyboardChs7Button, NULL);
            ituButtonSetString(loginKeyboardChs8Button, NULL);
            ituButtonSetString(loginKeyboardChs9Button, NULL);
            ituButtonSetString(loginKeyboardChs0Button, NULL);
        }
    }
	return true;
}

bool LoginOnEnter(ITUWidget* widget, char* param)
{
	if (!mainMenuLayer)
    {
        mainMenuLayer = ituSceneFindWidget(&theScene, "mainMenuLayer");
        assert(mainMenuLayer);

        loginNameTextBox = ituSceneFindWidget(&theScene, "loginNameTextBox");
        assert(loginNameTextBox);
            
        loginPasswordTextBox = ituSceneFindWidget(&theScene, "loginPasswordTextBox");
        assert(loginPasswordTextBox);

        loginKeyboard = ituSceneFindWidget(&theScene, "loginKeyboard");
        assert(loginKeyboard);

        loginKeyboardEnUpperBackground = ituSceneFindWidget(&theScene, "loginKeyboardEnUpperBackground");
        assert(loginKeyboardEnUpperBackground);

        loginKeyboardEnLowerBackground = ituSceneFindWidget(&theScene, "loginKeyboardEnLowerBackground");
        assert(loginKeyboardEnLowerBackground);

        loginKeyboardChsBackground = ituSceneFindWidget(&theScene, "loginKeyboardChsBackground");
        assert(loginKeyboardChsBackground);

        loginKeyboardSymbolBackground = ituSceneFindWidget(&theScene, "loginKeyboardSymbolBackground");
        assert(loginKeyboardSymbolBackground);

        loginKeyboardSymbolButton = ituSceneFindWidget(&theScene, "loginKeyboardSymbolButton");
        assert(loginKeyboardSymbolButton);

        loginKeyboardEnUpperButton = ituSceneFindWidget(&theScene, "loginKeyboardEnUpperButton");
        assert(loginKeyboardEnUpperButton);

        loginKeyboardEnLowerButton = ituSceneFindWidget(&theScene, "loginKeyboardEnLowerButton");
        assert(loginKeyboardEnLowerButton);

        loginKeyboardChsButton = ituSceneFindWidget(&theScene, "loginKeyboardChsButton");
        assert(loginKeyboardChsButton);

		loginKeyboardChs1Button = ituSceneFindWidget(&theScene, "loginKeyboardChs1Button");
        assert(loginKeyboardChs1Button);

		loginKeyboardChs2Button = ituSceneFindWidget(&theScene, "loginKeyboardChs2Button");
        assert(loginKeyboardChs2Button);

		loginKeyboardChs3Button = ituSceneFindWidget(&theScene, "loginKeyboardChs3Button");
        assert(loginKeyboardChs3Button);

		loginKeyboardChs4Button = ituSceneFindWidget(&theScene, "loginKeyboardChs4Button");
        assert(loginKeyboardChs4Button);

		loginKeyboardChs5Button = ituSceneFindWidget(&theScene, "loginKeyboardChs5Button");
        assert(loginKeyboardChs5Button);

		loginKeyboardChs6Button = ituSceneFindWidget(&theScene, "loginKeyboardChs6Button");
        assert(loginKeyboardChs6Button);

		loginKeyboardChs7Button = ituSceneFindWidget(&theScene, "loginKeyboardChs7Button");
        assert(loginKeyboardChs7Button);

		loginKeyboardChs8Button = ituSceneFindWidget(&theScene, "loginKeyboardChs8Button");
        assert(loginKeyboardChs8Button);

		loginKeyboardChs9Button = ituSceneFindWidget(&theScene, "loginKeyboardChs9Button");
        assert(loginKeyboardChs9Button);

		loginKeyboardChs0Button = ituSceneFindWidget(&theScene, "loginKeyboardChs0Button");
        assert(loginKeyboardChs0Button);

		loginBackground = ituSceneFindWidget(&theScene, "loginBackground");
        assert(loginBackground);

		loginErrorBackground = ituSceneFindWidget(&theScene, "loginErrorBackground");
        assert(loginErrorBackground);
	}

    ituWidgetSetVisible(loginKeyboardEnUpperBackground, false);
    ituWidgetSetVisible(loginKeyboardEnLowerBackground, true);
    ituWidgetSetVisible(loginKeyboardChsBackground, false);
    ituWidgetSetVisible(loginKeyboardSymbolBackground, false);
    ituWidgetSetVisible(loginKeyboardEnUpperButton, false);
    ituWidgetSetVisible(loginKeyboardEnLowerButton, true);
    ituWidgetSetVisible(loginKeyboardChsButton, false);
    ituButtonSetString(loginKeyboardChs1Button, NULL);
    ituButtonSetString(loginKeyboardChs2Button, NULL);
    ituButtonSetString(loginKeyboardChs3Button, NULL);
    ituButtonSetString(loginKeyboardChs4Button, NULL);
    ituButtonSetString(loginKeyboardChs5Button, NULL);
    ituButtonSetString(loginKeyboardChs6Button, NULL);
    ituButtonSetString(loginKeyboardChs7Button, NULL);
    ituButtonSetString(loginKeyboardChs8Button, NULL);
    ituButtonSetString(loginKeyboardChs9Button, NULL);
    ituButtonSetString(loginKeyboardChs0Button, NULL);
    ituTextBoxSetString(loginNameTextBox, NULL);
    ituTextBoxSetString(loginPasswordTextBox, NULL);
    ituFocusWidget(loginNameTextBox);

    loginKeyboardChsIndex = -1;
    loginKeyboardChsString = NULL;
    loginKeyboardChsStringIndex = 0;

    SceneEnableKeypad(false);
    LinphoneEnableDoNotDisturb();

	return true;
}

bool LoginOnLeave(ITUWidget* widget, char* param)
{
    SceneUpdateIndicators();
	return true;
}

void LoginReset(void)
{
    mainMenuLayer = NULL;
}
