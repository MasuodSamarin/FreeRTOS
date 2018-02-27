/*********************************************************************************************
    *   Filename        : le_device_db.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __LE_DEVICE_DB_H
#define __LE_DEVICE_DB_H

#include <utils.h>

#if defined __cplusplus
extern "C" {
#endif

    /**

    	LE Device DB for pure LE Peripherals is only required for signed writes

    	Per bonded device, it can store
    	- it stores the Identity Resolving Key (IRK) and its address to resolve private addresses
        - it stores the LTK + EDIV, RAND. EDIV + RAND allow a LE Perihperal to reconstruct the LTK
        - it stores the Connection Signature Resolving Key (CSRK) and the last used counter.
        	The CSRK is used to generate the signatur on the remote device and is needed to verify the signature itself
    		The Counter is necessary to prevent reply attacks

    */


// LE Device db interface


    /**
     * @brief init
     */
    void le_device_db_init(void);

    /**
     * @brief add device to db
     * @param addr_type, address of the device
     * @param irk of the device
     * @returns index if successful, -1 otherwise
     */
    int le_device_db_add(int addr_type, bd_addr_t addr, sm_key_t irk);

    /**
     * @brief get number of devices in db for enumeration
     * @returns number of device in db
     */
    int le_device_db_count(void);

    /**
     * @brief get device information: addr type and address needed to identify device
     * @param index
     * @param addr_type, address of the device as output
     * @param irk of the device
     */
    void le_device_db_info(int index, int *addr_type, bd_addr_t addr, sm_key_t irk);


    /**
     * @brief set remote encryption info
     * @brief index
     * @brief ediv
     * @brief rand
     * @brief ltk
     * @brief key size
     * @brief authenticated
     * @brief authorized
     */
    void le_device_db_encryption_set(int index, uint16_t ediv, uint8_t rand[8], sm_key_t ltk, int key_size, int authenticated, int authorized);

    /**
     * @brief get remote encryption info
     * @brief index
     * @brief ediv
     * @brief rand
     * @brief ltk
     * @brief key size
     * @brief authenticated
     * @brief authorized
     */
    void le_device_db_encryption_get(int index, uint16_t *ediv, uint8_t rand[8], sm_key_t ltk,  int *key_size, int *authenticated, int *authorized);

    /**
     * @brief set local signing key for this device
     * @param index
     * @param signing key as input
     */
    void le_device_db_local_csrk_set(int index, sm_key_t csrk);

    /**
     * @brief get local signing key for this device
     * @param index
     * @param signing key as output
     */
    void le_device_db_local_csrk_get(int index, sm_key_t csrk);

    /**
     * @brief set remote signing key for this device
     * @param index
     * @param signing key as input
     */
    void le_device_db_remote_csrk_set(int index, sm_key_t csrk);

    /**
     * @brief get remote signing key for this device
     * @param index
     * @param signing key as output
     */
    void le_device_db_remote_csrk_get(int index, sm_key_t csrk);

    /**
     * @brief query last used/seen signing counter
     * @param index
     * @returns next expected counter, 0 after devices was added
     */
    uint32_t le_device_db_remote_counter_get(int index);

    /**
     * @brief update signing counter
     * @param index
     * @param counter to store
     */
    void le_device_db_remote_counter_set(int index, uint32_t counter);

    /**
     * @brief query last used/seen signing counter
     * @param index
     * @returns next expected counter, 0 after devices was added
     */
    uint32_t le_device_db_local_counter_get(int index);

    /**
     * @brief update signing counter
     * @param index
     * @param counter to store
     */
    void le_device_db_local_counter_set(int index, uint32_t counter);

    /**
     * @brief free device
     * @param index
     */
    void le_device_db_remove(int index);

    void le_device_db_dump(void);

    /* API_END */

#if defined __cplusplus
}
#endif

#endif // __LE_DEVICE_DB_H
