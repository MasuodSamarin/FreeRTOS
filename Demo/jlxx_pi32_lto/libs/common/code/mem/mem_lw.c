#include "my_malloc.h"
#include "lbuf.h"

static u8 mem_pool[8 * 1024] AT(.mem_pool);

static struct lbuff_head *mem_pool_ptr;

void mem_init(void)
{
    mem_pool_ptr = lbuf_init(mem_pool, sizeof(mem_pool));
}

void *malloc(u32 size)
{
    void *mem;

    mem = lbuf_alloc(mem_pool_ptr, size);
    if (mem == NULL) {
        return NULL;
    }

    return mem;
}

void free(void *mem)
{
    lbuf_free(mem);
}
