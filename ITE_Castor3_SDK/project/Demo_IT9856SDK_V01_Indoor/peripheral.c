#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"

void PeripheralInit(void)
{

}

bool PeripheralDoorIsOpened(void)
{
    // TODO: IMPLEMENT
	return false;
}

void PeripheralOpenDoor(void)
{
    // TODO: IMPLEMENT
    puts("open door");
}

char* PeripheralReadCard(void)
{
    // TODO: IMPLEMENT
    //return "1234567890";
    return NULL;
}
