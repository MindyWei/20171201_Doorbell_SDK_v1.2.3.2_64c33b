#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

// dummy data
#define AIRCONDITIONER_COUNT 2

static HouseInfo airConditionerInfoArray[AIRCONDITIONER_COUNT] = 
{
    {
        HOUSE_AIRCONDITIONER,
        "AirConditioner #1",
        HOUSE_ON,
        0
    },
    {
        HOUSE_AIRCONDITIONER,
        "AirConditioner #2",
        HOUSE_OFF,
        1
    },
};

static HouseAirConditionerSetting airConditionerSetting =
{
    HOUSE_AC_AUTO,
    1,
    22
};

static void AirConditionerInit(void)
{
    // TODO: IMPLEMENT
}

static void AirConditionerExit(void)
{
    // TODO: IMPLEMENT
}

static int AirConditionerGetInfoArray(HouseInfo infoArray[], int count)
{
    // TODO: IMPLEMENT
    int i;

    for (i = 0; i < count && i < AIRCONDITIONER_COUNT; i++)
    {
        infoArray[i].device = airConditionerInfoArray[i].device;
        strcpy(infoArray[i].name, airConditionerInfoArray[i].name);
        infoArray[i].status = airConditionerInfoArray[i].status;
        infoArray[i].index = airConditionerInfoArray[i].index;
    }
    return i;
}

static int AirConditionerExecAction(int index, HouseAction action, void* data)
{
    // TODO: IMPLEMENT
    switch (action)
    {
    case HOUSE_OPEN:
        airConditionerInfoArray[index].status = HOUSE_ON;
        break;

    case HOUSE_CLOSE:
        airConditionerInfoArray[index].status = HOUSE_OFF;
        break;

    case HOUSE_STOP:
        break;

    case HOUSE_GET:
        memcpy(data, &airConditionerSetting, sizeof (HouseAirConditionerSetting));
        break;

    case HOUSE_SET:
        memcpy(&airConditionerSetting, data, sizeof (HouseAirConditionerSetting));
        break;

    default:
        return -1;
    }
    return 0;
}

HouseEntry houseAirConditioner =
{
    AirConditionerInit,
    AirConditionerExit,
    AirConditionerGetInfoArray,
    AirConditionerExecAction
};
