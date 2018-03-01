#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITUSprite* phoneIndicatorSprite;
static ITUSprite* networkIndicatorSprite;

static ITUBackground* missedCallsCountBackground;
static ITUText* missedCallsCountText;

static ITUBackground* guardMessagesCountBackground;
static ITUText* guardMessagesCountText;

bool MainSetDoNotDisturbModeOnCustom(ITUWidget* widget, char* param)
{
    DoNotDisturbMode mode = atoi(param);
    if (!phoneIndicatorSprite)
    {
        phoneIndicatorSprite = ituSceneFindWidget(&theScene, "phoneIndicatorSprite");
        assert(phoneIndicatorSprite);
    }

    switch (mode)
    {
    case DONOT_DISTURB_NONE:
        ituSpriteGoto(phoneIndicatorSprite, 0);
        break;

    case DONOT_DISTURB_MUTE:
        ituSpriteGoto(phoneIndicatorSprite, 1);
        break;

    case DONOT_DISTURB_ALL:
        ituSpriteGoto(phoneIndicatorSprite, 2);
        break;
    }
	return true;
}

bool MainNotifyMissedCallsOnCustom(ITUWidget* widget, char* param)
{
    char buf[8];
    int count = atoi(param);

	printf("MissCalls = %d\n", count);
    if (!missedCallsCountBackground)
    {
        missedCallsCountBackground = ituSceneFindWidget(&theScene, "missedCallsCountBackground");
        assert(missedCallsCountBackground);

        missedCallsCountText = ituSceneFindWidget(&theScene, "missedCallsCountText");
        assert(missedCallsCountText);
    }

    if (count == 0)
    {
        ituWidgetSetVisible(missedCallsCountBackground, false);
    }
    else
    {
        ituWidgetSetVisible(missedCallsCountBackground, true);
    }
    sprintf(buf, "%d", count);
    ituTextSetString(missedCallsCountText, buf);
	return true;
}

bool MainNotifyNetworkOnCustom(ITUWidget* widget, char* param)
{
    int on = atoi(param);

    if (!networkIndicatorSprite)
    {
        networkIndicatorSprite = ituSceneFindWidget(&theScene, "networkIndicatorSprite");
        assert(networkIndicatorSprite);
    }

	if (on)
        ituSpriteGoto(networkIndicatorSprite, 1);
	else
        ituSpriteGoto(networkIndicatorSprite, 0);

	return true;
}

bool MainNotifyGuardMessagesOnCustom(ITUWidget* widget, char* param)
{
    char buf[8];
    int count = atoi(param);

	printf("Guard messages = %d\n", count);
    if (!guardMessagesCountBackground)
    {
        guardMessagesCountBackground = ituSceneFindWidget(&theScene, "guardMessagesCountBackground");
        assert(guardMessagesCountBackground);

        guardMessagesCountText = ituSceneFindWidget(&theScene, "guardMessagesCountText");
        assert(guardMessagesCountText);
    }

	if (count == 0)
    {
        ituWidgetSetVisible(guardMessagesCountBackground, false);
	}
    else
    {
        ituWidgetSetVisible(guardMessagesCountBackground, true);
	}
    sprintf(buf, "%d", count);
    ituTextSetString(guardMessagesCountText, buf);
	return true;
}

bool MainMenuOnEnter(ITUWidget* widget, char* param)
{
    return true;
}

bool MainMenuExitPopupButtonOnPress(ITUWidget* widget, char* param)
{
    return true;
}

bool MainMenuDisturbPopupButtonOnPress(ITUWidget* widget, char* param)
{
    if (!phoneIndicatorSprite)
    {
        phoneIndicatorSprite = ituSceneFindWidget(&theScene, "phoneIndicatorSprite");
        assert(phoneIndicatorSprite);
    }

    switch (theConfig.do_not_disturb_mode)
    {
    case DONOT_DISTURB_NONE:
        {
            AudioMute();
            theConfig.do_not_disturb_mode = DONOT_DISTURB_MUTE;
            ituSpriteGoto(phoneIndicatorSprite, 1);
        }
        break;

    case DONOT_DISTURB_MUTE:
        {
            AudioUnMute();
            LinphoneEnableDoNotDisturb();
            theConfig.do_not_disturb_mode = DONOT_DISTURB_ALL;
            ituSpriteGoto(phoneIndicatorSprite, 2);
        }
        break;

    case DONOT_DISTURB_ALL:
        {
            LinphoneDisableDoNotDisturb();
            theConfig.do_not_disturb_mode = DONOT_DISTURB_NONE;
            ituSpriteGoto(phoneIndicatorSprite, 0);
        }
        break;
    }
    ConfigSave();
	return true;
}

void MainMenuReset(void)
{
    phoneIndicatorSprite            = NULL;
    networkIndicatorSprite          = NULL;
    missedCallsCountBackground      = NULL;
    guardMessagesCountBackground    = NULL;
}