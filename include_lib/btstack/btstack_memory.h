/*********************************************************************************************
    *   Filename        : btstack_memory.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:13

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/



/*
 *  btstsack_memory.h
 *
 *  @brief BTstack memory management via configurable memory pools
 *
 */

#ifndef __BTSTACK_MEMORY_H
#define __BTSTACK_MEMORY_H

#if defined __cplusplus
extern "C" {
#endif

#include "btstack-config.h"

#include "hci.h"
#include "l2cap.h"
#include "classic/rfcomm.h"
#include "classic/bnep.h"
#include "remote_device_db.h"

#ifdef ENABLE_BLE
#include "ble/gatt_client.h"
#include "ble/sm.h"
#endif

    void btstack_memory_init(void);

    void le_btstack_memory_init(void);

    hci_connection_t *le_btstack_memory_hci_connection_get(void);
    void le_btstack_memory_hci_connection_free(hci_connection_t *hci_connection);
// hci_connection
    hci_connection_t *btstack_memory_hci_connection_get(void);
    void   btstack_memory_hci_connection_free(hci_connection_t *hci_connection);

// l2cap_service, l2cap_channel
    l2cap_service_t *btstack_memory_l2cap_service_get(void);
    void   btstack_memory_l2cap_service_free(l2cap_service_t *l2cap_service);
    l2cap_channel_t *btstack_memory_l2cap_channel_get(void);
    void   btstack_memory_l2cap_channel_free(l2cap_channel_t *l2cap_channel);

// rfcomm_multiplexer, rfcomm_service, rfcomm_channel
    rfcomm_multiplexer_t *btstack_memory_rfcomm_multiplexer_get(void);
    void   btstack_memory_rfcomm_multiplexer_free(rfcomm_multiplexer_t *rfcomm_multiplexer);
    rfcomm_service_t *btstack_memory_rfcomm_service_get(void);
    void   btstack_memory_rfcomm_service_free(rfcomm_service_t *rfcomm_service);
    rfcomm_channel_t *btstack_memory_rfcomm_channel_get(void);
    void   btstack_memory_rfcomm_channel_free(rfcomm_channel_t *rfcomm_channel);

// db_mem_device_name, db_mem_device_link_key, db_mem_service
    db_mem_device_name_t *btstack_memory_db_mem_device_name_get(void);
    void   btstack_memory_db_mem_device_name_free(db_mem_device_name_t *db_mem_device_name);
    db_mem_device_link_key_t *btstack_memory_db_mem_device_link_key_get(void);
    void   btstack_memory_db_mem_device_link_key_free(db_mem_device_link_key_t *db_mem_device_link_key);
    db_mem_service_t *btstack_memory_db_mem_service_get(void);
    void   btstack_memory_db_mem_service_free(db_mem_service_t *db_mem_service);

// bnep_service, bnep_channel
    bnep_service_t *btstack_memory_bnep_service_get(void);
    void   btstack_memory_bnep_service_free(bnep_service_t *bnep_service);
    bnep_channel_t *btstack_memory_bnep_channel_get(void);
    void   btstack_memory_bnep_channel_free(bnep_channel_t *bnep_channel);

#ifdef ENABLE_BLE
// gatt_client, gatt_subclient
    gatt_client_t *btstack_memory_gatt_client_get(void);
    void   btstack_memory_gatt_client_free(gatt_client_t *gatt_client);
    whitelist_entry_t *btstack_memory_whitelist_entry_get(void);
    void   btstack_memory_whitelist_entry_free(whitelist_entry_t *whitelist_entry);
    sm_lookup_entry_t *btstack_memory_sm_lookup_entry_get(void);
    void   btstack_memory_sm_lookup_entry_free(sm_lookup_entry_t *sm_lookup_entry);
#endif

    /*******************************************************************/
    /*
     *-------------------BTstack memory footprint
     */
#define BTSTACK_NV_MEMORY_SIZE                  (1024*2)
#define BTSTACK_CLASSIC_STATIC_MEMORY_SIZE      (3*1024)
#define BTSTACK_CLASSIC_DYNAMIC_MEMORY_SIZE     (512)

#define  BT_MEM_CLASSIC     BIT(0)
#define  BT_MEM_LE          BIT(1)
#define  BT_MEM_COMMON      BIT(2)


    enum {
        BT_CLASS_MEM_STATIC = 0,
        BT_CLASS_MEM_DYNAMIC,
    };

    void *bt_classic_malloc(u8 type, int size);

    void bt_classic_free(void *p);

    void btstack_memory_apply(u8 mode);

    void btstack_memory_release(u8 mode);

    void btstack_nv_memory_apply(void);

    void btstack_nv_memory_release(void);

    void *btstack_nv_memory_malloc(int size);

    void btstack_nv_memory_free(void *p);

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_MEMORY_H

