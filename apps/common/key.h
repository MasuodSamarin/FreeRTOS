/*********************************************************************************************
    *   Filename        : key.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-16 10:38

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _KEY_H_
#define _KEY_H_

#include "cpu.h"


///<配置按键类型：ADC 类型按键
// #define KEY_DRV_AD_EN
///<配置按键类型：IO 类型按键
// #define KEY_DRV_IO_EN
///<配置按键类型：MATRIX 类型按键
// #define KEY_DRV_MATRIX_EN

enum {
    KEY_TYPE_AD,
    KEY_TYPE_IO,
    KEY_TYPE_MATRIX,
};


#if (defined KEY_DRV_AD_EN || defined KEY_DRV_IO_EN || defined KEY_DRV_MATRIX_EN)
/**
 * @brief __keymsg_q_pend
 *
 * @return
 */
u32 __keymsg_q_pend(void);
#define key_get_input               __keymsg_q_pend

/**
 * @brief __keymsg_register_handler : register handler for key number output
 *
 * @param handler :
 */
void __keymsg_register_handler(u8(*handler)(u8, u8, u8));
#define key_input_register_handler  __keymsg_register_handler

/**
 * @brief __key_input_init
 */
// void __key_input_init();
// #define key_input_init              __key_input_init
#else

u32 __keymsg_q_pend_null(void);
#define key_get_input               __keymsg_q_pend_null

void __keymsg_register_handler_null(u8(*handler)(u8, u8, u8));
#define key_input_register_handler  __keymsg_register_handler_null

#endif
/**
 * @brief Input Table for AD-KEY / IO-KEY / IR-KEY / TOUCH-KEY
 */
enum {
    //AD-KEY part
    INPUT_AD_KEY0_SHORT,
    INPUT_AD_KEY1_SHORT,
    INPUT_AD_KEY2_SHORT,
    INPUT_AD_KEY3_SHORT,
    INPUT_AD_KEY4_SHORT,
    INPUT_AD_KEY5_SHORT,
    INPUT_AD_KEY6_SHORT,
    INPUT_AD_KEY7_SHORT,
    INPUT_AD_KEY8_SHORT,
    INPUT_AD_KEY9_SHORT,

    //IO-KEY part
    INPUT_IO_KEY0_SHORT,
    INPUT_IO_KEY1_SHORT,
    INPUT_IO_KEY2_SHORT,
    INPUT_IO_KEY3_SHORT,
    INPUT_IO_KEY4_SHORT,
    INPUT_IO_KEY5_SHORT,
    INPUT_IO_KEY6_SHORT,
    INPUT_IO_KEY7_SHORT,
    INPUT_IO_KEY8_SHORT,
    INPUT_IO_KEY9_SHORT,

    INPUT_IO_KEY0_LONG,
    INPUT_IO_KEY1_LONG,
    INPUT_IO_KEY2_LONG,
    INPUT_IO_KEY3_LONG,
    INPUT_IO_KEY4_LONG,
    INPUT_IO_KEY5_LONG,
    INPUT_IO_KEY6_LONG,
    INPUT_IO_KEY7_LONG,
    INPUT_IO_KEY8_LONG,
    INPUT_IO_KEY9_LONG,

    INPUT_IO_KEY0_HOLD,
    INPUT_IO_KEY1_HOLD,
    INPUT_IO_KEY2_HOLD,
    INPUT_IO_KEY3_HOLD,
    INPUT_IO_KEY4_HOLD,
    INPUT_IO_KEY5_HOLD,
    INPUT_IO_KEY6_HOLD,
    INPUT_IO_KEY7_HOLD,
    INPUT_IO_KEY8_HOLD,
    INPUT_IO_KEY9_HOLD,

    INPUT_IO_KEY0_LONGUP,
    INPUT_IO_KEY1_LONGUP,
    INPUT_IO_KEY2_LONGUP,
    INPUT_IO_KEY3_LONGUP,
    INPUT_IO_KEY4_LONGUP,
    INPUT_IO_KEY5_LONGUP,
    INPUT_IO_KEY6_LONGUP,
    INPUT_IO_KEY7_LONGUP,
    INPUT_IO_KEY8_LONGUP,
    INPUT_IO_KEY9_LONGUP,

    //AD-KEY part
    INPUT_MATRIX_KEY0_SHORT,
    INPUT_MATRIX_KEY1_SHORT,
    INPUT_MATRIX_KEY2_SHORT,
    INPUT_MATRIX_KEY3_SHORT,
    INPUT_MATRIX_KEY4_SHORT,
    INPUT_MATRIX_KEY5_SHORT,
    INPUT_MATRIX_KEY6_SHORT,
    INPUT_MATRIX_KEY7_SHORT,
    INPUT_MATRIX_KEY8_SHORT,
    INPUT_MATRIX_KEY9_SHORT,

    INPUT_MATRIX_KEY0_LONG,
    INPUT_MATRIX_KEY0_LONGUP,

    INPUT_KEY_INVALID = 0xff,
};

/*******************************************************************/
/*
 *-------------------AD Key mapping
 */
#define AD_KEY_SHORT   \
            /*00*/      INPUT_AD_KEY0_SHORT,\
            /*01*/      INPUT_AD_KEY1_SHORT,\
            /*02*/      INPUT_AD_KEY2_SHORT,\
            /*03*/      INPUT_AD_KEY3_SHORT,\
            /*04*/      INPUT_AD_KEY4_SHORT,\
            /*05*/      INPUT_AD_KEY5_SHORT,\
            /*06*/      INPUT_AD_KEY6_SHORT,\
            /*07*/      INPUT_AD_KEY7_SHORT,\
            /*08*/      INPUT_AD_KEY8_SHORT,\
            /*09*/      INPUT_AD_KEY9_SHORT,\

#define AD_KEY_LONG   \
            /*00*/      INPUT_KEY_INVALID,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define AD_KEY_HOLD   \
            /*00*/      INPUT_KEY_INVALID,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define AD_KEY_LONG_UP   \
            /*00*/      INPUT_KEY_INVALID,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\


static const u8 ad_key_input_table[4][10] = {
    {AD_KEY_SHORT},
    {AD_KEY_LONG},
    {AD_KEY_HOLD},
    {AD_KEY_LONG_UP},
};

/*******************************************************************/
/*
 *-------------------IO Key mapping
 */
#define IO_KEY_SHORT   \
            /*00*/      INPUT_IO_KEY0_SHORT,\
            /*01*/      INPUT_IO_KEY1_SHORT,\
            /*02*/      INPUT_IO_KEY2_SHORT,\
            /*03*/      INPUT_IO_KEY3_SHORT,\
            /*04*/      INPUT_IO_KEY4_SHORT,\
            /*05*/      INPUT_IO_KEY5_SHORT,\
            /*06*/      INPUT_IO_KEY6_SHORT,\
            /*07*/      INPUT_IO_KEY7_SHORT,\
            /*08*/      INPUT_IO_KEY8_SHORT,\
            /*09*/      INPUT_IO_KEY9_SHORT,\

#define IO_KEY_LONG   \
            /*00*/      INPUT_IO_KEY0_LONG,\
            /*01*/      INPUT_IO_KEY1_LONG,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define IO_KEY_HOLD   \
            /*00*/      INPUT_IO_KEY0_HOLD,\
            /*01*/      INPUT_IO_KEY1_HOLD,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define IO_KEY_LONG_UP   \
            /*00*/      INPUT_IO_KEY0_LONGUP,\
            /*01*/      INPUT_IO_KEY1_LONGUP,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

static const u8 io_key_input_table[4][10] = {
    {IO_KEY_SHORT},
    {IO_KEY_LONG},
    {IO_KEY_HOLD},
    {IO_KEY_LONG_UP},
};

/*******************************************************************/
/*
 *-------------------IO Key mapping
 */
#define MATRIX_KEY_SHORT   \
            /*00*/      INPUT_MATRIX_KEY0_SHORT,\
            /*01*/      INPUT_MATRIX_KEY1_SHORT,\
            /*02*/      INPUT_MATRIX_KEY2_SHORT,\
            /*03*/      INPUT_MATRIX_KEY3_SHORT,\
            /*04*/      INPUT_MATRIX_KEY4_SHORT,\
            /*05*/      INPUT_MATRIX_KEY5_SHORT,\
            /*06*/      INPUT_MATRIX_KEY6_SHORT,\
            /*07*/      INPUT_MATRIX_KEY7_SHORT,\
            /*08*/      INPUT_MATRIX_KEY8_SHORT,\
            /*09*/      INPUT_MATRIX_KEY9_SHORT,\

#define MATRIX_KEY_LONG   \
            /*00*/      INPUT_MATRIX_KEY0_LONG,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define MATRIX_KEY_HOLD   \
            /*00*/      INPUT_KEY_INVALID,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

#define MATRIX_KEY_LONG_UP   \
            /*00*/      INPUT_MATRIX_KEY0_LONGUP,\
            /*01*/      INPUT_KEY_INVALID,\
            /*02*/      INPUT_KEY_INVALID,\
            /*03*/      INPUT_KEY_INVALID,\
            /*04*/      INPUT_KEY_INVALID,\
            /*05*/      INPUT_KEY_INVALID,\
            /*06*/      INPUT_KEY_INVALID,\
            /*07*/      INPUT_KEY_INVALID,\
            /*08*/      INPUT_KEY_INVALID,\
            /*09*/      INPUT_KEY_INVALID,\

static const u8 matrix_key_input_table[4][10] = {
    {MATRIX_KEY_SHORT},
    {MATRIX_KEY_LONG},
    {MATRIX_KEY_HOLD},
    {MATRIX_KEY_LONG_UP},
};

#endif
