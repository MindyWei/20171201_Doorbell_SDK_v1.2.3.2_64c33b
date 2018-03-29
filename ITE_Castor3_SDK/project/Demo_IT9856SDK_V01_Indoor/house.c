#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

extern HouseEntry houseAirConditioner;
extern HouseEntry houseCurtain;
extern HouseEntry houseLight;
extern HouseEntry housePlug;
extern HouseEntry houseTV;

static const HouseEntry* houseEntries[] =
{
    &houseAirConditioner,
    &houseCurtain,
    &houseLight,
    &housePlug,
    &houseTV,
};

void HouseInit(void)
{
    int i;

    for (i = 0; i < HOUSE_DEVICE_COUNT; i++)
    {
        houseEntries[i]->Init();
    }
    HouseSetMode(theConfig.house_mode);
}

void HouseExit(void)
{
    int i;

    for (i = 0; i < HOUSE_DEVICE_COUNT; i++)
    {
        houseEntries[i]->Exit();
    }
}

void HouseSetMode(HouseMode mode)
{
    int i;

    if (theConfig.house_mode == mode)
        return;

    switch (mode)
    {
    case HOUSE_INDOOR:
        for (i = 0; i < HOUSE_MAX_LIGHT_COUNT; i++)
        {
            if (theConfig.house_indoor_lights[i] == '1')
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_OPEN, NULL);
            else
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_CLOSE, NULL);
        }
        break;

    case HOUSE_OUTDOOR:
        for (i = 0; i < HOUSE_MAX_LIGHT_COUNT; i++)
        {
            if (theConfig.house_outdoor_lights[i] == '1')
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_OPEN, NULL);
            else
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_CLOSE, NULL);
        }
        break;

    case HOUSE_SLEEP:
        for (i = 0; i < HOUSE_MAX_LIGHT_COUNT; i++)
        {
            if (theConfig.house_sleep_lights[i] == '1')
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_OPEN, NULL);
            else
                HouseExecAction(HOUSE_LIGHT, i, HOUSE_CLOSE, NULL);
        }
        break;
    }
    theConfig.house_mode = mode;
}

int HouseGetInfoArray(HouseDevice dev, HouseInfo infoArray[], int count)
{
    return houseEntries[dev]->GetInfoArray(infoArray, count);
}

int HouseExecAction(HouseDevice dev, int index, HouseAction action, void* data)
{
    return houseEntries[dev]->ExecAction(index, action, data);
}
