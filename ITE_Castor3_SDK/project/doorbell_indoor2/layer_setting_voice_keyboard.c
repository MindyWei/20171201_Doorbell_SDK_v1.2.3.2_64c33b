#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "ite/ith.h"

extern unsigned char* py_ime(unsigned char input_py_val[], int* len);

static ITUTextBox* settingVoiceKeyboardUserTextBox;
static ITUKeyboard* settingVoiceKeyboard;
static ITUBackground* settingVoiceKeyboardEnUpperBackground;
static ITUBackground* settingVoiceKeyboardEnLowerBackground;
static ITUBackground* settingVoiceKeyboardChsBackground;
static ITUBackground* settingVoiceKeyboardSymbolBackground;
static ITUButton* settingVoiceKeyboardChs1Button;
static ITUButton* settingVoiceKeyboardChs2Button;
static ITUButton* settingVoiceKeyboardChs3Button;
static ITUButton* settingVoiceKeyboardChs4Button;
static ITUButton* settingVoiceKeyboardChs5Button;
static ITUButton* settingVoiceKeyboardChs6Button;
static ITUButton* settingVoiceKeyboardChs7Button;
static ITUButton* settingVoiceKeyboardChs8Button;
static ITUButton* settingVoiceKeyboardChs9Button;
static ITUButton* settingVoiceKeyboardChs0Button;
static ITUButton* settingVoiceKeyboardSymbolButton;
static ITUButton* settingVoiceKeyboardEnUpperButton;
static ITUButton* settingVoiceKeyboardEnLowerButton;
static ITUButton* settingVoiceKeyboardChsButton;

static int settingVoiceKeyboardChsIndex;
static unsigned char* settingVoiceKeyboardChsString;
static int settingVoiceKeyboardChsStringIndex;

bool SettingVoiceKeyboardEnterButtonOnPress(ITUWidget* widget, char* param)
{
    char* str = ituTextGetString(settingVoiceKeyboard->target);
    int count = str ? strlen(str) : 0;

    if (count == 0)
        return false;

	return true;
}

bool SettingVoiceKeyboardBackButtonOnPress(ITUWidget* widget, char* param)
{
    if (settingVoiceKeyboardChsIndex >= 0)
    {
        char buf[512];

        strcpy(buf, ituTextGetString(settingVoiceKeyboard->target));
        buf[settingVoiceKeyboardChsIndex] = '\0';
        ituTextBoxSetString((ITUTextBox*)settingVoiceKeyboard->target, buf);
        settingVoiceKeyboardChsIndex = -1;
        ituButtonSetString(settingVoiceKeyboardChs1Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs2Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs3Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs4Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs5Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs6Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs7Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs8Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs9Button, NULL);
        ituButtonSetString(settingVoiceKeyboardChs0Button, NULL);
    }
    else
        ituTextBoxBack((ITUTextBox*)settingVoiceKeyboard->target);
	return true;
}

bool SettingVoiceKeyboardEnUpperButtonOnPress(ITUWidget* widget, char* param)
{
    settingVoiceKeyboardChsString = NULL;
	return true;
}

bool SettingVoiceKeyboardChsButtonOnPress(ITUWidget* widget, char* param)
{
    ituButtonSetString(settingVoiceKeyboardChs1Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs2Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs3Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs4Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs5Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs6Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs7Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs8Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs9Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs0Button, NULL);
	return true;
}

bool SettingVoiceKeyboardPageUpButtonOnPress(ITUWidget* widget, char* param)
{
    if (settingVoiceKeyboardChsString)
    {
        if (settingVoiceKeyboardChsStringIndex >= 20)
        {
            int i, count;

            settingVoiceKeyboardChsStringIndex -= 20;

            count = strlen(settingVoiceKeyboardChsString) - settingVoiceKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = settingVoiceKeyboardChsString[settingVoiceKeyboardChsStringIndex + i] | ((wchar_t)settingVoiceKeyboardChsString[settingVoiceKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = settingVoiceKeyboardChs1Button;
                    else if (i == 2)
                        btn = settingVoiceKeyboardChs2Button;
                    else if (i == 4)
                        btn = settingVoiceKeyboardChs3Button;
                    else if (i == 6)
                        btn = settingVoiceKeyboardChs4Button;
                    else if (i == 8)
                        btn = settingVoiceKeyboardChs5Button;
                    else if (i == 10)
                        btn = settingVoiceKeyboardChs6Button;
                    else if (i == 12)
                        btn = settingVoiceKeyboardChs7Button;
                    else if (i == 14)
                        btn = settingVoiceKeyboardChs8Button;
                    else if (i == 16)
                        btn = settingVoiceKeyboardChs9Button;
                    else if (i == 18)
                        btn = settingVoiceKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = settingVoiceKeyboardChs1Button;
                else if (i == 2)
                    btn = settingVoiceKeyboardChs2Button;
                else if (i == 4)
                    btn = settingVoiceKeyboardChs3Button;
                else if (i == 6)
                    btn = settingVoiceKeyboardChs4Button;
                else if (i == 8)
                    btn = settingVoiceKeyboardChs5Button;
                else if (i == 10)
                    btn = settingVoiceKeyboardChs6Button;
                else if (i == 12)
                    btn = settingVoiceKeyboardChs7Button;
                else if (i == 14)
                    btn = settingVoiceKeyboardChs8Button;
                else if (i == 16)
                    btn = settingVoiceKeyboardChs9Button;
                else if (i == 18)
                    btn = settingVoiceKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool SettingVoiceKeyboardPageDownButtonOnPress(ITUWidget* widget, char* param)
{
    if (settingVoiceKeyboardChsString)
    {
        int count = strlen(settingVoiceKeyboardChsString);
        if (settingVoiceKeyboardChsStringIndex + 20 <= count)
        {
            int i;

            settingVoiceKeyboardChsStringIndex += 20;

            count = strlen(settingVoiceKeyboardChsString) - settingVoiceKeyboardChsStringIndex;
            if (count > 20)
                count = 20;
            else if (count % 2)
                count--;

            for (i = 0; i < count; i += 2)
            {
                char buf[64];
                int len;

                wchar_t wc = settingVoiceKeyboardChsString[settingVoiceKeyboardChsStringIndex + i] | ((wchar_t)settingVoiceKeyboardChsString[settingVoiceKeyboardChsStringIndex + i + 1] << 8);
                len = wctomb(buf, wc);
                if (len > 0)
                {
                    ITUButton* btn = NULL;
                    buf[len] = '\0';

                    if (i == 0)
                        btn = settingVoiceKeyboardChs1Button;
                    else if (i == 2)
                        btn = settingVoiceKeyboardChs2Button;
                    else if (i == 4)
                        btn = settingVoiceKeyboardChs3Button;
                    else if (i == 6)
                        btn = settingVoiceKeyboardChs4Button;
                    else if (i == 8)
                        btn = settingVoiceKeyboardChs5Button;
                    else if (i == 10)
                        btn = settingVoiceKeyboardChs6Button;
                    else if (i == 12)
                        btn = settingVoiceKeyboardChs7Button;
                    else if (i == 14)
                        btn = settingVoiceKeyboardChs8Button;
                    else if (i == 16)
                        btn = settingVoiceKeyboardChs9Button;
                    else if (i == 18)
                        btn = settingVoiceKeyboardChs0Button;

                    if (btn)
                        ituButtonSetString(btn, buf);
                }
            }

            for (; i < 20; i += 2)
            {
                ITUButton* btn = NULL;

                if (i == 0)
                    btn = settingVoiceKeyboardChs1Button;
                else if (i == 2)
                    btn = settingVoiceKeyboardChs2Button;
                else if (i == 4)
                    btn = settingVoiceKeyboardChs3Button;
                else if (i == 6)
                    btn = settingVoiceKeyboardChs4Button;
                else if (i == 8)
                    btn = settingVoiceKeyboardChs5Button;
                else if (i == 10)
                    btn = settingVoiceKeyboardChs6Button;
                else if (i == 12)
                    btn = settingVoiceKeyboardChs7Button;
                else if (i == 14)
                    btn = settingVoiceKeyboardChs8Button;
                else if (i == 16)
                    btn = settingVoiceKeyboardChs9Button;
                else if (i == 18)
                    btn = settingVoiceKeyboardChs0Button;

                if (btn)
                    ituButtonSetString(btn, NULL);
            }
        }
    }
	return true;
}

bool SettingVoiceKeyboardChsCharButtonOnPress(ITUWidget* widget, char* param)
{
    ITUButton* btn = (ITUButton*) widget;
    char* input = ituTextGetString(&btn->text);
    char buf[512];

    if (*input >= 'a' && *input <= 'z')
    {
        char* str = ituTextGetString(settingVoiceKeyboard->target);
        int count = str ? strlen(str) : 0;

        if (count < settingVoiceKeyboardUserTextBox->maxLen)
        {
            unsigned char* imestr;
            wchar_t wc;
            int len, i;

            if (settingVoiceKeyboardChsIndex == -1)
            {
                settingVoiceKeyboardChsIndex = count;
            }
            if (count - settingVoiceKeyboardChsIndex < 6)
                ituTextBoxInput((ITUTextBox*)settingVoiceKeyboard->target, input);

            str = ituTextGetString(settingVoiceKeyboard->target);
	        imestr = py_ime(&str[settingVoiceKeyboardChsIndex], &count);
            if (imestr)
            {
                if (count > 20)
                    count = 20;
                else if (count % 2)
                    count--;

                ituButtonSetString(settingVoiceKeyboardChs1Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs2Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs3Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs4Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs5Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs6Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs7Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs8Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs9Button, NULL);
                ituButtonSetString(settingVoiceKeyboardChs0Button, NULL);

                for (i = 0; i < count; i += 2)
                {
                    wc = imestr[i] | ((wchar_t)imestr[i + 1] << 8);
                    len = wctomb(buf, wc);
                    if (len > 0)
                    {
                        ITUButton* btn = NULL;
                        buf[len] = '\0';

                        if (i == 0)
                            btn = settingVoiceKeyboardChs1Button;
                        else if (i == 2)
                            btn = settingVoiceKeyboardChs2Button;
                        else if (i == 4)
                            btn = settingVoiceKeyboardChs3Button;
                        else if (i == 6)
                            btn = settingVoiceKeyboardChs4Button;
                        else if (i == 8)
                            btn = settingVoiceKeyboardChs5Button;
                        else if (i == 10)
                            btn = settingVoiceKeyboardChs6Button;
                        else if (i == 12)
                            btn = settingVoiceKeyboardChs7Button;
                        else if (i == 14)
                            btn = settingVoiceKeyboardChs8Button;
                        else if (i == 16)
                            btn = settingVoiceKeyboardChs9Button;
                        else if (i == 18)
                            btn = settingVoiceKeyboardChs0Button;

                        if (btn)
                            ituButtonSetString(btn, buf);
                    }
                }

                if (count > 0)
                {
                    settingVoiceKeyboardChsString = imestr;
                    settingVoiceKeyboardChsStringIndex = 0;
                }
            }
        }
    }
    else
    {
        if (settingVoiceKeyboardChsIndex >= 0)
        {
            strcpy(buf, ituTextGetString(settingVoiceKeyboard->target));
            buf[settingVoiceKeyboardChsIndex] = '\0';
            strcat(buf, ituTextGetString(&btn->text));
            ituTextBoxSetString((ITUTextBox*)settingVoiceKeyboard->target, buf);
            settingVoiceKeyboardChsIndex = -1;
            ituButtonSetString(settingVoiceKeyboardChs1Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs2Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs3Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs4Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs5Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs6Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs7Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs8Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs9Button, NULL);
            ituButtonSetString(settingVoiceKeyboardChs0Button, NULL);
        }
    }
	return true;
}

bool SettingVoiceKeyboardOnEnter(ITUWidget* widget, char* param)
{
	if (!settingVoiceKeyboardUserTextBox)
    {
        settingVoiceKeyboardUserTextBox = ituSceneFindWidget(&theScene, "settingVoiceKeyboardUserTextBox");
        assert(settingVoiceKeyboardUserTextBox);

        settingVoiceKeyboard = ituSceneFindWidget(&theScene, "settingVoiceKeyboard");
        assert(settingVoiceKeyboard);
        settingVoiceKeyboard->target = (ITUWidget*)settingVoiceKeyboardUserTextBox;

        settingVoiceKeyboardEnUpperBackground = ituSceneFindWidget(&theScene, "settingVoiceKeyboardEnUpperBackground");
        assert(settingVoiceKeyboardEnUpperBackground);

        settingVoiceKeyboardEnLowerBackground = ituSceneFindWidget(&theScene, "settingVoiceKeyboardEnLowerBackground");
        assert(settingVoiceKeyboardEnLowerBackground);

        settingVoiceKeyboardChsBackground = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChsBackground");
        assert(settingVoiceKeyboardChsBackground);

        settingVoiceKeyboardSymbolBackground = ituSceneFindWidget(&theScene, "settingVoiceKeyboardSymbolBackground");
        assert(settingVoiceKeyboardSymbolBackground);

        settingVoiceKeyboardSymbolButton = ituSceneFindWidget(&theScene, "settingVoiceKeyboardSymbolButton");
        assert(settingVoiceKeyboardSymbolButton);

        settingVoiceKeyboardEnUpperButton = ituSceneFindWidget(&theScene, "settingVoiceKeyboardEnUpperButton");
        assert(settingVoiceKeyboardEnUpperButton);

        settingVoiceKeyboardEnLowerButton = ituSceneFindWidget(&theScene, "settingVoiceKeyboardEnLowerButton");
        assert(settingVoiceKeyboardEnLowerButton);

        settingVoiceKeyboardChsButton = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChsButton");
        assert(settingVoiceKeyboardChsButton);

		settingVoiceKeyboardChs1Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs1Button");
        assert(settingVoiceKeyboardChs1Button);

		settingVoiceKeyboardChs2Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs2Button");
        assert(settingVoiceKeyboardChs2Button);

		settingVoiceKeyboardChs3Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs3Button");
        assert(settingVoiceKeyboardChs3Button);

		settingVoiceKeyboardChs4Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs4Button");
        assert(settingVoiceKeyboardChs4Button);

		settingVoiceKeyboardChs5Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs5Button");
        assert(settingVoiceKeyboardChs5Button);

		settingVoiceKeyboardChs6Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs6Button");
        assert(settingVoiceKeyboardChs6Button);

		settingVoiceKeyboardChs7Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs7Button");
        assert(settingVoiceKeyboardChs7Button);

		settingVoiceKeyboardChs8Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs8Button");
        assert(settingVoiceKeyboardChs8Button);

		settingVoiceKeyboardChs9Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs9Button");
        assert(settingVoiceKeyboardChs9Button);

		settingVoiceKeyboardChs0Button = ituSceneFindWidget(&theScene, "settingVoiceKeyboardChs0Button");
        assert(settingVoiceKeyboardChs0Button);
	}

    ituWidgetSetVisible(settingVoiceKeyboardEnUpperBackground, false);
    ituWidgetSetVisible(settingVoiceKeyboardEnLowerBackground, true);
    ituWidgetSetVisible(settingVoiceKeyboardChsBackground, false);
    ituWidgetSetVisible(settingVoiceKeyboardSymbolBackground, false);
    ituWidgetSetVisible(settingVoiceKeyboardEnUpperButton, false);
    ituWidgetSetVisible(settingVoiceKeyboardEnLowerButton, true);
    ituWidgetSetVisible(settingVoiceKeyboardChsButton, false);
    ituButtonSetString(settingVoiceKeyboardChs1Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs2Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs3Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs4Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs5Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs6Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs7Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs8Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs9Button, NULL);
    ituButtonSetString(settingVoiceKeyboardChs0Button, NULL);

    settingVoiceKeyboardChsIndex = -1;
    settingVoiceKeyboardChsString = NULL;
    settingVoiceKeyboardChsStringIndex = 0;

    ituTextBoxSetString(settingVoiceKeyboardUserTextBox, NULL);

	return true;
}

void SettingVoiceKeyboardReset(void)
{
    settingVoiceKeyboardUserTextBox = NULL;
}
