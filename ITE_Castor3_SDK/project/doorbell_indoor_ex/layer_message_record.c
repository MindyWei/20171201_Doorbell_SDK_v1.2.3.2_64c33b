#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "scene.h"

static ITUScrollListBox* messageRecordTopicScrollListBox;
static ITUScrollListBox* messageRecordTimeScrollListBox;
static ITUScrollIconListBox* messageRecordScrollIconListBox;

bool MessageRecordScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageRecordTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageRecordTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageRecordScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageRecordDeleteButtonOnPress(ITUWidget* widget, char* param)
{  
	return true;
}

bool MessageRecordOnEnter(ITUWidget* widget, char* param)
{
    if (!messageRecordTopicScrollListBox)
    {
        messageRecordTopicScrollListBox = ituSceneFindWidget(&theScene, "messageRecordTopicScrollListBox");
        assert(messageRecordTopicScrollListBox);

        messageRecordTimeScrollListBox = ituSceneFindWidget(&theScene, "messageRecordTimeScrollListBox");
        assert(messageRecordTimeScrollListBox);
	
        messageRecordScrollIconListBox = ituSceneFindWidget(&theScene, "messageRecordScrollIconListBox");
        assert(messageRecordScrollIconListBox);    
    }

    if (strcmp(param, "messageRecordViewLayer"))
    {
        ituListBoxReload((ITUListBox*)messageRecordTopicScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordTimeScrollListBox);
        ituListBoxReload((ITUListBox*)messageRecordScrollIconListBox);
    }
    else
    {
        MessageRecordTopicScrollListBoxOnLoad((ITUWidget*)messageRecordTopicScrollListBox, NULL);
        MessageRecordTimeScrollListBoxOnLoad((ITUWidget*)messageRecordTimeScrollListBox, NULL);
        MessageRecordScrollIconListBoxOnLoad((ITUWidget*)messageRecordScrollIconListBox, NULL);
    }
	return true;
}

void MessageRecordReset(void)
{
    messageRecordTopicScrollListBox = NULL;
}
