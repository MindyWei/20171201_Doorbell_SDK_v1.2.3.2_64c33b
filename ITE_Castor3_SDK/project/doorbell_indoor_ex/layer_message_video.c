#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "scene.h"

static ITUScrollListBox* messageVideoTopicScrollListBox;
static ITUScrollListBox* messageVideoTimeScrollListBox;

bool MessageVideoTopicScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    return true;
}

bool MessageVideoTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
	return true;
}

bool MessageVideoScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
	return false;
}

bool MessageVideoDeleteButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool MessageVideoOnEnter(ITUWidget* widget, char* param)
{
    if (!messageVideoTopicScrollListBox)
    {
        messageVideoTopicScrollListBox = ituSceneFindWidget(&theScene, "messageVideoTopicScrollListBox");
        assert(messageVideoTopicScrollListBox);

        messageVideoTimeScrollListBox = ituSceneFindWidget(&theScene, "messageVideoTimeScrollListBox");
        assert(messageVideoTimeScrollListBox);
	}
   
	return true;
}

bool MessageVideoOnLeave(ITUWidget* widget, char* param)
{    
	return true;
}

void MessageVideoReset(void)
{
    messageVideoTopicScrollListBox = NULL;
}
