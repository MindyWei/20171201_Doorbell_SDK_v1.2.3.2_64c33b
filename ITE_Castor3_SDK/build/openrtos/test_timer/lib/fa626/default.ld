OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
INPUT(crti.o crtbegin.o crtend.o crtn.o)
GROUP(-lopenrtos -litp_boot -lith -lith_platform -lboot -lmalloc -lc -lm -lgcc)
MEMORY {
  RAM (rwx) : ORIGIN = 0x00000000, LENGTH = 0x4000000 - (((1600 * 480)+63) & 0xffffffc0) * 2 - (((1600 * 480)+63) & 0xffffffc0) - 0 - (((0)+63) & 0xffffffc0)
}
SECTIONS
{
  .reset : {
    *startup.o (.text)
    . = ALIGN(0x4);
  } >RAM
  .bootimage_func :
  {
    . = ALIGN (4);
    __bootimage_func_start = .;
    KEEP(*(.bootimage_func))
    . = ALIGN (4);
    __bootimage_func_end = .;
  } >RAM
  .power_sleep_func :
  {
    . = ALIGN (4);
    __power_sleep_func_start = .;
    KEEP(*(.power_sleep_func))
    . = ALIGN (4);
    __power_sleep_func_end = .;
  } >RAM
  .power_suspend_func :
  {
    . = ALIGN (4);
    __power_suspend_func_start = .;
    KEEP(*(.power_suspend_func))
    . = ALIGN (4);
    __power_suspend_func_end = .;
  } >RAM
  .interp : { *(.interp) }
  .note.gnu.build-id : { *(.note.gnu.build-id) }
  .hash : { *(.hash) }
  .gnu.hash : { *(.gnu.hash) }
  .dynsym : { *(.dynsym) }
  .dynstr : { *(.dynstr) }
  .gnu.version : { *(.gnu.version) }
  .gnu.version_d : { *(.gnu.version_d) }
  .gnu.version_r : { *(.gnu.version_r) }
  .rel.init : { *(.rel.init) }
  .rela.init : { *(.rela.init) }
  .rel.text : { *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*) }
  .rela.text : { *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*) }
  .rel.fini : { *(.rel.fini) }
  .rela.fini : { *(.rela.fini) }
  .rel.rodata : { *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*) }
  .rela.rodata : { *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*) }
  .rel.data.rel.ro : { *(.rel.data.rel.ro .rel.data.rel.ro.* .rel.gnu.linkonce.d.rel.ro.*) }
  .rela.data.rel.ro : { *(.rela.data.rel.ro .rela.data.rel.ro.* .rela.gnu.linkonce.d.rel.ro.*) }
  .rel.data : { *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*) }
  .rela.data : { *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*) }
  .rel.tdata : { *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*) }
  .rela.tdata : { *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*) }
  .rel.tbss : { *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*) }
  .rela.tbss : { *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*) }
  .rel.ctors : { *(.rel.ctors) }
  .rela.ctors : { *(.rela.ctors) }
  .rel.dtors : { *(.rel.dtors) }
  .rela.dtors : { *(.rela.dtors) }
  .rel.got : { *(.rel.got) }
  .rela.got : { *(.rela.got) }
  .rel.bss : { *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*) }
  .rela.bss : { *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*) }
  .rel.iplt :
    {
      PROVIDE_HIDDEN (__rel_iplt_start = .);
      *(.rel.iplt)
      PROVIDE_HIDDEN (__rel_iplt_end = .);
    }
  .rela.iplt :
    {
      PROVIDE_HIDDEN (__rela_iplt_start = .);
      *(.rela.iplt)
      PROVIDE_HIDDEN (__rela_iplt_end = .);
    }
  .rel.plt :
    {
      *(.rel.plt)
    }
  .rela.plt :
    {
      *(.rela.plt)
    }
  .init :
  {
    KEEP (*(SORT_NONE(.init)))
  } >RAM
  .plt : { *(.plt) } >RAM
  .iplt : { *(.iplt) } >RAM
  .text :
  {
    _codec_end_addr = .;
    codec_end_addr = .;
    _text_start = .;
    *(.text.unlikely .text.*_unlikely)
    *(.text.exit .text.exit.*)
    *(.text.startup .text.startup.*)
    *(.text.hot .text.hot.*)
    basic_op2.o(.text)
    cfft_64.o(.text)
    fdsr.o(.text)
    HowlingCtrl.o(.text)
    argb2yuv.o(.text)
    *(.text .stub .text.* .gnu.linkonce.t.*)
    *(.gnu.warning)
    *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
    _text_end = .;
  } >RAM
  .fini :
  {
    KEEP (*(.fini))
  } >RAM
  PROVIDE (__etext = .);
  PROVIDE (_etext = .);
  PROVIDE (etext = .);
  .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) } >RAM
  .rodata1 : { *(.rodata1) } >RAM
  .ARM.extab : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >RAM
   PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } >RAM
   PROVIDE_HIDDEN (__exidx_end = .);
  .eh_frame_hdr : { *(.eh_frame_hdr) } >RAM
  .eh_frame : ONLY_IF_RO { KEEP (*(.eh_frame)) } >RAM
  .gcc_except_table : ONLY_IF_RO { *(.gcc_except_table .gcc_except_table.*) } >RAM
  .exception_ranges : ONLY_IF_RO { *(.exception_ranges .exception_ranges*) } >RAM
  . = ALIGN(CONSTANT (MAXPAGESIZE)) + (. & (CONSTANT (MAXPAGESIZE) - 1));
  .eh_frame : ONLY_IF_RW { KEEP (*(.eh_frame)) } >RAM
  .gcc_except_table : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) } >RAM
  .exception_ranges : ONLY_IF_RW { *(.exception_ranges .exception_ranges*) } >RAM
  .tdata : { *(.tdata .tdata.* .gnu.linkonce.td.*) } >RAM
  .tbss : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) } >RAM
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >RAM
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >RAM
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >RAM
  .ctors :
  {
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >RAM
  .dtors :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >RAM
  .jcr : { KEEP (*(.jcr)) }
  .data.rel.ro : { *(.data.rel.ro.local* .gnu.linkonce.d.rel.ro.local.*) *(.data.rel.ro .data.rel.ro.* .gnu.linkonce.d.rel.ro.*) } >RAM
  .dynamic : { *(.dynamic) } >RAM
  .got : { *(.got.plt) *(.igot.plt) *(.got) *(.igot) } >RAM
  .data :
  {
    __data_start = . ;
    *(.data .data.* .gnu.linkonce.d.*)
    HowlingCtrl.o(.data)
    SORT(CONSTRUCTORS)
  } >RAM
  .data1 : { *(.data1) } >RAM
  _edata = .; PROVIDE (edata = .);
  .tlb : {
    . = ALIGN ((0x4000));
    _tlb_base = .;
    KEEP(*(.tlb*));
  } >RAM
  .bss : {
   __bss_start = .;
   __bss_start__ = .;
   *(.dynbss)
   *(.bss .bss.* .gnu.linkonce.b.*)
   *(COMMON)
   . = ALIGN(. != 0 ? 32 / 8 : 1);
   . = ALIGN(8);
   _bss_end__ = . ;
   __bss_end__ = . ;
   __end__ = . ;
   _end = .; PROVIDE (end = .);
  } >RAM
  .mmap : {
    . = ALIGN(8);
    __mmap_start__ = . ;
    *(.mmap)
    . += (((0)+63) & 0xffffffc0);
    __mmap_end__ = .;
  } >RAM
  .stack : {
    . = ALIGN(8);
    __stack_start__ = . ;
    . += (((4096)+63) & 0xffffffc0);
    __irq_stack_top__ = . ;
    . += (((4096)+63) & 0xffffffc0);
    __fiq_stack_top__ = . ;
    . += (((4096)+63) & 0xffffffc0);
    __svc_stack_top__ = . ;
    . += (((512)+63) & 0xffffffc0);
    __abt_stack_top__ = . ;
    . += (((512)+63) & 0xffffffc0);
    __und_stack_top__ = . ;
    . += (((4096)+63) & 0xffffffc0);
    __c_stack_top__ = . ;
    __stack_end__ = .;
  } >RAM
  .reserved : {
    . = ALIGN(8);
    __suspend_regs_base = .;
    . += (((4 * 4)+63) & 0xffffffc0);
    __printbuf_base = .;
    . += (((0)+63) & 0xffffffc0);
    __cmdq_base = .;
    . += (((0)+63) & 0xffffffc0);
  } >RAM
  .heap : {
    . = ALIGN(8);
    __heap_start__ = . ;
    *(.heap)
  } >RAM
  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  .debug 0 : { *(.debug) }
  .line 0 : { *(.line) }
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_sfnames 0 : { *(.debug_sfnames) }
  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info 0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev 0 : { *(.debug_abbrev) }
  .debug_line 0 : { *(.debug_line) }
  .debug_frame 0 : { *(.debug_frame) }
  .debug_str 0 : { *(.debug_str) }
  .debug_loc 0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }
  .debug_weaknames 0 : { *(.debug_weaknames) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames 0 : { *(.debug_varnames) }
  .debug_pubtypes 0 : { *(.debug_pubtypes) }
  .debug_ranges 0 : { *(.debug_ranges) }
  .debug_macro 0 : { *(.debug_macro) }
  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
  .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
  /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
  __heap_end__ = 0x4000000 - (((0)+63) & 0xffffffc0) - (((1600 * 480)+63) & 0xffffffc0) * 2 - (((1600 * 480)+63) & 0xffffffc0) - 0;
  __lcd_base_a = 0x4000000 - (((0)+63) & 0xffffffc0) - (((1600 * 480)+63) & 0xffffffc0) * 2 - (((1600 * 480)+63) & 0xffffffc0) - 0;
  __lcd_base_b = 0x4000000 - (((0)+63) & 0xffffffc0) - (((1600 * 480)+63) & 0xffffffc0) - (((1600 * 480)+63) & 0xffffffc0) - 0;
  __lcd_base_c = 0x4000000 - (((0)+63) & 0xffffffc0) - (((1600 * 480)+63) & 0xffffffc0) - 0;
  __wt_base = 0x4000000 - (((0)+63) & 0xffffffc0) - 0;
  __mem_end = 0x4000000;
}
