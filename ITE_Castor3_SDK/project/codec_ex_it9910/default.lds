/*
 * SMedia Tech. Corp. 2008
 * default link script
 *
 * defined symbol: CODEC_SIZE
  *
 * Notes: it is not a standard link script, must use gcc preprocessor
 *        to generate the standard link script.
 */

#include "codecs_defs.h"

// sm32 is predefined by gcc compiler, must undefine it.
#undef  sm32

// define stack size
#define STACK_SIZE              80K

// begin of link script
OUTPUT_FORMAT("elf32-sm32", "elf32-sm32", "elf32-sm32")
OUTPUT_ARCH(sm32)
_start = 0x100;
ENTRY(_start)

GROUP(-lc -lm -lgcc)

MEMORY
{
  except : ORIGIN = 0x00000000, LENGTH = 0x1000
  ram    : ORIGIN = 0x1000, LENGTH = CODEC_SIZE
}

SECTIONS
{
  .reset :
  {
    *(.reset)
  } > except

  .codecs_ex_header :
  {
    _codec_ex_start_addr = .;
    codec_ex_start_addr  = .;
    KEEP(*(.codecs_ex_header))

    // codec stack for audio codec on processor 2
    _codec_ex_stack = .;
  } > ram

  .text :
  {
    __text_start = .;
    *(.text .stub .text.* .gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    *(.gnu.warning)
    __text_end = .;
  } > ram

  .data ALIGN(0x4) :
  {
    __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    /* In 3.4.4, libgcc2.c actually required -1 here, rather than the count
       , as in ld documentation
    */
    LONG(-1)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;

    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG(-1)
    *(.dtors)
    LONG(0)
    __DTOR_END__ = .;

    KEEP (*(.gcc_except_table)) *(.gcc_except_table.*)

    *(.sdata .sdata.* .gnu.linkonce.s.*)
    *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
    *(.rodata .rodata.* .gnu.linkonce.r.*)
    *(.rodata1)
    *(.data .data.* .gnu.linkonce.d.*)
    KEEP (*(.gnu.linkonce.d.*personality*))
    . = ALIGN(32 / 8);
  } > ram

  __bss_start = .;
  _bss_start = .;
  codec_ex_bss_start = .;
  .bss ALIGN(0x4) :
  {
   *(.dynbss)
   *(.sbss .sbss.* .gnu.linkonce.b.*)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   /* Align here to ensure that the .bss section occupies space up to
      _end.  Align after .bss to ensure correct alignment even if the
      .bss section disappears because there are no input sections.  */
   . = ALIGN(32 / 8);
  } > ram
  _end = .;
  _bss_end = .;
  codec_ex_bss_end = .;
  PROVIDE (end = .);


  .stack ALIGN(4) (NOLOAD) :
    {
    __stack_start = .;
    . += STACK_SIZE;
    __stack_end = .;
    _stack      = .;
    __end       = .;

    _codec_ex_end_addr = .;
    codec_ex_end_addr  = .;
  } > ram

  /DISCARD/ : { *(.note.GNU-stack) }
}
