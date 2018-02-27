/*********************************************************************************************
    *   Filename        : input_task.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-02-17 12:03

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef _INPUT_TASK_H_
#define _INPUT_TASK_H_

#include "cpu.h"

/**
 * @brief input_register_hanler : register input callback
 *
 * @param handler :
 */
void input_register_hanler(void (*handler)(u32));

#endif
