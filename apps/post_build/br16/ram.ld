MEMORY
{
    flash(x)        : ORIGIN = 0x0001000000, LENGTH = 16M
    ram0            : ORIGIN = 0x300000, LENGTH = 104k
    ram1            : ORIGIN = 0x340000, LENGTH = 16k-16
}

ENTRY(_start)

SECTIONS
{
  . = ORIGIN(ram0);
  .data :SUBALIGN(4)
  {
      *(.data*)
      *memcpy.o(.text .rodata*)
      *memset.o(.text .rodata*)
      *(.flushinv_icache)

	  *(.common)

  } > ram0

  .bss :SUBALIGN(4)
  {
      *(.stack)
      *(.bss)
  } > ram0

  SDRAM_BEGIN_ADDR = ORIGIN( ram0);
  SRAM_END_ADDR  = . ;
  sram_end  = . - ORIGIN(ram0);

   . = ORIGIN(ram1);
  .data1 :SUBALIGN(4)
  {
       . = ALIGN(4);

       *(.ram1_data)
       *(.ram1_text)

  } > ram1



  . = ORIGIN(flash) ;
  .text  ALIGN(4):
  {
	  *startup.o(.text)

       /* Align 4 otherwise will fill 0 not the same addr with the segament*/
		. = (. +3) & ~ 3;
		_sys_initcall_begin = .;
		   *(.sys.initcall)
		_sys_initcall_end = .;

		. = (. +3) & ~ 3;
		_early_initcall_begin = .;
		   *(.early.initcall)
		_early_initcall_end = .;

		. = (. +3) & ~ 3;
		_initcall_begin = .;
		   *(.initcall)
		_initcall_end = .;

		. = (. +3) & ~ 3;
		_late_initcall_begin = .;
		   *(.late.initcall)
		_late_initcall_end = .;

		. = (. +3) & ~ 3;
		_app_initcall_begin = .;
		   *(.app.initcall)
		_app_initcall_end = .;

		. = (. +3) & ~ 3;
		_app_handler_begin = .;
		   *(.app.handler)
		_app_handler_end = .;

      *(.text* .rodata*)

  } > flash

  text_end = . ;

  ram1_addr  = 0x340000;
  ram1_begin = text_end + SIZEOF(.data) ;
  ram1_size  = 0x4000-32;


  data_addr  = ADDR(.data) ;
  data_begin = text_end ;
  data1_size =  SIZEOF(.data) ;


  bss_begin = ADDR(.bss ) ;
  bss_size  = SIZEOF(.bss);




}
