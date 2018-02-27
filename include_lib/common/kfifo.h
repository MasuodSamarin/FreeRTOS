#ifndef KFIFO_HEADER_H
#define KFIFO_HEADER_H

#include "typedef.h"
// #include <pos/rtos.h>
#include "os/rtos.h"

//
#define pthread_mutex_init(a, b)        os_mutex_create(a)
#define pthread_mutex_lock(a)           os_mutex_pend(a, 0)
#define pthread_mutex_unlock(a)         os_mutex_post

typedef OS_MUTEX pthread_mutex_t;

//判断x是否是2的次方
#define IS_POWER_OF_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
//取a和b中最小值
// #define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct ring_buffer {
    void *buffer;     //缓冲区
    u32 size;       //大小
    u32 in;         //入口位置
    u32 out;        //出口位置
    pthread_mutex_t *f_lock;    //互斥锁
};

#endif
