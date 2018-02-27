/*********************************************************************************************
    *   Filename        : memory_pool.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:15

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

/*
 *  memory_pool.h
 *
 *  @Brief Fixed-size block allocation
 *
 *  @Assumption block_size >= sizeof(void *)
 *  @Assumption size of storage >= count * block_size
 *
 *  @Note minimal implementation, no error checking/handling
 */

#ifndef __MEMORY_POOL_H
#define __MEMORY_POOL_H

#if defined __cplusplus
extern "C" {
#endif

    typedef void *memory_pool_t;

// initialize memory pool with with given storage, block size and count
    void   memory_pool_create(memory_pool_t *pool, void *storage, int count, int block_size);

// get free block from pool, @returns NULL or pointer to block
    void *memory_pool_get(memory_pool_t *pool);

// return previously reserved block to memory pool
    void   memory_pool_free(memory_pool_t *pool, void *block);

#if defined __cplusplus
}
#endif

#endif // __MEMORY_POOL_H
