#ifndef __MESH_CLIENT_PROVISION_H__
#define __MESH_CLIENT_PROVISION_H__

typedef struct {
    void (*link_open)(uint32_t link_id);
    void (*link_closed)(uint32_t link_id);
    void (*timeout)();
    void (*packet_received)(const uint8_t *packet, uint32_t len);
    void (*trans_idle)();
} mesh_provision_generic_callback_t;

typedef struct {
    void (*init)(mesh_provision_generic_callback_t *cb);
    void (*start)(void);
    void (*wait_for_packet)(void);
    void (*send_packet)(const uint8_t *packet, uint32_t len);
} mesh_provision_bearer_callback_t;


void mesh_client_provision_init(mesh_provision_bearer_callback_t *cb);
void mesh_client_provision_start(void);

#endif /* __MESH_CLIENT_PROVISION_H__ */
