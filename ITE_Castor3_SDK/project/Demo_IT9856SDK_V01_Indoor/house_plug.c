#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

// dummy data
#define PLUG_COUNT 2

static HouseInfo plugInfoArray[PLUG_COUNT] =
{
    {
        HOUSE_PLUG,
        "Plug #1",
        HOUSE_ON,
        0
    },
    {
        HOUSE_PLUG,
        "Plug #2",
        HOUSE_OFF,
        1
    },
};

static void PlugInit(void)
{
    // TODO: IMPLEMENT
}

static void PlugExit(void)
{
    // TODO: IMPLEMENT
}

static int PlugGetInfoArray(HouseInfo infoArray[], int count)
{
    // TODO: IMPLEMENT
    int i;

    for (i = 0; i < count && i < PLUG_COUNT; i++)
    {
        infoArray[i].device = plugInfoArray[i].device;
        strcpy(infoArray[i].name, plugInfoArray[i].name);
        infoArray[i].status = plugInfoArray[i].status;
        infoArray[i].index = plugInfoArray[i].index;
    }
    return i;
}

static int PlugExecAction(int index, HouseAction action, void* data)
{
    // TODO: IMPLEMENT
    switch (action)
    {
    case HOUSE_OPEN:
        plugInfoArray[index].status = HOUSE_ON;
        break;

    case HOUSE_CLOSE:
        plugInfoArray[index].status = HOUSE_OFF;
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

HouseEntry housePlug =
{
    PlugInit,
    PlugExit,
    PlugGetInfoArray,
    PlugExecAction
};
