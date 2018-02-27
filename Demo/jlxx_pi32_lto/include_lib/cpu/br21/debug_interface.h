/*********************************************************************************************
    *   Filename        : debug_interface.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-14 16:51

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef	_DEBUG_INTERFACE_H_
#define _DEBUG_INTERFACE_H_

#include "typedef.h"

enum {
    PC_WR_RAM = 0,
    PRP_WR_RAM,
};

struct debug_driver {
    void (*on)();
    void (*off)();

    void (*suspend)();
    void (*resume)();

    void (*ioctrl)(u8, ...);
};

#define REGISTER_DEBUG_DRIVER(drv) \
	const struct debug_driver *__debug_drv \
			SEC_USED(.text) = &drv

extern const struct debug_driver *__debug_drv;

#endif
