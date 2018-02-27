#ifndef LBUF_H
#define LBUF_H

#include "typedef.h"
#include "list.h"


struct lbuff_head {
    struct list_head head;
    struct list_head free;
    //u16 index;
};


int lbuf_get_chunk_num(u32 total_size, u32 chunk_size);

struct lbuff_head *lbuf_init(void *buf, u32 buf_len);

void *lbuf_alloc(struct lbuff_head *head, u32 len);

void *lbuf_realloc(void *lbuf, int size);

void lbuf_push(void *lbuf);

void *lbuf_pop(struct lbuff_head *head);

void *lbuf_query_used(struct lbuff_head *head);

void lbuf_free(void *lbuf);

int lbuf_empty(struct lbuff_head *head);

void lbuf_clear(struct lbuff_head *head);

bool lbuf_more_data(struct lbuff_head *head);

u32 lbuf_remain_len(struct lbuff_head *head, u32 len);

// void lbuf_debug(struct lbuff_head *head);
void lbuf_debug(struct lbuff_head *head, u8 flag);

#define AT_LBUF_CODE         AT(.lbuf_code)

#endif

