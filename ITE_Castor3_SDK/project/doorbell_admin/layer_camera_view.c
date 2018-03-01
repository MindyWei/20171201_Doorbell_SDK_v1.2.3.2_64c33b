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
static ITUCheckBox*     cameraBindingCheckBox;
static ITUCheckBox*     cameraViewBindingCheckBox;
static ITUButton*       cameraViewConnectButton;
static ITUCheckBox*     cameraViewVolCheckBox;
static ITUBackground*   cameraVolBackground;	
static ITUBackground*   cameraViewVolBackground;	
static ITUCheckBox*     cameraViewVol0CheckBox;
static ITUCheckBox*     cameraViewVol1CheckBox;
static ITUCheckBox*     cameraViewVol2CheckBox;
static ITUCheckBox*     cameraViewVol3CheckBox;
static ITUCheckBox*     cameraViewVol4CheckBox;
static ITUCheckBox*     cameraViewVol5CheckBox;
static ITUCheckBox*     cameraViewVol6CheckBox;

static DeviceInfo       cameraDeviceInfo;
static bool             cameraViewIsLeaving, cameraViewIsSnapshoting;
static bool             cameraViewVideoRecordEnable;
static int              cameraViewMaxVol;

extern float        cameraCountDown;
extern float 		CameraLockRecordButtonCountDown;
extern uint32_t 	CameraLockRecordButtonLastTick;
extern bool 		CameraLockRecordButton;

extern float        IPCamDisplayCountDown;
extern bool         bWaitingIPCamDisplay;

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
    if (ituWidgetIsVisible(cameraViewBottomContainer))
    {
        ituWidgetSetVisible(cameraViewBottomContainer, false);
    }
    else
    {
        cameraViewIsLeaving = true;
        ituSceneExecuteCommand(&theScene, 5, CameraViewSetCameraBackgroundWindowID, 0);
        ituLayerGoto(cameraLayer);
    }
	return true;
}

bool CameraViewRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraViewVideoRecordEnable)
    {
		if(CameraLockRecordButton) return false;
        if(SceneCameraIsViewing() && cameraCountDown < 2.0f) return false;
        if (cameraDeviceInfo.type == DEVICE_IPCAM)
        {
            if (IPCamStartRecord(MEDIA_VIDEO, cameraDeviceInfo.ip))
                return false;    
        }
        else if (CaptureStartRecord(MEDIA_VIDEO, cameraDeviceInfo.ip))
            return false;

        CameraLockRecordButton = true;
        CameraLockRecordButtonCountDown = 1.0f;
        CameraLockRecordButtonLastTick = SDL_GetTicks();
    }
	return true;
}

bool CameraViewStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraViewVideoRecordEnable)
    {
		if(CameraLockRecordButton) return false;
        if (cameraDeviceInfo.type == DEVICE_IPCAM)
        {
            IPCamStopRecord();
        }
        else
        {
            CaptureStopRecord();
        }    
		CameraLockRecordButton = true;
        CameraLockRecordButtonCountDown = 1.0f;
        CameraLockRecordButtonLastTick = SDL_GetTicks();
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
    ituSceneExecuteCommand(&theScene, 1000/33, CameraViewEnableSnapshot, 0);
	return true;
}

bool CameraViewBindingCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(cameraViewBindingCheckBox))
    {
        char temp_url[64] = {0};

        sprintf(temp_url,"%s:%s@%s", cameraDeviceInfo.id, cameraDeviceInfo.pw, cameraDeviceInfo.ip);
        strcpy(theConfig.ipcam_ipaddr, temp_url);
        printf("Binding camera: %s\n", theConfig.ipcam_ipaddr);
        LinphoneBindIPCamEnable();
        ituCheckBoxSetChecked(cameraBindingCheckBox, true);
        //ipcamIsBinding = true;
    }
    else
    {
        printf("Disable bind camera\n");
        theConfig.ipcam_ipaddr[0] = '\0';
        LinphoneBindIPCamDisable();
        ituCheckBoxSetChecked(cameraBindingCheckBox, false);
        //ipcamIsBinding = false;
    }
	return true;
}

bool CameraViewConnectButtonOnPress(ITUWidget* widget, char* param)
{
    LinphoneUnMuteMic();
    return true;
}

bool CameraViewVolCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(cameraViewVolBackground))
    {
        ituWidgetSetVisible(cameraViewVolBackground, false);
    }
    else
    {
        ituWidgetSetVisible(cameraViewVolBackground, true);
    }
	return true;
}

bool CameraViewVolNumberCheckBoxOnPress(ITUWidget* widget, char* param)
{
    ITUCheckBox* checkbox = (ITUCheckBox*) widget;
    int level, vol;

    vol = atoi(ituTextGetString(&checkbox->btn.text));
    level = vol * 100 / cameraViewMaxVol;

    if (level > 100)
        level = 100;
    else if (level < 0)
        level = 0;

    LinphoneSetVoiceLevel(level);

    ituCheckBoxSetChecked(cameraViewVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(cameraViewVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(cameraViewVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(cameraViewVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(cameraViewVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(cameraViewVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(cameraViewVol6CheckBox, vol >= 6);

	return true;
}

void CameraViewShow(char* camera, char* ip)
{
    if (!cameraLayer)
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

        cameraBindingCheckBox = ituSceneFindWidget(&theScene, "cameraBindingCheckBox");
        assert(cameraBindingCheckBox);

        cameraViewBindingCheckBox = ituSceneFindWidget(&theScene, "cameraViewBindingCheckBox");
        assert(cameraViewBindingCheckBox);

        cameraViewConnectButton = ituSceneFindWidget(&theScene, "cameraViewConnectButton");
        assert(cameraViewConnectButton);

        cameraViewVolCheckBox = ituSceneFindWidget(&theScene, "cameraViewVolCheckBox");
        assert(cameraViewVolCheckBox);

        cameraVolBackground = ituSceneFindWidget(&theScene, "cameraVolBackground");
        assert(cameraVolBackground);

        cameraViewVolBackground = ituSceneFindWidget(&theScene, "cameraViewVolBackground");
        assert(cameraViewVolBackground);

        cameraViewVol0CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol0CheckBox");
        assert(cameraViewVol0CheckBox);

        cameraViewVol1CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol1CheckBox");
        assert(cameraViewVol1CheckBox);

        cameraViewVol2CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol2CheckBox");
        assert(cameraViewVol2CheckBox);

        cameraViewVol3CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol3CheckBox");
        assert(cameraViewVol3CheckBox);

        cameraViewVol4CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol4CheckBox");
        assert(cameraViewVol4CheckBox);

        cameraViewVol5CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol5CheckBox");
        assert(cameraViewVol5CheckBox);

        cameraViewVol6CheckBox = ituSceneFindWidget(&theScene, "cameraViewVol6CheckBox");
        assert(cameraViewVol6CheckBox);

        // status
        cameraViewMaxVol = 6;
    }
    AddressBookGetDeviceInfo(&cameraDeviceInfo, ip);
    cameraViewIsLeaving = false;
    cameraViewIsSnapshoting = false;

    if (CaptureGetPath())
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

    ituWidgetSetVisible(cameraViewVolBackground, ituWidgetIsVisible(cameraVolBackground));
}

bool CameraViewOnEnter(ITUWidget* widget, char* param)
{
    int vol = LinphoneGetVoiceLevel() * cameraViewMaxVol / 100;

    ituCheckBoxSetChecked(cameraViewVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(cameraViewVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(cameraViewVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(cameraViewVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(cameraViewVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(cameraViewVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(cameraViewVol6CheckBox, vol >= 6);

    if (cameraDeviceInfo.type == DEVICE_IPCAM)
    {
        ituWidgetSetVisible(cameraViewConnectButton, false);
        ituWidgetSetVisible(cameraViewVolCheckBox, false);
    }
    else
    {
        ituWidgetSetVisible(cameraViewConnectButton, true);
        ituWidgetSetVisible(cameraViewVolCheckBox, true);
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
        ConfigSave();
#if (CFG_CHIP_FAMILY == 9850)
        if(strcmp(param, "calledLayer") && strcmp(param, "cameraLobbyLayer"))
            SceneLeaveVideoState();
#endif        
    }
    bWaitingIPCamDisplay = false;
    IPCamDisplayCountDown = 0;
	return true;
}

void CameraViewReset(void)
{
	cameraLayer         = NULL;
    cameraViewIsLeaving = false;
}
