/*********************************************************************************************
    *   Filename        : cbuf.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-19 17:56

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
/* #include <string.h> */
#include "cbuf.h"
/* #include "rtos/os_cpu.h" */

/* #define DEBUG_ENABLE */
#include "debug.h"



u32 cbuf_read(cbuffer_t *cbuffer, void *buf, u32 len)
{
    CPU_SR_ALLOC();
    u32 r_len = len;
    u32 copy_len;

    if (!cbuffer) {
        return 0;
    }

    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin;
    }

    if (cbuffer->data_len < len) {
        /* memset(buf, 0, len); */
        return 0;
    }


    copy_len = (u32)cbuffer->end - (u32)cbuffer->read_ptr;
    if (copy_len > len) {
        copy_len = len;
    }
    len -= copy_len;

    memcpy(buf, cbuffer->read_ptr, copy_len);
    //printf_data(cbuffer->read_ptr,copy_len) ;

    if (len == 0) {
        cbuffer->read_ptr += copy_len;
    } else {
        memcpy((u8 *)buf + copy_len, cbuffer->begin, len);
        //printf_data(cbuffer->begin,len);
        cbuffer->read_ptr = cbuffer->begin + len;
    }

    OS_ENTER_CRITICAL();
    cbuffer->tmp_len = cbuffer->data_len -= r_len;

    cbuffer->tmp_len = cbuffer->data_len;

    OS_EXIT_CRITICAL();

    return r_len;
}

u32 cbuf_prewrite(cbuffer_t *cbuffer, void *buf, u32 len)
{
    CPU_SR_ALLOC();
    u32 length;
    u32 remain_len;

    if (!cbuffer) {
        return 0;
    }

    if ((cbuffer->total_len - cbuffer->tmp_len) < len) {
        return 0;
    }


    length = (u32)cbuffer->end - (u32)cbuffer->tmp_ptr;
    if (length >= len) {
        memcpy(cbuffer->tmp_ptr, buf, len);
        cbuffer->tmp_ptr += len;
    } else {
        remain_len = len - length;
        memcpy(cbuffer->tmp_ptr, buf, length);
        memcpy(cbuffer->begin, ((u8 *)buf) + length, remain_len);
        cbuffer->tmp_ptr = (u8 *)cbuffer->begin + remain_len;
    }

    OS_ENTER_CRITICAL();
    cbuffer->tmp_len += len;
    OS_EXIT_CRITICAL();

    return len;
}

void cbuf_updata_prewrite(cbuffer_t *cbuffer)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    cbuffer->data_len = cbuffer->tmp_len;
    cbuffer->data_len = cbuffer->tmp_len;
    cbuffer->write_ptr = cbuffer->tmp_ptr;
    OS_EXIT_CRITICAL();

}
void  cbuf_discard_prewrite(cbuffer_t *cbuffer)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_ptr = cbuffer->write_ptr ;
    OS_EXIT_CRITICAL();
}
u32 cbuf_write(cbuffer_t *cbuffer, void *buf, u32 len)
{
    CPU_SR_ALLOC();
    u32 length;
    u32 remain_len;

    if (!cbuffer) {
        return 0;
    }

    if ((cbuffer->total_len - cbuffer->data_len) < len) {
        len = cbuffer->total_len - cbuffer->data_len ;
        if (len == 0) {
            return 0;
        }
    }


    length = (u32)cbuffer->end - (u32)cbuffer->write_ptr;
    if (length >= len) {
        memcpy(cbuffer->write_ptr, buf, len);
        cbuffer->write_ptr += len;
    } else {
        remain_len = len - length;
        memcpy(cbuffer->write_ptr, buf, length);
        memcpy(cbuffer->begin, ((u8 *)buf) + length, remain_len);
        cbuffer->write_ptr = (u8 *)cbuffer->begin + remain_len;
    }

    OS_ENTER_CRITICAL();
    cbuffer->data_len += len;
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_ptr = cbuffer->write_ptr ;
    OS_EXIT_CRITICAL();

    return len;
}

u32 cbuf_is_write_able(cbuffer_t *cbuffer, u32 len)
{
    u32 w_len;

    if (!cbuffer) {
        return 0;
    }
    w_len = cbuffer->total_len - cbuffer->data_len;
    if (w_len < len) {
        return 0;
    }

    return w_len;
}

void *cbuf_write_alloc(cbuffer_t *cbuffer, u32 *len)
{
    u32 data_len;

    if (!cbuffer) {
        /* cbuf_puts("null ptr \r\n") ; */
        return 0;
    }

    *len = cbuffer->end - cbuffer->write_ptr;
    data_len = cbuffer->total_len - cbuffer->data_len;
    if (*len == 0) {
        cbuffer->write_ptr = cbuffer->begin;
        *len = data_len;
    }
    if (*len > data_len) {
        *len = data_len;
    }
    return cbuffer->write_ptr;
}

void cbuf_write_updata(cbuffer_t *cbuffer, u32 len)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    cbuffer->tmp_ptr = cbuffer->write_ptr += len;
    cbuffer->tmp_len = cbuffer->data_len += len;
    OS_EXIT_CRITICAL();
}


void *cbuf_read_alloc(cbuffer_t *cbuffer, u32 *len)
{
    u32 data_len;

    if (!cbuffer) {
        /* cbuf_puts("null ptr \r\n") ; */
        return 0;
    }
    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin;
    }


    data_len = cbuffer->data_len ;

    *len  = (u32)cbuffer->end - (u32)cbuffer->read_ptr;
    if (data_len <= *len) {
        *len = data_len;
    }
    return cbuffer->read_ptr;
}

void cbuf_read_updata(cbuffer_t *cbuffer, u32 len)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();

    cbuffer->read_ptr += len;
    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin;
    }

    cbuffer->tmp_len =  cbuffer->data_len -= len;

    OS_EXIT_CRITICAL();
}



void cbuf_init(cbuffer_t *cbuffer, void *buf, u32 size)
{
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;
    cbuffer->begin = buf;
    cbuffer->read_ptr = buf;
    cbuffer->write_ptr = buf;
    cbuffer->tmp_ptr = buf;
    cbuffer->end = (u8 *)buf + size;
    cbuffer->total_len = size;
}


void cbuf_clear(cbuffer_t *cbuffer)
{
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    cbuffer->read_ptr = cbuffer->begin;
    cbuffer->tmp_ptr = cbuffer->write_ptr = cbuffer->begin;
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;
    OS_EXIT_CRITICAL();
}


u32 cbuf_rewrite(cbuffer_t *cbuffer, void *begin, void *buf, u32 len)
{
//    u32 sr;
    u32 length;
    u32 remain_len;
    u8 *write_ptr = (u8 *)begin ;
    if (!cbuffer) {
        return 0;
    }

    length = (u32)cbuffer->end - (u32)write_ptr;
    if (length >= len) {

        memcpy(cbuffer->write_ptr, buf, len);
        write_ptr += len;

    } else {

        remain_len = len - length;
        memcpy(write_ptr, buf, length);
        memcpy(cbuffer->begin, ((u8 *)buf) + length, remain_len);

    }

    return len;
}
