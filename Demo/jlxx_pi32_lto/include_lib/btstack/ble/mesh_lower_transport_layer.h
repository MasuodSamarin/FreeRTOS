#ifndef __MESH_LOWER_TRANSPORT_LAYER_H__
#define __MESH_LOWER_TRANSPORT_LAYER_H__

typedef struct {
    void (*packet_received)(const uint8_t *data, uint8_t len);
} mesh_lower_transport_layer_callback_t;

#endif /* __MESH_LOWER_TRANSPORT_LAYER_H__ */