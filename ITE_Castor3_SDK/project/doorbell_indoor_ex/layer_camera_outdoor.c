#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "config.h"

static ITUBackground*   cameraOutdoorBackground;
static ITUText*         cameraOutdoorSourceText;
static ITUText*         cameraOutdoorCountDownText;

bool CameraOutdoorButtonOnPress(ITUWidget* widget, char* param)
{
    if (!cameraOutdoorBackground)
    {
        cameraOutdoorBackground = ituSceneFindWidget(&theScene, "cameraOutdoorBackground");
        assert(cameraOutdoorBackground);
	}	
	return true;
}

bool CameraOutdoorRecordButtonOnPress(ITUWidget* widget, char* param)
{   
	return true;
}

bool CameraOutdoorStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool CameraOutdoorUnlockButtonOnPress(ITUWidget* widget, char* param)
{    
    return true;
}

bool CameraOutdoorSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
	return true;
}

bool CameraOutdoorOnTimer(ITUWidget* widget, char* param)
{
	return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void cameraOutdoorBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;
    
    ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}
#endif

bool CameraOutdoorOnEnter(ITUWidget* widget, char* param)
{   
    if (!cameraOutdoorBackground)
    {
        cameraOutdoorBackground = ituSceneFindWidget(&theScene, "cameraOutdoorBackground");
        assert(cameraOutdoorBackground);
        
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraOutdoorBackground, cameraOutdoorBackgroundDraw);
#endif

        cameraOutdoorSourceText = ituSceneFindWidget(&theScene, "cameraOutdoorSourceText");
        assert(cameraOutdoorSourceText);

        cameraOutdoorCountDownText = ituSceneFindWidget(&theScene, "cameraOutdoorCountDownText");
        assert(cameraOutdoorCountDownText);
    }   
    return true;
}

bool CameraOutdoorOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void CameraOutdoorReset(void)
{
    cameraOutdoorSourceText    = NULL;
	cameraOutdoorBackground    = NULL;
}
