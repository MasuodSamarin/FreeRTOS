#ifndef __HID_SERVER_H__
#define __HID_SERVER_H__

#include "typedef.h"

// att handle
#define HID_SERVER_FIRST_HANDLE             0x0f
#define HID_CONSUMER_REPORT_HANDLE          (HID_SERVER_FIRST_HANDLE + 10)
#define HID_KEYBOARD_REPORT_HANDLE          (HID_SERVER_FIRST_HANDLE + 14)

// consumer key
#define CONSUMER_VOLUME_INC             0x0001
#define CONSUMER_VOLUME_DEC             0x0002
#define CONSUMER_PLAY_PAUSE             0x0004
#define CONSUMER_MUTE                   0x0008
#define CONSUMER_SCAN_PREV_TRACK        0x0010
#define CONSUMER_SCAN_NEXT_TRACK        0x0020
#define CONSUMER_SCAN_FRAME_FORWARD     0x0040
#define CONSUMER_SCAN_FRAME_BACK        0x0080

// keyboard key
#define KEYBOARD_VOLUME_UP              0x6
#define KEYBOARD_VOLUME_DOWN            0x7
/* #define KEYBOARD_VOLUME_UP          0x80 */
/* #define KEYBOARD_VOLUME_DOWN        0x81 */

// func declaration
void hid_server_init(void const *profile_data, void *read_callback, void *write_callback);

#endif // #define __HID_SERVER_H__
