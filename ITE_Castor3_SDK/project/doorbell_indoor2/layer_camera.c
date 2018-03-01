#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "scene.h"
#include "doorbell.h"
#include "castor3player.h"

#define CAMERA_TIMEOUT      (30.999f)  // 30 secs

static ITUScrollListBox* cameraScrollListBox;
static ITUBackground* cameraScreenBackground;
static ITUText* cameraCountDownText;
static ITUBackground* cameraViewScreenBackground;
static ITUButton* cameraRecordButton;
static ITUButton* cameraStopRecordButton;
static ITUButton* cameraSnapshotButton;

static int cameraInfoCount, cameraPageIndex, cameraShowingIndex;
static DeviceInfo cameraInfoArray[MAX_CAMERA_COUNT];
static DeviceInfo cameraDeviceInfo;
static uint32_t cameraLastTick;
static bool cameraVideoRecordEnable, cameraIsSnapshoting;

float       cameraCountDown = 0;
bool        LockRecordButton = false;
float       LockRecordButtonCountDown = 0;
uint32_t    LockRecordButtonLastTick = 0;

float       IPCamDisplayCountDown = 0;
uint32_t    IPCamDisplayLastTick = 0;
bool        bWaitingIPCamDisplay = false;

extern bool isIPCAM_FirstVideoGetted;
extern bool captureDeletePic;

bool CameraScrollListBoxOnLoad(ITUWidget* widget, char* param)
{
    ITCTree* node;
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUScrollListBox* slistbox = (ITUScrollListBox*)widget;
    int i, j, count;
    DeviceInfo* info;

    count = ituScrollListBoxGetItemCount(slistbox);
    node = ituScrollListBoxGetLastPageItem(slistbox);
    listbox->pageCount = cameraInfoCount ? (cameraInfoCount + count - 1) / count : 1;

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

    for (; j < cameraInfoCount; j++)
    {
        ITUScrollText* scrolltext = (ITUScrollText*) node;

        info = &cameraInfoArray[j];
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

	if (listbox->pageIndex == cameraPageIndex)
		ituListBoxSelect(listbox, cameraShowingIndex);

    return true;
}

static void CameraWatch(int arg)
{
    ITUWidget* item = ituListBoxGetFocusItem(&cameraScrollListBox->listbox);
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
    ITUWidget* item = ituListBoxGetFocusItem(&cameraScrollListBox->listbox);
    if (item)
    {
        char rtsp_url[64] = {0};
        char buf[4];
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(item);
        
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

bool CameraScrollListOnSelection(ITUWidget* widget, char* param)
{
    ITUListBox* listbox = (ITUListBox*)widget;
    ITUWidget* item = ituListBoxGetFocusItem(&cameraScrollListBox->listbox);

    if (item)
    {
        DeviceInfo* info = (DeviceInfo*)ituWidgetGetCustomData(item);
        if (info)
        {
            if(cameraDeviceInfo.type == DEVICE_IPCAM)
            {
                if (cameraVideoRecordEnable)
                {
                    IPCamStopRecord();
                    ituWidgetSetVisible(cameraRecordButton, true);
                    ituWidgetSetVisible(cameraStopRecordButton, false);
                }
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
			cameraPageIndex = cameraScrollListBox->listbox.pageIndex;
			cameraShowingIndex = cameraScrollListBox->listbox.focusIndex;
        }
    }
    return true;
}

bool CameraScreenButtonOnPress(ITUWidget* widget, char* param)
{
	if (cameraScrollListBox->listbox.focusIndex >= 0 || SceneCameraIsViewing())
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

bool CameraStopRecordButtonOnPress(ITUWidget* widget, char* param)
{
    if (cameraVideoRecordEnable)
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
    ituSceneExecuteCommand(&theScene, 1000/MS_PER_FRAME, CameraEnableSnapshot, 0);
	return true;
}

bool CameraDisconnectButtonOnPress(ITUWidget* widget, char* param)
{
    if(cameraDeviceInfo.type == DEVICE_IPCAM)
    {
        if (cameraVideoRecordEnable)
        {
            IPCamStopRecord();
            ituWidgetSetVisible(cameraRecordButton, true);
            ituWidgetSetVisible(cameraStopRecordButton, false);
        }
#ifdef CFG_RTSP_CLIENT_ENABLE            
        stopRTSPClient();
#endif
        IPCamStopStreaming();
        SceneCameraSetViewing(false);
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
        cameraCountDown = 0.0f;
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
        // UI receive this event to invisible cameraViewLayer
        ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
        cameraCountDown = 0.0f;
		cameraPageIndex = cameraShowingIndex = -1;
    }
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

    if (LockRecordButtonCountDown > 0.0f && LockRecordButton)
    {
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - LockRecordButtonLastTick;
        if (diff >= 1000)
        {
            LockRecordButtonCountDown -= (float)diff / 1000.0f;
            LockRecordButtonLastTick = tick;
        }
        if (LockRecordButtonCountDown <= 0.0f)
        {
            LockRecordButton = false;
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
                if (cameraVideoRecordEnable)
                {
                    IPCamStopRecord();
                    ituWidgetSetVisible(cameraRecordButton, true);
                    ituWidgetSetVisible(cameraStopRecordButton, false);
                }
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
                // UI receive this event to invisible cameraViewLayer
                ituSceneSendEvent(&theScene, EVENT_CUSTOM_CAMERA_TIMEOUT, NULL);
				cameraPageIndex = cameraShowingIndex = -1;
            }
        }
    }

	if(captureDeletePic)
	{
		ituWidgetSetVisible(cameraSnapshotButton, false);
	}
	else
	{
		ituWidgetSetVisible(cameraSnapshotButton, true);
	}
	
    return false;
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
    if (!cameraScreenBackground)
    {
        cameraScreenBackground = ituSceneFindWidget(&theScene, "cameraScreenBackground");
        assert(cameraScreenBackground);
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraScreenBackground, cameraScreenBackgroundDraw);
#endif 

        cameraScrollListBox = ituSceneFindWidget(&theScene, "cameraScrollListBox");
        assert(cameraScrollListBox);

        cameraCountDownText = ituSceneFindWidget(&theScene, "cameraCountDownText");
        assert(cameraCountDownText);

        cameraViewScreenBackground = ituSceneFindWidget(&theScene, "cameraViewScreenBackground");
        assert(cameraViewScreenBackground);    
#if (CFG_CHIP_FAMILY == 9850) && (CFG_VIDEO_ENABLE)        
        ituWidgetSetDraw(cameraViewScreenBackground, cameraViewScreenBackgroundDraw);
#endif           

        cameraRecordButton = ituSceneFindWidget(&theScene, "cameraRecordButton");
        assert(cameraRecordButton);

        cameraStopRecordButton = ituSceneFindWidget(&theScene, "cameraStopRecordButton");
        assert(cameraStopRecordButton);

		cameraSnapshotButton = ituSceneFindWidget(&theScene, "cameraSnapshotButton"); 
		assert(cameraSnapshotButton);
    }

    if (strcmp(param, "cameraViewLayer"))
    {
        SceneEnterVideoState(33);

    }
    
    if (theDeviceInfo.type == DEVICE_INDOOR2 && CaptureGetPath())
        cameraVideoRecordEnable = true;
    else
        cameraVideoRecordEnable = false;

    cameraIsSnapshoting = false;

    if (strcmp(param, "cameraViewLayer") || !SceneCameraIsViewing())
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

        // count down
        ituTextSetString(cameraCountDownText, NULL);
        cameraCountDown = 0.0f;

        SceneCameraSetViewing(false);
        LinphoneSetWindowID(cameraScreenBackground);

        if (cameraInfoCount == 0)
            cameraInfoCount = AddressBookGetCameraInfoArray(cameraInfoArray, MAX_CAMERA_COUNT, &theDeviceInfo);

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

        if(strcmp(param, "calledLayer") && strcmp(param, "videoRecordLayer"))
            SceneLeaveVideoState();
        SceneContinuePlayGuardAlarm();
    }
    return true;
}

void CameraReset(void)
{
    cameraScreenBackground = NULL;
}
