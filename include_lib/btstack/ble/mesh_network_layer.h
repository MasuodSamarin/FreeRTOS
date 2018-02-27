#ifndef __MESH_NETWORK_LAYER_H__
#define __MESH_NETWORK_LAYER_H__

typedef struct {
    const uint8_t *packet;
    uint8_t len;
    uint8_t ref_count;
} mesh_network_packet_wrap_t;

struct mesh_network_interface {
    void *aux;
    void (*packet_received)(struct mesh_network_interface *interface,
                            const uint8_t *data, uint8_t len);
    void (*send_packet)(struct mesh_network_interface *interface,
                        mesh_network_packet_wrap_t *packet);
    void (*start)(void);
    void (*stop)(void);
};

typedef struct mesh_network_interface mesh_network_interface_t;

// wrap may be invalid after calling this function
// don't keep any ref to wrap
void mesh_network_packet_decrease_ref_count(
    mesh_network_packet_wrap_t *wrap);
void mesh_network_packet_increase_ref_count(
    mesh_network_packet_wrap_t *wrap);

void mesh_network_layer_init(void);
void mesh_network_layer_send_packet(const uint8_t *data, uint8_t len,
                                    uint32_t netkey_id, uint32_t element_id);

#endif /* __MESH_NETWORK_LAYER_H__ */