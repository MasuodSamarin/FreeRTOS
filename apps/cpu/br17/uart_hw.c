#include "clock_api.h"
#include "uart_interface.h"
#include "sdk_cfg.h"

///<locate all rodata(in this file) to x segment
#pragma str_literal_override("uart_hw.rodata")

struct uart_hdl {
    u8 enable;
};

static struct uart_hdl hdl;

#define __this  (&hdl)



AT_UART_HW_CODE
void uart_init(u32 baud)
{
    SFR(JL_CLOCK->CLK_CON1, 10, 2, 0); //use PLL48M clk

#if (DEBUG_UART == UART0)
#ifdef UART_TXPA5_RXPA6
    JL_IOMAP->CON0 &= ~(BIT(7) | BIT(6));
    JL_PORTA->OUT |= BIT(5) ;
    JL_PORTA->DIR |= BIT(6) ;
    JL_PORTA->DIR &= ~BIT(5) ;
#endif

#ifdef UART_TXPB6_RXPB7
    JL_IOMAP->CON0 &= ~(BIT(7) | BIT(6));
    JL_IOMAP->CON0 |= BIT(6);
    JL_PORTB->OUT |= BIT(6) ;
    JL_PORTB->DIR |= BIT(7) ;
    JL_PORTB->DIR &= ~BIT(6) ;
#endif

#ifdef UART_TXPC2_RXPC3
    JL_IOMAP->CON0 &= ~(BIT(7) | BIT(6));
    JL_IOMAP->CON0 |= BIT(7);
    JL_PORTC->OUT |= BIT(2) ;
    JL_PORTC->DIR |= BIT(3) ;
    JL_PORTC->DIR &= ~BIT(2) ;
#endif

#ifdef UART_TXPA14_RXPA15
    JL_IOMAP->CON0 |= BIT(14) | BIT(15);
    JL_PORTA->OUT |= BIT(14) ;
    JL_PORTA->DIR |= BIT(15) ;
    JL_PORTA->DIR &= ~BIT(14) ;
#endif

    JL_UART0->BAUD = (APP_UART_CLK / baud) / 4 - 1;
    JL_UART0->CON = BIT(13) | BIT(12) | BIT(0);       //rx_ie , CLR RX PND

#elif (DEBUG_UART == UART1)
#ifdef UART_TXPB0_RXPB1
    JL_IOMAP->CON1 &= ~(BIT(3) | BIT(2));
    JL_PORTB->OUT |= BIT(0) ;
    JL_PORTB->DIR |= BIT(1) ;
    JL_PORTB->DIR &= ~BIT(0) ;
#endif

#ifdef UART_TXPC0_RXPC1
    JL_IOMAP->CON1 &= ~(BIT(3) | BIT(2));
    JL_IOMAP->CON1 |= BIT(2);
    JL_PORTC->OUT |= BIT(0) ;
    JL_PORTC->DIR |= BIT(1) ;
    JL_PORTC->DIR &= ~BIT(0) ;
#endif

#ifdef UART_TXPA1_RXPA2
    JL_IOMAP->CON1 &= ~(BIT(3) | BIT(2));
    JL_IOMAP->CON1 |= BIT(3);
    JL_PORTA->OUT |= BIT(1) ;
    JL_PORTA->DIR |= BIT(2) ;
    JL_PORTA->DIR &= ~BIT(1) ;
#endif

#ifdef UART_USBP_USBM
    JL_IOMAP->CON1 |= BIT(3) | BIT(2);
    JL_USB->CON0 |= BIT(0) | BIT(1);
    JL_USB->IO_CON = 0;
    JL_USB->IO_CON |= BIT(0); //TX DP
    JL_USB->IO_CON |= BIT(3);//RX DM
    JL_USB->IO_CON &= ~BIT(2);//tx dp

    JL_USB->IO_CON &= ~BIT(5);//DM下拉
    JL_USB->IO_CON &= ~BIT(7);//DM上拉
#endif/*UART_USBP_USBM*/

    JL_UART1->BAUD = (UART_CLK / baud) / 4 - 1;
    JL_UART1->CON = BIT(13) | BIT(12) | BIT(0);       //rx_ie , CLR RX PND

#elif (DEBUG_UART == UART2)
#ifdef UART_TXPA3_RXPA4
    JL_IOMAP->CON1 &= ~(BIT(15) | BIT(14));
    JL_PORTA->OUT |= BIT(3) ;
    JL_PORTA->DIR |= BIT(4) ;
    JL_PORTA->DIR &= ~BIT(3) ;
#endif

#ifdef UART_TXPA9_RXPA10
    JL_IOMAP->CON1 &= ~(BIT(15) | BIT(14));
    JL_IOMAP->CON1 |= BIT(14);
    JL_PORTA->OUT |= BIT(9) ;
    JL_PORTA->DIR |= BIT(10) ;
    JL_PORTA->DIR &= ~BIT(9) ;
#endif

#ifdef UART_TXPB9_RXPB10
    JL_IOMAP->CON1 &= ~(BIT(15) | BIT(14));
    JL_IOMAP->CON1 |= BIT(15);
    JL_PORTB->OUT |= BIT(9) ;
    JL_PORTB->DIR |= BIT(10) ;
    JL_PORTB->DIR &= ~BIT(9) ;
#endif

#ifdef UART_TXPC4_RXPC5
    JL_IOMAP->CON1 |= BIT(15) | BIT(14);
    JL_PORTC->OUT |= BIT(4) ;
    JL_PORTC->DIR |= BIT(5) ;
    JL_PORTC->DIR &= ~BIT(4) ;
#endif

    JL_UART2->BAUD = (UART_CLK / baud) / 4 - 1;
    JL_UART2->CON = BIT(13) | BIT(12) | BIT(0);       //rx_ie , CLR RX PND
#endif

    //set_uart_map(DEBUG_UART);
    __this->enable = 1;
}

/*串口映射*/

AT_UART_HW_CODE
void set_uart_map(u8 uart_index)
{
#if (APP_UART_DEBUG == 1)
    //选择当前使用的UART进行映射
    switch (uart_index) {
    case UART0:
        JL_IOMAP->CON1 &= ~(BIT(8) | BIT(9) | BIT(10)); //选择UT0_TX
        break;
    case UART1:
        JL_IOMAP->CON1 &= ~(BIT(8) | BIT(9) | BIT(10)); //选择UT1_TX
        JL_IOMAP->CON1 |= BIT(8);
        break;
    case UART2:
        JL_IOMAP->CON1 |= BIT(8) | BIT(9) | BIT(10); //选择UT2_TX
        JL_IOMAP->CON1 |= BIT(8) | BIT(9) | BIT(10);
        break;
    default:
        return;
    }
    //设置要作为串口的GPIO
    JL_PORTA->DIR &= ~BIT(2);
    JL_PORTA->DIE &= ~BIT(2);
    JL_PORTA->PD  |= BIT(2);
    JL_PORTA->PU  |= BIT(2);
#endif
}



#if (DEBUG_UART == UART0)

AT_UART_HW_CODE
void putbyte(char a)
{
    if (__this->enable == 0) {
        return;
    }

    if (a == '\n') {
        JL_UART0->BUF = '\r';
        __asm__ volatile("csync");
        while ((JL_UART0->CON & BIT(15)) == 0);
    }

    JL_UART0->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART0->CON & BIT(15)) == 0);    //TX IDLE
}


AT_UART_HW_CODE
char getbyte()
{
    char c = 0;

    if ((JL_UART0->CON & BIT(14))) {
        c = JL_UART0->BUF;
        JL_UART0->CON |= BIT(12);
    }

    return c;
}

AT_UART_HW_CODE
char uart_read()
{
    char c = 0;

    if ((JL_UART0->CON & BIT(14))) {
        c = JL_UART0->BUF;
        JL_UART0->CON |= BIT(12);
    }

    return c;
}

#elif (DEBUG_UART == UART1)

AT_UART_HW_CODE
void putbyte(char a)
{
    if (__this->enable == 0) {
        return;
    }

    if (a == '\n') {
        JL_UART1->BUF = '\r';
        __asm__ volatile("csync");

        while ((JL_UART1->CON & BIT(15)) == 0);
    }

    JL_UART1->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART1->CON & BIT(15)) == 0);    //TX IDLE
}


AT_UART_HW_CODE
char getbyte()
{
    char c = 0;

    if ((JL_UART1->CON & BIT(14))) {
        c = JL_UART1->BUF;
        JL_UART1->CON |= BIT(12);
    }

    return c;
}

AT_UART_HW_CODE
char uart_read()
{
    char c = 0;

    if ((JL_UART1->CON & BIT(14))) {
        c = JL_UART1->BUF;
        JL_UART1->CON |= BIT(12);
    }

    return c;
}

#elif (DEBUG_UART == UART2)

AT_UART_HW_CODE
void putbyte(char a)
{
    if (__this->enable == 0) {
        return;
    }

    if (a == '\n') {
        JL_UART2->BUF = '\r';
        __asm__ volatile("csync");

        while ((JL_UART2->CON & BIT(15)) == 0);
    }

    JL_UART2->BUF = a;
    __asm__ volatile("csync");

    while ((JL_UART2->CON & BIT(15)) == 0);    //TX IDLE
}


AT_UART_HW_CODE
char getbyte()
{
    char c = 0;

    if ((JL_UART2->CON & BIT(14))) {
        c = JL_UART2->BUF;
        JL_UART2->CON |= BIT(12);
    }

    return c;
}

AT_UART_HW_CODE
char uart_read()
{
    char c = 0;

    if ((JL_UART2->CON & BIT(14))) {
        c = JL_UART2->BUF;
        JL_UART2->CON |= BIT(12);
    }

    return c;
}
#else

AT_UART_HW_CODE
void putbyte(char a)
{
    /* uart_putbyte("uart1",a); */
}

AT_UART_HW_CODE
char getbyte()
{
    return 0;
}

AT_UART_HW_CODE
char uart_read()
{
    return 0;
}
#endif


AT_UART_HW_CODE
void putchar(char a)
{
    if (a == '\n') {
        putbyte(0x0d);
        putbyte(0x0a);
    } else {
        putbyte(a);
    }
}


AT_UART_HW_CODE
void put_u4hex(u8 dat)
{
    dat = 0xf & dat;

    if (dat > 9) {
        putbyte(dat - 10 + 'A');
    } else {
        putbyte(dat + '0');
    }
}

AT_UART_HW_CODE
void put_u32hex(u32 dat)
{
    putbyte('0');
    putbyte('x');
    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar('\n');
}


AT_UART_HW_CODE
void put_u32hex0(u32 dat)
{

    put_u4hex(dat >> 28);
    put_u4hex(dat >> 24);

    put_u4hex(dat >> 20);
    put_u4hex(dat >> 16);

    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
}


AT_UART_HW_CODE
void put_u64hex(u64 dat)
{
    putbyte('0');
    putbyte('x');
    put_u32hex0(dat >> 32);
    put_u32hex0(dat);
    putchar('\n');
}

AT_UART_HW_CODE
void put_u16hex(u16 dat)
{
    putbyte('0');
    putbyte('x');


    put_u4hex(dat >> 12);
    put_u4hex(dat >> 8);

    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
}


AT_UART_HW_CODE
void put_u8hex(u8 dat)
{
    put_u4hex(dat >> 4);
    put_u4hex(dat);
    putchar(' ');
}


AT_UART_HW_CODE
void printf_buf(u8 *buf, u32 len)
{
    u32 i ;

    // put_u32hex(buf);
    // putchar('\n');
    for (i = 0 ; i < len ; i++) {
        if ((i % 16) == 0) {
            putbyte('\n') ;
        }

        put_u8hex(buf[i]) ;
    }

    putbyte('\n') ;

}


AT_UART_HW_CODE
int puts(char *out)
{
    while (*out != '\0') {
        putchar(*out);
        out++;
    }

    return 0;
}


AT_UART_HW_CODE
void put_buf(u8 *buf, u32 len)
{
    u32 i ;
    for (i = 0 ; i < len ; i++) {
        if ((i % 16) == 0) {
            putchar('\n') ;
        }
        put_u8hex(buf[i]) ;
    }
    putchar('\n') ;
}
