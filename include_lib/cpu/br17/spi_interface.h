/*********************************************************************************************
    *   Filename        : spi_interface.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-14 17:08

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef	_SPI_INTERFACE_H_
#define _SPI_INTERFACE_H_

#include "typedef.h"

typedef enum _SPI_PORT {
    SPI0_PORT_TEAM_A = 0, //SPI0 A 出口
    SPI0_PORT_TEAM_B,     //SPI0 B 出口
} SPI_PORT;

enum {
    SET_2BIT_MODE = 0,
    SET_WIRE_MODE,
    SET_SPI_INIT,
};

#define SPI_PARM_VALID_MASK	  0x5a5a0000
#define SPI_MODE_MASK         (0x00000c00|SPI_PARM_VALID_MASK)
#define SPI_CLK_MASK    	  0x000000ff

typedef enum _SPI_MODE {
    SPI_2WIRE_MODE = 0x0 | SPI_PARM_VALID_MASK,
    SPI_ODD_MODE   = 0x00000400 | SPI_PARM_VALID_MASK,
    SPI_DUAL_MODE  = 0x00000800 | SPI_PARM_VALID_MASK,
    SPI_QUAD_MODE  = 0x00000c00 | SPI_PARM_VALID_MASK,
} SPI_MODE;

typedef enum _FLASH_MODE {
    FAST_READ_OUTPUT_MODE = 0x0 | SPI_PARM_VALID_MASK,
    FAST_READ_IO_MODE = 0x00001000 | SPI_PARM_VALID_MASK,
    FAST_READ_IO_CONTINUOUS_READ_MODE = 0x00002000 | SPI_PARM_VALID_MASK,
} FLASH_MODE;

struct spi_driver {
    void (*wait_ok)();
    void (*cs)(u8);

    void (*write_byte)(u8);
    u8(*read_byte)();

    s32(*read)(u8 *, u16);
    s32(*write)(u8 *, u16);

    void (*suspend)(void);
    void (*resume)(void);

    void (*ioctrl)(int, ...);
};

#define REGISTER_SPI_DRIVER(drv) \
    const struct spi_driver *__spi0_drv \
            SEC_USED(.spi0_drv) = &drv

extern const struct spi_driver *__spi0_drv;

#define AT_SPI0_HW_CODE     AT(.spi0_hw_code)

#endif
