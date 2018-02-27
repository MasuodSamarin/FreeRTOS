MEMORY
{
    flash(x)        : ORIGIN = 0x0001000000,    LENGTH = 16M
    ram0            : ORIGIN = 0x300000,        LENGTH = 104k
    /*0x80 offset & 12+40 for uboot used*/
    nvram           : ORIGIN = 0x340080,        LENGTH = 24k-12-40
}

ENTRY(_start)

SECTIONS
{
  . = ORIGIN(ram0);
  .data :SUBALIGN(4)
  {
      *(.data*)
      *(.flushinv_icache)

	  *(.volatile_ram)
	  *(.volatile_ram_code)

      /*内存保护符号使用说明，将start&end 包含需保护的段*/
      mpu_prp_start = .;
      mpu_prp_end = .;
      mpu_pc_start = .;
      mpu_pc_end = .;

  } > ram0

  .bss :SUBALIGN(4)
  {
      *(.stack)
      *(.ustack)
       . = ALIGN(4);

       /*ble baseband - data segment*/

      *(.bss)
  } > ram0

  SDRAM_BEGIN_ADDR = ORIGIN(ram0);
  SRAM_END_ADDR  = . ;
  sram_end  = . - ORIGIN(ram0);

   . = ORIGIN(nvram);
  .data1 :SUBALIGN(4)
  {
       . = ALIGN(4);

        *(.ram1_data)
        *(.ram1_text)

        /*ble poweroff - data segment*/
	    *(.non_volatile_ram)
	    *(.ble_bb_ram)
	    *(.ble_memory_pool)
        *(.ble_rf_ram)

        nvram_data_size = .;

        /*ble poweroff - code segment*/
        *(.bt_rf_code)

        /*ble poweroff - dependency*/
	    *(.non_volatile_ram_code)
	    *(.uart_hw_code)
	    *(.irq_hw_code)
	    *(.irq_drv)
	    *(.lbuf_code)
	    *(.sys_timer_code)

        /*lib dependen - code by vm/poweroff*/
        *memcpy.o(.text .rodata*)
        *memset.o(.text .rodata*)

        *irq_asm.o(.text .rodata*)
        *(uart_hw.rodata*)
        *(RF_ble.rodata*)
        *(bt_poweroff_reboot.rodata*)
        *(debug_init.rodata*)

        /*clean icache, after flash change*/
        *(.icache_drv)
        *(.icache_hw_code)
        *(.spi0_drv)
        *(.spi0_hw_code)

        nvram_code_size = (. - nvram_data_size);

  } > nvram



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

        /* user - app code*/
		. = (. +3) & ~ 3;
		_app_initcall_begin = .;
		   *(.app.initcall)
		_app_initcall_end = .;

		. = ALIGN(4);
        /* btstack - common code*/
	    *(.bt_memory_code)

        *(.power)
        btstack_le_code_start = .;
        /* btstack - le code*/
        *(.ble)
        *(.ble_rf_code)
        *(.ble_ll_probe_code)

	    *(.ble_ll_code)
		*(.ble_h4_code)
		*(.ble_hci_code)
		*(.ble_l2cap_code)
		*(.ble_gap_code)
		*(.ble_att_code)
		*(.ble_sm_code)
		*(.ble_gatt_code)
		*(.ble_ancs_code)
		*(.ble_app_code)
        btstack_le_code_size = (. - btstack_le_code_start);


        /* vm - code*/
        vm_used_code_start = .;
		*(.winbond_flash_code)
		*(.vm_code)
        vm_used_code_end = .;

        *(.text* .rodata*)

        /*代码运行保护符号使用说明，将start&end 包含需保护的段*/
        mpu_pc_ex_start = .;
        mpu_pc_ex_end = .;
  } > flash

  text_end = . ;


  data_addr  = ADDR(.data) ;
  data_begin = text_end ;
  data_size =  SIZEOF(.data) ;

  data1_addr  = ADDR(.data1);
  data1_begin = text_end + SIZEOF(.data) ;
  data1_size  = SIZEOF(.data1);

  bss_begin = ADDR(.bss) ;
  bss_size  = SIZEOF(.bss);

}
