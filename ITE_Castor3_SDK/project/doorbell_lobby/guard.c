#include <sys/ioctl.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

typedef struct
{
    bool enabled;
    bool alarming;
    unsigned long time;
    int alarmDelay;
} GuardSensorStatus;

static GuardSensorStatus guardSensorStatusArray[GUARD_SENSOR_COUNT];

#ifdef CFG_GUARD_ENABLE
static int guardSensorGpioTable[GUARD_SENSOR_COUNT] = { CFG_GPIO_GUARD_SENSORS };
#endif

static bool guardSensorAlarmedTable[GUARD_SENSOR_COUNT];
static unsigned int guardSensorLastValue = 0;
static bool guardAlarming;

#define GUARD_ALARM_DELAY      (1 * 1000 / 33)  // 1 sec
#ifdef CFG_GUARD_ENABLE
static void GuardSensorIntrHandler(unsigned int pin, void* arg)
{
    int i;
    
    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensorGpioTable[i] == pin)
        {
            if (ithGpioGet(pin))
                guardSensorLastValue |= 0x1 << i;
            else
                guardSensorLastValue &= ~(0x1 << i);
            break;
        }
    }
    //ithPrintf("ithGpioGet(%d)=0x%X\n", pin, ithGpioGet(pin));
    ithGpioClearIntr(pin);
}
#endif // CFG_GUARD_ENABLE

static void GuardEnableSensor(GuardSensor sensor)
{
    if (!guardSensorStatusArray[sensor].enabled)
    {
        struct timeval tv;

    #ifdef CFG_GUARD_ENABLE
        unsigned int pin = guardSensorGpioTable[sensor];
        ithGpioRegisterIntrHandler(pin, GuardSensorIntrHandler, NULL);
        ithGpioCtrlEnable(pin, ITH_GPIO_INTR_BOTHEDGE);
        ithGpioEnableIntr(pin);
        ithGpioSetIn(pin);

        if (theConfig.guard_sensor_initvalues[sensor] == '1')
            ithGpioSet(pin);
        else
            ithGpioClear(pin);

        ithGpioEnable(pin);
    #endif // CFG_GUARD_ENABLE

        guardSensorStatusArray[sensor].enabled = true;
        guardSensorStatusArray[sensor].alarmDelay = -1;

        gettimeofday(&tv, NULL);
        guardSensorStatusArray[sensor].time = tv.tv_sec;

        EventWriteGuardLog(sensor, GUARD_ENABLE, tv.tv_sec);
    }
}

static void GuardDisableSensor(GuardSensor sensor)
{
    if (guardSensorStatusArray[sensor].enabled)
    {
        struct timeval tv;

    #ifdef CFG_GUARD_ENABLE
        unsigned int pin = guardSensorGpioTable[sensor];
        ithGpioDisableIntr(pin);
    #endif // CFG_GUARD_ENABLE

        guardSensorStatusArray[sensor].enabled = false;
        guardSensorStatusArray[sensor].alarmDelay = -1;
        gettimeofday(&tv, NULL);
        guardSensorStatusArray[sensor].time = tv.tv_sec;

        if (guardSensorStatusArray[sensor].alarming)
        {
            guardSensorStatusArray[sensor].alarming = false;
            EventWriteGuardLog(sensor, GUARD_UNALARM, tv.tv_sec);
        }
        EventWriteGuardLog(sensor, GUARD_DISABLE, tv.tv_sec);
    }
}

void GuardInit(void)
{
    int i;
    struct timeval tv;

    guardSensorLastValue = 0;
    guardAlarming = false;

    gettimeofday(&tv, NULL);

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        guardSensorStatusArray[i].enabled = false;
        guardSensorStatusArray[i].time = tv.tv_sec;
        guardSensorStatusArray[i].alarming = false; //Benson
        guardSensorStatusArray[i].alarmDelay = -1;
    }

#ifdef CFG_GUARD_ENABLE
    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (theConfig.guard_sensors[i] == '1')
            GuardEnableSensor(i);
    }

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensorStatusArray[i].enabled)
        {
            if (ithGpioGet(guardSensorGpioTable[i]))
                guardSensorLastValue |= 0x1 << i;
        }
    }
#endif // CFG_GUARD_ENABLE
}

void GuardSetMode(void)
{
    int i;
    char* guardSensors = theConfig.guard_sensors;

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (guardSensors[i] == '1')
            GuardEnableSensor(i);
        else
            GuardDisableSensor(i);
    }
}

int GuardCheck(void)
{
    int i, result = 0;

    for (i = 0; i < GUARD_SENSOR_COUNT; i++)
    {
        if (theConfig.guard_sensors[i] == '1')
        {
            if (guardSensorLastValue & (0x1 << i))
            {
                if (theConfig.guard_sensor_initvalues[i] == '0')
                {
                    if (guardSensorStatusArray[i].alarmDelay == -1)
                    {
                        if (i == GUARD_DOOR)
                            guardSensorStatusArray[i].alarmDelay = theConfig.guard_time * 1000 / 33;
                        else
                            guardSensorStatusArray[i].alarmDelay = GUARD_ALARM_DELAY;
                    }
                    else if (guardSensorStatusArray[i].alarmDelay > 0)
                    {
                        guardSensorStatusArray[i].alarmDelay--;
                    }
                    
                    if (guardSensorStatusArray[i].alarmDelay == 0)
                    {
                        if (!guardSensorStatusArray[i].alarming)
                        {
                           struct timeval tv;
                           gettimeofday(&tv, NULL);
                        
                           guardSensorStatusArray[i].alarming = true;
                           guardSensorStatusArray[i].time = tv.tv_sec;
                        
                           EventWriteGuardLog(i, GUARD_ALARM, tv.tv_sec);
                        }
                        if (i == GUARD_EMERGENCY)
                        {
                            result = 1;
                        }
                        else if (result == 0)
                        {
                            result = -1;
                        }
                    }
                }
            }
            else
            {
                if (guardSensorStatusArray[i].alarming)
                {
                    if (i == GUARD_EMERGENCY)
                    {
                        result = 1;
                    }
                    else if (result == 0)
                    {
                        result = -1;
                    }
                }
                else
                {
                    guardSensorStatusArray[i].alarmDelay = -1;
                }
            }
        }
        else
        {
            if (theConfig.guard_sensor_initvalues[i] == '1')
            {
                if (guardSensorStatusArray[i].alarmDelay == -1)
                {
                    if (i == GUARD_DOOR)
                        guardSensorStatusArray[i].alarmDelay = theConfig.guard_time * 1000 / 33;
                    else
                        guardSensorStatusArray[i].alarmDelay = GUARD_ALARM_DELAY;
                }
                else if (guardSensorStatusArray[i].alarmDelay > 0)
                {
                    guardSensorStatusArray[i].alarmDelay--;
                }
                    
                if (guardSensorStatusArray[i].alarmDelay == 0)
                {
                    if (!guardSensorStatusArray[i].alarming)
                    {
                        struct timeval tv;
                        gettimeofday(&tv, NULL);

                        guardSensorStatusArray[i].alarming = true;
                        guardSensorStatusArray[i].time = tv.tv_sec;
                        EventWriteGuardLog(i, GUARD_ALARM, tv.tv_sec);
                    }
                    if (i == GUARD_EMERGENCY)
                    {
                        result = 1;
                    }
                    else if (result == 0)
                    {
                        result = -1;
                    }
                }
            }
            else
            {
                if (guardSensorStatusArray[i].alarming)
                {
                    if (i == GUARD_EMERGENCY)
                    {
                        result = 1;
                    }
                    else if (result == 0)
                    {
                        result = -1;
                    }
                }
                else
                {
                    guardSensorStatusArray[i].alarmDelay = -1;
                }
            }
        }
    }
    return result;
}

bool GuardIsAlarming(void)
{
    return guardAlarming;
}

void GuardSetAlarming(bool alarming)
{
    guardAlarming = alarming;
}
