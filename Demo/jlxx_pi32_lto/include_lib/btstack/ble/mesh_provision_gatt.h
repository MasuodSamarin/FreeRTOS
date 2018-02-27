#ifndef __MESH_PROVISION_GATT_H__
#define __MESH_PROVISION_GATT_H__

#include "ble/mesh_client_provision.h"

void mesh_pb_gatt_init(mesh_provision_generic_callback_t *cb);
void mesh_pb_gatt_send_packet(const uint8_t *packet, uint32_t len);
void mesh_pb_gatt_wait_for_packet(void);
void mesh_pb_gatt_start_provision(void);

#endif /* __MESH_PROVISION_GATT_H__ */