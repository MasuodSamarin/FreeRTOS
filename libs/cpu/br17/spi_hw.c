#include "stdarg.h"
#include "spi_interface.h"
#include "icache_interface.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"

#pragma str_literal_override("bt_poweroff_reboot.rodata")


struct _spi0_struct {
    u32 mode;
};
static struct _spi0_struct spi0_struct = {
    0,
};

extern void delay(u32 d);
//-----------TEAM A spi0 io set
// set cs
#define SPI0_A_CS_BIT			BIT(3)
#define SPI0_A_CS_PORT(x)		JL_PORTD->x
// set clk
#define SPI0_A_CLK_BIT			BIT(0)
#define SPI0_A_CLK_PORT(x)		JL_PORTD->x
// set d0
#define SPI0_A_D0_BIT			BIT(1)
#define SPI0_A_D0_PORT(x)		JL_PORTD->x
// set d1
#define SPI0_A_D1_BIT			BIT(2)
#define SPI0_A_D1_PORT(x)		JL_PORTD->x
// set d2
#define SPI0_A_D2_BIT			BIT(4)
#define SPI0_A_D2_PORT(x)		JL_PORTB->x
// set d3
#define SPI0_A_D3_BIT			BIT(3)
#define SPI0_A_D3_PORT(x)		JL_PORTB->x

//-----------TEAM B spi0 io set
// set cs
#define SPI0_B_CS_BIT			BIT(6)
#define SPI0_B_CS_PORT(x)		JL_PORTB->x
// set clk
#define SPI0_B_CLK_BIT			BIT(8)
#define SPI0_B_CLK_PORT(x)		JL_PORTB->x
// set d0
#define SPI0_B_D0_BIT			BIT(7)
#define SPI0_B_D0_PORT(x)		JL_PORTB->x
// set d1
#define SPI0_B_D1_BIT			BIT(5)
#define SPI0_B_D1_PORT(x)		JL_PORTB->x
// set d2
#define SPI0_B_D2_BIT			BIT(4)
#define SPI0_B_D2_PORT(x)		JL_PORTB->x
// set d3
#define SPI0_B_D3_BIT			BIT(3)
#define SPI0_B_D3_PORT(x)		JL_PORTB->x


AT_SPI0_HW_CODE
static void __spi0_wait_ok(void)
{
    while (!(JL_SPI0->CON & BIT(15)));
    JL_SPI0->CON |= BIT(14);
}

AT_SPI0_HW_CODE
static void __spi0_cs(u8 cs)
{
    if (JL_IOMAP->CON0 & BIT(5)) { //bootB - portb
        SPI0_B_CS_PORT(DIR) &= ~SPI0_B_CS_BIT;
        cs ? (SPI0_B_CS_PORT(OUT) |= SPI0_B_CS_BIT) : (SPI0_B_CS_PORT(OUT) &= ~SPI0_B_CS_BIT); //CS
    } else {
        SPI0_A_CS_PORT(DIR) &= ~SPI0_A_CS_BIT;
        cs ? (SPI0_A_CS_PORT(OUT) |= SPI0_A_CS_BIT) : (SPI0_A_CS_PORT(OUT) &= ~SPI0_A_CS_BIT); //CS
    }

    delay(10);
}

AT_SPI0_HW_CODE
static void __spi0_write_byte(u8 dat)
{
    JL_SPI0->CON &= ~BIT(12);
    JL_SPI0->BUF = dat;
    __spi0_wait_ok();
}

AT_SPI0_HW_CODE
static u8 __spi0_read_byte(void)
{
    u8 ret_byte;

    JL_SPI0->CON |= BIT(12);
    JL_SPI0->BUF = 0xff;
    __spi0_wait_ok();
    ret_byte = JL_SPI0->BUF;

    return ret_byte;
}

AT_SPI0_HW_CODE
static s32 __spi0_write(u8 *buf, u16 len)
{
#if 0
    JL_SPI0->CON &= ~BIT(12);  //DIR
    if (spi0_struct.mode == SPI_QUAD_MODE) { //4bit mode
        JL_SPI0->CON &= ~BIT(10);
        JL_SPI0->CON |= BIT(11) ;
    } else { //1bit mode
        JL_SPI0->CON &= ~(BIT(10) | BIT(11));
    }

    JL_SPI0->ADR = (u32)buf;
    JL_SPI0->CNT = len;
    __spi0_wait_ok();

    if ((spi0_struct.mode == SPI_DUAL_MODE) || (spi0_struct.mode == SPI_QUAD_MODE)) {
        JL_SPI0->CON &= ~(BIT(10) | BIT(11)) ; //恢复成1bit模式
    }

    return 0;

#else

    JL_SPI0->CON &= ~BIT(12);  //DIR
    if (spi0_struct.mode == SPI_QUAD_MODE) { //4bit mode
        JL_SPI0->CON &= ~(BIT(10) | BIT(9));
        JL_SPI0->CON |= BIT(8) | BIT(11) ;
    } else { //1bit mode
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11));
    }

    while (len--) {
        __spi0_write_byte(*buf++);
    }

    if ((spi0_struct.mode == SPI_DUAL_MODE) || (spi0_struct.mode == SPI_QUAD_MODE)) {
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11)) ;
    }

    return 0;
#endif
}

AT_SPI0_HW_CODE
static s32 __spi0_write_2bitMode(u8 *buf, u16 len)
{
    JL_SPI0->CON &= ~BIT(12);  //DIR

    JL_SPI0->CON |= BIT(10) ;
    JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(11));

    while (len--) {
        __spi0_write_byte(*buf++);
    }
    if ((spi0_struct.mode == SPI_DUAL_MODE) || (spi0_struct.mode == SPI_QUAD_MODE)) {
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11)) ;
    }

    return 0;
}


AT_SPI0_HW_CODE
static s32 __spi0_read(u8 *buf, u16 len)
{
    JL_SPI0->CON |= BIT(12);    //DIR

    if (spi0_struct.mode == SPI_QUAD_MODE) { //4bit mode
        JL_SPI0->CON &= ~BIT(3);

        JL_SPI0->CON &= ~(BIT(9) | BIT(10));
        JL_SPI0->CON |= BIT(8) | BIT(11);
    } else  if (spi0_struct.mode == SPI_DUAL_MODE) { //2bit mode
        JL_SPI0->CON &= ~BIT(3);

        JL_SPI0->CON |= BIT(10);
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(11));
    } else {  //1bit mode
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11)) ; //»Ö¸´³É1bitÄ£Ê½
    }
    while (len--) {
        *buf = __spi0_read_byte();
        buf++;
    }

    if ((spi0_struct.mode == SPI_DUAL_MODE) || (spi0_struct.mode == SPI_QUAD_MODE)) {
        JL_SPI0->CON &= ~(BIT(8) | BIT(9) | BIT(10) | BIT(11)) ; //»Ö¸´³É1bitÄ£Ê½
        JL_SPI0->CON |= BIT(3);
    }
    return 0;
}

AT_SPI0_HW_CODE
static void __spi0_suspend(void)
{
    JL_SPI0->CON = 0;
}

AT_SPI0_HW_CODE
static void __spi0_resume(void)
{
    JL_SPI0->CON = 0;
    JL_SPI0->CON = 0x1039;
}


AT_SPI0_HW_CODE
static void __spi0_io_init(SPI_MODE mode, SPI_PORT io_port)
{
    if (io_port == SPI0_PORT_TEAM_B) {
        JL_IOMAP->CON0 |= BIT(5); // pd0--pd5

        SPI0_B_CLK_PORT(DIR) &= ~SPI0_B_CLK_BIT; 	//CLK
        SPI0_B_D0_PORT(DIR)  &= ~SPI0_B_D0_BIT; 	//D0
        SPI0_B_CLK_PORT(HD)  |=  SPI0_B_CLK_BIT; 	//CLK

        if (mode == SPI_QUAD_MODE) {
            SPI0_B_D1_PORT(DIR) |=  SPI0_B_D1_BIT;	//D1
            SPI0_B_D2_PORT(DIR) &= ~SPI0_B_D2_BIT;	//WP(D2)
            SPI0_B_D3_PORT(DIR) &= ~SPI0_B_D3_BIT;	//HOLD(D3)
            SPI0_B_D2_PORT(OUT) |=  SPI0_B_D2_BIT;	//WP(D2)
            SPI0_B_D3_PORT(OUT) |=  SPI0_B_D3_BIT;	//HOLD(D3)
        } else if ((mode == SPI_ODD_MODE) || (mode == SPI_DUAL_MODE)) {
            SPI0_B_D1_PORT(DIR) |= SPI0_B_D1_BIT;		//D1
        } else if (SPI_2WIRE_MODE == mode) {
            JL_IOMAP->CON1 |= BIT(1);//芯片内部的di do短接
        } else {
            log_info("error spi config\n");
        }

        log_info("spi is portd_b\n");
    } else {
        JL_IOMAP->CON0 &= ~BIT(5); // pd0/pd1/pd3/pd5/pd6/pd7

        SPI0_A_CLK_PORT(DIR) &= ~SPI0_A_CLK_BIT; 	//CLK
        SPI0_A_D0_PORT(DIR)  &= ~SPI0_A_D0_BIT; 	//D0
        SPI0_A_CLK_PORT(HD)  |=  SPI0_A_CLK_BIT; 	//CLK

        if (mode == SPI_QUAD_MODE) {
            SPI0_A_D1_PORT(DIR) |=  SPI0_A_D1_BIT;	//D1
            SPI0_A_D2_PORT(DIR) &= ~SPI0_A_D2_BIT;	//WP(D2)
            SPI0_A_D3_PORT(DIR) &= ~SPI0_A_D3_BIT;	//HOLD(D3)
            SPI0_A_D2_PORT(OUT) |=  SPI0_A_D2_BIT;	//WP(D2)
            SPI0_A_D3_PORT(OUT) |=  SPI0_A_D3_BIT;	//HOLD(D3)
        } else if ((mode == SPI_ODD_MODE) || (mode == SPI_DUAL_MODE)) {
            SPI0_A_D1_PORT(DIR) |= SPI0_A_D1_BIT;		//D1
        } else if (SPI_2WIRE_MODE == mode) {
            JL_IOMAP->CON1 |= BIT(1); //芯片内部的di do短接
        } else {
            log_info("error spi config\n");
        }

        log_info("spi is portd_a\n");
    }
}

AT_SPI0_HW_CODE
static s32 __spi0_init(SPI_MODE wire_mode, SPI_PORT io_port, u8 spi_baud)
{
    log_info("wire_mode=0x%x\n", wire_mode);
    log_info("io_port=0x%x\n", io_port);
    log_info("spi_baud=0x%x\n", spi_baud);

    __icache_drv->ioctrl(CACHE_SET_SPI_CACHE_DIS);
    __icache_drv->suspend();

    JL_SPI0->CON =  0x0038;
    JL_IOMAP->CON1 &= ~BIT(1);		//芯片内部的di do短接 disable
    JL_SPI0->BAUD = spi_baud;

    __spi0_io_init(wire_mode, io_port);

    if (SPI_2WIRE_MODE == wire_mode) {
        JL_SPI0->CON &= ~BIT(3); //单向模式
    }

    JL_SPI0->CON |= BIT(0);
    __spi0_cs(1);

    return 0;
}

AT_SPI0_HW_CODE
static void spi0_ioctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case SET_2BIT_MODE:
        __spi0_write_2bitMode((u8 *)va_arg(argptr, int), (u16)va_arg(argptr, int));
        break;
    case SET_WIRE_MODE:
        spi0_struct.mode = (u8)va_arg(argptr, int);
        break;
    case SET_SPI_INIT:
        log_info("SET_SPI_INIT\n");
        __spi0_init((SPI_MODE)va_arg(argptr, int), \
                    (SPI_PORT)va_arg(argptr, int), \
                    (u8)va_arg(argptr, int));
        break;
    }

    va_end(argptr);
}

AT_SPI0_HW_CODE
static struct spi_driver spi0_drv_ins = {
    .wait_ok    = __spi0_wait_ok,
    .cs         = __spi0_cs,

    .write_byte = __spi0_write_byte,
    .read_byte  = __spi0_read_byte,

    .read       = __spi0_read,
    .write      = __spi0_write,

    .suspend    = __spi0_suspend,
    .resume     = __spi0_resume,

    .ioctrl     = spi0_ioctrl,
};
REGISTER_SPI_DRIVER(spi0_drv_ins);
