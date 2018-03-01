#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

// dummy data
#define LIGHT_COUNT 6

static HouseInfo lightInfoArray[LIGHT_COUNT] =
{
    {
        HOUSE_LIGHT,
        "Light #1",
        HOUSE_ON,
        0
    },
    {
        HOUSE_LIGHT,
        "Light #2",
        HOUSE_OFF,
        1
    },
    {
        HOUSE_LIGHT,
        "Light #3",
        HOUSE_OFF,
        2
    },
    {
        HOUSE_LIGHT,
        "Light #4",
        HOUSE_OFF,
        3
    },
    {
        HOUSE_LIGHT,
        "Light #5",
        HOUSE_OFF,
        4
    },
    {
        HOUSE_LIGHT,
        "Light #6",
        HOUSE_OFF,
        5
    },
};

static void LightInit(void)
{
    // TODO: IMPLEMENT
}

static void LightExit(void)
{
    // TODO: IMPLEMENT
}

static int LightGetInfoArray(HouseInfo infoArray[], int count)
{
    // TODO: IMPLEMENT
    int i;

    for (i = 0; i < count && i < LIGHT_COUNT; i++)
    {
        infoArray[i].device = lightInfoArray[i].device;
        strcpy(infoArray[i].name, lightInfoArray[i].name);
        infoArray[i].status = lightInfoArray[i].status;
        infoArray[i].index = lightInfoArray[i].index;
    }
    return i;
}

static int LightExecAction(int index, HouseAction action, void* data)
{
    // TODO: IMPLEMENT
    switch (action)
    {
    case HOUSE_OPEN:
        lightInfoArray[index].status = HOUSE_ON;
        break;

    case HOUSE_CLOSE:
        lightInfoArray[index].status = HOUSE_OFF;
        break;

    case HOUSE_STOP:
        break;

    case HOUSE_GET:
        break;

    case HOUSE_SET:
        break;

    default:
        return -1;
    }
    return 0;
}

HouseEntry houseLight =
{
    LightInit,
    LightExit,
    LightGetInfoArray,
    LightExecAction
};
