#ifndef __BT_CONFIG_H_
#define __BT_CONFIG_H_

// #define EMBEDDED

// #define HAVE_INIT_SCRIPT
// #define HAVE_EHCILL

#define HAVE_BZERO
//#define HAVE_TIME_MS
#define HAVE_TICK

// #define HAVE_HCI_DUMP
// #define ENABLE_LOG_INFO
// #define ENABLE_LOG_ERROR
#define ENABLE_BLE
// #define ENABLE_CLASSIC

#define HCI_ACL_PAYLOAD_SIZE                512//0x110

#define MAX_NO_BNEP_SERVICES                0
#define MAX_NO_BNEP_CHANNELS                0
#define MAX_NO_GATT_SUBCLIENTS              2
#define MAX_NO_HCI_CONNECTIONS              2
#define MAX_NO_L2CAP_SERVICES               0
#define MAX_NO_L2CAP_CHANNELS               0
#define MAX_NO_RFCOMM_MULTIPLEXERS          0
#define MAX_NO_RFCOMM_SERVICES              0
#define MAX_NO_RFCOMM_CHANNELS              0
#define MAX_NO_DB_MEM_DEVICE_LINK_KEYS      0
#define MAX_NO_DB_MEM_DEVICE_NAMES          0
#define MAX_NO_DB_MEM_SERVICES              0
#define MAX_NO_GATT_CLIENTS                 2
#define MAX_NO_WHITELIST_ENTRIES            0
#define MAX_NO_SM_LOOKUP_ENTRIES            0

///<BT Stack Code Segment Location
#define AT_BT_RF_CODE           //AT(.bt_rf_code)

#define AT_BLE_RF_CODE          //AT(.ble_rf_code)
#define AT_BLE_LL_PROBE_CODE    //AT(.ble_ll_probe_code)
#define AT_BLE_LL_CODE          //AT(.ble_ll_code)
#define AT_BLE_H4_CODE          //AT(.ble_h4_code)

#define AT_BLE_HCI_CODE         //AT(.ble_hci_code)
#define AT_BLE_L2CAP_CODE       //AT(.ble_l2cap_code)
#define AT_BLE_GAP_CODE         //AT(.ble_gap_code)

#define AT_BLE_ATT_CODE         //AT(.ble_att_code)
#define AT_BLE_ATT_SERVER_CODE  //AT(.ble_att_server_code)
#define AT_BLE_SM_CODE          //AT(.ble_sm_code)

#define AT_BLE_GATT_CLIENT_CODE //AT(.ble_gatt_client_code)
#define AT_BLE_ANCS_CLIENT_CODE //AT(.ble_ancs_client_code)

#define AT_BLE_MESH_PB_ADV_CODE

#define AT_BLE_APP_CODE         //AT(.ble_app_code)

#endif
