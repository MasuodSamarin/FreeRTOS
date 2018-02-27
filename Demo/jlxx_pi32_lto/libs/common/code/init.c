#include "init.h"


#define do_initcall(a) \
	do{ \
		extern initcall_t * a##_begin[];  \
		extern initcall_t * a##_end[];  \
		__do_initcall(a##_begin, a##_end); \
	}while(0)


static void __do_initcall(initcall_t *begin[], initcall_t *end[])
{
    initcall_t *func;

    for (func = (initcall_t *)begin; func < (initcall_t *)end; func++) {
        /* printf("func : %x\n", func); */
        (*func)();
    }
}


static void do_sys_initcall()
{
    do_initcall(_sys_initcall);

    do_initcall(_early_initcall);

    do_initcall(_initcall);

    do_initcall(_late_initcall);

    do_initcall(_app_initcall);
}

void system_init_call()
{
    do_sys_initcall();
}
