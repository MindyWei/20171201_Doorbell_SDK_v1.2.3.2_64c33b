#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"


static ITULayer* calledLayer;
static ITULayer* videoRecordLayer;
static ITUSprite* guardIndicatorSprite;
static ITUSprite* networkIndicatorSprite;
static ITUSprite* networkWiFiIndicatorSprite;
static ITUSprite* phoneIndicatorSprite;
static ITUBackground* missedCallsCountBackground;
static ITUText* missedCallsCountText;

static ITUSprite* messageIndicatorSprite;
static ITUBackground* messagesCountBackground;
static ITUText* messagesCountText;


bool MainSetDoNotDisturbModeOnCustom(ITUWidget* widget, char* param)
{
    DoNotDisturbMode mode = atoi(param);
    if (!phoneIndicatorSprite)
    {
        phoneIndicatorSprite = ituSceneFindWidget(&theScene, "phoneIndicatorSprite");
        assert(phoneIndicatorSprite);

        missedCallsCountBackground = ituSceneFindWidget(&theScene, "missedCallsCountBackground");
        assert(missedCallsCountBackground);
    }

    switch (mode)
    {
    case DONOT_DISTURB_NONE:
        {
            if (ituWidgetIsVisible(missedCallsCountBackground))
                ituSpriteGoto(phoneIndicatorSprite, 1);
            else
                ituSpriteGoto(phoneIndicatorSprite, 0);
        }
        break;

    case DONOT_DISTURB_MUTE:
        ituSpriteGoto(phoneIndicatorSprite, 2);
        break;

    case DONOT_DISTURB_ALL:
    case DONOT_DISTURB_TIME:
        ituSpriteGoto(phoneIndicatorSprite, 3);
        break;
    }
	return true;
}

bool MainNotifyMissedCallsOnCustom(ITUWidget* widget, char* param)
{
    char buf[8];
    int count = atoi(param);

	printf("MissCalls = %d\n", count);
    if (!phoneIndicatorSprite)
    {
        phoneIndicatorSprite = ituSceneFindWidget(&theScene, "phoneIndicatorSprite");
        assert(phoneIndicatorSprite);

        missedCallsCountBackground = ituSceneFindWidget(&theScene, "missedCallsCountBackground");
        assert(missedCallsCountBackground);
    }

    if (!missedCallsCountText)
    {
        missedCallsCountText = ituSceneFindWidget(&theScene, "missedCallsCountText");
        assert(missedCallsCountText);
    }

    if (phoneIndicatorSprite->frame <= 1)
    {
        if (count == 0)
        {
            ituSpriteGoto(phoneIndicatorSprite, 0);
            ituWidgetSetVisible(missedCallsCountBackground, false);
        }
        else
        {
            ituSpriteGoto(phoneIndicatorSprite, 1);
            ituWidgetSetVisible(missedCallsCountBackground, true);
        }
    }
    else
    {
        if (count == 0)
        {
            ituWidgetSetVisible(missedCallsCountBackground, false);
        }
        else
        {
            ituWidgetSetVisible(missedCallsCountBackground, true);
        }
    }
    sprintf(buf, "%d", count);
    ituTextSetString(missedCallsCountText, buf);
	return true;
}

bool MainNotifyMessagesOnCustom(ITUWidget* widget, char* param)
{
    char buf[8];
    int count = atoi(param);

	printf("Messages = %d\n", count);
    if (!messageIndicatorSprite)
    {
        messageIndicatorSprite = ituSceneFindWidget(&theScene, "messageIndicatorSprite");
        assert(messageIndicatorSprite);

        messagesCountBackground = ituSceneFindWidget(&theScene, "messagesCountBackground");
        assert(messagesCountBackground);

        messagesCountText = ituSceneFindWidget(&theScene, "messagesCountText");
        assert(messagesCountText);
    }

	if (count == 0)
    {
        ituSpriteGoto(messageIndicatorSprite, 0);
        ituWidgetSetVisible(messagesCountBackground, false);
	} else
    {
        ituSpriteGoto(messageIndicatorSprite, 1);
        ituWidgetSetVisible(messagesCountBackground, true);
	}
    sprintf(buf, "%d", count);
    ituTextSetString(messagesCountText, buf);
	return true;
}

bool MainNotifyGuardOnCustom(ITUWidget* widget, char* param)
{
    int on = atoi(param);

    if (!guardIndicatorSprite)
    {
        guardIndicatorSprite = ituSceneFindWidget(&theScene, "guardIndicatorSprite");
        assert(guardIndicatorSprite);
    }

	if (on)
        ituSpriteGoto(guardIndicatorSprite, 1);
	else
        ituSpriteGoto(guardIndicatorSprite, 0);

	return true;
}

bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param)
{
    int on = atoi(param);

    if (!networkIndicatorSprite)
    {
        networkIndicatorSprite = ituSceneFindWidget(&theScene, "networkIndicatorSprite");
        assert(networkIndicatorSprite);

        networkWiFiIndicatorSprite = ituSceneFindWidget(&theScene, "networkWiFiIndicatorSprite");
        assert(networkWiFiIndicatorSprite);

    #ifdef CFG_NET_WIFI
        ituWidgetSetVisible(networkIndicatorSprite, false);
        ituWidgetSetVisible(networkWiFiIndicatorSprite, true);
    #else
        ituWidgetSetVisible(networkIndicatorSprite, true);
        ituWidgetSetVisible(networkWiFiIndicatorSprite, false);
    #endif // CFG_NET_WIFI
    }

    #ifdef CFG_NET_WIFI
	    if (on)
            ituSpriteGoto(networkWiFiIndicatorSprite, 1);
	    else
            ituSpriteGoto(networkWiFiIndicatorSprite, 0);
    #else
	    if (on)
            ituSpriteGoto(networkIndicatorSprite, 1);
	    else
            ituSpriteGoto(networkIndicatorSprite, 0);
    #endif // CFG_NET_WIFI

    return true;
}

bool MainNotifyCallIncomingOnCustom(ITUWidget* widget, char* param)
{
	char addr[16];
	int id=0, video=0;

    if (!calledLayer)
    {
        calledLayer = ituSceneFindWidget(&theScene, "calledLayer");
        assert(calledLayer);
    }
	sscanf(param, "%s %d %d", addr, &id, &video);
	CalledIncomingShow(id, addr, video);
	ituLayerGoto(calledLayer);
	return true;

}

bool MainNotifyVoiceMemoIncomingOnCustom(ITUWidget* widget, char* param)
{
    char addr[16];
	int id=0, video=0;

    if (!videoRecordLayer)
    {
        videoRecordLayer = ituSceneFindWidget(&theScene, "videoRecordLayer");
        assert(videoRecordLayer);
    }
	sscanf(param, "%s %d %d", addr, &id, &video);

    if (theDeviceInfo.type == DEVICE_INDOOR2)
        VideoRecordIncomingShow(id, addr, video);

	ituLayerGoto(videoRecordLayer);

	return true;
}

void MainReset(void)
{
    calledLayer             = NULL;
    videoRecordLayer        = NULL;
    phoneIndicatorSprite    = NULL;
    messageIndicatorSprite  = NULL;
    guardIndicatorSprite    = NULL;
    networkIndicatorSprite  = NULL;
    missedCallsCountText    = NULL;
}

