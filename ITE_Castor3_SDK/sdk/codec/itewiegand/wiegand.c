#include <string.h>
#include "mmio.h"
#include "ith_risc.h"

#define MAX_ARRAY_COUNT 65
///////////////////////////////////////////////////////////////////////////
//unsigned long wg0_cardTempHigh2 = 0; // 93bits
//unsigned long wg0_cardTempHigh  = 0; // 62bits
//unsigned long wg0_cardTemp      = 0; // 31bits
int           wg0_lastWiegand;
int           wg0_bitCount      = 0;

//unsigned long wg1_cardTempHigh2 = 0; // 93bits
//unsigned long wg1_cardTempHigh  = 0; // 62bits
//unsigned long wg1_cardTemp      = 0; // 31bits
int           wg1_lastWiegand;
int           wg1_bitCount      = 0;

int           w0_gpio0          = -1;
int           w0_gpio1          = -1;
int           w1_gpio0          = -1;
int           w1_gpio1          = -1;

int          w0_prevstate0 = 1;
int          w0_prevstate1 = 1;
int          w1_prevstate0 = 1;
int          w1_prevstate1 = 1;

unsigned long wiegandCard0[MAX_ARRAY_COUNT] = {0};
unsigned long wiegandCard1[MAX_ARRAY_COUNT] = {0};
//unsigned long* wiegandDataBuffer = &wiegandCard[1];

void ithGpioSetMode(unsigned int pin, ITHGpioMode mode)
{
    uint32_t value, mask;
    
    //ithEnterCritical();
    
	// for UART1 output
	if (mode == ITH_GPIO_MODE_TX)
	{
		if (pin < 32)
		{
		    ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_URTXSEL1_REG, pin);
		}
		else
		{
		    ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_URTXSEL2_REG, pin - 32);
		}

		mode = 0;
	}
	else if (mode == ITH_GPIO_MODE_RX)
	{
		if (pin < 32)
		{
		    ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_URRXSEL1_REG, pin);
		}
		else
		{
		    ithSetRegBitA(ITH_GPIO_BASE + ITH_GPIO_URRXSEL2_REG, pin - 32);
		}
		mode = 0;
	}

	if (pin < 16)
	{
		value = mode << (pin * 2);
		mask = 0x3 << (pin * 2);
		ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO1_MODE_REG, value, mask);
	}
	else if (pin < 32)
	{
		value = mode << ((pin - 16) * 2);
		mask = 0x3 << ((pin - 16) * 2);
		ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO2_MODE_REG, value, mask);
	}
	else if (pin < 48)
	{
		value = mode << ((pin - 32) * 2);
		mask = 0x3 << ((pin - 32) * 2);
		ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO3_MODE_REG, value, mask);
	}
	else
	{
		value = mode << ((pin - 48) * 2);
		mask = 0x3 << ((pin - 48) * 2);
		ithWriteRegMaskA(ITH_GPIO_BASE + ITH_GPIO4_MODE_REG, value, mask);
	}

	//ithExitCritical();
}

void wg0_begin(int gpiod0, int gpiod1)
{

    wg0_bitCount     = 0;

	ithGpioSetIn(gpiod0);
	ithGpioEnable(gpiod0);
	
	ithGpioSetIn(gpiod1);
	ithGpioEnable(gpiod1);
	
    w0_gpio0 = gpiod0;
    w0_gpio1 = gpiod1;
}

void wg1_begin(int gpiod0, int gpiod1)
{

    wg1_bitCount     = 0;
	
	ithGpioSetIn(gpiod0);
	ithGpioEnable(gpiod0);
	
	ithGpioSetIn(gpiod1);
	ithGpioEnable(gpiod1);
	
    w1_gpio0 = gpiod0;
    w1_gpio1 = gpiod1;
}

void wg0_ReadD0()
{
	int dataIndex = wg0_bitCount >> 5;
	if ((wg0_bitCount & 0x1F) != 0x1F)
	{
		wiegandCard0[dataIndex] <<= 1;
	}
	wg0_bitCount++;
}

void wg0_ReadD1()
{
	int dataIndex = wg0_bitCount >> 5;
	wiegandCard0[dataIndex] |= 1;
	if ((wg0_bitCount & 0x1F) != 0x1F)
	{
		wiegandCard0[dataIndex] <<= 1;
	}
	wg0_bitCount++;
}

void wg1_ReadD0()
{
	int dataIndex = wg1_bitCount >> 5;
	if ((wg1_bitCount & 0x1F) != 0x1F)
	{
		wiegandCard1[dataIndex] <<= 1;
	}
	wg1_bitCount++;
}

void wg1_ReadD1()
{
	int dataIndex = wg1_bitCount >> 5;
	wiegandCard1[dataIndex] |= 1;
	if ((wg1_bitCount & 0x1F) != 0x1F)
	{
		wiegandCard1[dataIndex] <<= 1;
	}
	wg1_bitCount++;

}

int wg0_DoWiegandConversion(unsigned long *card_id)
{
    int                    result = 0;
    int                    elapsedTime;
    volatile unsigned long gpioReg0, gpioReg1;
	int state0=0;
	int state1=0;
	int i = 0;

    if (w0_gpio0 == -1 || w0_gpio1 == -1)
        return result;

	state0 = ithGpioGet(w0_gpio0);
	if (w0_prevstate0 != (state0 >> w0_gpio0) && w0_prevstate0 == 1)
	{
		wg0_ReadD0();
		wg0_lastWiegand = get_wiegand_timer(1);
	}
	w0_prevstate0 = (state0 >> w0_gpio0);
	
	state1 = ithGpioGet(w0_gpio1);
	if (w0_prevstate1 != (state1 >> w0_gpio1) && w0_prevstate1 == 1)
	{
		wg0_ReadD1();
        wg0_lastWiegand = get_wiegand_timer(1); // timer 0 used by system, do not use timer 0 
	}
	w0_prevstate1 = (state1 >> w0_gpio1);	

    elapsedTime = (get_wiegand_timer(1) - wg0_lastWiegand) / (PalGetSysClock() / 1000);
	
    if (elapsedTime > 25) // if no more signal coming through after 25ms
    {
        if (wg0_lastWiegand)
        {
			int lastIndex = wg0_bitCount >> 5;
		    int shiftOffset = (wg0_bitCount & 0x1F);

			wiegandCard0[lastIndex] >>= 1;

			while (lastIndex > 0)
			{
			   if (shiftOffset)
			   {
				   wiegandCard0[lastIndex] = (wiegandCard0[lastIndex] | (wiegandCard0[lastIndex - 1] << shiftOffset));
				   wiegandCard0[lastIndex - 1] >>= (32 - shiftOffset);
			   }
			   lastIndex--;
			}
			
			lastIndex = wg0_bitCount >> 5;
			card_id[0] = wg0_bitCount;
			for(i=0; i <= lastIndex;  i++)
			{
				card_id[i+1] = wiegandCard0[i];
				wiegandCard0[i] = 0;
			}
			
			wg0_bitCount     = 0;
			wg0_lastWiegand  = 0;
			result           = 1;
			
        }
        reset_wiegand_timer(1);
    }

    return result;
}

int wg1_DoWiegandConversion(unsigned long *card_id)
{
    int                    result = 0;
    int                    elapsedTime;
    volatile unsigned long gpioReg0, gpioReg1;

	int state0=0;
	int state1=0;
	int i = 0;

    if (w1_gpio0 == -1 || w1_gpio1 == -1)
        return result;

	state0 = ithGpioGet(w1_gpio0);
	if (w1_prevstate0 != (state0 >> w1_gpio0) && w1_prevstate0 == 1)
	{
		wg1_ReadD0();
		wg1_lastWiegand = get_wiegand_timer(2);
	}
	w1_prevstate0 = (state0 >> w1_gpio0);
	
	state1 = ithGpioGet(w1_gpio1);
	if (w1_prevstate1 != (state1 >> w1_gpio1) && w1_prevstate1 == 1)
	{
		wg1_ReadD1();
        wg1_lastWiegand = get_wiegand_timer(2); // timer 0 used by system, do not use timer 0 
	}
	w1_prevstate1 = (state1 >> w1_gpio1);
	
    elapsedTime = (get_wiegand_timer(2) - wg1_lastWiegand) / (PalGetSysClock() / 1000);
	
    if (elapsedTime > 25) // if no more signal coming through after 25ms
    {
        if (wg1_lastWiegand)
        {
			int lastIndex = wg1_bitCount >> 5;
		    int shiftOffset = (wg1_bitCount & 0x1F);

			wiegandCard1[lastIndex] >>= 1;

			while (lastIndex > 0)
			{
			   if (shiftOffset)
			   {
				   wiegandCard1[lastIndex] = (wiegandCard1[lastIndex] | (wiegandCard1[lastIndex - 1] << shiftOffset));
				   wiegandCard1[lastIndex - 1] >>= (32 - shiftOffset);
			   }
			   lastIndex--;
			}

			lastIndex = wg1_bitCount >> 5;
			card_id[0] = wg1_bitCount;
			for(i=0; i <= lastIndex;  i++)
			{
				card_id[i+1] = wiegandCard1[i];
				wiegandCard1[i] = 0;
			}
			
			wg1_bitCount     = 0;
			wg1_lastWiegand  = 0;
			result           = 1;
			
        }
        reset_wiegand_timer(2);
    }
	
    return result;
}
