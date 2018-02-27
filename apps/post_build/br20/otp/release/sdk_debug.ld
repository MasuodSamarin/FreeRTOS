
MEMORY
{
    mask_ram0       : ORIGIN = 0x0000200,           LENGTH = 0x200
    ram0            : ORIGIN = 0x0000400,           LENGTH = 80K-30K-0x400
    overlay_ram0    : ORIGIN = 50K,                 LENGTH = 30K
	rom_code(x)     : ORIGIN = 0x0022800,           LENGTH = 128K-10K
    code0(x)        : ORIGIN = CODE_START_ADDR,     LENGTH = 64K
}

ENTRY(_start)


SECTIONS
{
  . = ORIGIN(ram0);
  .bss (NOLOAD) :SUBALIGN(4)
  {
      *(.bss)
      *(.ram1_bss)
      *(.stack)

		bredr_base_addr = .;
      *(.bt_classic_base)
      *(.bt_sco_codec_data)
      *(.bt_classic_data)
      *(.bt_stack)

      *(.dac_buf_sec)

		SYS_BSS_START = .;
        *(.system_bss)
		SYS_BSS_SIZE = (. - SYS_BSS_START);

		TIMER_BSS_START = .;
        *(.timer_app_bss)
		TIMER_BSS_SIZE = (. - TIMER_BSS_START);

		BT_USER_BSS_START = .;
        *(.bt_stack_bss)
        *(.bt_user_bss)
        *(.bt_app_bss)
		BT_USER_BSS_SIZE = (. - BT_USER_BSS_START);

		IRQ_BSS_START = .;
        *(.irq_api_bss)
		IRQ_BSS_SIZE = (. - IRQ_BSS_START);

		CLOCK_BSS_START = .;
        *(.clk_driver_bss)
        *(.clk_api_bss)
        *(.clk_app_bss)
		CLOCK_BSS_SIZE = (. - CLOCK_BSS_START);

		RTC_BSS_START = .;
        *(.rtc_driver_bss)
        *(.rtc_api_bss)
        *(.rtc_app_bss)
		RTC_BSS_SIZE = (. - RTC_BSS_START);

		AEC_BSS_START = .;
        *(.aec_app_bss)
		AEC_BSS_SIZE = (. - AEC_BSS_START);

		AUDIO_BSS_START = .;
        *(.dac_driver_bss)
        *(.dac_api_bss)
        *(.dac_app_bss)

        *(.ladc_driver_bss)
        *(.ladc_api_bss)
        *(.ladc_app_bss)

        *(.aux_driver_bss)
        *(.aux_api_bss)
        *(.aux_app_bss)

        *(.tone_app_bss)

        *(.eq_app_bss)
		AUDIO_BSS_SIZE = (. - AUDIO_BSS_START);

		POWER_BSS_START = .;
        *(.power_driver_bss)
		POWER_BSS_SIZE = (. - POWER_BSS_START);

		DECODE_BSS_START = .;
        *(.a2dp_decode_bss)
        *(.bt_tone_bss)
        *(.music_decoder_bss)
        *(.music_decoder_config_bss)
		DECOdE_BSS_SIZE = (. - TIMER_BSS_START);

		APP_DEAL_BSS_START = .;
        *(.task_bt_bss)
        *(.task_common_bss)
        *(.key_bss)
        *(.key_drv_io_bss)
		APP_DEAL_BSS_SIZE = (. - TIMER_BSS_START);

  } > ram0

  .data :SUBALIGN(4)
  {
       . = ALIGN(4);
      *(.data*)

      *(.mask_call*)
	   bredr_base_updata_ram1 = .;

	   _bredr_nv_mem_begin = .;
       *(.ram1_data)
	   _bredr_nv_mem_end = .;

	   _ble_nv_mem_begin = .;
       *(.bt_nv_data)
	   _ble_nv_mem_end = .;

      *(.eq_buf_sec)
      *(.sd_buf)
      *(.usb_buf)

	  *(.common)

      *memcpy.o(.text .rodata*)
      *memset.o(.text .rodata*)
      *icache_hw.o(.text)
	   _system_cfg_begin = .;
      *(.system_cfg)
      _system_cfg_end = .;

		SYS_DATA_START = .;
        *(.system_data)
		SYS_DATA_SIZE = (. - SYS_DATA_START);

		TIMER_DATA_START = .;
        *(.timer_app_data)
		TIMER_DATA_SIZE = (. - TIMER_DATA_START);

		BT_USER_DATA_START = .;
        *(.bt_stack_data)
        *(.bt_cpu_data)
        *(.bt_app_data)
		BT_USER_DATA_SIZE = (. - BT_USER_DATA_START);

		IRQ_DATA_START = .;
        *(.irq_api_data)
		IRQ_DATA_SIZE = (. - IRQ_DATA_START);

		CLOCK_DATA_START = .;
        *(.clk_driver_data)
        *(.clk_api_data)
        *(.clk_app_data)
		CLOCK_DATA_SIZE = (. - CLOCK_DATA_START);

		RTC_DATA_START = .;
        *(.rtc_driver_data)
        *(.rtc_api_data)
        *(.rtc_app_data)
		RTC_DATA_SIZE = (. - RTC_DATA_START);

		AEC_DATA_START = .;
        *(.aec_app_data)
		AEC_DATA_SIZE = (. - AEC_DATA_START);

		AUDIO_DATA_START = .;
        *(.dac_driver_data)
        *(.dac_api_data)
        *(.dac_app_data)

        *(.ladc_driver_data)
        *(.ladc_api_data)
        *(.ladc_app_data)

        *(.aux_driver_data)
        *(.aux_api_data)
        *(.aux_app_data)

        *(.tone_app_data)

        *(.eq_app_data)
		AUDIO_DATA_SIZE = (. - AUDIO_DATA_START);

		POWER_DATA_START = .;
        *(.power_driver_data)
		POWER_DATA_SIZE = (. - POWER_DATA_START);

		DECODE_DATA_START = .;
        *(.a2dp_decode_data)
        *(.bt_tone_data)
        *(.music_decoder_data)
        *(.music_decoder_config_data)
		DECODE_DATA_SIZE = (. - DECODE_DATA_START);

		APP_DEAL_DATA_START = .;
        *(.task_bt_data)
        *(.task_common_data)
        *(.key_drv_io_data)
        *(.key_data)
		APP_DEAL_DATA_SIZE = (. - APP_DEAL_DATA_START);

  } > ram0

  RAM_BEGIN_ADDR = ORIGIN(ram0);
  RAM_END_ADDR  = . ;
  ram_end  = . - ORIGIN(ram0);

  OVERLAY :SUBALIGN(4)
  {
	 .overlay1
	  {
	      *(.bt_sbc)
	      *(.bt_sbc_dec)
	      *(.music_sbc_dec)
	  }
	.overlay2
	  {
	      *(.bt_aec)
	  }
	.overlay3
	  {
	      *(.music_mem)
	      *(.fs_fat)
	      *(.fs_syd)
	      *(.music_mp3_dec)
	  }
  }>overlay_ram0


  .rom_code_seg (NOLOAD) :
  {
	  INCLUDE "download/br20/post_build/libs_stubs.ld";
  } > rom_code

  . = ORIGIN(code0) ;
  .text  ALIGN(4):
  {
	  *startup.o(.text)
	  *(.powerdown_text)
      *(.const_tab)

	  *(.ver_magic)
	  *(.version)

      *(.text* .rodata*)
         /*  -----蓝牙的链表接口定义一定要确保对齐,其它代码段不要在中间定义-----  */
         /*begin*/
        . = ALIGN(4);
	lmp_hci_handler_begin = .;
		*(.lmp_hci_handler)
	lmp_hci_handler_end = .;

        a2dp_sink_media_probe_begin = .;
		*(.a2dp_sink_media_probe)
	a2dp_sink_media_probe_end = .;

	a2dp_sink_media_codec_begin = .;
		*(.a2dp_sink_media_codec)
	a2dp_sink_media_codec_end = .;

	a2dp_source_media_codec_begin = .;
		*(.a2dp_source_media_codec)
	a2dp_source_media_codec_end = .;

	a2dp_event_handler_begin = .;
		KEEP(*(.a2dp_event_handler))
	a2dp_event_handler_end = .;

        clock_switcher_begin = . ;
        *(.clock_switch.fun)
        clock_switcher_end = . ;

        bt_sleep_begin = .;
          *(.bt_sleep)
        bt_sleep_end = .;

        /*  -----end-----  */
        *(.ble)
		*(.bredr)
		*(.bt_power)
        *(.poweroff_flash)

        ble_text_begin = . ;
        *(.ble_irq_text)
        ble_text_end = . ;

        bredr_text_begin = . ;
        *(.bredr_irq_text)
        bredr_text_end = . ;

        *(.sys_cfg)

        poweroff_text_begin = . ;
        *(.poweroff_text)
        poweroff_text_end = . ;

		SYS_CODE_START = .;
        *(.system_const)
        *(.system_code)
		SYS_CODE_SIZE = (. - SYS_CODE_START);

		TIMER_CODE_START = .;
        *(.timer_app_const)
        *(.timer_app_code)
		TIMER_CODE_SIZE = (. - TIMER_CODE_START);

		IRQ_CODE_START = .;
        *(.irq_api_const)
        *(.irq_api_code)
		IRQ_CODE_SIZE = (. - IRQ_CODE_START);

		BT_USER_CODE_START = .;
      	*(.bt_stack_const)
        *(.bt_stack_code)

      	*(.bt_cpu_const)
        *(.bt_cpu_code)

      	*(.bt_app_const)
        *(.bt_app_code)
		BT_USER_CODE_SIZE = (. - BT_USER_CODE_START);

		CLOCK_CODE_START = .;
        *(.clk_driver_const)
        *(.clk_driver_code)

        *(.clk_api_const)
        *(.clk_api_code)

        *(.clk_app_const)
        *(.clk_app_code)
		CLOCK_CODE_SIZE = (. - CLOCK_CODE_START);

		RTC_CODE_START = .;
        *(.rtc_driver_const)
        *(.rtc_driver_code)

        *(.rtc_api_const)
        *(.rtc_api_code)

        *(.rtc_app_const)
        *(.rtc_app_code)
		RTC_CODE_SIZE = (. - RTC_CODE_START);

		AEC_CODE_START = .;
        *(.aec_app_const)
        *(.aec_app_code)
		AEC_CODE_SIZE = (. - AEC_CODE_START);

		AUDIO_CODE_START = .;
        *(.dac_driver_code)
        *(.dac_driver_const)
        *(.dac_api_code)
        *(.dac_api_const)
        *(.dac_app_code)
        *(.dac_app_const)

        *(.ladc_driver_code)
        *(.ladc_driver_const)
        *(.ladc_api_code)
        *(.ladc_api_const)
        *(.ladc_app_code)
        *(.ladc_app_const)

        *(.aux_driver_code)
        *(.aux_driver_const)
        *(.aux_api_code)
        *(.aux_api_const)
        *(.aux_app_code)
        *(.aux_app_const)

        *(.tone_app_code)
        *(.tone_app_const)

        *(.eq_app_code)
        *(.eq_app_const)
		AUDIO_CODE_SIZE = (. - AUDIO_CODE_START);

		POWER_CODE_START = .;
        *(.power_driver_const)
        *(.power_driver_code)
		POWER_CODE_SIZE = (. - POWER_CODE_START);

		DECODE_CODE_START = .;
        *(.a2dp_decode_const)
        *(.a2dp_decode_code)
		*(.bt_tone_const)
        *(.bt_tone_code)
		*(.music_decoder_const)
        *(.music_decoder_code)
        *(.music_decoder_config_const)
        *(.music_decoder_config_code)
		DECODE_CODE_SIZE = (. - DECODE_CODE_START);

		APP_DEAL_CODE_START = .;
        *(.task_bt_const)
        *(.task_bt_code)
        *(.task_common_const)
        *(.task_common_code)
		*(.key_drv_io_const)
        *(.key_drv_io_code)
		*(.key_const)
        *(.key_code)
		APP_DEAL_CODE_SIZE = (. - APP_DEAL_CODE_START);
  } > code0

  text_end = . ;

  /*bt config zone*/
  . = ORIGIN(code0)+LENGTH(code0)-512;
  .setup ALIGN(4):
  {
	  *(.bt_setup_sec)
  }

  .version : {
	  *(.ver_magic_start)
	  *(.version)
	  *(.ver_magic_end)
  } > code0

  /* see OTP info.xls */
  config_info_begin = ORIGIN(code0) + LENGTH(code0) - 512;
  config_info_size  = 512 - 32;

  text_size = SIZEOF(.text);

  data_addr  = ADDR(.data) ;
  data_begin = text_end ;
  data_size =  SIZEOF(.data) ;

  bss_begin = 0x200;
  bss_size  = SIZEOF(.bss) + 0x200;

  APP_CODE_SIZE = SYS_CODE_SIZE + TIMER_CODE_SIZE + IRQ_CODE_SIZE + BT_USER_CODE_SIZE + CLOCK_CODE_SIZE + RTC_CODE_SIZE + AUDIO_CODE_SIZE + POWER_CODE_SIZE + DECODE_CODE_SIZE + APP_DEAL_CODE_SIZE + AEC_CODE_SIZE - CODE_START_ADDR - CODE_START_ADDR;

  ASSERT((text_size + data_size) < 64K, "OTP over flow!")
}

