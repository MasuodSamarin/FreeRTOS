/*********************************************************************************************
    *   Filename        : gap_le.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-01-17 15:16

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __GAP_LE_H
#define __GAP_LE_H

#if defined __cplusplus
extern "C" {
#endif

#include <utils.h>


    /**
     * @brief Enable privacy by using random addresses
     * @param random_address_type to use (incl. OFF)
     */
    void gap_random_address_set_mode(gap_random_address_type_t random_address_type);

    /**
     * @brief Sets update period for random address
     * @param period_ms in ms
     */
    void gap_random_address_set_update_period(int period_ms);

    /**
     * @brief Updates the connection parameters for a given LE connection
     * @param handle
     * @param conn_interval_min (unit: 1.25ms)
     * @param conn_interval_max (unit: 1.25ms)
     * @param conn_latency
     * @param supervision_timeout (unit: 10ms)
     * @returns 0 if ok
     */
    // void gap_update_connection_parameters(hci_con_handle_t con_handle, uint16_t conn_interval_min,
    //                                       uint16_t conn_interval_max, uint16_t conn_latency, uint16_t supervision_timeout);

    /**
     * @brief Set data length for a given LE connection
     * @param connection_handle
     * @param txoctets  (27 ~ 251 bytes)
     * @param txtime (328 ~ 2120 us)
     */
    void gap_set_data_length(hci_con_handle_t con_handle, u16 txoctets, u16 txtime);
#if defined __cplusplus
}
#endif

#endif // __GAP_LE_H
