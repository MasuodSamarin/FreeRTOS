/*********************************************************************************************
    *   Filename        : input_task.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-17 11:28

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "key.h"
#include "init.h"
#include "task_schedule.h"
#include "input_task.h"
#include "power_api.h"
#include "debug.h"


struct input_handle {
    void (*handler)(u32);
};

static struct input_handle hdl;

#define __this  (&hdl)

static u8 input_task_handler(u8 key_type, u8 status, u8 number)
{
    if (key_type == KEY_TYPE_AD) {
        /* printf("input task - AD %x - %x \n", status, number); */
        return ad_key_input_table[status][number];
    }

    if (key_type == KEY_TYPE_IO) {
        /* printf("input task - IO %x - %x \n", status, number); */
        return io_key_input_table[status][number];
    }

    if (key_type == KEY_TYPE_MATRIX) {
        return matrix_key_input_table[status][number];
    }

    return INPUT_KEY_INVALID;
}


static void input_task(void *args)
{
    /* if (!power_is_poweroff_post()) { */
    /* log_info("\n**********"); */
    /* log_info(TASK_NAME_INPUT); */
    /* log_info("**********\n"); */
    /* } */

    /* while (1);  */
    {

        u32 input;

        input = key_get_input();

        switch (input) {
        case INPUT_IO_KEY0_SHORT:
            /* log_info("INPUT_IO_KEY0_SHORT\n"); */
            break;
        case INPUT_IO_KEY1_SHORT:
            /* log_info("INPUT_IO_KEY1_SHORT\n"); */
            break;
        }

        if (__this->handler) {
            __this->handler(input);
        }
    }
}

static void input_task_init(void)
{
    if (!power_is_poweroff_post()) {
        log_info("app_initcall - user input task\n");
    }

    rtos_task_create(input_task, (void *)0, TASK_PRIO_INPUT, 10, TASK_NAME_INPUT);

    key_input_register_handler(input_task_handler);
}
/* app_initcall(input_task_init); */

void input_register_hanler(void (*handler)(u32))
{
    __this->handler = handler;
}
