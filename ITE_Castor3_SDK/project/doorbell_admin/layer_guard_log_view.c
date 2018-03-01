#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "doorbell.h"
#include "scene.h"

static ITUText* guardLogViewAddressText;
static ITUText* guardLogViewEventText;
static ITUText* guardLogViewStateText;
static ITUText* guardLogViewHandlerText;
static ITUText* guardLogViewTimeText;
static GuardLog* guardLogViewLog;
static int guardLogViewLogIndex;

void GuardLogViewShow(int index)
{
    if (!guardLogViewAddressText)
    {
        guardLogViewAddressText = ituSceneFindWidget(&theScene, "guardLogViewAddressText");
        assert(guardLogViewAddressText);

        guardLogViewEventText = ituSceneFindWidget(&theScene, "guardLogViewEventText");
        assert(guardLogViewEventText);

        guardLogViewStateText = ituSceneFindWidget(&theScene, "guardLogViewStateText");
        assert(guardLogViewStateText);

        guardLogViewHandlerText = ituSceneFindWidget(&theScene, "guardLogViewHandlerText");
        assert(guardLogViewHandlerText);

        guardLogViewTimeText = ituSceneFindWidget(&theScene, "guardLogViewTimeText");
        assert(guardLogViewTimeText);
	}

    guardLogViewLog = GuardGetLog(index);
    if (guardLogViewLog)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buf[80];

        ituTextSetString(guardLogViewAddressText, 
            StringGetRoomAddress(
                guardLogViewLog->area, 
                guardLogViewLog->building,
                guardLogViewLog->unit, 
                guardLogViewLog->floor, 
                guardLogViewLog->room,
                guardLogViewLog->ext));

        ituTextSetString(guardLogViewEventText, StringGetGuardType(guardLogViewLog->type));
        ituTextSetString(guardLogViewStateText, StringGetGuardState(guardLogViewLog->state));
        ituTextSetString(guardLogViewHandlerText, guardLogViewLog->handler);

        rawtime = guardLogViewLog->timestamp;
        timeinfo = localtime(&rawtime);
        strftime(buf, sizeof(buf),"%Y.%m.%d %H:%M:%S", timeinfo);

        ituTextSetString(guardLogViewTimeText, buf);

        guardLogViewLog->read = 1;
        guardLogViewLogIndex = index;
    }
}

bool GuardLogViewHandleButtonOnPress(ITUWidget* widget, char* param)
{
    if (guardLogViewLog)
    {
        GuardHandleLog(guardLogViewLogIndex, UserListCurrentUserID());
        GuardSendSyncLog(guardLogViewLog);
        ituTextSetString(guardLogViewStateText, StringGetGuardState(guardLogViewLog->state));
        ituTextSetString(guardLogViewHandlerText, guardLogViewLog->handler);
    }
	return true;
}

bool GuardLogViewOnEnter(ITUWidget* widget, char* param)
{
    if (!guardLogViewAddressText)
    {
        guardLogViewAddressText = ituSceneFindWidget(&theScene, "guardLogViewAddressText");
        assert(guardLogViewAddressText);

        guardLogViewEventText = ituSceneFindWidget(&theScene, "guardLogViewEventText");
        assert(guardLogViewEventText);

        guardLogViewStateText = ituSceneFindWidget(&theScene, "guardLogViewStateText");
        assert(guardLogViewStateText);

        guardLogViewHandlerText = ituSceneFindWidget(&theScene, "guardLogViewHandlerText");
        assert(guardLogViewHandlerText);
	}
	return true;
}

bool GuardLogViewOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void GuardLogViewReset(void)
{
    guardLogViewAddressText = NULL;
}
