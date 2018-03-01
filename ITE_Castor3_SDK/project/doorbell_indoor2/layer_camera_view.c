#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scene.h"
#include "doorbell.h"

static ITULayer*        cameraLayer;
static ITUBackground*   cameraScreenBackground;
static ITUWidget*       cameraViewBottomContainer;
static ITUButton*       cameraViewRecordButton;
static ITUButton*       cameraViewStopRecordButton;
static ITUText*         cameraViewSourceText;

static DeviceInfo       cameraDeviceInfo;
static bool             cameraViewIsLeaving, cameraViewIsSnapshoting;
static bool             cameraViewVideoRecordEnable;

extern float            cameraCountDown;
extern bool             LockRecordButton;
extern float            LockRecordButtonCountDown;
extern uint32_t         LockRecordButtonLastTick;

extern float            IPCamDisplayCountDown;
extern bool             bWaitingIPCamDisplay;

static void CameraViewSetCameraBackgroundWindowID(int arg)
{
    if (cameraViewIsLeaving)
        LinphoneSetWindowID(cameraScreenBackground);
}

static void CameraViewEnableSnapshot(int arg)
{
    cameraViewIsSnapshoting = false;
}

bool CameraViewScreenButtonOnPress(ITUWidget* widget, char* param)
{
    if (!cameraLayer)
    {
        cameraLayer = ituSceneFindWidget(&theScene, "cameraLayer");
        assert(cameraLayer);
    }
    cameraViewIsLeaving = true;
    ituSceneExecuteCommand(&theScene, 5, CameraViewSetCameraBackgroundWindowID, 0);
    ituLayerGoto(cameraLayer);
	return true;
}

bool CameraViewRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraViewVideoRecordEnable)
    {
        if(LockRecordButton) return false;
        if(SceneCameraIsViewing() && cameraCountDown < 2.0f) return false;
        if (cameraDeviceInfo.type == DEVICE_IPCAM)
        {
            if (IPCamStartRecord(MEDIA_VIDEO, cameraDeviceInfo.ip))
                return false;    
        }
        else if (CaptureStartRecord(MEDIA_VIDEO, cameraDeviceInfo.ip))
            return false;

        LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraViewStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraViewVideoRecordEnable)
    {
        if(LockRecordButton) return false;
        if (cameraDeviceInfo.type == DEVICE_IPCAM)
        {
            IPCamStopRecord();
        }
        else
        {
            CaptureStopRecord();
        }

        LockRecordButton = true;
        LockRecordButtonCountDown = 1.0f;
        LockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraViewUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    SceneOpenDoor(cameraDeviceInfo.ip);
    return true;
}

bool CameraViewSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraViewIsSnapshoting)
        return false;

    if (cameraDeviceInfo.type == DEVICE_IPCAM)
        IPCamSnapshot(cameraDeviceInfo.ip, 0);
    else    
        CaptureSnapshot(cameraDeviceInfo.ip, 0);
    cameraViewIsSnapshoting = true;
    ituSceneExecuteCommand(&theScene, 1000/MS_PER_FRAME, CameraViewEnableSnapshot, 0);
	return true;
}

bool CameraViewDisconnectButtonOnPress(ITUWidget* widget, char* param)
{
    if(cameraDeviceInfo.type == DEVICE_IPCAM)
    {
        IPCamStopRecord();
#ifdef CFG_RTSP_CLIENT_ENABLE            
        stopRTSPClient();
#endif
        IPCamStopStreaming();
        SceneCameraSetViewing(false);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
    }
    else if (SceneCameraIsViewing())
    {
        if (cameraViewVideoRecordEnable)            
        {
            CaptureStopRecord();
        }
        LinphoneStopCamera();
        SceneCameraSetViewing(false);
        // UI receive this event to invisible cameraViewLayer
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
    }    
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
    AddressBookGetDeviceInfo(&cameraDeviceInfo, ip);
    cameraViewIsLeaving = false;
    cameraViewIsSnapshoting = false;

    if (cameraDeviceInfo.alias[0] == '\0')
    {
        char* desc = StringGetCameraPosition(cameraDeviceInfo.area, cameraDeviceInfo.building, cameraDeviceInfo.unit);
        ituTextSetString(cameraViewSourceText, desc);
        free(desc);
    }
    else
    {
        ituTextSetString(cameraViewSourceText, cameraDeviceInfo.alias);
    }

    if (theDeviceInfo.type == DEVICE_INDOOR2 && CaptureGetPath())
        cameraViewVideoRecordEnable = true;
    else
        cameraViewVideoRecordEnable = false;

    if (cameraViewVideoRecordEnable)        
    {
        if (CaptureIsRecording() || IPCamIsRecording())
        {
            ituWidgetSetVisible(cameraViewRecordButton, false);
            ituWidgetSetVisible(cameraViewStopRecordButton, true);
        }
        else
        {
            ituWidgetSetVisible(cameraViewRecordButton, true);
            ituWidgetSetVisible(cameraViewStopRecordButton, false);
        }
    }
    else
    {
        ituWidgetSetVisible(cameraViewRecordButton, false);
        ituWidgetSetVisible(cameraViewStopRecordButton, false);
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
    if (strcmp(param, "cameraLayer"))
    {
        if(cameraDeviceInfo.type == DEVICE_IPCAM)
        {
            IPCamStopRecord();
#ifdef CFG_RTSP_CLIENT_ENABLE        
            stopRTSPClient();
#endif
            IPCamStopStreaming();
            SceneCameraSetViewing(false);
        }
        else
        {
            if (cameraViewVideoRecordEnable)        
                CaptureStopRecord();

            LinphoneStopCamera();
            SceneCameraSetViewing(false);
        }
        
        if(strcmp(param, "calledLayer") && strcmp(param, "videoRecordLayer"))
            SceneLeaveVideoState();
    }
    bWaitingIPCamDisplay = false;
    IPCamDisplayCountDown = 0;
	return true;
}

void CameraViewReset(void)
{
	cameraLayer             = NULL;
    cameraScreenBackground  = NULL;
    cameraViewIsLeaving     = false;
}
