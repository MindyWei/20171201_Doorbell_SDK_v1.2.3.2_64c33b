/*
 * SMedia Tech. Corp. 2008
 * default link script
 *
 * defined symbol: CODEC_START_ADDR
 *
 * Notes: it is not a standard link script, must use gcc preprocessor
 *        to generate the standard link script.
 */

#include "codecs_defs.h"

// sm32 is predefined by gcc compiler, must undefine it.
#undef  sm32

// begin of link script
ENTRY(codec_ex_start)
OUTPUT_ARCH(sm32)
GROUP(-lc -lgcc)
SEARCH_DIR(.)

MEMORY
{
    PLUGIN_RAM : ORIGIN = CODEC_START_ADDR+RISC1_SIZE, LENGTH = 120k //CODEC_EX_SIZE
}

SECTIONS
{
    .codecs_ex_header :
    {
        _risc2_start_addr = .;
        risc2_start_addr  = .;
        *(codec_crt0.o)
        KEEP(*(.codecs_ex_header))
        . = ALIGN(4);
    } > PLUGIN_RAM

    .text ALIGN(4) :
    {
        *(.text*)
    } > PLUGIN_RAM

    .data ALIGN(4) :
    {
        *(.rodata*)
        *(.data*)
        . = ALIGN(4);
    } > PLUGIN_RAM

    /DISCARD/ :
    {
        *(.eh_frame)
    }

    .bss ALIGN(4) (NOLOAD):
    {
        risc2_bss_start = .;
        _risc2_bss_start = .;
        *(.bss* .sbss*)
        *(COMMON)
        . = ALIGN(0x4);
        _risc2_bss_end = .;
        risc2_bss_end = .;
        _risc2_end_addr = .;
        risc2_end_addr = .;
    } > PLUGIN_RAM

    /* Special trick to avoid a linker error when no other sections are
       left after garbage collection (codec plugins not for this platform) */
    .comment 0 :
    {
        KEEP(*(.comment))
    }
}

