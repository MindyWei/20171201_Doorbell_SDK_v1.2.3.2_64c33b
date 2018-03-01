#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITULayer* settingLayer;
static ITULayer* settingLocalPositionLayer;
static ITURadioBox* settingLocalPositionRadioBox;
static ITURadioBox* settingLocalPositionTypeRadioBox;
static ITUTextBox* settingLocalPositionTypeTextBox;
static ITURadioBox* settingLocalPositionPosRadioBox;
static ITUTextBox* settingLocalPositionPosTextBox;
static ITURadioBox* settingLocalPositionExtensionRadioBox;
static ITUTextBox* settingLocalPositionExtensionTextBox;
static ITURadioBox* settingLocalPositionActiveRadioBox;
static ITUBackground* settingLocalPositionWarningBackground;

static ITUTextBox* settingLocalPositionActiveTextBox;

bool SettingLocalPositionKeyStarOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingLocalPositionWarningBackground))
    {
        ituWidgetSetVisible(settingLocalPositionWarningBackground, false);
        return true;
    }

    if (settingLocalPositionActiveRadioBox == settingLocalPositionTypeRadioBox)
    {
        settingLocalPositionActiveRadioBox = settingLocalPositionPosRadioBox;
        settingLocalPositionActiveTextBox = settingLocalPositionPosTextBox;
        ituRadioBoxSetChecked(settingLocalPositionPosRadioBox, true);
        ituFocusWidget(settingLocalPositionPosTextBox);
    }
    else if (settingLocalPositionActiveRadioBox == settingLocalPositionPosRadioBox)
    {
        settingLocalPositionActiveRadioBox = settingLocalPositionExtensionRadioBox;
        settingLocalPositionActiveTextBox = settingLocalPositionExtensionTextBox;
        ituRadioBoxSetChecked(settingLocalPositionExtensionRadioBox, true);
        ituFocusWidget(settingLocalPositionExtensionTextBox);
    }
    else
    {
        char* pos = ituTextBoxGetString(settingLocalPositionPosTextBox);
        char* ext = ituTextBoxGetString(settingLocalPositionExtensionTextBox);
        char* t = ituTextBoxGetString(settingLocalPositionTypeTextBox);
        int type = t ? atoi(t) : -1;
        bool exist = AddressBookIsEntranceExist(type, pos, ext);

        if (!exist)
        {
             // info incorrect
            char buf[4];
            sprintf(buf, "%d", theConfig.local_type);
            ituTextBoxSetString(settingLocalPositionTypeTextBox, buf);
            ituTextBoxSetString(settingLocalPositionPosTextBox, theConfig.local_pos);
            ituTextBoxSetString(settingLocalPositionExtensionTextBox, theConfig.local_ext);
            settingLocalPositionActiveRadioBox = settingLocalPositionTypeRadioBox;
            settingLocalPositionActiveTextBox = settingLocalPositionTypeTextBox;
            ituRadioBoxSetChecked(settingLocalPositionTypeRadioBox, true);
            ituFocusWidget(settingLocalPositionTypeTextBox);

            ituWidgetSetVisible(settingLocalPositionWarningBackground, true);
        }
        else
        {
            theConfig.local_type = type;

            if (pos)
                strcpy(theConfig.local_pos, pos);
            else
                theConfig.local_pos[0] = '\0';

            if (ext)
                strcpy(theConfig.local_ext, ext);
            else
                theConfig.local_ext[0] = '\0';

            ConfigSaveAll();

            // reset network
            SceneQuit(QUIT_RESET_NETWORK);

            ituRadioBoxSetChecked(settingLocalPositionActiveRadioBox, false);

	        ituWidgetEnable(settingLayer);
			ituLayerGoto(settingLayer);
            ituFocusWidget(settingLocalPositionRadioBox);
            ituWidgetSetVisible(settingLocalPositionLayer, true);
        }
    }
    return true;
}

bool SettingLocalPositionKeySharpOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingLocalPositionWarningBackground))
        return true;

    if (settingLocalPositionActiveRadioBox == settingLocalPositionTypeRadioBox)
    {
		if(ituTextBoxGetString(settingLocalPositionTypeTextBox))
		{
            ituTextBoxSetString(settingLocalPositionTypeTextBox, NULL);
		}
		else
		{
            ituRadioBoxSetChecked(settingLocalPositionTypeRadioBox, false);
	        ituWidgetEnable(settingLayer);
            ituLayerGoto(settingLayer);
            ituFocusWidget(settingLocalPositionRadioBox);
            ituWidgetSetVisible(settingLocalPositionLayer, true);
		}
    }
    else if (settingLocalPositionActiveRadioBox == settingLocalPositionPosRadioBox)
    {
		if(ituTextBoxGetString(settingLocalPositionPosTextBox))
		{
            ituTextBoxSetString(settingLocalPositionPosTextBox, NULL);
		}
		else
		{       
            settingLocalPositionActiveRadioBox = settingLocalPositionTypeRadioBox;
            settingLocalPositionActiveTextBox = settingLocalPositionTypeTextBox;
            ituRadioBoxSetChecked(settingLocalPositionTypeRadioBox, true);
            ituTextBoxSetString(settingLocalPositionTypeTextBox, NULL);
            ituFocusWidget(settingLocalPositionTypeTextBox);
        }
	}
    else //settingLocalPositionActiveRadioBox == settingLocalPositionExtensionRadioBox
    {
		if(ituTextBoxGetString(settingLocalPositionExtensionTextBox))
		{
            ituTextBoxSetString(settingLocalPositionExtensionTextBox, NULL);
		}
		else
		{       
            settingLocalPositionActiveRadioBox = settingLocalPositionPosRadioBox;
            settingLocalPositionActiveTextBox = settingLocalPositionPosTextBox;
            ituRadioBoxSetChecked(settingLocalPositionPosRadioBox, true);
            ituTextBoxSetString(settingLocalPositionPosTextBox, NULL);
            ituFocusWidget(settingLocalPositionPosTextBox);
        }
	}
	return true;
}

bool SettingLocalPositionKeyManagerOnPress(ITUWidget* widget, char* param)
{
    char buf[4];

    if (ituWidgetIsVisible(settingLocalPositionWarningBackground))
        return true;

    sprintf(buf, "%d", theConfig.local_type);
    ituTextBoxSetString(settingLocalPositionTypeTextBox, buf);
    ituTextBoxSetString(settingLocalPositionPosTextBox, theConfig.local_pos);
    ituTextBoxSetString(settingLocalPositionExtensionTextBox, theConfig.local_ext);
	return true;
}

bool SettingLocalPositionKeyNumberOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(settingLocalPositionWarningBackground))
        return true;

    ituTextBoxInput(settingLocalPositionActiveTextBox, param);
	return true;
}

void SettingLocalPositionShow(void)
{
    char buf[4];
    if (!settingLocalPositionTypeTextBox)
    {
        settingLocalPositionTypeTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionTypeTextBox");
        assert(settingLocalPositionTypeTextBox);

        settingLocalPositionPosTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionPosTextBox");
        assert(settingLocalPositionPosTextBox);

        settingLocalPositionExtensionTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionExtensionTextBox");
        assert(settingLocalPositionExtensionTextBox);

        settingLocalPositionWarningBackground = ituSceneFindWidget(&theScene, "settingLocalPositionWarningBackground");
        assert(settingLocalPositionWarningBackground);
	}
    // current settings
    sprintf(buf, "%d", theConfig.local_type);
    ituTextBoxSetString(settingLocalPositionTypeTextBox, buf);
    ituTextBoxSetString(settingLocalPositionPosTextBox, theConfig.local_pos);
    ituTextBoxSetString(settingLocalPositionExtensionTextBox, theConfig.local_ext);
    ituWidgetSetVisible(settingLocalPositionWarningBackground, false);
}

bool SettingLocalPositionOnEnter(ITUWidget* widget, char* param)
{
    if (!settingLocalPositionLayer)
    {
        settingLocalPositionLayer = ituSceneFindWidget(&theScene, "settingLocalPositionLayer");
        assert(settingLocalPositionLayer);

        settingLayer = ituSceneFindWidget(&theScene, "settingLayer");
        assert(settingLayer);

        settingLocalPositionRadioBox = ituSceneFindWidget(&theScene, "settingLocalPositionRadioBox");
        assert(settingLocalPositionRadioBox);

        settingLocalPositionTypeRadioBox = ituSceneFindWidget(&theScene, "settingLocalPositionTypeRadioBox");
        assert(settingLocalPositionTypeRadioBox);

        settingLocalPositionPosRadioBox = ituSceneFindWidget(&theScene, "settingLocalPositionPosRadioBox");
        assert(settingLocalPositionPosRadioBox);

        settingLocalPositionExtensionRadioBox = ituSceneFindWidget(&theScene, "settingLocalPositionExtensionRadioBox");
        assert(settingLocalPositionExtensionRadioBox);

        if (!settingLocalPositionTypeTextBox)
        {
            settingLocalPositionTypeTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionTypeTextBox");
            assert(settingLocalPositionTypeTextBox);

            settingLocalPositionPosTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionPosTextBox");
            assert(settingLocalPositionPosTextBox);

            settingLocalPositionExtensionTextBox = ituSceneFindWidget(&theScene, "settingLocalPositionExtensionTextBox");
            assert(settingLocalPositionExtensionTextBox);

            settingLocalPositionWarningBackground = ituSceneFindWidget(&theScene, "settingLocalPositionWarningBackground");
            assert(settingLocalPositionWarningBackground);
	    }
    }
    settingLocalPositionActiveRadioBox = settingLocalPositionTypeRadioBox;
    settingLocalPositionActiveTextBox = settingLocalPositionTypeTextBox;
    ituFocusWidget(settingLocalPositionTypeTextBox);
    ituRadioBoxSetChecked(settingLocalPositionTypeRadioBox, true);
	//Disable settingLayer to avoid receive event
	ituWidgetDisable(settingLayer);
	return true;
}

void SettingLocalPositionReset(void)
{
    settingLocalPositionLayer = NULL;
	settingLocalPositionTypeTextBox = NULL;
}
