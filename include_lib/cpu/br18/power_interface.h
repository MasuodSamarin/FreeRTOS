#ifndef _POWER_INTERFACE_H_
#define _POWER_INTERFACE_H_

#include "typedef.h"

#define RAM1_START      0x40000L
#define RAM1_SIZE       (24*1024L)
#define RAM1_END        (RAM1_START + RAM1_SIZE)

enum {
    MAGIC_ADDR = 2,
    ENTRY_ADDR = 3,
};
#define RAM1_MAGIC_ADDR (RAM1_END - MAGIC_ADDR*4)
#define RAM1_ENTRY_ADDR (RAM1_END - ENTRY_ADDR*4)


struct power_operation {
    u32(*get_timeout)(void *priv);

    void (*suspend_probe)(void *priv);

    void (*suspend_post)(void *priv, u32 usec);

    void (*resume)(void *priv, u32 usec);

    void (*off_probe)(void *priv);

    void (*off_post)(void *priv, u32 usec);

    void (*on)(void *priv, u32 usec);
};

struct power_driver {
    void (*init)();

    u32(*set_suspend_timer)(u32 usec);

    u32(*set_off_timer)(u32 usec);

    void (*suspend_enter)(void);

    void (*suspend_exit)(u32 usec);

    void (*off_enter)(void);

    void (*off_exit)(u32 usec);

    u32(*ioctrl)(int ctrl, ...);
};


// #define REGISTER_POWER_OPERATION(ops) \
// const struct bt_power_driver *__power_ops \
// __attribute__((section(".bt_power"))) = &ops
#define REGISTER_POWER_OPERATION(ops) \
	const struct power_driver *__power_ops \
			SEC(.power) = &ops

extern const struct power_driver *__power_ops;


#endif
