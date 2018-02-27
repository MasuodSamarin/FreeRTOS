/*********************************************************************************************
    *   Filename        : uart.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-14 19:07

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _UART_H_
#define _UART_H_

#include "typedef.h"

/**
 * @brief puts
 *
 * @param out
 *
 * @return
 */
int puts(char *out);

/**
 * @brief putchar
 *
 * @param a
 */
void putchar(char a);
/**
 * @brief put_buf
 *
 * @param buf
 * @param len
 */
void put_buf(u8 *buf, u32 len);
/**
 * @brief printf_buf
 *
 * @param buf
 * @param len
 */
void printf_buf(u8 *buf, u32 len);
/**
 * @brief put_u8hex
 *
 * @param dat
 */
void put_u8hex(u8 dat);
/**
 * @brief put_u8hex0
 *
 * @param dat
 */
void put_u8hex0(u8 dat);
/**
 * @brief put_u32hex
 *
 * @param dat
 */
void put_u32hex(u32 dat);
/**
 * @brief put_u32hex0
 *
 * @param dat
 */
void put_u32hex0(u32 dat);
/**
 * @brief put_u16hex
 *
 * @param dat
 */
void put_u16hex(u16 dat);

#endif
