/*********************************************************************************************
    *   Filename        : btstack_link_key_db.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:17

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __BTSTACK_LINK_KEY_DB_H
#define __BTSTACK_LINK_KEY_DB_H

// #include "bluetooth.h"

#if defined __cplusplus
extern "C" {
#endif

    /* API_START */

    typedef struct {

        // management
        void (*open)(void);
        void (*set_local_bd_addr)(bd_addr_t bd_addr);
        void (*close)(void);

        // link key
        int (*get_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t *type);
        void (*put_link_key)(bd_addr_t bd_addr, link_key_t link_key, link_key_type_t   type);
        void (*delete_link_key)(bd_addr_t bd_addr);

    } btstack_link_key_db_t;

    /* API_END */

#if defined __cplusplus
}
#endif

#endif // __BTSTACK_LINK_KEY_DB_H
