#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

extern ITUTextBox* redirectAreaTextBox;
extern ITUTextBox* redirectBuildingTextBox;
extern ITUTextBox* redirectUnitTextBox;
extern ITUTextBox* redirectFloorTextBox;
extern ITUTextBox* redirectRoomTextBox;

static ITULayer* redirectResidentLayer;
static ITULayer* redirectAdminLayer;
static ITUListBox* redirectAdminListBox;
static ITUListBox* redirectMiscListBox;

// status
extern ITUTextBox* activeRedirectDialTextBox;

bool RedirectNumberButtonOnPress(ITUWidget* widget, char* param)
{
    if (!redirectResidentLayer)
    {
        redirectResidentLayer = ituSceneFindWidget(&theScene, "redirectResidentLayer");
        assert(redirectResidentLayer);
    }

    if (ituWidgetIsVisible(redirectResidentLayer))
    {
        ITUButton* btn = (ITUButton*) widget;
        char* input = ituTextGetString(&btn->text);
        char* str = ituTextGetString(activeRedirectDialTextBox);
        int count = str ? strlen(str) : 0;

        if (count >= activeRedirectDialTextBox->maxLen)
        {
            if (activeRedirectDialTextBox == redirectAreaTextBox)
                activeRedirectDialTextBox = redirectBuildingTextBox;
            else if (activeRedirectDialTextBox == redirectBuildingTextBox)
                activeRedirectDialTextBox = redirectUnitTextBox;
            else if (activeRedirectDialTextBox == redirectUnitTextBox)
                activeRedirectDialTextBox = redirectFloorTextBox;
            else if (activeRedirectDialTextBox == redirectFloorTextBox)
                activeRedirectDialTextBox = redirectRoomTextBox;
        }
        ituTextBoxInput(activeRedirectDialTextBox, input);
    }
	return true;
}

bool RedirectCallButtonOnPress(ITUWidget* widget, char* param)
{
    if (!redirectAdminLayer)
    {
        if (!redirectResidentLayer)
        {
            redirectResidentLayer = ituSceneFindWidget(&theScene, "redirectResidentLayer");
            assert(redirectResidentLayer);
        }
        redirectAdminLayer = ituSceneFindWidget(&theScene, "redirectAdminLayer");
        assert(redirectAdminLayer);

        redirectAdminListBox = ituSceneFindWidget(&theScene, "redirectAdminListBox");
        assert(redirectAdminListBox);

        redirectMiscListBox = ituSceneFindWidget(&theScene, "redirectMiscListBox");
        assert(redirectMiscListBox);
    }

    if (ituWidgetIsVisible(redirectResidentLayer))
    {
        if (ituTextGetString(activeRedirectDialTextBox))
        {
            if (activeRedirectDialTextBox == redirectRoomTextBox)
            {
                char* ip = AddressBookGetDeviceIP(
                    ituTextBoxGetString(redirectAreaTextBox), 
                    ituTextBoxGetString(redirectBuildingTextBox), 
                    ituTextBoxGetString(redirectUnitTextBox), 
                    ituTextBoxGetString(redirectFloorTextBox), 
                    ituTextBoxGetString(redirectRoomTextBox), 
                    "01");
                if (ip)
                {
                    if (SceneIsCallConnected())
                    {
                        LinphoneTransfer(ip);
                    }
                    else
                    {
		                LinphoneRedirect(ip);
                    }
                    free(ip);
                    return true; // goto mainMenuLayer
                }
            }
        }
    }
    else if (ituWidgetIsVisible(redirectAdminLayer))
    {
        ITUWidget* item = ituListBoxGetFocusItem(redirectAdminListBox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

            if (SceneIsCallConnected())
            {
                LinphoneTransfer(info->ip);
            }
            else
            {
		        LinphoneRedirect(info->ip);
            }
	        return true; // goto mainMenuLayer
        }
    }
    else
    {
        ITUWidget* item = ituListBoxGetFocusItem(redirectMiscListBox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

            if (SceneIsCallConnected())
            {
                LinphoneTransfer(info->ip);
            }
            else
            {
		        LinphoneRedirect(info->ip);
            }
	        return true; // goto mainMenuLayer
        }
    }
	return false;
}

void RedirectHangUp(int arg)
{
    LinphoneTerminate(0);
    SceneHangUp();
}

void RedirectReset(void)
{
    redirectResidentLayer = NULL;
    redirectAdminLayer = NULL;
}

