#ifndef _HCI_JL_CONTROLLER_H_
#define _HCI_JL_CONTROLLER_H_


#include "typedef.h"

/********************************************************************************/
/*
 *                  Vendor Controller Commands - JL Group
 *                  Manufacturer Specific
 */
#define HCI_WRITE_BT_ADDRESS                                0x0001
#define HCI_SET_LOCAL_INFO                                  0x0002
#define HCI_SET_PIN_CODE                                    0x0003
#define HCI_CANCEL_PAGE                                     0x0004
#define HCI_RESTART_SBC_CMD                                 0x000a
#define HCI_SYNC_OSC_CMD                                    0x000b
#define HCI_SYNC_SRC_CMD                                    0x000c
#define HCI_SYNC_SBC_CMD                                    0x000d
#define HCI_SLAVE_SNED_CMD                                  0x000f
#define HCI_TEST_KEY_CMD                                    0x0010

#endif
