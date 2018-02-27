/*********************************************************************************************
    *   Filename        : key_drv_io.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-17 15:57

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __KEY_DRV_IO_H__
#define __KEY_DRV_IO_H__

#include "cpu.h"
#include "key_drv_interface.h"

#define REGISTER_KEY_IO_DRIVER(drv) \
	const struct key_driver *__io_key_drv \
			SEC(.io_key_drv) = &drv

extern const struct key_driver *__io_key_drv;

#endif/*__KEY_DRV_IO_H__*/
