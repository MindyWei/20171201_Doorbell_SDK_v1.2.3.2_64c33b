#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "scene.h"

static ITULayer* messageServiceViewLayer;
static ITUScrollListBox* messageServiceTopicScrollListBox;
static ITUScrollListBox* messageServiceTimeScrollListBox;

bool MessageServiceTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageServiceTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageServiceScrollListBoxOnSelection(ITUWidget* widget, char* param)
{    
	return false;
}

bool MessageServiceDeleteButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool MessageServiceOnEnter(ITUWidget* widget, char* param)
{
    if (!messageServiceTopicScrollListBox)
    {
        messageServiceTopicScrollListBox = ituSceneFindWidget(&theScene, "messageServiceTopicScrollListBox");
        assert(messageServiceTopicScrollListBox);

        messageServiceTimeScrollListBox = ituSceneFindWidget(&theScene, "messageServiceTimeScrollListBox");
        assert(messageServiceTimeScrollListBox);

        messageServiceViewLayer = ituSceneFindWidget(&theScene, "messageServiceViewLayer");
        assert(messageServiceViewLayer);
	}    
	return true;
}

bool MessageServiceOnLeave(ITUWidget* widget, char* param)
{    
	return true;
}

void MessageServiceReset(void)
{
    messageServiceTopicScrollListBox = NULL;
}
