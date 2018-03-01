#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"
#include "castor3player.h"

#define CAMERA_TIMEOUT      (30.999f)  // 30 secs

static ITUListBox* cameraListBox;
static ITUBackground* cameraScreenBackground;
static ITUText* cameraCountDownText;
static ITUBackground* cameraViewScreenBackground;
static ITUWidget* cameraBottomContainer;
static ITUButton* cameraRecordButton;
static ITUButton* cameraStopRecordButton;
static ITUCheckBox* cameraBindingCheckBox;
static ITUCheckBox* cameraViewBindingCheckBox;
static ITUButton* cameraConnectButton;
static ITUCheckBox* cameraVolCheckBox;
static ITUBackground*   cameraVolBackground;	
static ITUCheckBox*     cameraVol0CheckBox;
static ITUCheckBox*     cameraVol1CheckBox;
static ITUCheckBox*     cameraVol2CheckBox;
static ITUCheckBox*     cameraVol3CheckBox;
static ITUCheckBox*     cameraVol4CheckBox;
static ITUCheckBox*     cameraVol5CheckBox;
static ITUCheckBox*     cameraVol6CheckBox;

static int cameraInfoCount, cameraPageIndex, cameraShowingIndex;
static DeviceInfo cameraInfoArray[MAX_CAMERA_COUNT];
static DeviceInfo cameraDeviceInfo;
static uint32_t cameraLastTick;
static bool cameraVideoRecordEnable, cameraIsSnapshoting;
static char cameraOldIPCamIP[64];
static int cameraMaxVol;

float       cameraCountDown = 0;
float 		CameraLockRecordButtonCountDown = 0;
uint32_t 	CameraLockRecordButtonLastTick = 0;
bool 		CameraLockRecordButton = false;

float       IPCamDisplayCountDown = 0;
uint32_t    IPCamDisplayLastTick = 0;
bool        bWaitingIPCamDisplay = false;

extern bool isIPCAM_FirstVideoGetted; 
bool CameraListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUScrollText* scrolltext;
    int i, count, entryCount;
	ITUListBox* listbox = (ITUListBox*)widget;
    assert(listbox);

    entryCount = cameraInfoCount;
    count = itcTreeGetChildCount(listbox);
    listbox->pageCount = entryCount ? (entryCount + count - 1) / count : 1;
    if (listbox->pageIndex == 0)
    {
        // initialize
        listbox->pageIndex = 1;
        listbox->focusIndex = -1;
    }
    node = ((ITCTree*)listbox)->child;
    
    i = (listbox->pageIndex - 1) * count;
    count = 0;
    if (i < entryCount)
	{
        for (; i < entryCount; i++)
        {
            DeviceInfo* info;
            scrolltext = (ITUScrollText*) node;
    
            info = &cameraInfoArray[i];

            if (info->alias[0] == '\0')
            {
                char* desc = StringGetCameraPosition(info->area, info->building, info->unit);
                ituTextSetString(scrolltext, desc);
                free(desc);
            }
            else
            {
                ituTextSetString(scrolltext, info->alias);
            }
            ituWidgetSetCustomData(scrolltext, info);
            count++;
    
            node = node->sibling;
            if (node == NULL)
                break;
        }
    }

    for (; node; node = node->sibling)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;
        ituTextSetString(scrolltext, "");
    }

    listbox->itemCount = count;

	if (listbox->pageIndex == cameraPageIndex)
		ituListBoxSelect(listbox, cameraShowingIndex);

    return true;
}

static void CameraWatch(int arg)
{
    ITUWidget* item = ituListBoxGetFocusItem(cameraListBox);
    if (item)
    {
        char buf[4];
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(item);

        LinphoneWatchCamera(info->ip);
        SceneSetRemoteIP(info->ip);
        //LinphoneSetWindowID(cameraScreenBackground);

        cameraCountDown    = CAMERA_TIMEOUT;
        cameraLastTick     = SDL_GetTicks();

        sprintf(buf, "%d", (int)cameraCountDown);
        ituTextSetString(cameraCountDownText, buf);

        SceneCameraSetViewing(true);
    }
}

static void IPCamWatch(int arg)
{
    ITUWidget* item = ituListBoxGetFocusItem(cameraListBox);
    if (item)
    {
        char rtsp_url[64] = {0};
        char buf[4];
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(item);
        //itv_ff_enable(0, false);
        sprintf(rtsp_url,"%s:%s@%s", info->id, info->pw, info->ip);
        IPCamStartStreaming();
#ifdef CFG_RTSP_CLIENT_ENABLE
        SetRTSPClientMode(IPCAM_MODE);
        startRTSPClient(rtsp_url, 554, NULL, NULL);
#endif
        cameraCountDown    = CAMERA_TIMEOUT;
        cameraLastTick     = SDL_GetTicks();
        sprintf(buf, "%d", (int)cameraCountDown);
        ituTextSetString(cameraCountDownText, buf);

        SceneCameraSetViewing(true);
    }
}

bool CameraListOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUWidget* item = ituListBoxGetFocusItem(cameraListBox);

    if (item)
    {
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(item);
        if (info)
        {
            if(cameraDeviceInfo.type == DEVICE_IPCAM)
            {
                IPCamStopRecord();
                ituWidgetSetVisible(cameraRecordButton, true);
                ituWidgetSetVisible(cameraStopRecordButton, false);
#ifdef CFG_RTSP_CLIENT_ENABLE            
                stopRTSPClient();
#endif
                IPCamStopStreaming();
            }
            
            memcpy(&cameraDeviceInfo, info, sizeof(DeviceInfo));

            if (info->type == DEVICE_IPCAM)
            {
                if (SceneCameraIsViewing())
                {
                    if (cameraVideoRecordEnable)
                    {
                        CaptureStopRecord();
                        ituWidgetSetVisible(cameraRecordButton, true);
                        ituWidgetSetVisible(cameraStopRecordButton, false);
                    }
                    LinphoneStopCamera();
                    ituSceneExecuteCommand(&theScene, 6, IPCamWatch, 0);
                }
                else
                {
                    IPCamWatch(0);
                }                  
            }
            else
            {                        
                if (SceneCameraIsViewing())
                {
                    if (cameraVideoRecordEnable)
                    {
                        CaptureStopRecord();
                        ituWidgetSetVisible(cameraRecordButton, true);
                        ituWidgetSetVisible(cameraStopRecordButton, false);
                    }
                    LinphoneStopCamera();
                    ituSceneExecuteCommand(&theScene, 6, CameraWatch, 0);
                }
                else
                {
                    CameraWatch(0);
                }
            }

            if (info->type == DEVICE_IPCAM)
            {
                char temp_url[64] = {0};

                sprintf(temp_url,"%s:%s@%s", info->id, info->pw, info->ip);
                ituWidgetSetVisible(cameraBindingCheckBox, true);
                ituWidgetSetVisible(cameraViewBindingCheckBox, true);
                ituWidgetSetVisible(cameraConnectButton, false);
                ituWidgetSetVisible(cameraVolCheckBox, false);

                if (strcmp(theConfig.ipcam_ipaddr, temp_url) == 0)
                {
                    ituCheckBoxSetChecked(cameraBindingCheckBox, true);
                    ituCheckBoxSetChecked(cameraViewBindingCheckBox, true);
                }
                else
                {
                    ituCheckBoxSetChecked(cameraBindingCheckBox, false);
                    ituCheckBoxSetChecked(cameraViewBindingCheckBox, false);
                }
            }
            else
            {
                ituWidgetSetVisible(cameraBindingCheckBox, false);
                ituWidgetSetVisible(cameraViewBindingCheckBox, false);
                ituWidgetSetVisible(cameraConnectButton, true);
                ituWidgetSetVisible(cameraVolCheckBox, true);
            }
            ituWidgetSetVisible(cameraVolBackground, false);
            ituWidgetSetVisible(cameraBottomContainer, true);

			cameraPageIndex = cameraListBox->pageIndex;
			cameraShowingIndex = cameraListBox->focusIndex;
        }
    }
    return true;
}

bool CameraScreenButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraListBox->focusIndex >= 0 || SceneCameraIsViewing())
    {
		CameraViewShow(NULL, cameraDeviceInfo.ip);
        LinphoneSetWindowID(cameraViewScreenBackground);
        return true;
    }
    return false;
}

static void CameraEnableSnapshot(int arg)
{
    cameraIsSnapshoting = false;
}

bool CameraRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraVideoRecordEnable)
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

bool CameraStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraVideoRecordEnable)
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

bool CameraUnlockButtonOnPress(ITUWidget* widget, char* param)
{
    SceneOpenDoor(cameraDeviceInfo.ip);
    return true;
}

bool CameraSnapshotButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraIsSnapshoting)
        return false;

    if (cameraDeviceInfo.type == DEVICE_IPCAM)
        IPCamSnapshot(cameraDeviceInfo.ip, 0);
    else    
        CaptureSnapshot(cameraDeviceInfo.ip, 0);
    cameraIsSnapshoting = true;
    ituSceneExecuteCommand(&theScene, 1000/33, CameraEnableSnapshot, 0);
	return true;
}

bool CameraBindingCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituCheckBoxIsChecked(cameraBindingCheckBox))
    {
        char temp_url[64] = {0};

        sprintf(temp_url,"%s:%s@%s", cameraDeviceInfo.id, cameraDeviceInfo.pw, cameraDeviceInfo.ip);
        strcpy(theConfig.ipcam_ipaddr, temp_url);
        printf("Binding camera: %s\n", theConfig.ipcam_ipaddr);
        LinphoneBindIPCamEnable();
        ituCheckBoxSetChecked(cameraViewBindingCheckBox, true);
        //ipcamIsBinding = true;
    }
    else
    {
        printf("Disable bind camera\n");
        theConfig.ipcam_ipaddr[0] = '\0';
        LinphoneBindIPCamDisable();
        ituCheckBoxSetChecked(cameraViewBindingCheckBox, false);
        //ipcamIsBinding = false;
    }
	return true;
}

bool CameraConnectButtonOnPress(ITUWidget* widget, char* param)
{
    LinphoneUnMuteMic();
    return true;
}

bool CameraVolCheckBoxOnPress(ITUWidget* widget, char* param)
{
    if (ituWidgetIsVisible(cameraVolBackground))
    {
        ituWidgetSetVisible(cameraVolBackground, false);
    }
    else
    {
        ituWidgetSetVisible(cameraVolBackground, true);
    }
	return true;
}

bool CameraVolNumberCheckBoxOnPress(ITUWidget* widget, char* param)
{
    ITUCheckBox* checkbox = (ITUCheckBox*) widget;
    int level, vol;

    vol = atoi(ituTextGetString(&checkbox->btn.text));
    level = vol * 100 / cameraMaxVol;

    if (level > 100)
        level = 100;
    else if (level < 0)
        level = 0;

    LinphoneSetVoiceLevel(level);

    ituCheckBoxSetChecked(cameraVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(cameraVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(cameraVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(cameraVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(cameraVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(cameraVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(cameraVol6CheckBox, vol >= 6);

	return true;
}

bool CameraOnTimer(ITUWidget* widget, char* param)
{
    if (IPCamDisplayCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - IPCamDisplayLastTick;
        if (diff >= 1000)
        {
            IPCamDisplayCountDown -= (float)diff / 1000.0f;
            IPCamDisplayLastTick = tick;
        }
        if (IPCamDisplayCountDown <= 0.0f)
        {
            IPCamDisplayCountDown = 0;
        }
	}
    
    if (CameraLockRecordButtonCountDown > 0.0f && CameraLockRecordButton)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - CameraLockRecordButtonLastTick;
        if (diff >= 1000)
        {
            CameraLockRecordButtonCountDown -= (float)diff / 1000.0f;
            CameraLockRecordButtonLastTick = tick;
        }
        if (CameraLockRecordButtonCountDown <= 0.0f)
        {
            CameraLockRecordButton = false;
        }
	}
    if (cameraCountDown > 0.0f)
    {
        uint32_t diff, tick = SDL_GetTicks();

        if (tick >= cameraLastTick)
        {
            diff = tick - cameraLastTick;
        }
        else
        {
            diff = 0xFFFFFFFF - cameraLastTick + tick;
        }

        //if (diff >= 1000)
        {
            char buf[4];

            cameraCountDown -= (float)diff / 1000.0f;
            cameraLastTick = tick;

            if (cameraCountDown >= 0.0f)
            {
                sprintf(buf, "%d", (int)cameraCountDown);
                ituTextSetString(cameraCountDownText, buf);
            }
        }
        if (cameraCountDown <= 0.0f)
        {
            if(cameraDeviceInfo.type == DEVICE_IPCAM)
            {
                IPCamStopRecord();
                ituWidgetSetVisible(cameraRecordButton, true);
                ituWidgetSetVisible(cameraStopRecordButton, false);
#ifdef CFG_RTSP_CLIENT_ENABLE            
                stopRTSPClient();
#endif
                IPCamStopStreaming();
                SceneCameraSetViewing(false);
                ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
				cameraPageIndex = cameraShowingIndex = -1;
            }
            else if (SceneCameraIsViewing())
            {
                if (cameraVideoRecordEnable)            
                {
                    CaptureStopRecord();
                    ituWidgetSetVisible(cameraRecordButton, true);
                    ituWidgetSetVisible(cameraStopRecordButton, false);
                }
                LinphoneStopCamera();
                SceneCameraSetViewing(false);
                LinphoneUnMuteMic();
                SceneHangUp();
                // UI receive this event to invisible cameraViewLayer
                ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
				cameraPageIndex = cameraShowingIndex = -1;
            }
        }
    }
    return true;
}

#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)
static void cameraScreenBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;

    if(cameraDeviceInfo.type == DEVICE_IPCAM && !isIPCAM_FirstVideoGetted)
    {
        ITUColor color;
    
        color.alpha = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        ituColorFill(dest, destx, desty, rect->width, rect->height, &color);
    }
    else    
        ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    ituWidgetDrawImpl(widget, dest, x, y, alpha);
}

static void cameraViewScreenBackgroundDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    int destx, desty;
    ITURectangle* rect = (ITURectangle*) &widget->rect;

    destx = rect->x + x;
    desty = rect->y + y;

    if(cameraDeviceInfo.type == DEVICE_IPCAM && !isIPCAM_FirstVideoGetted)
    {
        ITUColor color;
    
        color.alpha = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        ituColorFill(dest, destx, desty, rect->width, rect->height, &color);

        bWaitingIPCamDisplay = true;
    }
    else if(cameraDeviceInfo.type == DEVICE_IPCAM && (bWaitingIPCamDisplay || IPCamDisplayCountDown))
    {
        ITUColor color;
    
        color.alpha = 0;
        color.red = 0;
        color.green = 0;
        color.blue = 0;
        ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
        ituColorFill(dest, destx, desty, rect->width, rect->height, &color);
        
        if(bWaitingIPCamDisplay)
        {
            bWaitingIPCamDisplay = false;
            IPCamDisplayCountDown = 0.5f;
            IPCamDisplayLastTick = SDL_GetTicks();
        }
    }
    else
    {
        ituDrawVideoSurface(dest, destx, desty, rect->width, rect->height);
    }    
    ituWidgetDrawImpl(widget, dest, x, y, alpha);   
}
#endif

bool CameraOnEnter(ITUWidget* widget, char* param)
{
    int vol;
    ITUWidget* item;

    if (!cameraScreenBackground)
    {
        cameraScreenBackground = ituSceneFindWidget(&theScene, "cameraScreenBackground");
        assert(cameraScreenBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraScreenBackground, cameraScreenBackgroundDraw);
#endif	

        cameraListBox = ituSceneFindWidget(&theScene, "cameraListBox");
        assert(cameraListBox);

        cameraCountDownText = ituSceneFindWidget(&theScene, "cameraCountDownText");
        assert(cameraCountDownText);

        cameraViewScreenBackground = ituSceneFindWidget(&theScene, "cameraViewScreenBackground");
        assert(cameraViewScreenBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraViewScreenBackground, cameraViewScreenBackgroundDraw);
#endif

        cameraBottomContainer = ituSceneFindWidget(&theScene, "cameraBottomContainer");
        assert(cameraBottomContainer);

        cameraRecordButton = ituSceneFindWidget(&theScene, "cameraRecordButton");
        assert(cameraRecordButton);

        cameraStopRecordButton = ituSceneFindWidget(&theScene, "cameraStopRecordButton");
        assert(cameraStopRecordButton);

        cameraBindingCheckBox = ituSceneFindWidget(&theScene, "cameraBindingCheckBox");
        assert(cameraBindingCheckBox);

        cameraViewBindingCheckBox = ituSceneFindWidget(&theScene, "cameraViewBindingCheckBox");
        assert(cameraViewBindingCheckBox);

        cameraConnectButton = ituSceneFindWidget(&theScene, "cameraConnectButton");
        assert(cameraConnectButton);

        cameraVolCheckBox = ituSceneFindWidget(&theScene, "cameraVolCheckBox");
        assert(cameraVolCheckBox);

        cameraVolBackground = ituSceneFindWidget(&theScene, "cameraVolBackground");
        assert(cameraVolBackground);

        cameraVol0CheckBox = ituSceneFindWidget(&theScene, "cameraVol0CheckBox");
        assert(cameraVol0CheckBox);

        cameraVol1CheckBox = ituSceneFindWidget(&theScene, "cameraVol1CheckBox");
        assert(cameraVol1CheckBox);

        cameraVol2CheckBox = ituSceneFindWidget(&theScene, "cameraVol2CheckBox");
        assert(cameraVol2CheckBox);

        cameraVol3CheckBox = ituSceneFindWidget(&theScene, "cameraVol3CheckBox");
        assert(cameraVol3CheckBox);

        cameraVol4CheckBox = ituSceneFindWidget(&theScene, "cameraVol4CheckBox");
        assert(cameraVol4CheckBox);

        cameraVol5CheckBox = ituSceneFindWidget(&theScene, "cameraVol5CheckBox");
        assert(cameraVol5CheckBox);

        cameraVol6CheckBox = ituSceneFindWidget(&theScene, "cameraVol6CheckBox");
        assert(cameraVol6CheckBox);

        // status
        cameraMaxVol = 6;
    }
    
    strcpy(cameraOldIPCamIP, theConfig.ipcam_ipaddr);

    if (CaptureGetPath())
        cameraVideoRecordEnable = true;
    else
        cameraVideoRecordEnable = false;

    cameraIsSnapshoting = false;

    vol = LinphoneGetVoiceLevel() * cameraMaxVol / 100;

    ituCheckBoxSetChecked(cameraVol0CheckBox, vol >= 0);
    ituCheckBoxSetChecked(cameraVol1CheckBox, vol >= 1);
    ituCheckBoxSetChecked(cameraVol2CheckBox, vol >= 2);
    ituCheckBoxSetChecked(cameraVol3CheckBox, vol >= 3);
    ituCheckBoxSetChecked(cameraVol4CheckBox, vol >= 4);
    ituCheckBoxSetChecked(cameraVol5CheckBox, vol >= 5);
    ituCheckBoxSetChecked(cameraVol6CheckBox, vol >= 6);
#if (CFG_CHIP_FAMILY == 9850)
    SceneEnterVideoState(33);
#endif    

    item = ituListBoxGetFocusItem(cameraListBox);

    if (strcmp(param, "cameraViewLayer") || !item)
    {
        if (cameraVideoRecordEnable)        
        {
            ituWidgetSetVisible(cameraRecordButton, true);
            ituWidgetSetVisible(cameraStopRecordButton, false);
        }
        else
        {
            ituWidgetSetVisible(cameraRecordButton, false);
            ituWidgetSetVisible(cameraStopRecordButton, false);
        }
        ituWidgetSetVisible(cameraBindingCheckBox, false);

        // count down
        ituTextSetString(cameraCountDownText, NULL);
        cameraCountDown = 0.0f;

        SceneCameraSetViewing(false);
        LinphoneSetWindowID(cameraScreenBackground);
        ituWidgetSetVisible(cameraBottomContainer, false);

        if (cameraInfoCount == 0)
            cameraInfoCount = AddressBookGetCameraInfoArray(cameraInfoArray, MAX_CAMERA_COUNT);

		cameraPageIndex = cameraShowingIndex = -1;

        return true;
    }
    else
    {
        if (cameraVideoRecordEnable)        
        {
            if (CaptureIsRecording() || IPCamIsRecording())
            {
                ituWidgetSetVisible(cameraRecordButton, false);
                ituWidgetSetVisible(cameraStopRecordButton, true);
            }
            else
            {
                ituWidgetSetVisible(cameraRecordButton, true);
                ituWidgetSetVisible(cameraStopRecordButton, false);
            }
        }
        else
        {
            ituWidgetSetVisible(cameraRecordButton, false);
            ituWidgetSetVisible(cameraStopRecordButton, false);
        }
        LinphoneSetWindowID(cameraScreenBackground);
        return false;
    }        
}

bool CameraOnLeave(ITUWidget* widget, char* param)
{    
    if (strcmp(param, "cameraViewLayer"))
    {
        //LinphoneUnMuteMic();
        //SceneHangUp();

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
            if (cameraVideoRecordEnable)        
                CaptureStopRecord();

            LinphoneStopCamera();
            SceneCameraSetViewing(false);
        }

        //if (strcmp(cameraOldIPCamIP, theConfig.ipcam_ipaddr))
        ConfigSave();

        if(strcmp(param, "calledLayer") && strcmp(param, "cameraLobbyLayer"))
        {
            LinphoneUnMuteMic();
            SceneHangUp();
#if (CFG_CHIP_FAMILY == 9850)            
            SceneLeaveVideoState();
#endif
        }    
    }
    return true;
}

void CameraReset(void)
{
    cameraScreenBackground = NULL;
}
