#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"

static ITUBackground* voiceMemoBackground;
static ITUSprite* voiceMemoSprite;
static ITUText* voiceMemoTimeText;
static ITUScrollIconListBox* voiceMemoScrollIconListBox;
static ITUScrollListBox* voiceMemoNameScrollListBox;
static ITUScrollListBox* voiceMemoTimeScrollListBox;
static ITUBackground* voiceMemoDialogBackground;

static int voiceMemoPlayingPage, voiceMemoPlayingIndex, voiceMemoDeleteIndex;
static bool voiceMemoPlaying, voiceMemoRecording;
static float voiceMemoPlayingCountDown, voiceMemoPlayingCount, voiceMemoRecordingCount;
static uint32_t voiceMemoPlayingLastTick, voiceMemoRecordingLastTick;
static ITUScrollText* voiceMemoPlayingIconScrollText;

bool VoiceMemoScrollIconListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollIconListBox* silistbox = (ITUScrollIconListBox*)widget;
    int i, j, count, entryCount;
    VoiceMemoEntry* entry;

    entryCount = VoiceMemoGetCount();
    count = ituScrollIconListBoxGetItemCount(silistbox);
    node = ituScrollIconListBoxGetLastPageItem(silistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;

            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        entry = VoiceMemoGetEntry(j);

        if (listbox->pageIndex == voiceMemoPlayingPage && j == voiceMemoPlayingIndex)
            ituTextSetString(scrolltext, "1");
        else
            ituTextSetString(scrolltext, "0");

        ituWidgetSetCustomData(scrolltext, j);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool VoiceMemoNameScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;
    VoiceMemoEntry* entry;

    entryCount = VoiceMemoGetCount();
    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        entry = VoiceMemoGetEntry(j);
        ituTextSetString(scrolltext, entry->name);
        ituWidgetSetCustomData(scrolltext, j);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);
        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

bool VoiceMemoTimeScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count, entryCount;
    VoiceMemoEntry* entry;

    entryCount = VoiceMemoGetCount();
    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;

    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
		listbox->focusIndex = -1;
    }

    if (listbox->pageIndex <= 1)
    {
        for (i = 0; i < count; i++)
        {
            ITUScrollText* scrolltext = (ITUScrollText*) node;
            ituScrollTextSetString(scrolltext, "");

            node = node->sibling;
        }
    }

    i = 0;
    j = count * (listbox->pageIndex - 2);
    if (j < 0)
        j = 0;

    for (; j < entryCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        struct tm* timeinfo;
        char buf[32];

        entry = VoiceMemoGetEntry(j);

        timeinfo = localtime((const time_t*)&entry->time);
        if (timeinfo)
            strftime(buf, sizeof(buf),"%H:%M", timeinfo);
        else
            buf[0] = '\0';

        ituTextSetString(scrolltext, buf);
        ituWidgetSetCustomData(scrolltext, j);

        ituListBoxSetItemRead(listbox, (ITUWidget*)scrolltext, entry->played);

        i++;

        node = node->sibling;

        if (node == NULL)
            break;
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituScrollTextSetString(scrolltext, "");
    }

    if (listbox->pageIndex == listbox->pageCount)
    {
        if (i == 0)
        {
            listbox->itemCount = i;
        }
        else
        {
            listbox->itemCount = i % count;
            if (listbox->itemCount == 0)
                listbox->itemCount = count;
        }
    }
    else
        listbox->itemCount = count;

    return true;
}

static void VoiceMemoResetPlayButton(int arg)
{
    voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
    VoiceMemoStopPlay();
    SceneContinuePlayGuardAlarm();
    voiceMemoPlaying = false;
    SceneCameraSetViewing(false);
    ituTextSetString(voiceMemoTimeText, "00:00");
}

bool VoiceMemoScrollListBoxOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollText* item;

    if (voiceMemoRecording)
        return false;

    if (voiceMemoPlaying)
    {
        VoiceMemoStopPlay();
        voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
        voiceMemoPlaying = false;
        ituTextSetString(voiceMemoTimeText, "00:00");
        voiceMemoPlayingCountDown = 0.0f;
        SceneCameraSetViewing(false);
        ituTextSetString(voiceMemoPlayingIconScrollText, "0");

        item = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)voiceMemoScrollIconListBox);
        if (item && item == voiceMemoPlayingIconScrollText)
        {
            SceneContinuePlayGuardAlarm();
            return false;
        }
        voiceMemoPlayingIconScrollText = NULL;
    }

    item = (ITUScrollText*) ituListBoxGetFocusItem(listbox);
    if (item)
    {
        int i = (int)ituWidgetGetCustomData(item);
        SceneCameraSetViewing(true);
		if (VoiceMemoStartPlay(i) == 0)
        {
            VoiceMemoEntry* entry = VoiceMemoGetEntry(i);

            if (entry->sec > 0)
            {
                voiceMemoPlayingCountDown = (float)entry->sec + 0.5f;
                voiceMemoPlayingCount = 0.0f;
                voiceMemoPlayingLastTick = SDL_GetTicks();
                voiceMemoPlayingPage = listbox->pageIndex;
                voiceMemoPlayingIndex = i;
                voiceMemoPlaying = true;
                
                voiceMemoPlayingIconScrollText = (ITUScrollText*) ituListBoxGetFocusItem((ITUListBox*)voiceMemoScrollIconListBox);
                if (voiceMemoPlayingIconScrollText)
                    ituTextSetString(voiceMemoPlayingIconScrollText, "1");
            }
            else
                ituSceneExecuteCommand(&theScene, 1, VoiceMemoResetPlayButton, 0);

            ituListBoxSetItemRead((ITUListBox*)voiceMemoNameScrollListBox, (ITUWidget*)ituListBoxGetFocusItem((ITUListBox*)voiceMemoNameScrollListBox), true);
            ituListBoxSetItemRead((ITUListBox*)voiceMemoTimeScrollListBox, (ITUWidget*)ituListBoxGetFocusItem((ITUListBox*)voiceMemoTimeScrollListBox), true);
		}
    }
    return true;
}

bool VoiceMemoScrollListBoxOnMouseLongPress(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    int index = atoi(param);
    
    if (widget->type == ITU_SCROLLICONLISTBOX)
        voiceMemoDeleteIndex = (listbox->pageIndex - 1) * ituScrollIconListBoxGetItemCount((ITUScrollIconListBox*)listbox) + index;
    else
        voiceMemoDeleteIndex = (listbox->pageIndex - 1) * ituScrollListBoxGetItemCount((ITUScrollListBox*)listbox) + index;

    ituWidgetDisable(voiceMemoBackground);
    ituWidgetSetVisible(voiceMemoDialogBackground, true);

    //printf("voiceMemoDeleteIndex=%d\n", voiceMemoDeleteIndex);
    return true;
}

static void VoiceMemoReload(int arg)
{
    ituListBoxReload((ITUListBox*)voiceMemoScrollIconListBox);
    ituListBoxReload((ITUListBox*)voiceMemoNameScrollListBox);
    ituListBoxReload((ITUListBox*)voiceMemoTimeScrollListBox);
}

bool VoiceMemoRecCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (voiceMemoPlaying)
    {
        VoiceMemoStopPlay();
        voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
        voiceMemoPlaying = false;
        ituTextSetString(voiceMemoTimeText, "00:00");
        voiceMemoPlayingCountDown = 0.0f;

        if (voiceMemoPlayingIconScrollText)
        {
            ituTextSetString(voiceMemoPlayingIconScrollText, "0");
            voiceMemoPlayingIconScrollText = NULL;
        }
    }

    if (voiceMemoRecording)
    {
        VoiceMemoStopRecord();
        SceneContinuePlayGuardAlarm();
        if (theDeviceInfo.type == DEVICE_INDOOR2 && LinphoneIsInDoNotDisturb())
            LinphoneDisableBusy();

        ituSceneExecuteCommand(&theScene, 6, VoiceMemoReload, 0);
        ituSpriteGoto(voiceMemoSprite, 0);
        voiceMemoRecording = false;
        ituTextSetString(voiceMemoTimeText, "00:00");
        SceneCameraSetViewing(false);
    }
    else
    {
        SceneCameraSetViewing(true);
        if (VoiceMemoStartRecord())
            return false;

        if (theDeviceInfo.type == DEVICE_INDOOR2 && LinphoneIsInDoNotDisturb())
            LinphoneEnableBusy();

        ituSpriteGoto(voiceMemoSprite, 1);
        voiceMemoRecordingLastTick = SDL_GetTicks();
        voiceMemoRecordingCount = 0.0f;
        voiceMemoRecording = true;
    }
    return true;
}

bool VoiceMemoStopButtonOnPress(ITUWidget* widget, char* param)
{
    if (voiceMemoRecording)
    {
        VoiceMemoStopRecord();
        SceneContinuePlayGuardAlarm();
        if (theDeviceInfo.type == DEVICE_INDOOR2 && LinphoneIsInDoNotDisturb())
            LinphoneDisableBusy();

        ituSceneExecuteCommand(&theScene, 6, VoiceMemoReload, 0);
        ituSpriteGoto(voiceMemoSprite, 0);
        voiceMemoRecording = false;
        ituTextSetString(voiceMemoTimeText, "00:00");
        SceneCameraSetViewing(false);
    }
    else if (voiceMemoPlaying)
    {
        VoiceMemoStopPlay();
        SceneContinuePlayGuardAlarm();
        voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
        voiceMemoPlaying = false;
        ituTextSetString(voiceMemoTimeText, "00:00");
        SceneCameraSetViewing(false);
        voiceMemoPlayingCountDown = 0.0f;

        if (voiceMemoPlayingIconScrollText)
        {
            ituTextSetString(voiceMemoPlayingIconScrollText, "0");
            voiceMemoPlayingIconScrollText = NULL;
        }
    }
    return true;
}

bool VoiceMemoDeleteButtonOnPress(ITUWidget* widget, char* param)
{
	if (voiceMemoDeleteIndex >= 0)
	{
        if (voiceMemoDeleteIndex == voiceMemoPlayingIndex)
        {
            VoiceMemoStopPlay();
            SceneContinuePlayGuardAlarm();
            voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
            voiceMemoPlaying = false;
            ituTextSetString(voiceMemoTimeText, "00:00");
            voiceMemoPlayingCountDown = 0.0f;

            if (voiceMemoPlayingIconScrollText)
            {
                ituTextSetString(voiceMemoPlayingIconScrollText, "0");
                voiceMemoPlayingIconScrollText = NULL;
            }
        }

        VoiceMemoDeleteEntry(voiceMemoDeleteIndex);
        ituSceneExecuteCommand(&theScene, 6, VoiceMemoReload, 0);
        voiceMemoDeleteIndex = -1;
	}
    return true;
}

bool VoiceMemoOnTimer(ITUWidget* widget, char* param)
{
    if (voiceMemoPlaying || voiceMemoRecording)
    {
        ScreenSaverRefresh();

	    if (voiceMemoPlayingCountDown > 0.0f)
	    {
            uint32_t tick = SDL_GetTicks();
            uint32_t diff = tick - voiceMemoPlayingLastTick;
            if (diff >= 1000)
            {
                int h, m, s;
                char buf[32];

                voiceMemoPlayingCountDown -= (float)diff / 1000.0f;
                voiceMemoPlayingCount += (float)diff / 1000.0f;
                voiceMemoPlayingLastTick = tick;

                s = (int)voiceMemoPlayingCount;
                m = s / 60;
                h = m / 60;
                s -= m * 60;
                m -= h * 60;

                if (h > 0)
                    sprintf(buf, "%02d:%02d:%02d", h, m, s);
                else
                    sprintf(buf, "%02d:%02d", m, s);

                ituTextSetString(voiceMemoTimeText, buf);
            }
            if (voiceMemoPlayingCountDown <= 0.0f)
            {
                if (voiceMemoPlayingIconScrollText && ((ITUListBox*)voiceMemoScrollIconListBox)->pageIndex == voiceMemoPlayingPage)
                {
                    ituTextSetString(voiceMemoPlayingIconScrollText, "0");
                }
                voiceMemoPlayingPage = voiceMemoPlayingIndex = -1;
                voiceMemoPlayingIconScrollText = NULL;
                VoiceMemoResetPlayButton(0);
            }
	    }

        if (voiceMemoRecording)
        {
            uint32_t tick = SDL_GetTicks();
            uint32_t diff = tick - voiceMemoRecordingLastTick;
            if (diff >= 1000)
            {
                int h, m, s;
                char buf[32];

                voiceMemoRecordingCount += (float)diff / 1000.0f;
                voiceMemoRecordingLastTick = tick;

                s = (int)voiceMemoRecordingCount;
                m = s / 60;
                h = m / 60;
                s -= m * 60;
                m -= h * 60;

                if (h > 0)
                    sprintf(buf, "%02d:%02d:%02d", h, m, s);
                else
                    sprintf(buf, "%02d:%02d", m, s);

                ituTextSetString(voiceMemoTimeText, buf);
            }

            if (VoiceMemoCheckDiskSpace() == false)
            {
                VoiceMemoStopRecord();
                SceneContinuePlayGuardAlarm();
                if (theDeviceInfo.type == DEVICE_INDOOR2 && LinphoneIsInDoNotDisturb())
                    LinphoneDisableBusy();

                ituSpriteGoto(voiceMemoSprite, 0);
                ituSceneExecuteCommand(&theScene, 6, VoiceMemoReload, 0);
                voiceMemoRecording = false;

                ituTextSetString(voiceMemoTimeText, "00:00");
            }
        }
    }
    return true;
}

bool VoiceMemoOnEnter(ITUWidget* widget, char* param)
{
    if (!voiceMemoBackground)
    {
        voiceMemoBackground = ituSceneFindWidget(&theScene, "voiceMemoBackground");
        assert(voiceMemoBackground);

        voiceMemoSprite = ituSceneFindWidget(&theScene, "voiceMemoSprite");
        assert(voiceMemoSprite);

        voiceMemoTimeText = ituSceneFindWidget(&theScene, "voiceMemoTimeText");
        assert(voiceMemoTimeText);

        voiceMemoScrollIconListBox = ituSceneFindWidget(&theScene, "voiceMemoScrollIconListBox");
        assert(voiceMemoScrollIconListBox);

        voiceMemoNameScrollListBox = ituSceneFindWidget(&theScene, "voiceMemoNameScrollListBox");
        assert(voiceMemoNameScrollListBox);

        voiceMemoTimeScrollListBox = ituSceneFindWidget(&theScene, "voiceMemoTimeScrollListBox");
        assert(voiceMemoTimeScrollListBox);

        voiceMemoDialogBackground = ituSceneFindWidget(&theScene, "voiceMemoDialogBackground");
        assert(voiceMemoDialogBackground);
    }

    ituTextSetString(voiceMemoTimeText, "00:00");

    voiceMemoPlayingPage = voiceMemoPlayingIndex = voiceMemoDeleteIndex = -1;
    voiceMemoPlaying = voiceMemoRecording = false;
    voiceMemoPlayingCountDown = 0.0f;
    voiceMemoRecordingLastTick = 0;
    voiceMemoPlayingIconScrollText = NULL;

    return true;
}

static void VoiceMemoLeaveStopRecord(int arg)
{
    VoiceMemoStopRecord();
    SceneCameraSetViewing(false);
    voiceMemoRecording = false;
    SceneResetLongPressTime();
}

bool VoiceMemoOnLeave(ITUWidget* widget, char* param)
{
    if (voiceMemoRecording)
        ituSceneExecuteCommand(&theScene, 6, VoiceMemoLeaveStopRecord, 0);

    if (theDeviceInfo.type == DEVICE_INDOOR2 && LinphoneIsInDoNotDisturb())
        LinphoneDisableBusy();

	if (voiceMemoPlaying)
    	VoiceMemoStopPlay();
		
    SceneContinuePlayGuardAlarm();
    voiceMemoPlaying = false;

    return true;
}

void VoiceMemoReset(void)
{
    voiceMemoBackground = NULL;
}
