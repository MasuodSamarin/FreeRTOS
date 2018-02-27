#ifndef INIT_H
#define INIT_H

#include "typedef.h"

typedef void (*initcall_t)(void);

#define sys_initcall(fn)  \
	const initcall_t __initcall_##fn \
		SEC_USED(.sys.initcall) = fn

#define early_initcall(fn)  \
	const initcall_t __initcall_##fn \
		SEC_USED(.early.initcall) = fn

#define __initcall(fn)  \
	const initcall_t __initcall_##fn \
		SEC_USED(.initcall) = fn

#define late_initcall(fn)  \
	const initcall_t __initcall_##fn \
		SEC_USED(.late.initcall) = fn

#define app_initcall(fn)  \
	const initcall_t __initcall_##fn \
		SEC_USED(.app.initcall) = fn


struct handler {
    void (*open)(void);
    void (*close)(void);
};

#define app_handler(hdl)  \
    const struct handler hdl \
        SEC_USED(.app.handler)

void system_init_call();













#endif

