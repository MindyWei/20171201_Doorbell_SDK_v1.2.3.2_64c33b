#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

#define MAX_FILE_SIZE  6000000
static ITUIcon* messageServiceViewImageIcon;

static uint8_t* messageImageContent;
static int messageImageContentSize;
static bool PicPlayerLoaded;

static void MessageServiceViewClear(void)
{
    if (messageImageContentSize == 0)
        return;

    messageImageContentSize = 0; // to avoid reentrant

    free(messageImageContent);
    messageImageContent = NULL;
    ituIconReleaseSurface(messageServiceViewImageIcon);
}

static void MessageServiceGetContent(uint8_t* image, int imageSize)
{
    MessageServiceViewClear();
    messageImageContent = image;
    messageImageContentSize = imageSize;
    if(imageSize && imageSize < MAX_FILE_SIZE)
    {
        PicPlayerLoaded = true;
    }else
    {
        printf("JPG file can not bigger than 6M bytes!!\n");
        free(messageImageContent);
        messageImageContent = NULL;
        messageImageContentSize = 0;
    }
}

void MessageServiceViewShow(char* image)
{
    if (!messageServiceViewImageIcon)
    {
        messageServiceViewImageIcon = ituSceneFindWidget(&theScene, "messageServiceViewImageIcon");
        assert(messageServiceViewImageIcon);
    }

    messageImageContent = NULL;
    messageImageContentSize = 0;
	MessageTextContentGet(image, MessageServiceGetContent);
}

bool MessageServiceViewOnEnter(ITUWidget* widget, char* param)
{
    SceneLeaveVideoState();
    if (!messageServiceViewImageIcon)
    {
        messageServiceViewImageIcon = ituSceneFindWidget(&theScene, "messageServiceViewImageIcon");
        assert(messageServiceViewImageIcon);
    }

	return true;
}

bool MessageServiceViewOnTimer(ITUWidget* widget, char* param)
{
    if (PicPlayerLoaded)
    {
        //show the retrieve Jpeg image file.
        ituIconLoadJpegData(messageServiceViewImageIcon, messageImageContent, messageImageContentSize);
        free(messageImageContent);
        messageImageContentSize = 0;
        PicPlayerLoaded = false;
    }
}


bool MessageServiceViewOnLeave(ITUWidget* widget, char* param)
{
    MessageServiceViewClear();
    //SceneEnterVideoState(33);
	return true;
}

void MessageServiceViewReset(void)
{
    messageServiceViewImageIcon = NULL;
}
