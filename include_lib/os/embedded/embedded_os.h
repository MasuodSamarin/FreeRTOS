#ifndef _EMBEDDED_OS_H_
#define _EMBEDDED_OS_H_

#include "typedef.h"
#include "list.h"

/*
 *********************************************************************************************************
 *                                             ERROR CODES
 *********************************************************************************************************
*/
enum {
    OS_NO_ERR = 0,
    OS_TRUE,
    OS_ERR_EVENT_TYPE,
    OS_ERR_PEND_ISR,
    OS_ERR_POST_NULL_PTR,
    OS_ERR_PEVENT_NULL,
    OS_ERR_POST_ISR,
    OS_ERR_QUERY_ISR,
    OS_ERR_INVALID_OPT,
    OS_ERR_TASK_WAITING,
    OS_ERR_PDATA_NULL,
    OS_TIMEOUT,
    OS_TASK_NOT_EXIST,
    OS_ERR_EVENT_NAME_TOO_LONG,
    OS_ERR_FLAG_NAME_TOO_LONG,
    OS_ERR_TASK_NAME_TOO_LONG,
    OS_ERR_PNAME_NULL,
    OS_ERR_TASK_CREATE_ISR,
    OS_MBOX_FULL,
    OS_Q_FULL,
    OS_Q_EMPTY,
    OS_Q_ERR,
    OS_ERR_NO_QBUF,
    OS_PRIO_EXIST,
    OS_NAME_EXIST,
    OS_STACK_TOO_LARGE,
    OS_PRIO_ERR,
    OS_PRIO_INVALID,
    OS_SEM_OVF,
    OS_TASK_DEL_ERR,
    OS_TASK_DEL_IDLE,
    OS_TASK_DEL_ISR,
    OS_NO_MORE_TCB,
    OS_TIME_NOT_DLY,
    OS_TIME_INVALID_MINUTES,
    OS_TIME_INVALID_SECONDS,
    OS_TIME_INVALID_MILLI,
    OS_TIME_ZERO_DLY,
    OS_TASK_SUSPEND_PRIO,
    OS_TASK_SUSPEND_IDLE,
    OS_TASK_RESUME_PRIO,
    OS_TASK_NOT_SUSPENDED,
    OS_MEM_INVALID_PART,
    OS_MEM_INVALID_BLKS,
    OS_MEM_INVALID_SIZE,
    OS_MEM_NO_FREE_BLKS,
    OS_MEM_FULL,
    OS_MEM_INVALID_PBLK,
    OS_MEM_INVALID_PMEM,
    OS_MEM_INVALID_PDATA,
    OS_MEM_INVALID_ADDR,
    OS_MEM_NAME_TOO_LONG,
    OS_ERR_MEM_NO_MEM,
    OS_ERR_NOT_MUTEX_OWNER,
    OS_TASK_OPT_ERR,
    OS_ERR_DEL_ISR,
    OS_ERR_CREATE_ISR,
    OS_FLAG_INVALID_PGRP,
    OS_FLAG_ERR_WAIT_TYPE,
    OS_FLAG_ERR_NOT_RDY,
    OS_FLAG_INVALID_OPT,
    OS_FLAG_GRP_DEPLETED,
    OS_ERR_PIP_LOWER,
    OS_ERR_MSG_POOL_LEN,
    OS_ERR_MSG_POOL_EMPTY,
    OS_ERR_MSG_POOL_NULL_PTR,
    OS_ERR_MSG_POOL_FULL,


};

typedef unsigned long QS;

typedef struct _OS_Q {                   /* QUEUE CONTROL BLOCK                                         */
    QS         OSQIn;
    QS         OSQOut;
    QS         OSQSize;             /* Size of queue (maximum number of entries)                   */
    QS         OSQEntries;          /* Current number of entries in the queue                      */
    void       **OSQStart;            /* Pointer to start of queue data                              */
} OS_Q;

typedef struct _OS_TCB {
    struct list_head entry;
    void (*task)(struct _OS_TCB *);
    char *name;
    u16 credits;
    u8 prio;
    u8 qsize;
    u8 queue[0];
    u32 name_crc32;
} OS_TCB;

typedef struct _OS_EVENT {
    struct list_head entry;
    OS_TCB *ptcb;
    u16 credits;
    u8 type;
    void *OSEventPtr;
} OS_EVENT;

typedef OS_EVENT OS_SEM, OS_MUTEX, OS_MSGQ;


#endif  /*_EMBEDDED_H_*/
