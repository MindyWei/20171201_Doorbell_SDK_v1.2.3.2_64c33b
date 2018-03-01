#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

extern ITUBackground* phoneResidentRoomBackground;
extern ITUTextBox* phoneAreaTextBox;
extern ITUTextBox* phoneBuildingTextBox;
extern ITUTextBox* phoneUnitTextBox;
extern ITUTextBox* phoneFloorTextBox;
extern ITUTextBox* phoneRoomTextBox;
extern ITUBackground* phoneResidentAliasBackground;

static ITULayer* phoneResidentLayer;
static ITULayer* phoneAdminLayer;
static ITUListBox* phoneAdminListBox;
static ITUListBox* phoneMiscListBox;

// status
extern ITUTextBox* activePhoneDialTextBox;

bool PhoneNumberButtonOnPress(ITUWidget* widget, char* param)
{
    if (!phoneResidentLayer)
    {
        phoneResidentLayer = ituSceneFindWidget(&theScene, "phoneResidentLayer");
        assert(phoneResidentLayer);
    }

    if (ituWidgetIsVisible(phoneResidentLayer))
    {
        ITUButton* btn = (ITUButton*) widget;
        char* input = param;
        char* str = ituTextGetString(activePhoneDialTextBox);
        int count = str ? strlen(str) : 0;

        ituWidgetSetVisible(phoneResidentRoomBackground, true);
        ituWidgetSetVisible(phoneResidentAliasBackground, false);

        if (count >= activePhoneDialTextBox->maxLen)
        {
            if (activePhoneDialTextBox == phoneAreaTextBox)
                activePhoneDialTextBox = phoneBuildingTextBox;
            else if (activePhoneDialTextBox == phoneBuildingTextBox)
                activePhoneDialTextBox = phoneUnitTextBox;
            else if (activePhoneDialTextBox == phoneUnitTextBox)
                activePhoneDialTextBox = phoneFloorTextBox;
            else if (activePhoneDialTextBox == phoneFloorTextBox)
                activePhoneDialTextBox = phoneRoomTextBox;
        }
        ituTextBoxInput(activePhoneDialTextBox, input);
    }
	return true;
}

bool PhoneCallButtonOnPress(ITUWidget* widget, char* param)
{
    if (!phoneAdminLayer)
    {
        if (!phoneResidentLayer)
        {
            phoneResidentLayer = ituSceneFindWidget(&theScene, "phoneResidentLayer");
            assert(phoneResidentLayer);
        }
        phoneAdminLayer = ituSceneFindWidget(&theScene, "phoneAdminLayer");
        assert(phoneAdminLayer);

        phoneAdminListBox = ituSceneFindWidget(&theScene, "phoneAdminListBox");
        assert(phoneAdminListBox);

        phoneMiscListBox = ituSceneFindWidget(&theScene, "phoneMiscListBox");
        assert(phoneMiscListBox);
    }

    if (ituWidgetIsVisible(phoneResidentLayer))
    {
        if (ituTextGetString(activePhoneDialTextBox))
        {
            if (activePhoneDialTextBox == phoneRoomTextBox)
            {
                if (CallingCall(
                    ituTextGetString(phoneAreaTextBox),
                    ituTextGetString(phoneBuildingTextBox),
                    ituTextGetString(phoneUnitTextBox),
                    ituTextGetString(phoneFloorTextBox),
                    ituTextGetString(phoneRoomTextBox),
                    NULL) == 0)
				    return true; // goto callingLayer
            }
        }
    }
    else if (ituWidgetIsVisible(phoneAdminLayer))
    {
        ITUWidget* item = ituListBoxGetFocusItem(phoneAdminListBox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

		    if (CallingCallByIP(info->ip) == 0)
            {
			    return true; // goto callingLayer
		    }
        }
    }
    else
    {
        ITUWidget* item = ituListBoxGetFocusItem(phoneMiscListBox);
        if (item)
        {
            DeviceInfo* info = (DeviceInfo*) ituWidgetGetCustomData(item);

		    if (CallingCallByIP(info->ip) == 0)
            {
			    return true; // goto callingLayer
		    }
        }
    }
	return false;
}

void PhoneReset(void)
{
    phoneResidentLayer = NULL;
    phoneAdminLayer = NULL;
}

