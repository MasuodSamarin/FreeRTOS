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
#include "key_drv_io.h"
#include "sdk_cfg.h"

#define GET_ARRAY_MEMBER(x)     (sizeof(x)/sizeof(u32)/2)

const u32 io_use_port[][2] = {
    {(u32)JL_PORTA, 8},
    {(u32)JL_PORTA, 9},
    {(u32)JL_PORTA, 10},
    {(u32)JL_PORTA, 11},
    {(u32)JL_PORTA, 12},
};

static void io_key_init(void)
{
    u32 i;
    JL_PORT_TypeDef *io_port;

    for (i = 0; i < GET_ARRAY_MEMBER(io_use_port); i++) {
        io_port = (JL_PORT_TypeDef *)io_use_port[i][0];
        io_port->DIE |=  BIT(io_use_port[i][1]);
        io_port->DIR |=  BIT(io_use_port[i][1]);
        io_port->PU  |=  BIT(io_use_port[i][1]);
        io_port->PD  &= ~BIT(io_use_port[i][1]);
        io_port->HD  &= ~BIT(io_use_port[i][1]);
    }
}

static u8 io_key_get_number(void)
{
    u8 number = KEY_NUMBER_INVALID;
    u32 i;
    JL_PORT_TypeDef *io_port;

    for (i = 0; i < GET_ARRAY_MEMBER(io_use_port); i++) {
        io_port = (JL_PORT_TypeDef *)io_use_port[i][0];

        if (!(io_port->IN & BIT(io_use_port[i][1]))) {
            number = i;
            break;
        }
    }

    return number;
}

static const struct key_driver key_drv_io = {
    .init               = io_key_init,

    .get_key_number     = io_key_get_number,

    .ioctrl             = NULL,
};
REGISTER_KEY_IO_DRIVER(key_drv_io);
