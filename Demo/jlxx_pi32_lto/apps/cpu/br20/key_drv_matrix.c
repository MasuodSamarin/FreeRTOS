/*********************************************************************************************
    *   Filename        : key_drv_io.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-17 15:58

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "key.h"
#include "init.h"
#include "key_drv_matrix.h"
#include "sdk_cfg.h"
#include "uart.h"

const u32 port_in_col[][2] = {
    {(u32)JL_PORTA, 0},
    {(u32)JL_PORTA, 1},
    {(u32)JL_PORTA, 2},
    {(u32)JL_PORTA, 3},
    {(u32)JL_PORTB, 1},
    {(u32)JL_PORTB, 2},
};

const u32 port_out_row[][2] = {
    {(u32)JL_PORTA, 10},
    {(u32)JL_PORTA, 11},
    {(u32)JL_PORTA, 12},
    {(u32)JL_PORTB, 0},
};


#define DOUBLE_KEY_SCAN_EN          1
#define SCAN_WAY_USE_PULL_UP_EN     0
#define GET_ARRAY_MEMBER(x)     (sizeof(x)/sizeof(u32)/2)
#define KEY_REAL_NUM()          (GET_ARRAY_MEMBER(port_out_row)*GET_ARRAY_MEMBER(port_in_col))

#define COL_PORT_IN_INIT()                                      \
    if (SCAN_WAY_USE_PULL_UP_EN ) {                             \
        col_port_in->PU  |=  BIT(port_in_col[i][1]);            \
        col_port_in->PD  &= ~BIT(port_in_col[i][1]);            \
    } else {                                                    \
        col_port_in->PU  &= ~BIT(port_in_col[i][1]);            \
        col_port_in->PD  |=  BIT(port_in_col[i][1]);            \
    }

#define ROW_PORT_OUT()                                          \
        if (SCAN_WAY_USE_PULL_UP_EN) {                          \
            row_port_out->OUT &= ~BIT(port_out_row[i][1]);      \
        } else {                                                \
            row_port_out->OUT |=  BIT(port_out_row[i][1]);      \
        }                                                       \
        row_port_out->DIR &= ~BIT(port_out_row[i][1])

#define ROW_PORT_OUT_FLOAT()                                    \
        if (SCAN_WAY_USE_PULL_UP_EN) {                          \
            /*放电用*/                                          \
            row_port_out->OUT |=  BIT(port_out_row[i][1]);      \
        } else {                                                \
            /*放电用*/                                          \
            row_port_out->OUT &= ~BIT(port_out_row[i][1]);      \
        }                                                       \
        row_port_out->DIR |= BIT(port_out_row[i][1])

#define COL_PORT_IN()                                           \
        SCAN_WAY_USE_PULL_UP_EN?                                \
        !(col_port_in->IN & BIT(port_in_col[j][1])) : (col_port_in->IN & BIT(port_in_col[j][1]))


static void matrix_key_init(void)
{
    u32 i;
    JL_PORT_TypeDef *col_port_in;
    JL_PORT_TypeDef *row_port_out;

    // 预防AMUX口复用
    SFR(JL_AUDIO->ADA_CON0, 22, 1, 0b1);
    SFR(JL_AUDIO->ADA_CON0, 23, 3, 0b110);

    //port in init
    for (i = 0; i < GET_ARRAY_MEMBER(port_in_col); i++) {
        col_port_in = (JL_PORT_TypeDef *)port_in_col[i][0];
        col_port_in->DIE |=  BIT(port_in_col[i][1]);
        col_port_in->DIR |=  BIT(port_in_col[i][1]);
        COL_PORT_IN_INIT();
        col_port_in->HD  &= ~BIT(port_in_col[i][1]);
    }
    //port out init
    for (i = 0; i < GET_ARRAY_MEMBER(port_out_row); i++) {
        row_port_out = (JL_PORT_TypeDef *)port_out_row[i][0];
        row_port_out->DIE |=  BIT(port_out_row[i][1]);
        row_port_out->DIR |=  BIT(port_out_row[i][1]);
        row_port_out->PU  &= ~BIT(port_out_row[i][1]);
        row_port_out->PD  &= ~BIT(port_out_row[i][1]);
        row_port_out->HD  &= ~BIT(port_out_row[i][1]);
    }
}

static u8 matrix_key_get_number(void)
{
    u8 i, j;
    JL_PORT_TypeDef *col_port_in, *row_port_out;
    u8 key_num = INPUT_KEY_INVALID;
    u8 key[2] = {
        INPUT_KEY_INVALID, INPUT_KEY_INVALID,
    };
    u8 *key_p = (u8 *)key;

    matrix_key_init();

    for (i = 0; i < GET_ARRAY_MEMBER(port_out_row); i++) {
        row_port_out = (JL_PORT_TypeDef *)port_out_row[i][0];
        ROW_PORT_OUT();

        for (j = 0; j < GET_ARRAY_MEMBER(port_in_col); j++) {
            col_port_in = (JL_PORT_TypeDef *)port_in_col[j][0];
            if (COL_PORT_IN()) {
                *key_p++ = GET_ARRAY_MEMBER(port_in_col) * i + j;
                if (key[DOUBLE_KEY_SCAN_EN] != INPUT_KEY_INVALID) {
                    ROW_PORT_OUT_FLOAT();
                    goto __get_key_num;
                }
            }
        }
        ROW_PORT_OUT_FLOAT();
    }

__get_key_num:
    if (key[1] == INPUT_KEY_INVALID) {
        key_num = key[0];
    } else {
        key_num = KEY_REAL_NUM() + ((KEY_REAL_NUM() - 1) * key[0]) + key[1];
    }
    if (key[0] != INPUT_KEY_INVALID) {
        /* put_u8hex(key[0]); */
        /* put_u8hex(key[1]); */
        /* putchar('\n'); */
    }

    return key_num;
}

static const struct key_driver key_drv_matrix = {
    .init               = matrix_key_init,

    .get_key_number     = matrix_key_get_number,

    .ioctrl             = NULL,
};
REGISTER_KEY_MATRIX_DRIVER(key_drv_matrix);

