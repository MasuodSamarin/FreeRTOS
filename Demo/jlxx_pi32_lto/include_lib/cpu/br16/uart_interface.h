#ifndef _UART_INTERFACE_H_
#define _UART_INTERFACE_H_

#include "typedef.h"

#define UART_TXPND       15
#define UART_RXPND       14
#define UART_CTXPND      13
#define UART_CRXPND      12
#define UART_OTPND       11
#define UART_COTPND      10
#define UART_TB8         9
#define UART_RB8         8
#define UART_RDC         7
#define UART_DMA_EN      6
#define UART_OT_IEN      5
#define UART_DIV3N4      4
#define UART_RX_IEN      3
#define UART_TX_IEN      2
#define UART_9BIT_EN     1
#define UART_EN          0

enum {
    UART_TXPA8_RXPA9 = 0,
    UART_TXPB10_RXPB11,
    UART_TXPC6_RXPC7,
    UART_TXPD6_RXPD7,

    UART_TXPA6_RXPA7,
    UART_TXPB2_RXPB3,
    UART_TXPB6_RXPB7,
    UART_USBP_USBM,

    UART_TXPA4_RXPA5,
    UART_TXPA10_RXPA11,
    UART_TXPB8_RXPB9,
    UART_TXPC12_RXPC13,
};

void set_uart_map(u8 uart_index);
void uart1_init(void);
void uart0_init(u32 baudrate);
void uart_init(u32 baud);
int puts_without_lock(char *out);
/* int putchar(int a) ; */
void uart0_putbuf(u8 *uart_buf, u16 buf_len);
char getbyte();
char uart0_getbyte();
bool init_uart_mutex();



#endif

