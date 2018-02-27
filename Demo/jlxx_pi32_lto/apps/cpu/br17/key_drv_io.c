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



#define SET_IO_IO(i,x)       {JL_PORT##i->PD &= ~BIT(x), JL_PORT##i->PU |= BIT(x), JL_PORT##i->DIR |= BIT(x), JL_PORT##i->DIE |= BIT(x);}

#define GET_IO_IO(i,x)       (JL_PORT##i->IN & BIT(x))

static void io_key_init(void)
{
    SET_IO_IO(A, 8);
    SET_IO_IO(A, 9);
    SET_IO_IO(A, 10);
    SET_IO_IO(A, 11);
    SET_IO_IO(A, 12);
    /* printf("io key get number %x - %x\n", GET_IO_IO(A, 8), GET_IO_IO(A, 9)); */
}


static u8 io_key_get_number(void)
{
    u8 number = KEY_NUMBER_INVALID;

    if (GET_IO_IO(A, 8) == 0) {
        number = 0;
    }
    if (GET_IO_IO(A, 9) == 0) {
        number = 1;
    }
    if (GET_IO_IO(A, 10) == 0) {
        number = 2;
    }
    if (GET_IO_IO(A, 11) == 0) {
        number = 3;
    }
    if (GET_IO_IO(A, 12) == 0) {
        number = 4;
    }

    /* printf("io key get number %x - %x\n", GET_IO_IO(A, 8), GET_IO_IO(A, 9)); */
    return number;
}

static const struct key_driver key_drv_io = {
    .init               = io_key_init,

    .get_key_number     = io_key_get_number,

    .ioctrl             = NULL,
};
REGISTER_KEY_IO_DRIVER(key_drv_io);
