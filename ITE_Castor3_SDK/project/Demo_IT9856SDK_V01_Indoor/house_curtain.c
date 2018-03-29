#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

// dummy data
#define CURTAIN_COUNT 2

static HouseInfo curtainInfoArray[CURTAIN_COUNT] =
{
    {
        HOUSE_CURTAIN,
        "Curtain #1",
        HOUSE_ON,
        0
    },
    {
        HOUSE_CURTAIN,
        "Curtain #2",
        HOUSE_OFF,
        1
    },
};

static void CurtainInit(void)
{
    // TODO: IMPLEMENT
}

static void CurtainExit(void)
{
    // TODO: IMPLEMENT
}

static int CurtainGetInfoArray(HouseInfo infoArray[], int count)
{
    // TODO: IMPLEMENT
    int i;

    for (i = 0; i < count && i < CURTAIN_COUNT; i++)
    {
        infoArray[i].device = curtainInfoArray[i].device;
        strcpy(infoArray[i].name, curtainInfoArray[i].name);
        infoArray[i].status = curtainInfoArray[i].status;
        infoArray[i].index = curtainInfoArray[i].index;
    }
    return i;
}

static int CurtainExecAction(int index, HouseAction action, void* data)
{
    // TODO: IMPLEMENT
    switch (action)
    {
    case HOUSE_OPEN:
        curtainInfoArray[index].status = HOUSE_ON;
        break;

    case HOUSE_CLOSE:
        curtainInfoArray[index].status = HOUSE_OFF;
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

HouseEntry houseCurtain =
{
    CurtainInit,
    CurtainExit,
    CurtainGetInfoArray,
    CurtainExecAction
};
