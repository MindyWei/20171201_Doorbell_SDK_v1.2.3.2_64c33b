#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "mmio.h"
//#include "ite/ite_risc.h"

///////////////////////////////////////////////////////////////////////////
//                              Constant Definition
///////////////////////////////////////////////////////////////////////////
#define RISC_COMMUNICATE_REG      0x16A8

#define COMMAND_BUFFER_SIZE  4
#define WIEGAND_BUFFER_SIZE  32

#define WIEGAND_CTXT_OFFSET  (382*1024) //RISC1_IMAGE_SIZE + AUDIO_MESSAGE_SIZE
//#define WIEGAND_CTXT_OFFSET  (RISC1_IMAGE_SIZE + AUDIO_MESSAGE_SIZE)

typedef struct WIEGAND_CONTEXT
{
    unsigned long cmdBuffer;
    unsigned long cmdBufferSize;
    unsigned long wg0wiegandBuffer;
    unsigned long wg0wiegandBufferSize;
	unsigned long wg1wiegandBuffer;
    unsigned long wg1wiegandBufferSize;
}WIEGAND_CONTEXT;

enum {
    CMD_WIEGAND_ENABLE = 11 
};

//=============================================================================
//                              Macro Definition
//=============================================================================
//#define ENDIAN_SWAP16(x) \
//        (((x) & (unsigned short) 0x00FF) << 8) | \
//        (((x) & (unsigned short) 0xFF00) >> 8)

#if (CFG_CHIP_FAMILY == 9070 ||CFG_CHIP_FAMILY == 9850)
#define ENDIAN_SWAP32(x) \
        (((x) & 0x000000FF) << 24) | \
        (((x) & 0x0000FF00) <<  8) | \
        (((x) & 0x00FF0000) >>  8) | \
        (((x) & 0xFF000000) >> 24)
#else
#define ENDIAN_SWAP32(x) x
#endif	

///////////////////////////////////////////////////////////////////////////
//                              Globale Variable
///////////////////////////////////////////////////////////////////////////

unsigned long cmdBuffer[COMMAND_BUFFER_SIZE]   __attribute__ ((aligned(16), section (".sbss")));
unsigned char wg0wiegandBuffer[WIEGAND_BUFFER_SIZE] __attribute__ ((aligned(16), section (".sbss")));
unsigned char wg1wiegandBuffer[WIEGAND_BUFFER_SIZE] __attribute__ ((aligned(16), section (".sbss")));
///////////////////////////////////////////////////////////////////////////
//                              Function Decleration
///////////////////////////////////////////////////////////////////////////

static unsigned long* card_id0 = (unsigned long*)wg0wiegandBuffer;
static unsigned long* card_id1 = (unsigned long*)wg1wiegandBuffer;


int main(int argc, char **argv)
{
    WIEGAND_CONTEXT *pWiegandCtxt = (WIEGAND_CONTEXT*) (WIEGAND_CTXT_OFFSET);
    pWiegandCtxt->cmdBuffer = ENDIAN_SWAP32(((unsigned long)cmdBuffer));
    pWiegandCtxt->cmdBufferSize = ENDIAN_SWAP32(COMMAND_BUFFER_SIZE);
    pWiegandCtxt->wg0wiegandBuffer = ENDIAN_SWAP32(((unsigned long)wg0wiegandBuffer));
    pWiegandCtxt->wg0wiegandBufferSize = ENDIAN_SWAP32(WIEGAND_BUFFER_SIZE);
	pWiegandCtxt->wg1wiegandBuffer = ENDIAN_SWAP32(((unsigned long)wg1wiegandBuffer));
    pWiegandCtxt->wg1wiegandBufferSize = ENDIAN_SWAP32(WIEGAND_BUFFER_SIZE);


	static int wiegandenable = 0;
	int command, parameter0, parameter1, parameter2;

	//MMIO_Write(0x16E2, WIEGAND_CTXT_OFFSET);
	//MMIO_Write(0x16E0, WIEGAND_CTXT_OFFSET >> 16);
	
    while(1)
	{
	
		unsigned short communicateReg = MMIO_Read(RISC_COMMUNICATE_REG);

		if (communicateReg == 0x1234)
        {
            break;
        }
		MMIO_Write(0x16A8, 1);

		if (MMIO_Read(0x16A4) == 0x1111)
        {         	 
        	 // read command and parameters

             //or32_invalidate_cache(commandBuffer, 32);
             dc_invalidate();      

        	 command = ENDIAN_SWAP32(cmdBuffer[0]);
               
	        if (command)
	        {
	            parameter0 = ENDIAN_SWAP32(cmdBuffer[1]);
	            parameter1 = ENDIAN_SWAP32(cmdBuffer[2]);
	            parameter2 = ENDIAN_SWAP32(cmdBuffer[3]);

				switch(command)
				{
					case CMD_WIEGAND_ENABLE:
					if (parameter0)
                    {
                        start_wiegand_timer(2); /* timer 0 used by system, do not use timer 0 */
                        wg1_begin(parameter1, parameter2);
                    }
                    else
                    {
                        start_wiegand_timer(1); /* timer 0 used by system, do not use timer 0 */
                        wg0_begin(parameter1, parameter2);						
                    }
                    wiegandenable = 1;
                    break;
					default:
                    break;
				}
				cmdBuffer[0] = 0;
	        }
			MMIO_Write(0x16A4, 0x0000);			
		}
		
		if (wiegandenable)
		{			
        	wg0_DoWiegandConversion(card_id0);
        	wg1_DoWiegandConversion(card_id1);
      	}
    }
	MMIO_Write(RISC_COMMUNICATE_REG, 0x5678);
    MMIO_Write(RISC_COMMUNICATE_REG, 0x0000);
}
