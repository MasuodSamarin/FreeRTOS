MEMORY
{
    code0(x)        : ORIGIN = 0x1000000,       LENGTH = 16M
    ram0            : ORIGIN = 0x0000200,       LENGTH = 64K-512
    /*0x80 offset & 12+40 for uboot used*/
    nv_ram          : ORIGIN = 0x0010000,       LENGTH = 16K
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
       . = ALIGN(4);
        *(.bss)
        *(.stack)
        *(.ustack)

        *(.mem_pool)

        BTSTACK_CONTROLLER_BSS_START = .;
        *(.bt_rf_bss)
        BTSTACK_CONTROLLER_BSS_SIZE = (. - BTSTACK_CONTROLLER_BSS_START);


        BTSTACK_LE_CONTROLLER_BSS_START = .;
		*(.device_manager_bss)
		*(.hci_controller_bss)
        *(.ble_rf_bss)
        *(.ble_ll_bss)
        *(.ble_hci_bss)
        BTSTACK_LE_CONTROLLER_BSS_SIZE = (. - BTSTACK_LE_CONTROLLER_BSS_START);
    } > ram0

    SDRAM_BEGIN_ADDR = ORIGIN(ram0);
    SRAM_END_ADDR  = . ;
    sram_end  = . - ORIGIN(ram0);

    . = ORIGIN(nv_ram);
    .nvdata :SUBALIGN(4)
    {
        . = ALIGN(4);
        NVRAM_DATA_START = .;

/*------btstack start*/
        BTSTACK_CONTROLLER_DATA_START = .;
        *(.bt_rf_data)
        BTSTACK_CONTROLLER_DATA_SIZE = (. - BTSTACK_CONTROLLER_DATA_START);

        BTSTACK_HOST_DATA_START = .;
        *(.bt_hci_data)
        *(.bt_l2cap_data)
        BTSTACK_HOST_DATA_SIZE = (. - BTSTACK_HOST_DATA_START);

        BTSTACK_LE_CONTROLLER_DATA_START = .;
		*(.device_manager_data)
		*(.hci_controller_data)
        *(.ble_rf_data)
        *(.ble_ll_data)
        *(.ble_hci_data)
        BTSTACK_LE_CONTROLLER_DATA_SIZE = (. - BTSTACK_LE_CONTROLLER_DATA_START);

        BTSTACK_LE_HOST_DATA_START = .;
        *(.ble_att_data)
        *(.ble_att_server_data)
        *(.ble_gatt_client_data)
        *(.ble_sm_data)
        BTSTACK_LE_HOST_DATA_SIZE = (. - BTSTACK_LE_HOST_DATA_START);

        BTSTACK_HOST_BSS_START = .;
        *(.bt_hci_bss)
        *(.bt_l2cap_bss)
        BTSTACK_HOST_BSS_SIZE = (. - BTSTACK_HOST_BSS_START);

        BTSTACK_LE_HOST_BSS_START = .;
        *(.ble_att_bss)
        *(.ble_att_server_bss)
        *(.ble_gatt_client_bss)
        *(.ble_sm_bss)
        BTSTACK_LE_HOST_BSS_SIZE = (. - BTSTACK_LE_HOST_BSS_START);
/*------btstack end*/

        /*ble poweroff - data segment*/
        *(.non_volatile_ram)
        *(.ble_bb_ram)
        *(.ble_memory_pool)
        NVRAM_DATA_SIZE = (. - NVRAM_DATA_START);


        NVRAM_CODE_START = .;
        *(.non_volatile_ram_code)

        /*lib dependen - code by vm/poweroff*/
        *memcpy.o(.text .rodata*)
        *memset.o(.text .rodata*)
        *memcmp.o(.text .rodata*)

        *irq_asm.o(.text .rodata*)

        /*ble poweroff - dependency*/
        *(bt_poweroff_reboot.rodata*)
        *(.uart_hw_code)
        *(uart_hw.rodata*)
        /*clean icache, after flash change*/
        *(.icache_drv)
        *(.icache_hw_code)
        *(.spi0_drv)
        *(.spi0_hw_code)

        *(.ble_code)
        NVRAM_CODE_SIZE = (. - NVRAM_CODE_START);
    } > nv_ram
    NVRAM_USED = SIZEOF(.nvdata);
    NVRAM_LIMIT = (LENGTH(nv_ram) - 20 - 64 - 128);
    ASSERT(NVRAM_USED < NVRAM_LIMIT, "NVRAM space overflow!")


    .nvram_info :SUBALIGN(4)
    {
        /* !!!Must match maskrom*/
        . = ORIGIN(nv_ram) + LENGTH(nv_ram) - 12;
        *(.nvram_info)
    } > nv_ram



    . = ORIGIN(code0) ;
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

        BTSTACK_CONTROLLER_CODE_START = .;
		*(.bt_rf_const)
		*(.bt_rf_code)
        BTSTACK_CONTROLLER_CODE_SIZE = (. - BTSTACK_CONTROLLER_CODE_START);

        BTSTACK_HOST_CODE_START = .;
		*(.bt_hci_const)
		*(.bt_hci_code)
		*(.bt_l2cap_const)
		*(.bt_l2cap_code)
        BTSTACK_HOST_CODE_SIZE = (. - BTSTACK_HOST_CODE_START);

        BTSTACK_LE_CONTROLLER_CODE_START = .;
        /* btstack - le code*/
        *(.ble)
        *(.ble_rf_const)
        *(.ble_rf_code)
        /* *(.ble_ll_probe_code) */

		*(.device_manager_const)
		*(.device_manager_code)
		*(.hci_controller_const)
		*(.hci_controller_code)
	    *(.ble_ll_const)
	    *(.ble_ll_code)
		*(.ble_hci_const)
		*(.ble_hci_code)
		*(.ble_h4_code)
        BTSTACK_LE_CONTROLLER_CODE_SIZE = (. - BTSTACK_LE_CONTROLLER_CODE_START);

        BTSTACK_LE_HOST_CODE_START = .;
		*(.ble_att_const)
		*(.ble_att_code)
		*(.ble_att_server_const)
		*(.ble_att_server_code)
		*(.ble_sm_code)
        *(.ble_sm_const)
		*(.ble_gatt_client_const)
		*(.ble_gatt_client_code)
		*(.ble_ancs_code)
		*(.ble_app_code)
        BTSTACK_LE_HOST_CODE_SIZE = (. - BTSTACK_LE_HOST_CODE_START);

        /* vm - code*/
        VM_USED_CODE_START = .;
		*(.winbond_flash_code)
		*(.vm_code)
        VM_USED_CODE_END = .;

        *(debug_init.rodata*)
        *(.irq_hw_code)
        *(.irq_drv)
        *(.lbuf_code)
        *(.sys_timer_code)

        *(.text* .rodata*)

        /*代码运行保护符号使用说明，将start&end 包含需保护的段*/
        MPU_PC_EX_START = .;
        MPU_PC_EX_END = .;
    } > code0

    text_end = . ;
    /*蓝牙协议栈代码统计*/
    BTSTACK_RAM_TOTOAL = BTSTACK_HOST_DATA_SIZE + BTSTACK_LE_CONTROLLER_DATA_SIZE + BTSTACK_LE_HOST_DATA_SIZE + BTSTACK_HOST_BSS_SIZE + BTSTACK_LE_CONTROLLER_BSS_SIZE + BTSTACK_LE_HOST_BSS_SIZE ;
    BTSTACK_CODE_TOTOAL = BTSTACK_LE_CONTROLLER_CODE_SIZE + BTSTACK_LE_HOST_CODE_SIZE + BTSTACK_HOST_CODE_SIZE;

    TEXT_USED = SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.nvdata);
    TEXT_LIMIT = LENGTH(code0);
    ASSERT(TEXT_USED  < TEXT_LIMIT, "Code space overflow!")

    data_addr  = ADDR(.data);
    data_begin = text_end;
    data_size =  SIZEOF(.data);

    data1_addr  = ADDR(.nvdata);
    data1_begin = text_end + SIZEOF(.data) ;
    data1_size  = SIZEOF(.nvdata);

    bss_begin = ADDR(.bss);
    bss_size  = SIZEOF(.bss);

}
