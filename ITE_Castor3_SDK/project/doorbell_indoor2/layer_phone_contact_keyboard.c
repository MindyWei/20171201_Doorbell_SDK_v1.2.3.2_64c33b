#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"

extern unsigned char* py_ime(unsigned char input_py_val[], int* len);

static ITULayer* phoneContactLayer;
static ITUTextBox* phoneContactKeyboardCommentTextBox;
static ITUKeyboard* phoneContactKeyboard;
static ITUBackground* phoneContactKeyboardEnUpperBackground;
static ITUBackground* phoneContactKeyboardEnLowerBackground;
static ITUBackground* phoneContactKeyboardChsBackground;
static ITUBackground* phoneContactKeyboardSymbolBackground;
static ITUButton* phoneContactKeyboardChs1Button;
static ITUButton* phoneContactKeyboardChs2Button;
static ITUButton* phoneContactKeyboardChs3Button;
static ITUButton* phoneContactKeyboardChs4Button;
static ITUButton* phoneContactKeyboardChs5Button;
static ITUButton* phoneContactKeyboardChs6Button;
static ITUButton* phoneContactKeyboardChs7Button;
static ITUButton* phoneContactKeyboardChs8Button;
static ITUButton* phoneContactKeyboardChs9Button;
static ITUButton* phoneContactKeyboardChs0Button;
static ITUButton* phoneContactKeyboardSymbolButton;
static ITUButton* phoneContactKeyboardEnUpperButton;
static ITUButton* phoneContactKeyboardEnLowerButton;
static ITUButton* phoneContactKeyboardChsButton;

static int phoneContactKeyboardChsIndex;
static unsigned char* phoneContactKeyboardChsString;
static int phoneContactKeyboardChsStringIndex;

bool PhoneContactKeyboardEnterButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool PhoneContactKeyboardBackButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(phoneContactKeyboard->target));
        buf[phoneContactKeyboardChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox*)phoneContactKeyboard->target, buf);
        phoneContactKeyboardChsIndex = -1;
        ituButtonSetString(phoneContactKeyboardChs1Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs2Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs3Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs4Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs5Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs6Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs7Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs8Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs9Button, NULL);
        ituButtonSetString(phoneContactKeyboardChs0Button, NULL);
    }
    else
        ituTextBoxBack((ITUTextBox*)phoneContactKeyboard->target);
	return true;
}

bool PhoneContactKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param)
{
    phoneContactKeyboardChsString = NULL;
	return true;
}

bool PhoneContactKeyboardChsButtonOnPress(ITUWidget* widget, char* param)
{
    ituButtonSetString(phoneContactKeyboardChs1Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs2Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs3Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs4Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs5Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs6Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs7Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs8Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs9Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs0Button, NULL);
	return true;
}

bool PhoneContactKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardChsString)
    {
        if (phoneContactKeyboardChsStringIndex >= 20)
        {
            int i, count;

            phoneContactKeyboardChsStringIndex -= 20;

            count = strlen(phoneContactKeyboardChsString) - phoneContactKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = phoneContactKeyboardChsString[phoneContactKeyboardChsStringIndex + i] | ((wchar_t)phoneContactKeyboardChsString[phoneContactKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = phoneContactKeyboardChs1Button;
                    else if (i == 2)
                        btn = phoneContactKeyboardChs2Button;
                    else if (i == 4)
                        btn = phoneContactKeyboardChs3Button;
                    else if (i == 6)
                        btn = phoneContactKeyboardChs4Button;
                    else if (i == 8)
                        btn = phoneContactKeyboardChs5Button;
                    else if (i == 10)
                        btn = phoneContactKeyboardChs6Button;
                    else if (i == 12)
                        btn = phoneContactKeyboardChs7Button;
                    else if (i == 14)
                        btn = phoneContactKeyboardChs8Button;
                    else if (i == 16)
                        btn = phoneContactKeyboardChs9Button;
                    else if (i == 18)
                        btn = phoneContactKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = phoneContactKeyboardChs1Button;
                else if (i == 2)
                    btn = phoneContactKeyboardChs2Button;
                else if (i == 4)
                    btn = phoneContactKeyboardChs3Button;
                else if (i == 6)
                    btn = phoneContactKeyboardChs4Button;
                else if (i == 8)
                    btn = phoneContactKeyboardChs5Button;
                else if (i == 10)
                    btn = phoneContactKeyboardChs6Button;
                else if (i == 12)
                    btn = phoneContactKeyboardChs7Button;
                else if (i == 14)
                    btn = phoneContactKeyboardChs8Button;
                else if (i == 16)
                    btn = phoneContactKeyboardChs9Button;
                else if (i == 18)
                    btn = phoneContactKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool PhoneContactKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param)
{
    if (phoneContactKeyboardChsString)
    {
        int count = strlen(phoneContactKeyboardChsString);
        if (phoneContactKeyboardChsStringIndex + 20 <= count)
        {
            int i;

            phoneContactKeyboardChsStringIndex += 20;

            count = strlen(phoneContactKeyboardChsString) - phoneContactKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = phoneContactKeyboardChsString[phoneContactKeyboardChsStringIndex + i] | ((wchar_t)phoneContactKeyboardChsString[phoneContactKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = phoneContactKeyboardChs1Button;
                    else if (i == 2)
                        btn = phoneContactKeyboardChs2Button;
                    else if (i == 4)
                        btn = phoneContactKeyboardChs3Button;
                    else if (i == 6)
                        btn = phoneContactKeyboardChs4Button;
                    else if (i == 8)
                        btn = phoneContactKeyboardChs5Button;
                    else if (i == 10)
                        btn = phoneContactKeyboardChs6Button;
                    else if (i == 12)
                        btn = phoneContactKeyboardChs7Button;
                    else if (i == 14)
                        btn = phoneContactKeyboardChs8Button;
                    else if (i == 16)
                        btn = phoneContactKeyboardChs9Button;
                    else if (i == 18)
                        btn = phoneContactKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = phoneContactKeyboardChs1Button;
                else if (i == 2)
                    btn = phoneContactKeyboardChs2Button;
                else if (i == 4)
                    btn = phoneContactKeyboardChs3Button;
                else if (i == 6)
                    btn = phoneContactKeyboardChs4Button;
                else if (i == 8)
                    btn = phoneContactKeyboardChs5Button;
                else if (i == 10)
                    btn = phoneContactKeyboardChs6Button;
                else if (i == 12)
                    btn = phoneContactKeyboardChs7Button;
                else if (i == 14)
                    btn = phoneContactKeyboardChs8Button;
                else if (i == 16)
                    btn = phoneContactKeyboardChs9Button;
                else if (i == 18)
                    btn = phoneContactKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool PhoneContactKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = ituTextGetString(&btn->text);
    char buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char* str = ituTextGetString(phoneContactKeyboard->target);
        int count = str ? strlen(str) : 0;

        if (count < phoneContactKeyboardCommentTextBox->maxLen)
        {
            unsigned char* imestr;
            wchar_t wc;
            int len, i;

            if (phoneContactKeyboardChsIndex == -1)
            {
                phoneContactKeyboardChsIndex = count;
            }
            if (count - phoneContactKeyboardChsIndex < 6)
                ituTextBoxInput((ITUTextBox*)phoneContactKeyboard->target, input);

            str = ituTextGetString(phoneContactKeyboard->target);
	        imestr = py_ime(&str[phoneContactKeyboardChsIndex], &count);
            if (imestr)
            {
                if (count > 20)
                    count = 20;
                else if (count % 2)
                    count--;

                ituButtonSetString(phoneContactKeyboardChs1Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs2Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs3Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs4Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs5Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs6Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs7Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs8Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs9Button, NULL);
                ituButtonSetString(phoneContactKeyboardChs0Button, NULL);

                for (i = 0; i < count; i += 2)
                {
                    wc = imestr[i] | ((wchar_t)imestr[i + 1] << 8);
                    len = wctomb(buf, wc);
                    if (len > 0)
                    {
                        ITUButton* btn = NULL;
                        buf[len] = '\0';

                        if (i == 0)
                            btn = phoneContactKeyboardChs1Button;
                        else if (i == 2)
                            btn = phoneContactKeyboardChs2Button;
                        else if (i == 4)
                            btn = phoneContactKeyboardChs3Button;
                        else if (i == 6)
                            btn = phoneContactKeyboardChs4Button;
                        else if (i == 8)
                            btn = phoneContactKeyboardChs5Button;
                        else if (i == 10)
                            btn = phoneContactKeyboardChs6Button;
                        else if (i == 12)
                            btn = phoneContactKeyboardChs7Button;
                        else if (i == 14)
                            btn = phoneContactKeyboardChs8Button;
                        else if (i == 16)
                            btn = phoneContactKeyboardChs9Button;
                        else if (i == 18)
                            btn = phoneContactKeyboardChs0Button;

                        if (btn)
                            ituButtonSetString(btn, buf);
                    }
                }

                if (count > 0)
                {
                    phoneContactKeyboardChsString = imestr;
                    phoneContactKeyboardChsStringIndex = 0;
                }
            }
        }
    }
    else
    {
        if (phoneContactKeyboardChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(phoneContactKeyboard->target));
            buf[phoneContactKeyboardChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox*)phoneContactKeyboard->target, buf);
            phoneContactKeyboardChsIndex = -1;
            ituButtonSetString(phoneContactKeyboardChs1Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs2Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs3Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs4Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs5Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs6Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs7Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs8Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs9Button, NULL);
            ituButtonSetString(phoneContactKeyboardChs0Button, NULL);
        }
    }
	return true;
}

bool PhoneContactKeyboardOnEnter(ITUWidget* widget, char* param)
{
	if (!phoneContactLayer)
    {
        phoneContactLayer = ituSceneFindWidget(&theScene, "phoneContactLayer");
        assert(phoneContactLayer);
            
        phoneContactKeyboardCommentTextBox = ituSceneFindWidget(&theScene, "phoneContactKeyboardCommentTextBox");
        assert(phoneContactKeyboardCommentTextBox);

        phoneContactKeyboard = ituSceneFindWidget(&theScene, "phoneContactKeyboard");
        assert(phoneContactKeyboard);
        phoneContactKeyboard->target = (ITUWidget*)phoneContactKeyboardCommentTextBox;

        phoneContactKeyboardEnUpperBackground = ituSceneFindWidget(&theScene, "phoneContactKeyboardEnUpperBackground");
        assert(phoneContactKeyboardEnUpperBackground);

        phoneContactKeyboardEnLowerBackground = ituSceneFindWidget(&theScene, "phoneContactKeyboardEnLowerBackground");
        assert(phoneContactKeyboardEnLowerBackground);

        phoneContactKeyboardChsBackground = ituSceneFindWidget(&theScene, "phoneContactKeyboardChsBackground");
        assert(phoneContactKeyboardChsBackground);

        phoneContactKeyboardSymbolBackground = ituSceneFindWidget(&theScene, "phoneContactKeyboardSymbolBackground");
        assert(phoneContactKeyboardSymbolBackground);

        phoneContactKeyboardSymbolButton = ituSceneFindWidget(&theScene, "phoneContactKeyboardSymbolButton");
        assert(phoneContactKeyboardSymbolButton);

        phoneContactKeyboardEnUpperButton = ituSceneFindWidget(&theScene, "phoneContactKeyboardEnUpperButton");
        assert(phoneContactKeyboardEnUpperButton);

        phoneContactKeyboardEnLowerButton = ituSceneFindWidget(&theScene, "phoneContactKeyboardEnLowerButton");
        assert(phoneContactKeyboardEnLowerButton);

        phoneContactKeyboardChsButton = ituSceneFindWidget(&theScene, "phoneContactKeyboardChsButton");
        assert(phoneContactKeyboardChsButton);

		phoneContactKeyboardChs1Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs1Button");
        assert(phoneContactKeyboardChs1Button);

		phoneContactKeyboardChs2Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs2Button");
        assert(phoneContactKeyboardChs2Button);

		phoneContactKeyboardChs3Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs3Button");
        assert(phoneContactKeyboardChs3Button);

		phoneContactKeyboardChs4Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs4Button");
        assert(phoneContactKeyboardChs4Button);

		phoneContactKeyboardChs5Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs5Button");
        assert(phoneContactKeyboardChs5Button);

		phoneContactKeyboardChs6Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs6Button");
        assert(phoneContactKeyboardChs6Button);

		phoneContactKeyboardChs7Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs7Button");
        assert(phoneContactKeyboardChs7Button);

		phoneContactKeyboardChs8Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs8Button");
        assert(phoneContactKeyboardChs8Button);

		phoneContactKeyboardChs9Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs9Button");
        assert(phoneContactKeyboardChs9Button);

		phoneContactKeyboardChs0Button = ituSceneFindWidget(&theScene, "phoneContactKeyboardChs0Button");
        assert(phoneContactKeyboardChs0Button);
	}

    ituWidgetSetVisible(phoneContactKeyboardEnUpperBackground, false);
    ituWidgetSetVisible(phoneContactKeyboardEnLowerBackground, true);
    ituWidgetSetVisible(phoneContactKeyboardChsBackground, false);
    ituWidgetSetVisible(phoneContactKeyboardSymbolBackground, false);
    ituWidgetSetVisible(phoneContactKeyboardEnUpperButton, false);
    ituWidgetSetVisible(phoneContactKeyboardEnLowerButton, true);
    ituWidgetSetVisible(phoneContactKeyboardChsButton, false);
    ituButtonSetString(phoneContactKeyboardChs1Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs2Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs3Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs4Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs5Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs6Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs7Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs8Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs9Button, NULL);
    ituButtonSetString(phoneContactKeyboardChs0Button, NULL);

    phoneContactKeyboardChsIndex = -1;
    phoneContactKeyboardChsString = NULL;
    phoneContactKeyboardChsStringIndex = 0;

	return true;
}

void PhoneContactKeyboardReset(void)
{
    phoneContactLayer = NULL;
}
