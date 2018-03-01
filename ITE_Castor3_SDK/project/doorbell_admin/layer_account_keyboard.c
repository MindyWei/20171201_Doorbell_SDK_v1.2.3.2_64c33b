#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"

extern unsigned char* py_ime(unsigned char input_py_val[], int* len);

static ITULayer* accountLayer;
static ITUBackground* accountKeyboardBackground;
static ITUTextBox* accountNameTextBox;
static ITUTextBox* accountPasswordTextBox;
static ITUKeyboard* accountKeyboard;
static ITUBackground* accountKeyboardEnUpperBackground;
static ITUBackground* accountKeyboardEnLowerBackground;
static ITUBackground* accountKeyboardChsBackground;
static ITUBackground* accountKeyboardSymbolBackground;
static ITUButton* accountKeyboardChs1Button;
static ITUButton* accountKeyboardChs2Button;
static ITUButton* accountKeyboardChs3Button;
static ITUButton* accountKeyboardChs4Button;
static ITUButton* accountKeyboardChs5Button;
static ITUButton* accountKeyboardChs6Button;
static ITUButton* accountKeyboardChs7Button;
static ITUButton* accountKeyboardChs8Button;
static ITUButton* accountKeyboardChs9Button;
static ITUButton* accountKeyboardChs0Button;
static ITUButton* accountKeyboardSymbolButton;
static ITUButton* accountKeyboardEnUpperButton;
static ITUButton* accountKeyboardEnLowerButton;
static ITUButton* accountKeyboardChsButton;
static ITUBackground* accountKeyboardWarnBackground;

static int accountKeyboardChsIndex;
static unsigned char* accountKeyboardChsString;
static int accountKeyboardChsStringIndex;

bool AccountKeyboardEnterButtonOnPress(ITUWidget* widget, char* param)
{
    char* s = ituTextGetString((ITUTextBox*)accountKeyboard->target);
    int count = s ? strlen(s) : 0;

    if (accountKeyboard->target == (ITUWidget*)accountNameTextBox)
    {
        if (count == 0)
        {
            ituLayerGoto(accountLayer);
            return true;
        }
        else if (count < 4)
        {
            return false;
        }
        accountKeyboard->target = (ITUWidget*)accountPasswordTextBox;
    }
    else
    {
        if (count < 4)
        {
            return false;
        }
        else
        {
    	    char* id = ituTextGetString(accountNameTextBox);
	        char* pw = ituTextGetString(accountPasswordTextBox);

            if (UserListAddUser(id, pw))
            {
                ituLayerGoto(accountLayer);
            }
            else
            {
                ituWidgetSetVisible(accountKeyboardWarnBackground, true);
                ituWidgetDisable(accountKeyboardBackground);
            }
        }
	}
	return true;
}

bool AccountKeyboardBackButtonOnPress(ITUWidget* widget, char* param)
{
    if (accountKeyboardChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(accountKeyboard->target));
        buf[accountKeyboardChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox*)accountKeyboard->target, buf);
        accountKeyboardChsIndex = -1;
        ituButtonSetString(accountKeyboardChs1Button, NULL);
        ituButtonSetString(accountKeyboardChs2Button, NULL);
        ituButtonSetString(accountKeyboardChs3Button, NULL);
        ituButtonSetString(accountKeyboardChs4Button, NULL);
        ituButtonSetString(accountKeyboardChs5Button, NULL);
        ituButtonSetString(accountKeyboardChs6Button, NULL);
        ituButtonSetString(accountKeyboardChs7Button, NULL);
        ituButtonSetString(accountKeyboardChs8Button, NULL);
        ituButtonSetString(accountKeyboardChs9Button, NULL);
        ituButtonSetString(accountKeyboardChs0Button, NULL);
    }
    else
        ituTextBoxBack((ITUTextBox*)accountKeyboard->target);
	return true;
}

bool AccountKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param)
{
    accountKeyboardChsString = NULL;
	return true;
}

bool AccountKeyboardChsButtonOnPress(ITUWidget* widget, char* param)
{
    ituButtonSetString(accountKeyboardChs1Button, NULL);
    ituButtonSetString(accountKeyboardChs2Button, NULL);
    ituButtonSetString(accountKeyboardChs3Button, NULL);
    ituButtonSetString(accountKeyboardChs4Button, NULL);
    ituButtonSetString(accountKeyboardChs5Button, NULL);
    ituButtonSetString(accountKeyboardChs6Button, NULL);
    ituButtonSetString(accountKeyboardChs7Button, NULL);
    ituButtonSetString(accountKeyboardChs8Button, NULL);
    ituButtonSetString(accountKeyboardChs9Button, NULL);
    ituButtonSetString(accountKeyboardChs0Button, NULL);
	return true;
}

bool AccountKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param)
{
    if (accountKeyboardChsString)
    {
        if (accountKeyboardChsStringIndex >= 20)
        {
            int i, count;

            accountKeyboardChsStringIndex -= 20;

            count = strlen(accountKeyboardChsString) - accountKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = accountKeyboardChsString[accountKeyboardChsStringIndex + i] | ((wchar_t)accountKeyboardChsString[accountKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = accountKeyboardChs1Button;
                    else if (i == 2)
                        btn = accountKeyboardChs2Button;
                    else if (i == 4)
                        btn = accountKeyboardChs3Button;
                    else if (i == 6)
                        btn = accountKeyboardChs4Button;
                    else if (i == 8)
                        btn = accountKeyboardChs5Button;
                    else if (i == 10)
                        btn = accountKeyboardChs6Button;
                    else if (i == 12)
                        btn = accountKeyboardChs7Button;
                    else if (i == 14)
                        btn = accountKeyboardChs8Button;
                    else if (i == 16)
                        btn = accountKeyboardChs9Button;
                    else if (i == 18)
                        btn = accountKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = accountKeyboardChs1Button;
                else if (i == 2)
                    btn = accountKeyboardChs2Button;
                else if (i == 4)
                    btn = accountKeyboardChs3Button;
                else if (i == 6)
                    btn = accountKeyboardChs4Button;
                else if (i == 8)
                    btn = accountKeyboardChs5Button;
                else if (i == 10)
                    btn = accountKeyboardChs6Button;
                else if (i == 12)
                    btn = accountKeyboardChs7Button;
                else if (i == 14)
                    btn = accountKeyboardChs8Button;
                else if (i == 16)
                    btn = accountKeyboardChs9Button;
                else if (i == 18)
                    btn = accountKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool AccountKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param)
{
    if (accountKeyboardChsString)
    {
        int count = strlen(accountKeyboardChsString);
        if (accountKeyboardChsStringIndex + 20 <= count)
        {
            int i;

            accountKeyboardChsStringIndex += 20;

            count = strlen(accountKeyboardChsString) - accountKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = accountKeyboardChsString[accountKeyboardChsStringIndex + i] | ((wchar_t)accountKeyboardChsString[accountKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = accountKeyboardChs1Button;
                    else if (i == 2)
                        btn = accountKeyboardChs2Button;
                    else if (i == 4)
                        btn = accountKeyboardChs3Button;
                    else if (i == 6)
                        btn = accountKeyboardChs4Button;
                    else if (i == 8)
                        btn = accountKeyboardChs5Button;
                    else if (i == 10)
                        btn = accountKeyboardChs6Button;
                    else if (i == 12)
                        btn = accountKeyboardChs7Button;
                    else if (i == 14)
                        btn = accountKeyboardChs8Button;
                    else if (i == 16)
                        btn = accountKeyboardChs9Button;
                    else if (i == 18)
                        btn = accountKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = accountKeyboardChs1Button;
                else if (i == 2)
                    btn = accountKeyboardChs2Button;
                else if (i == 4)
                    btn = accountKeyboardChs3Button;
                else if (i == 6)
                    btn = accountKeyboardChs4Button;
                else if (i == 8)
                    btn = accountKeyboardChs5Button;
                else if (i == 10)
                    btn = accountKeyboardChs6Button;
                else if (i == 12)
                    btn = accountKeyboardChs7Button;
                else if (i == 14)
                    btn = accountKeyboardChs8Button;
                else if (i == 16)
                    btn = accountKeyboardChs9Button;
                else if (i == 18)
                    btn = accountKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool AccountKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = ituTextGetString(&btn->text);
    char buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char* str = ituTextGetString(accountKeyboard->target);
        int count = str ? strlen(str) : 0;

        if (count < accountNameTextBox->maxLen)
        {
            unsigned char* imestr;
            wchar_t wc;
            int len, i;

            if (accountKeyboardChsIndex == -1)
            {
                accountKeyboardChsIndex = count;
            }
            if (count - accountKeyboardChsIndex < 6)
                ituTextBoxInput((ITUTextBox*)accountKeyboard->target, input);

            str = ituTextGetString(accountKeyboard->target);
	        imestr = py_ime(&str[accountKeyboardChsIndex], &count);
            if (imestr)
            {
                if (count > 20)
                    count = 20;
                else if (count % 2)
                    count--;

                ituButtonSetString(accountKeyboardChs1Button, NULL);
                ituButtonSetString(accountKeyboardChs2Button, NULL);
                ituButtonSetString(accountKeyboardChs3Button, NULL);
                ituButtonSetString(accountKeyboardChs4Button, NULL);
                ituButtonSetString(accountKeyboardChs5Button, NULL);
                ituButtonSetString(accountKeyboardChs6Button, NULL);
                ituButtonSetString(accountKeyboardChs7Button, NULL);
                ituButtonSetString(accountKeyboardChs8Button, NULL);
                ituButtonSetString(accountKeyboardChs9Button, NULL);
                ituButtonSetString(accountKeyboardChs0Button, NULL);

                for (i = 0; i < count; i += 2)
                {
                    wc = imestr[i] | ((wchar_t)imestr[i + 1] << 8);
                    len = wctomb(buf, wc);
                    if (len > 0)
                    {
                        ITUButton* btn = NULL;
                        buf[len] = '\0';

                        if (i == 0)
                            btn = accountKeyboardChs1Button;
                        else if (i == 2)
                            btn = accountKeyboardChs2Button;
                        else if (i == 4)
                            btn = accountKeyboardChs3Button;
                        else if (i == 6)
                            btn = accountKeyboardChs4Button;
                        else if (i == 8)
                            btn = accountKeyboardChs5Button;
                        else if (i == 10)
                            btn = accountKeyboardChs6Button;
                        else if (i == 12)
                            btn = accountKeyboardChs7Button;
                        else if (i == 14)
                            btn = accountKeyboardChs8Button;
                        else if (i == 16)
                            btn = accountKeyboardChs9Button;
                        else if (i == 18)
                            btn = accountKeyboardChs0Button;

                        if (btn)
                            ituButtonSetString(btn, buf);
                    }
                }

                if (count > 0)
                {
                    accountKeyboardChsString = imestr;
                    accountKeyboardChsStringIndex = 0;
                }
            }
        }
    }
    else
    {
        if (accountKeyboardChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(accountKeyboard->target));
            buf[accountKeyboardChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox*)accountKeyboard->target, buf);
            accountKeyboardChsIndex = -1;
            ituButtonSetString(accountKeyboardChs1Button, NULL);
            ituButtonSetString(accountKeyboardChs2Button, NULL);
            ituButtonSetString(accountKeyboardChs3Button, NULL);
            ituButtonSetString(accountKeyboardChs4Button, NULL);
            ituButtonSetString(accountKeyboardChs5Button, NULL);
            ituButtonSetString(accountKeyboardChs6Button, NULL);
            ituButtonSetString(accountKeyboardChs7Button, NULL);
            ituButtonSetString(accountKeyboardChs8Button, NULL);
            ituButtonSetString(accountKeyboardChs9Button, NULL);
            ituButtonSetString(accountKeyboardChs0Button, NULL);
        }
    }
	return true;
}

static bool AccountKeyboardBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        int x = arg2 - widget->rect.x;
        int y = arg3 - widget->rect.y;

        if (ituWidgetIsInside(widget, x, y))
        {
            int xx, yy, w, h;

            x = arg2;
            y = arg3;

            if (ituWidgetIsVisible(accountKeyboardWarnBackground))
            {
                ituWidgetGetGlobalPosition(accountKeyboardWarnBackground, &xx, &yy);
                w = ituWidgetGetWidth(accountKeyboardWarnBackground);
                h = ituWidgetGetHeight(accountKeyboardWarnBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituWidgetSetVisible(accountKeyboardWarnBackground, false);
                    ituWidgetEnable(widget);
                    return widget->dirty;
                }
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

bool AccountKeyboardOnEnter(ITUWidget* widget, char* param)
{
	if (!accountLayer)
    {
        accountLayer = ituSceneFindWidget(&theScene, "accountLayer");
        assert(accountLayer);

        accountKeyboardBackground = ituSceneFindWidget(&theScene, "accountKeyboardBackground");
        assert(accountKeyboardBackground);
        ituWidgetSetUpdate(accountKeyboardBackground, AccountKeyboardBackgroundUpdate);

        accountNameTextBox = ituSceneFindWidget(&theScene, "accountNameTextBox");
        assert(accountNameTextBox);
            
        accountPasswordTextBox = ituSceneFindWidget(&theScene, "accountPasswordTextBox");
        assert(accountPasswordTextBox);

        accountKeyboard = ituSceneFindWidget(&theScene, "accountKeyboard");
        assert(accountKeyboard);

        accountKeyboardEnUpperBackground = ituSceneFindWidget(&theScene, "accountKeyboardEnUpperBackground");
        assert(accountKeyboardEnUpperBackground);

        accountKeyboardEnLowerBackground = ituSceneFindWidget(&theScene, "accountKeyboardEnLowerBackground");
        assert(accountKeyboardEnLowerBackground);

        accountKeyboardChsBackground = ituSceneFindWidget(&theScene, "accountKeyboardChsBackground");
        assert(accountKeyboardChsBackground);

        accountKeyboardSymbolBackground = ituSceneFindWidget(&theScene, "accountKeyboardSymbolBackground");
        assert(accountKeyboardSymbolBackground);

        accountKeyboardSymbolButton = ituSceneFindWidget(&theScene, "accountKeyboardSymbolButton");
        assert(accountKeyboardSymbolButton);

        accountKeyboardEnUpperButton = ituSceneFindWidget(&theScene, "accountKeyboardEnUpperButton");
        assert(accountKeyboardEnUpperButton);

        accountKeyboardEnLowerButton = ituSceneFindWidget(&theScene, "accountKeyboardEnLowerButton");
        assert(accountKeyboardEnLowerButton);

        accountKeyboardChsButton = ituSceneFindWidget(&theScene, "accountKeyboardChsButton");
        assert(accountKeyboardChsButton);

		accountKeyboardChs1Button = ituSceneFindWidget(&theScene, "accountKeyboardChs1Button");
        assert(accountKeyboardChs1Button);

		accountKeyboardChs2Button = ituSceneFindWidget(&theScene, "accountKeyboardChs2Button");
        assert(accountKeyboardChs2Button);

		accountKeyboardChs3Button = ituSceneFindWidget(&theScene, "accountKeyboardChs3Button");
        assert(accountKeyboardChs3Button);

		accountKeyboardChs4Button = ituSceneFindWidget(&theScene, "accountKeyboardChs4Button");
        assert(accountKeyboardChs4Button);

		accountKeyboardChs5Button = ituSceneFindWidget(&theScene, "accountKeyboardChs5Button");
        assert(accountKeyboardChs5Button);

		accountKeyboardChs6Button = ituSceneFindWidget(&theScene, "accountKeyboardChs6Button");
        assert(accountKeyboardChs6Button);

		accountKeyboardChs7Button = ituSceneFindWidget(&theScene, "accountKeyboardChs7Button");
        assert(accountKeyboardChs7Button);

		accountKeyboardChs8Button = ituSceneFindWidget(&theScene, "accountKeyboardChs8Button");
        assert(accountKeyboardChs8Button);

		accountKeyboardChs9Button = ituSceneFindWidget(&theScene, "accountKeyboardChs9Button");
        assert(accountKeyboardChs9Button);

		accountKeyboardChs0Button = ituSceneFindWidget(&theScene, "accountKeyboardChs0Button");
        assert(accountKeyboardChs0Button);

		accountKeyboardWarnBackground = ituSceneFindWidget(&theScene, "accountKeyboardWarnBackground");
        assert(accountKeyboardWarnBackground);
	}

    ituWidgetSetVisible(accountKeyboardEnUpperBackground, false);
    ituWidgetSetVisible(accountKeyboardEnLowerBackground, true);
    ituWidgetSetVisible(accountKeyboardChsBackground, false);
    ituWidgetSetVisible(accountKeyboardSymbolBackground, false);
    ituWidgetSetVisible(accountKeyboardEnUpperButton, false);
    ituWidgetSetVisible(accountKeyboardEnLowerButton, true);
    ituWidgetSetVisible(accountKeyboardChsButton, false);
    ituButtonSetString(accountKeyboardChs1Button, NULL);
    ituButtonSetString(accountKeyboardChs2Button, NULL);
    ituButtonSetString(accountKeyboardChs3Button, NULL);
    ituButtonSetString(accountKeyboardChs4Button, NULL);
    ituButtonSetString(accountKeyboardChs5Button, NULL);
    ituButtonSetString(accountKeyboardChs6Button, NULL);
    ituButtonSetString(accountKeyboardChs7Button, NULL);
    ituButtonSetString(accountKeyboardChs8Button, NULL);
    ituButtonSetString(accountKeyboardChs9Button, NULL);
    ituButtonSetString(accountKeyboardChs0Button, NULL);

    accountKeyboardChsIndex = -1;
    accountKeyboardChsString = NULL;
    accountKeyboardChsStringIndex = 0;

	return true;
}

bool AccountKeyboardOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void AccountKeyboardReset(void)
{
    accountLayer = NULL;
}
