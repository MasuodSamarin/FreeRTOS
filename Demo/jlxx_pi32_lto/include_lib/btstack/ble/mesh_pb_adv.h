#ifndef __MESH_PB_ADV_H__
#define __MESH_PB_ADV_H__

#include "ble/mesh_client_provision.h"

void mesh_pb_adv_init(mesh_provision_generic_callback_t *cb);
void mesh_pb_adv_send_packet(const uint8_t *packet, uint32_t len);
void mesh_pb_adv_wait_for_packet(void);
void mesh_pb_adv_start_provision();

#endif /* __MESH_PB_ADV_H__*/