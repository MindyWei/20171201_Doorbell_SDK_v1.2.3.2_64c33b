#include <sys/ioctl.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "ite/itp.h"
#include "doorbell.h"
#include "ite/ite_risc.h"


void PeripheralInit(void)
{ 
#ifndef _WIN32
    int wiegand_bit_count;
    int wiegand_gpio[2];
    int swuart_gpio;
    int swuart_baudrate;
    
#if defined(CFG_WIEGAND0_ENABLE)
		itpRegisterDevice(ITP_DEVICE_WIEGAND0, &itpDeviceWiegand0);
#if !defined (CFG_DBG_SWUART_CODEC) && !defined (CFG_SWUART_CODEC_ENABLE)	    
		ioctl(ITP_DEVICE_WIEGAND0, ITP_IOCTL_INIT, NULL);
#endif    
		wiegand_bit_count = WIEGAND0_BIT_COUNT;
		ioctl(ITP_DEVICE_WIEGAND0, ITP_IOCTL_SET_BIT_COUNT, &wiegand_bit_count);
		wiegand_gpio[0] = CFG_WIEGAND0_GPIO0;
		wiegand_gpio[1] = CFG_WIEGAND0_GPIO1;
		ioctl(ITP_DEVICE_WIEGAND0, ITP_IOCTL_SET_GPIO_PIN, wiegand_gpio); // must set before enable
		ioctl(ITP_DEVICE_WIEGAND0, ITP_IOCTL_ENABLE, NULL);
#elif defined(CFG_WIEGAND1_ENABLE)
		itpRegisterDevice(ITP_DEVICE_WIEGAND1, &itpDeviceWiegand1);
#if !defined (CFG_DBG_SWUART_CODEC) && !defined (CFG_SWUART_CODEC_ENABLE)
		ioctl(ITP_DEVICE_WIEGAND1, ITP_IOCTL_INIT, NULL);
#endif    
		wiegand_bit_count = WIEGAND1_BIT_COUNT;
		ioctl(ITP_DEVICE_WIEGAND1, ITP_IOCTL_SET_BIT_COUNT, &wiegand_bit_count);
		wiegand_gpio[0] = CFG_WIEGAND1_GPIO0;
		wiegand_gpio[1] = CFG_WIEGAND1_GPIO1;
		ioctl(ITP_DEVICE_WIEGAND1, ITP_IOCTL_SET_GPIO_PIN, wiegand_gpio); // must set before enable
		ioctl(ITP_DEVICE_WIEGAND1, ITP_IOCTL_ENABLE, NULL);
#endif
#endif // !_WIN32

#ifdef CFG_GPIO_EXPANDER_ENABLE
    char pin =4;
    itpIOExpanderSetOut(IOEXPANDER_0, pin); //default set  pins 4= output.
    itpIOExpanderClear(IOEXPANDER_0, pin);    
#else
    ithGpioSetMode(ITH_GPIO_PIN48, ITH_GPIO_MODE0);
    ithGpioSetOut(ITH_GPIO_PIN48);
    ithGpioClear(ITH_GPIO_PIN48);
#endif
 
}

bool PeripheralDoorIsOpened(void)
{
    // TODO: IMPLEMENT
    //ithPrintf("PeripheralDoorIsOpened\n");
	return false;
}

void PeripheralOpenDoor(void)
{
    bool isCallConnected = SceneIsCallConnected();
    
#ifdef CFG_GPIO_EXPANDER_ENABLE
    char pin =4;
    unsigned char R_data;
    itpIOExpanderSet(IOEXPANDER_0,pin);
    usleep(1000*1000*1);
    itpIOExpanderClear(IOEXPANDER_0,pin);
#else
    ithGpioSet(ITH_GPIO_PIN48);
    usleep(1000*1000*1);
    ithGpioClear(ITH_GPIO_PIN48);
#endif
    puts("open door");
}

char* PeripheralReadCard(void)
{
    char* card_id = NULL;

#if defined(CFG_WIEGAND0_ENABLE)
    read(ITP_DEVICE_WIEGAND0, &card_id, 0);
#elif defined(CFG_WIEGAND1_ENABLE)
    read(ITP_DEVICE_WIEGAND1, &card_id, 0);
#endif
    return card_id;
}

void PeripheralCallElevator(char* floor, bool up)
{
    // TODO: IMPLEMENT
}
