#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

#define SNAPSHOT_PATH CFG_TEMP_DRIVE ":/capture"
static ITUText*         recordViewSourceText;
static ITUBackground*   recordViewBaseBackground;
static ITUBackground*   recordViewVolBackground;	
static ITUCheckBox*     recordViewVolCheckBox;
static ITUCheckBox*     recordViewVol0CheckBox;
static ITUCheckBox*     recordViewVol1CheckBox;
static ITUCheckBox*     recordViewVol2CheckBox;
static ITUCheckBox*     recordViewVol3CheckBox;
static ITUCheckBox*     recordViewVol4CheckBox;
static ITUCheckBox*     recordViewVol5CheckBox;
static ITUCheckBox*     recordViewVol6CheckBox;
static ITUBackground* recordViewImageBackground;
static ITUIcon* recordViewImageIcon;
static ITUBackground* recordViewVideoBackground;
static ITUBackground* recordViewAudioBackground;
static ITULayer* recordLayer;

// status
static int recordViewMaxVol;
static int recordViewVoiceLevelOld;
static int recordViewID;
static CaptureEntry* recordCaptureEntry;
static bool recordViewPlaying;

bool RecordViewVolCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(recordViewVolBackground))
    {
        ituWidgetSetVisible(recordViewVolBackground, false);
    }
    else
    {
        ituWidgetSetVisible(recordViewVolBackground, true);
    }
	return true;
}

bool RecordViewVolNumberCheckBoxOnPress(ITUWidget* widget, char* param)
{
    ITUCheckBox* checkbox = (ITUCheckBox*) widget;
    int level, vol;

    vol = atoi(ituTextGetString(&checkbox->btn.text));
    level = vol * 100 / recordViewMaxVol;

    if (level > 100)
        level = 100;
    else if (level < 0)
        level = 0;

    LinphoneSetVoiceLevel(level);

    ituCheckBoxSetChecked(recordViewVol0CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol1CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol2CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol3CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol4CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol5CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol6CheckBox, false);

    if (vol >= 0)
        ituCheckBoxSetChecked(recordViewVol0CheckBox, true);
    if (vol >= 1)
        ituCheckBoxSetChecked(recordViewVol1CheckBox, true);
    if (vol >= 2)
        ituCheckBoxSetChecked(recordViewVol2CheckBox, true);
    if (vol >= 3)
        ituCheckBoxSetChecked(recordViewVol3CheckBox, true);
    if (vol >= 4)
        ituCheckBoxSetChecked(recordViewVol4CheckBox, true);
    if (vol >= 5)
        ituCheckBoxSetChecked(recordViewVol5CheckBox, true);
    if (vol >= 6)
        ituCheckBoxSetChecked(recordViewVol6CheckBox, true);

	return true;
}

bool RecordViewOnTimer(ITUWidget* widget, char* param)
{
    if (!recordLayer)
    {
        recordLayer = ituSceneFindWidget(&theScene, "recordLayer");
        assert(recordLayer);
    }

    if (recordViewPlaying && CaptureIsPlayFinished())
    {
        ituLayerGoto(recordLayer);
        recordViewPlaying = false;
    }
    return true;
}

void RecordViewShow(char* topic, int id, CaptureEntry* entry)
{
    if (!recordViewSourceText)
    {
        recordViewSourceText = ituSceneFindWidget(&theScene, "recordViewSourceText");
        assert(recordViewSourceText);
    }
    ituTextSetString(recordViewSourceText, topic);
    recordViewID = id;
    recordCaptureEntry = entry;
}

static bool RecordViewBaseBackgroundUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    assert(widget);

    if (ev == ITU_EVENT_MOUSEDOWN)
    {
        int x = arg2 - widget->rect.x;
        int y = arg3 - widget->rect.y;

        if (ituWidgetIsInside(widget, x, y))
        {
            int xx, yy, w, h;

            x = arg2;
            y = arg3;

            if (ituWidgetIsVisible(recordViewVolBackground))
            {
                ituWidgetGetGlobalPosition(recordViewVolBackground, &xx, &yy);
                w = ituWidgetGetWidth(recordViewVolBackground);
                h = ituWidgetGetHeight(recordViewVolBackground);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituLayerGoto(recordLayer);
                    return widget->dirty;
                }
            }
            else if (ituWidgetIsVisible(recordViewVolCheckBox))
            {
                ituWidgetGetGlobalPosition(recordViewVolCheckBox, &xx, &yy);
                w = ituWidgetGetWidth(recordViewVolCheckBox);
                h = ituWidgetGetHeight(recordViewVolCheckBox);

                if (x < xx || x > (xx + w) || y < yy || y > (yy + h))
                {
                    ituLayerGoto(recordLayer);
                    return widget->dirty;
                }
            }
            else
            {
                ituLayerGoto(recordLayer);
                return widget->dirty;
            }
        }
    }
    result |= ituIconUpdate(widget, ev, arg1, arg2, arg3);

    return result;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void recordViewVideoBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool RecordViewOnEnter(ITUWidget* widget, char* param)
{
    int vol;

    if (!recordViewBaseBackground)
    {
        recordViewBaseBackground = ituSceneFindWidget(&theScene, "recordViewBaseBackground");
        assert(recordViewBaseBackground);
        ituWidgetSetUpdate(recordViewBaseBackground, RecordViewBaseBackgroundUpdate);

        recordViewSourceText = ituSceneFindWidget(&theScene, "recordViewSourceText");
        assert(recordViewSourceText);

        recordViewVolCheckBox = ituSceneFindWidget(&theScene, "recordViewVolCheckBox");
        assert(recordViewVolCheckBox);

        recordViewVolBackground = ituSceneFindWidget(&theScene, "recordViewVolBackground");
        assert(recordViewVolBackground);

        recordViewVol0CheckBox = ituSceneFindWidget(&theScene, "recordViewVol0CheckBox");
        assert(recordViewVol0CheckBox);

        recordViewVol1CheckBox = ituSceneFindWidget(&theScene, "recordViewVol1CheckBox");
        assert(recordViewVol1CheckBox);

        recordViewVol2CheckBox = ituSceneFindWidget(&theScene, "recordViewVol2CheckBox");
        assert(recordViewVol2CheckBox);

        recordViewVol3CheckBox = ituSceneFindWidget(&theScene, "recordViewVol3CheckBox");
        assert(recordViewVol3CheckBox);

        recordViewVol4CheckBox = ituSceneFindWidget(&theScene, "recordViewVol4CheckBox");
        assert(recordViewVol4CheckBox);

        recordViewVol5CheckBox = ituSceneFindWidget(&theScene, "recordViewVol5CheckBox");
        assert(recordViewVol5CheckBox);

        recordViewVol6CheckBox = ituSceneFindWidget(&theScene, "recordViewVol6CheckBox");
        assert(recordViewVol6CheckBox);

        recordViewImageBackground = ituSceneFindWidget(&theScene, "recordViewImageBackground");
        assert(recordViewImageBackground);

        recordViewImageIcon = ituSceneFindWidget(&theScene, "recordViewImageIcon");
        assert(recordViewImageIcon);

        recordViewVideoBackground = ituSceneFindWidget(&theScene, "recordViewVideoBackground");
        assert(recordViewVideoBackground);    
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(recordViewVideoBackground, recordViewVideoBackgroundDraw);
#endif

        recordViewAudioBackground = ituSceneFindWidget(&theScene, "recordViewAudioBackground");
        assert(recordViewAudioBackground);

        // status
        recordViewMaxVol = 6;
    }

    // volume
    ituCheckBoxSetChecked(recordViewVol0CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol1CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol2CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol3CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol4CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol5CheckBox, false);
    ituCheckBoxSetChecked(recordViewVol6CheckBox, false);

    vol = LinphoneGetVoiceLevel() * recordViewMaxVol / 100;
    if (vol >= 0)
        ituCheckBoxSetChecked(recordViewVol0CheckBox, true);
    if (vol >= 1)
        ituCheckBoxSetChecked(recordViewVol1CheckBox, true);
    if (vol >= 2)
        ituCheckBoxSetChecked(recordViewVol2CheckBox, true);
    if (vol >= 3)
        ituCheckBoxSetChecked(recordViewVol3CheckBox, true);
    if (vol >= 4)
        ituCheckBoxSetChecked(recordViewVol4CheckBox, true);
    if (vol >= 5)
        ituCheckBoxSetChecked(recordViewVol5CheckBox, true);
    if (vol >= 6)
        ituCheckBoxSetChecked(recordViewVol6CheckBox, true);

    recordViewVoiceLevelOld = LinphoneGetVoiceLevel();

    // video
    LinphoneSetWindowID(recordViewBaseBackground);

    if (recordCaptureEntry->type == MEDIA_IMAGE)
    {
        char buf[PATH_MAX];
        FILE* f;

        strcpy(buf, SNAPSHOT_PATH);
        strcat(buf, "/");
        strcat(buf, recordCaptureEntry->filename);

        // try to load jpeg file if exists
        printf("filename =%s\n",buf);
        f = fopen(buf, "rb");
        if (f)
        {
            uint8_t* data;
            int size;

            fseek(f, 0, SEEK_END);
            size = ftell(f);
            fseek(f, 0, SEEK_SET);

            data = malloc(size);
            if (data)
            {
#if (CFG_CHIP_FAMILY == 9070)
			    SceneLeaveVideoState();
#endif
                size = fread(data, 1, size, f);
                ituIconLoadJpegData(recordViewImageIcon, data, size);
                free(data);
            }
            fclose(f);
        }
        ituWidgetSetVisible(recordViewImageBackground, true);
        ituWidgetSetVisible(recordViewVideoBackground, false);
        ituWidgetSetVisible(recordViewAudioBackground, false);
        ituWidgetSetVisible(recordViewVolCheckBox, false);
        CaptureStartPlay(recordViewID);
    }
    else if (recordCaptureEntry->type == MEDIA_VIDEO)
    {
        ituWidgetSetVisible(recordViewImageBackground, false);
        ituWidgetSetVisible(recordViewVideoBackground, true);
        ituWidgetSetVisible(recordViewAudioBackground, false);
        ituWidgetSetVisible(recordViewVolCheckBox, true);
        CaptureStartPlay(recordViewID);
#if (CFG_CHIP_FAMILY == 9850)
        SceneEnterVideoState(33);
#endif        
        LinphoneSetWindowID(recordViewVideoBackground);
        recordViewPlaying = true;
    }
    else if (recordCaptureEntry->type == MEDIA_AUDIO)
    {
        ituWidgetSetVisible(recordViewImageBackground, false);
        ituWidgetSetVisible(recordViewVideoBackground, false);
        ituWidgetSetVisible(recordViewAudioBackground, true);
        ituWidgetSetVisible(recordViewVolCheckBox, true);
        CaptureStartPlay(recordViewID);
        recordViewPlaying = true;
    }
    SceneCameraSetViewing(true);
    return true;
}

bool RecordViewOnLeave(ITUWidget* widget, char* param)
{
    CaptureStopPlay();
    SceneCameraSetViewing(false);
#if (CFG_CHIP_FAMILY == 9850)
    if(strcmp(param, "calledLayer") && strcmp(param, "cameraLobbyLayer"))
	    SceneLeaveVideoState();
#else
    SceneEnterVideoState();
#endif
    recordViewPlaying = false;
    return true;
}

void RecordViewReset(void)
{
    recordViewBaseBackground = NULL;
    recordViewSourceText = NULL;
    recordLayer = NULL;
}
