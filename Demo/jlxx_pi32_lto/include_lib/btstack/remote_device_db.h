/*********************************************************************************************
    *   Filename        : remote_device_db.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:15

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

/**
 * interface to provide link key and remote name storage
 */

#ifndef __REMOTE_DEVICE_DB_H
#define __REMOTE_DEVICE_DB_H

#include <utils.h>
#include "gap.h"

#if defined __cplusplus
extern "C" {
#endif

    typedef struct {

        // management
        void (*open)(void);
        void (*close)(void);

        // link key
        int (*get_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t *type);
        void (*put_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t   type);
        void (*delete_link_key)(bd_addr_t bd_addr);

        // remote name
        int (*get_name)(bd_addr_t bd_addr, device_name_t *device_name);
        void (*put_name)(bd_addr_t bd_addr, device_name_t *device_name);
        void (*delete_name)(bd_addr_t bd_addr);

        // persistent rfcomm channel
        uint8_t (*persistent_rfcomm_channel)(char *servicename);

    } remote_device_db_t;

    extern       remote_device_db_t remote_device_db_iphone;
    extern const remote_device_db_t remote_device_db_memory;
    extern const remote_device_db_t remote_device_db_fs;

// MARK: non-persisten implementation
#include <linked_list.h>
#define MAX_NAME_LEN 32
    typedef struct {
        // linked list - assert: first field
        linked_item_t    item;

        bd_addr_t bd_addr;
    } db_mem_device_t;

    typedef struct {
        db_mem_device_t device;
        link_key_t link_key;
        link_key_type_t link_key_type;
    } db_mem_device_link_key_t;

    typedef struct {
        db_mem_device_t device;
        char device_name[MAX_NAME_LEN];
    } db_mem_device_name_t;

    typedef struct {
        // linked list - assert: first field
        linked_item_t    item;

        char service_name[MAX_NAME_LEN];
        uint8_t channel;
    } db_mem_service_t;

#if defined __cplusplus
}
#endif

#endif // __REMOTE_DEVICE_DB_H
