/*********************************************************************************************
    *   Filename        : clock_api.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-10 19:34

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "clock_interface.h"
#include "clock_api.h"


void clock_init(u8 sys_in, u32 input_freq, u32 out_freq)
{
    __clock_drv->on(sys_in, input_freq, out_freq);
    /* hw_clock_init(sys_in, input_freq, out_freq); */
}

void clock_switching(u32 out_freq)
{
    __clock_drv->switching(out_freq);
}

