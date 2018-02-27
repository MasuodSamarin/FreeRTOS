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
			sec(.text) = &drv

extern const struct debug_driver *__debug_drv;

#endif
