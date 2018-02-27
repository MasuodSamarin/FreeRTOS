/*********************************************************************************************
    *   Filename        : icache_interface.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-18 10:31

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _ICACHE_INTERFACE_H
#define _ICACHE_INTERFACE_H

#include "typedef.h"

#define FLASH_BASE_ADDR     0x1000000
#define TAG_RAM_START_ADDR  0x48000

#define CACHE_LINE_COUNT   32
#define CACHE_ADDR_ALIGN(addr)	 ((void *)(((unsigned int)(addr) + CACHE_LINE_COUNT - 1) & ~(unsigned int)(CACHE_LINE_COUNT-1)))

enum {
    CACHE_SET_SPI_CACHE_DIS = 0,
    CACHE_SET_SPI_CACHE_EN,
    CACHE_SET_SFC_INIT,
    CACHE_SET_SFC_RUN,
};

struct icache_driver {
    void (*resume)();
    void (*suspend)();

    void (*flush)();

    void (*ioctrl)(int, ...);
};

#define REGISTER_ICACHE_DRIVER(drv) \
	const struct icache_driver *__icache_drv \
			SEC_USED(.icache_drv) = &drv

extern const struct icache_driver *__icache_drv;

#define AT_ICACHE_HW_CODE AT(.icache_hw_code)

#endif
