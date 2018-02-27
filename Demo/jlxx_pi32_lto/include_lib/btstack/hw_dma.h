#ifndef _HW_DMA_H_
#define _HW_DMA_H_

#include "typedef.h"
#include "list.h"

struct _dma {
    struct list_head entry;
};

struct _dma_head {
    struct list_head head;
};

enum {
    DMA_TYPE_RX = 0,
    DMA_TYPE_TX,
};

void hw_dma_debug(void);

void hw_dma_init(void *rx_dma, u32 rx_size, void *tx_dma, u32 tx_size);

void *hw_dma_alloc(u8 type, u32 size);

void *hw_dma_realloc(void *dma, u32 new_size);

void hw_dma_free(void *dma);

void hw_dma_push(struct _dma_head *head, struct _dma *dma);

void *hw_dma_pop(struct _dma_head *head);

int hw_dma_empty(struct _dma_head *head);

void hw_dma_clear(struct _dma_head *head);

#endif //_HW_DMA_H_
