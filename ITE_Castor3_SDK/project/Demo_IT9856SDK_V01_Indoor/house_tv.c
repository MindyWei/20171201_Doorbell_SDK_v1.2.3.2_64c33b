#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

// dummy data
#define TV_COUNT 2

static HouseInfo tvInfoArray[TV_COUNT] =
{
    {
        HOUSE_TV,
        "TV #1",
        HOUSE_ON,
        0
    },
    {
        HOUSE_TV,
        "TV #2",
        HOUSE_OFF,
        1
    },
};

static void TVInit(void)
{
    // TODO: IMPLEMENT
}

static void TVExit(void)
{
    // TODO: IMPLEMENT
}

static int TVGetInfoArray(HouseInfo infoArray[], int count)
{
    // TODO: IMPLEMENT
    int i;

    for (i = 0; i < count && i < TV_COUNT; i++)
    {
        infoArray[i].device = tvInfoArray[i].device;
        strcpy(infoArray[i].name, tvInfoArray[i].name);
        infoArray[i].status = tvInfoArray[i].status;
        infoArray[i].index = tvInfoArray[i].index;
    }
    return i;
}

static int TVExecAction(int index, HouseAction action, void* data)
{
    // TODO: IMPLEMENT
    switch (action)
    {
    case HOUSE_OPEN:
        tvInfoArray[index].status = HOUSE_ON;
        break;

    case HOUSE_CLOSE:
        tvInfoArray[index].status = HOUSE_OFF;
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

HouseEntry houseTV =
{
    TVInit,
    TVExit,
    TVGetInfoArray,
    TVExecAction
};
