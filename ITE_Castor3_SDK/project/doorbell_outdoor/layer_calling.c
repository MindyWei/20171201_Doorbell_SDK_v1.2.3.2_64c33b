#include <sys/time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL/SDL.h"
#include "linphone/linphone_castor3.h"
#include "scene.h"
#include "doorbell.h"

#define HANGUP_DELAY (500 / 33)  // 0.5 secs

static float dialCountDown, callingCountDown, callingVoiceMemoCountDown;
static uint32_t dialLastTick, callingLastTick, callingVoiceMemoLastTick;
static DeviceInfo callingDeviceInfo;
static DeviceInfo callingManagerInfoArray[MAX_MANAGER_COUNT];
#ifdef CFG_EMERGENCY_DOORCAMERA
static int callingManagerIndex;
static int callingManagerCount;
void CallingPollingIndexReset(void)
{
    callingManagerIndex = -1;
    callingManagerCount = AddressBookGetManagerInfoArray(callingManagerInfoArray, MAX_MANAGER_COUNT);
}
#endif

int CallingCall(char* num)
{
#ifndef CFG_EMERGENCY_DOORCAMERA
//Outdoor calling to indoor
        DeviceInfo infos[LINPHONE_CASTOR3_MAX_IP_COUNT];
        int len = strlen(num);

        memcpy(&callingDeviceInfo, &theDeviceInfo, sizeof (DeviceInfo));

        if (theDeviceInfo.type == DEVICE_OUTDOOR)
        {
            if (len < 3 || len > 4)
                return -1;

            if (len == 4)
            {
                callingDeviceInfo.floor[0] = num[0];
                callingDeviceInfo.floor[1] = num[1];
                callingDeviceInfo.floor[2] = '\0';
                callingDeviceInfo.room[0] = num[2];
                callingDeviceInfo.room[1] = num[3];
                callingDeviceInfo.room[2] = '\0';
            }
            else
            {
                callingDeviceInfo.floor[0] = '0';
                callingDeviceInfo.floor[1] = num[0];
                callingDeviceInfo.floor[2] = '\0';
                callingDeviceInfo.room[0] = num[1];
                callingDeviceInfo.room[1] = num[2];
                callingDeviceInfo.room[2] = '\0';
            }
        }
        else
        {
            printf("unknown device type: %d\n", theDeviceInfo.type);
            return -1;
        }

        len = AddressBookGetDeviceInfoArray(infos, LINPHONE_CASTOR3_MAX_IP_COUNT, callingDeviceInfo.area, callingDeviceInfo.building, callingDeviceInfo.unit, callingDeviceInfo.floor, callingDeviceInfo.room);
        if (len > 0)
        {
            memcpy(&callingDeviceInfo, &infos[0], sizeof (DeviceInfo));

            LinphoneCallDevices(infos, len, true, true);
            SceneSetRemoteIP(infos[0].ip);
            dialCountDown = (float)theConfig.dial_time;
            dialLastTick = SDL_GetTicks();

            return 0;
        }
        else
        {
            return -1;
        } 
#else
//Emergency doorcamera calling to admin & center
        if(callingManagerIndex < callingManagerCount)
        {    
            callingManagerIndex++;
        
            if(callingManagerIndex<callingManagerCount)
            {
                //call admin
                sceneFlagdail();
                memcpy(&callingDeviceInfo, &callingManagerInfoArray[callingManagerIndex], sizeof (DeviceInfo));
                LinphoneCall(callingDeviceInfo.ip, true);
                SceneSetRemoteIP(callingDeviceInfo.ip);
                dialCountDown = (float)theConfig.dial_time;
                dialLastTick = SDL_GetTicks();
            }
            else
            {         
                //call center
                sceneFlagdail();
                memcpy(&callingDeviceInfo, &theCenterInfo, sizeof (DeviceInfo));
                LinphoneCall(callingDeviceInfo.ip, true);
                SceneSetRemoteIP(callingDeviceInfo.ip);
                dialCountDown = (float)theConfig.dial_time;
                dialLastTick = SDL_GetTicks();
            }
        }
        else{
            LinphonePlayHintSound(HINT_SOUND_REDIAL);
        }
        return 0;
#endif
}

bool CallingConnectedOnCustom(void)
{
    dialCountDown       = 0.0f;
    callingCountDown    = (float)theConfig.calling_time;
    callingLastTick     = SDL_GetTicks();
#ifdef CFG_EMERGENCY_DOORCAMERA
    callingManagerIndex = -1;
#endif
	return true;
}

bool CallingOnTimer(void)
{
	if (dialCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - dialLastTick;
        if (diff >= 1000)
        {
            dialCountDown -= (float)diff / 1000.0f;
            dialLastTick = tick;
        }
        if (dialCountDown <= 0.0f)
        {
            LinphoneTerminate();
            SceneHangUp();
            SceneFlagReset();
#ifdef CFG_EMERGENCY_DOORCAMERA
            CallingCall(NULL);
#endif
        }
	}
    else if (callingCountDown > 0.0f)
	{
        uint32_t tick = SDL_GetTicks();
        uint32_t diff = tick - callingLastTick;
        if (diff >= 1000)
        {
            callingCountDown -= (float)diff / 1000.0f;
            callingLastTick = tick;
        }
        if (callingCountDown <= 0.0f)
        {
            LinphoneTerminate();
            SceneHangUp();
            SceneFlagReset();
        }
	}
	return 0;
}
