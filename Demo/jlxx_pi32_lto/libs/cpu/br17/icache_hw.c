/*********************************************************************************************
    *   Filename        : icache_hw.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-18 10:36

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "stdarg.h"
#include "icache_interface.h"
#include "spi_interface.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"

#pragma str_literal_override("bt_poweroff_reboot.rodata")

/* struct icache_hdl{ */
/* u32 sfr; */
/* }; */

/* struct icache_hdl hdl; */

/* #define __this      (&hdl) */


#if 0
static void __hw_flush(u8 *p, u32 len)
{
    while (len > 0) {
        asm volatile("csync ; flushinv  [%0] ; csync ;":: "p"(p)) ;
        len -= CACHE_LINE_COUNT ;
        p += CACHE_LINE_COUNT;
    }
}
#else

AT_ICACHE_HW_CODE
static void __hw_flush(void *ptr, u32 len)
{
    //CPU_SR_ALLOC();
    //CPU_INT_DIS();
    JL_DSP->CON &= ~BIT(8);

    u32 *t = (u32 *)TAG_RAM_START_ADDR;
    u8 i, j;
    u32 valid_line_addr_in_cache;

    u32 ptr_align = ((u32)ptr & ~(CACHE_LINE_COUNT - 1));
    u32 t_len = (u32)(ptr - ptr_align);

    if (len > CACHE_LINE_COUNT - t_len) {
        if (t_len) {
            len += t_len;
        }
    }
    len = (len + CACHE_LINE_COUNT - 1) & ~(CACHE_LINE_COUNT - 1);

//    putbyte('#');put_u32hex0(ptr);put_u32hex0(ptr_align);put_u32hex0(len);

    for (j = 0; j < 128; j++) {
        for (i = 0; i < 4; i++) {
            valid_line_addr_in_cache = (*t) * 4096 + j * 0x20;

            if ((valid_line_addr_in_cache <= ptr_align + len) && (valid_line_addr_in_cache + CACHE_LINE_COUNT >= ptr_align)) {
//                putbyte('^');put_u32hex0(valid_line_addr_in_cache);
                *t = 0;
            }
            t += 1;
        }
    }

    JL_DSP->CON |= BIT(8);
    //CPU_INT_EN();
}

#endif //_CACHE_C_

AT_ICACHE_HW_CODE
static void __hw_suspend(void)
{
    /* __this->sfr = JL_SFC->CON; */

    while (!(JL_DSP->CON & BIT(5)));
    JL_SFC->CON &= ~BIT(0);
}

AT_ICACHE_HW_CODE
static void __hw_resume(void)
{
    /* JL_SFC->CON = __this->sfr; */
    JL_SFC->CON |= BIT(0);
}

AT_ICACHE_HW_CODE
static void __hw_set_spi_cache_disable(void)
{
    while (!(JL_DSP->CON & BIT(5)));
    JL_DSP->CON &= ~BIT(8);
}

AT_ICACHE_HW_CODE
static void __hw_set_spi_cache_enable(void)
{
    JL_DSP->CON |= BIT(8);
}

AT_ICACHE_HW_CODE
static void __hw_sfc_io_init(SPI_PORT sfc_port)
{
    if (sfc_port == SPI0_PORT_TEAM_B) {
        log_info("SFC_PORT_TEAM_B\n");
        JL_IOMAP->CON1 |= BIT(5);
    } else {
        log_info("SFC_PORT_TEAM_A\n");
        JL_IOMAP->CON1 &= ~BIT(5);
    }
}

AT_ICACHE_HW_CODE
static s32 __hw_sfc_init(SPI_MODE wire_mode, \
                         FLASH_MODE flash_mode, \
                         SPI_PORT sfc_port, \
                         u32 run_mode, \
                         u8 sfc_baud)
{
    __spi0_drv->suspend(); //suspend spi0 before set sfc

    JL_SFC->BAUD = sfc_baud;
    JL_SFC->CON = 0xf << 20;
    JL_SFC->CON = 0;

    if (wire_mode == SPI_QUAD_MODE) { ///4线模式
        log_info("SPI_QUAD_MODE:\n");

        if (flash_mode == FAST_READ_OUTPUT_MODE) { //quad fast read output(CMD 1WIRE, ADDR 1WIRE)
            JL_SFC->CON =  0x81035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(8) | BIT(9) ;
            log_info("FAST_READ_OUTPUT_MODE:\n");
        } else if (flash_mode == FAST_READ_IO_MODE) { //quad fast read i/o(CMD 1WIRE, ADDR 4WIRE)
            JL_SFC->CON =  0x61035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(8) | BIT(10);
            log_info("FAST_READ_IO_MODE:\n");
        } else if (flash_mode == FAST_READ_IO_CONTINUOUS_READ_MODE) { //no send command quad mode(ADDR 4WIRE)
            JL_SFC->CON =  0x61035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(8) | BIT(9) | BIT(10);
            log_info("FAST_READ_IO_CONTINUOUS_READ_MODE:\n");
        }
    } else if (wire_mode == SPI_DUAL_MODE) { ///2线模式
        log_info("SPI_DUAL_MODE:\n");

        if (flash_mode == FAST_READ_OUTPUT_MODE) { //dual fast read output(CMD 1WIRE, ADDR 1WIRE)
            JL_SFC->CON =  0x81035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(9);
            log_info("FAST_READ_OUTPUT_MODE:\n");
        } else if (flash_mode == FAST_READ_IO_MODE) { //dual fast read i/o(CMD 1WIRE, ADDR 4WIRE)
            JL_SFC->CON =  0x41035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(10);
            log_info("FAST_READ_IO_MODE:\n");
        } else if (flash_mode == FAST_READ_IO_CONTINUOUS_READ_MODE) { //no send command dual mode(ADDR 2WIRE)
            JL_SFC->CON =  0x41035;
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(10) | BIT(9);
            log_info("FAST_READ_IO_CONTINUOUS_READ_MODE:\n");
        }
    } else if (wire_mode == SPI_ODD_MODE) { ///1线模式
        log_info("SPI_ODD_MODE:\n");

        if (flash_mode == FAST_READ_IO_CONTINUOUS_READ_MODE) { //fast read
            JL_SFC->CON =  0x8103D; //dummy值为8
            JL_SFC->CON |= BIT(7);
            JL_SFC->CON |= BIT(8);
            log_info("FAST_READ_IO_CONTINUOUS_READ_MODE:\n");
        } else { //normal read
            JL_SFC->CON =  0x0103D; //dummy值为0
            JL_SFC->CON |= BIT(7);
            log_info("normal mode\n");
        }
    }

    __hw_sfc_io_init(sfc_port);

    u32 spi2_delay_arg = run_mode >> (9 + 4);
    spi2_delay_arg &= 3;
    log_info("spi2_input_delay:%d\n", spi2_delay_arg);
    JL_CLOCK->CLK_CON2 &=  ~(BIT(29) | BIT(28));
    JL_CLOCK->CLK_CON2 |= spi2_delay_arg << 28; //spi2 delay arg

    u32 spi2_dummy = run_mode >> (9);
    spi2_dummy &= 0xf;
    log_info("spi2_dummy:%d,%d\n", spi2_dummy, (JL_SFC->CON >> 16) & 0xf);

    u32 spi2_cscnt = run_mode >> (5);
    spi2_cscnt &= 0xf;
    log_info("spi2_cscnt:%d\n", spi2_cscnt);

    JL_SFC->CON &= ~(0xf << 20);
    JL_SFC->CON |= spi2_cscnt << 20;

    if (run_mode & BIT(9 + 4 + 2)) {
        log_info("csmode1\n");
        //JL_SFC->CON |= BIT(26);
    } else {
        log_info("csmode0\n");
        JL_SFC->CON &= ~BIT(26);
    }

    return 0;
}

AT_ICACHE_HW_CODE
static void __hw_sfc_run(u32 flash_base, \
                         u16 dec_key, \
                         u32 flash_size, \
                         u32 cfg_file_addr, \
                         u32 app_file_addr)
{
    ///set cache
    __hw_set_spi_cache_disable();
    memset((void *)TAG_RAM_START_ADDR, 0x0, 2 * 1024);

    ///set sfc flash base
    JL_SFC->BASE_ADR = flash_base;
    ///set key
    JL_ENC->KEY = dec_key;
    JL_ENC->CON = BIT(3);

    ///**********************VM 64+4K uncode space at last of flash****************************/
#define UNCODE_START_ADD    (FLASH_BASE_ADDR + ((flash_size - ((68+4)*1024)) - flash_base))
#define UNCODE_END_ADD      (FLASH_BASE_ADDR + flash_size - flash_base)

    JL_ENC->CON |= BIT(4);
    JL_ENC->UNENC_ADRH = UNCODE_END_ADD;
    JL_ENC->UNENC_ADRL = UNCODE_START_ADD;

    ///enable cache
    __hw_set_spi_cache_enable();

    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");

    ///run
    ((void(*)(u32, u32, u32))FLASH_BASE_ADDR)(cfg_file_addr - flash_base, app_file_addr, dec_key);
}

AT_ICACHE_HW_CODE
static void icache_ioctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case CACHE_SET_SPI_CACHE_DIS:
        __hw_set_spi_cache_disable();
        break;
    case CACHE_SET_SPI_CACHE_EN:
        __hw_set_spi_cache_enable();
        break;
    case CACHE_SET_SFC_INIT:
        __hw_sfc_init((SPI_MODE)va_arg(argptr, int), \
                      (FLASH_MODE)va_arg(argptr, int), \
                      (SPI_PORT)va_arg(argptr, int), \
                      (u32)va_arg(argptr, int), \
                      (u8)va_arg(argptr, int));
        break;
    case CACHE_SET_SFC_RUN:
        __hw_sfc_run((u32)va_arg(argptr, int), \
                     (u16)va_arg(argptr, int), \
                     (u32)va_arg(argptr, int), \
                     (u32)va_arg(argptr, int), \
                     (u32)va_arg(argptr, int));
        break;
    default:
        break;
    }

    va_end(argptr);
}


AT_ICACHE_HW_CODE
static struct icache_driver icache_drv_ins = {

    .resume     =   __hw_resume,

    .suspend    =   __hw_suspend,

    .flush      =   __hw_flush,

    .ioctrl     =   icache_ioctrl,
};
REGISTER_ICACHE_DRIVER(icache_drv_ins);
