#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "config.h"

static ITULayer*        cameraLayer;
static ITUBackground*   cameraScreenBackground;
static ITUWidget*       cameraViewBottomContainer;
static ITUButton*       cameraViewRecordButton;
static ITUButton*       cameraViewStopRecordButton;
static ITUText*         cameraViewSourceText;

bool CameraViewScreenButtonOnPress(ITUWidget* widget, char* param)
{
    if (!cameraLayer)
    {
        cameraLayer = ituSceneFindWidget(&theScene, "cameraLayer");
        assert(cameraLayer);
    }

    ituLayerGoto(cameraLayer);
	return true;
}

bool CameraViewRecordButtonOnPress(ITUWidget* widget, char* param)
{    
	return true;
}

bool CameraViewStopRecordButtonOnPress(ITUWidget* widget, char* param)
{    
	return true;
}

bool CameraViewUnlockButtonOnPress(ITUWidget* widget, char* param)
{   
    return true;
}

bool CameraViewSnapshotButtonOnPress(ITUWidget* widget, char* param)
{    
	return true;
}

bool CameraViewDisconnectButtonOnPress(ITUWidget* widget, char* param)
{   
	return true;
}

void CameraViewShow(char* camera, char* ip)
{
    if (!cameraScreenBackground)
    {
        cameraLayer = ituSceneFindWidget(&theScene, "cameraLayer");
        assert(cameraLayer);

        cameraScreenBackground = ituSceneFindWidget(&theScene, "cameraScreenBackground");
        assert(cameraScreenBackground);

        cameraViewBottomContainer = ituSceneFindWidget(&theScene, "cameraViewBottomContainer");
        assert(cameraViewBottomContainer);

        cameraViewRecordButton = ituSceneFindWidget(&theScene, "cameraViewRecordButton");
        assert(cameraViewRecordButton);

        cameraViewStopRecordButton = ituSceneFindWidget(&theScene, "cameraViewStopRecordButton");
        assert(cameraViewStopRecordButton);

        cameraViewSourceText = ituSceneFindWidget(&theScene, "cameraViewSourceText");
        assert(cameraViewSourceText);
    }
}

bool CameraViewOnEnter(ITUWidget* widget, char* param)
{
    if (strcmp(param, "cameraLayer"))
    {
    }
	return true;
}

bool CameraViewOnLeave(ITUWidget* widget, char* param)
{
	return true;
}

void CameraViewReset(void)
{
	cameraLayer             = NULL;
    cameraScreenBackground  = NULL;
}
