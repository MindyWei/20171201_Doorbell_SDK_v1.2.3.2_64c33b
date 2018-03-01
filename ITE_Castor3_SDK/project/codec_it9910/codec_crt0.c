/***************************************************************************
 * Copyright (c) 2008 SMedia Technology Corp. All Rights Reserved.
 *
 * @file
 * Codecs Startup Code
 *
 * @author Kuoping Hsu
 * @version 1.0
 *
 ***************************************************************************/
#include "codecs.h"
//#include "mmio.h"
#define AUDIO_DECODER_START_FALG  0x16ec

#if defined (CFG_CHIP_REV_A0) || defined(CFG_CHIP_PKG_IT9910)
/* For MMIO interface */
#  define MMIO_ADDR                 0xC0000000
#elif defined (CFG_CHIP_REV_A1)
  #  define MMIO_ADDR                 0xC0200000
#else
 #  define MMIO_ADDR                 0xC0200000
#endif

char __tm_info[] = "Audio Codec Plug-Ins (c) 2008 ITE Tech. Corp.";

// Put the *ci to the data section instead of bss section. Cause the
// bss section will clear in the codec_start function, it will
// reset the *ci to NULL.
struct _codec_api *ci __attribute__ ((section (".data"))) = 0;

extern unsigned char risc1_start_addr[];
extern unsigned char risc1_end_addr[];
extern unsigned char risc1_bss_start[];
extern unsigned char risc1_bss_end[];
extern unsigned char *risc1_qmem_start;
extern unsigned char *risc1_qmem_end;

int codec_start(struct _codec_api *api);
extern int codec_info();
//extern AUDIO_CODEC_STREAM* gpAudioStream;
#if 1 //defined(AUDIO_PLUGIN_MESSAGE_QUEUE)

struct _codec_header __header __attribute__ ((section (".codecs_header"))) = {
    CODEC_MAGIC, TARGET_ID, CODEC_API_VERSION,
    (unsigned char*)risc1_start_addr, (unsigned char*)risc1_end_addr,
    codec_start, 0xffffffff, codec_info
};
#else
const struct _codec_header __header __attribute__ ((section (".codecs_header"))) = {
    CODEC_MAGIC, TARGET_ID, CODEC_API_VERSION,
    (unsigned char*)risc1_start_addr, (unsigned char*)risc1_end_addr,
    codec_start, codec_info
};

#endif

void MMIO_Write(unsigned short addr, unsigned short data)
{
    *(volatile unsigned short *) (MMIO_ADDR + addr) = data;
}

unsigned int MMIO_Read(unsigned short addr)
{
    return *(volatile unsigned short *) (MMIO_ADDR + addr);
}
// startup entry
int codec_start(struct _codec_api *api)
{
    int i = 0;
    unsigned int *ptr;

    // Make sure the ci is not in .bss section. The .bss section will
    // clear later.
    ci = api;

    // Clear BSS section of CODEC. Be carefull, it will clear all of
    // global variable which un-initialized or initialize with zero's
    // variable.
    #if CFG_CHIP_FAMILY == 9910
    ptr = (unsigned int*)risc1_bss_start;
    do {
        *ptr++ = 0;
    } while((unsigned int)ptr <= (unsigned int)risc1_bss_end);
    
    /*ptr = (unsigned int*)risc1_qmem_start;
    do {
        *ptr++ = 0;
    } while((unsigned int)ptr <= (unsigned int)risc1_qmem_end);*/
    #endif

    //MMIO_Write(AUDIO_DECODER_START_FALG, 1);       

    main();

    // Never return by codec main functions.
    //taskSOFTWARE_BREAKPOINT();
    while(1) ;

    return 0;
}


// dummy __main function, do not remove it.
void __main(void) { /* dummy */ }
void free(void) { /* dummy */ }
void _exit(void) { while(1); }

