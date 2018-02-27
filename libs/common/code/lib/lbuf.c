#include "lbuf.h"

#define LOG_TAG     "[Lbuf]"
#define DEBUG_ENABLE
#include "debug.h"


#define __get_entry(lbuf)   \
	(struct hentry *)((u8 *)lbuf - sizeof(struct hentry))


struct hfree {
    struct list_head entry;
    u16 len;
};

struct hentry {
    struct list_head entry;
    struct lbuff_head *head;
    u16 len;
    u8 state;
    char ref;
};


int lbuf_get_chunk_num(u32 total_size, u32 chunk_size)
{
    return (total_size - sizeof(struct hfree)) / (chunk_size + sizeof(struct hentry) + 3);
}


AT_LBUF_CODE
struct lbuff_head *lbuf_init(void *buf, u32 buf_len)
{
    struct hfree *free;
    struct lbuff_head *head = buf;

    ASSERT((((u32)buf) & 0x03) == 0, "%x\n", buf);

    free = (struct hfree *)(head + 1);
    free->len = buf_len - sizeof(*head) - sizeof(*free);

    INIT_LIST_HEAD(&head->head);
    INIT_LIST_HEAD(&head->free);

    list_add_tail(&free->entry, &head->free);

    return head;
}


AT_LBUF_CODE
void *lbuf_alloc(struct lbuff_head *head, u32 len)
{
    struct hentry *entry;
    struct hfree *p;
    struct hfree *new;
    void *ret = NULL;
    CPU_SR_ALLOC();


    len += sizeof(*entry);
    if (len & 0x03) {
        len += 4 - (len & 0x03);
    }

    OS_ENTER_CRITICAL();

    list_for_each_entry(p, &head->free, entry) {
        if (p->len < len) {
            continue;
        }
        if (p->len > len + sizeof(struct hfree)) {
            new = (struct hfree *)((u8 *)p + len);
            new->len = p->len - len;
            __list_add(&new->entry, p->entry.prev, p->entry.next);
        } else {
            /* puts("***\n"); */
            len = p->len;
            __list_del_entry(&p->entry);
        }

        entry = (struct hentry *)p;
        entry->head = head;
        entry->state = 0;
        entry->len = len;
        entry->ref = 0;
        INIT_LIST_HEAD(&entry->entry);

        ret = entry + 1;
        /* printf("\n>>>alloc %x / %d / %d\n", entry, entry->len, len); */
        break;
    }
    if (ret == NULL) {
        /* puts("alloc-err\n"); */
        /* putchar('#'); */
    }

    OS_EXIT_CRITICAL();
    return ret;
}


AT_LBUF_CODE
void *lbuf_realloc(void *lbuf, int size)
{
    struct hentry *new;
    struct hentry *entry = __get_entry(lbuf);

    if (size & 0x03) {
        size += 4 - (size & 0x03);
    }

    ASSERT(size < entry->len);

    if (size > (entry->len - (sizeof(*entry)))) {
        /* puts("realloc error\n"); */
        /* printf("realloc entry->len %x / %d / %d\n", entry, entry->len, size + sizeof(*new) + sizeof(*entry)); */
        return NULL;
    }

    /* printf("@realloc entry->len %x / %d / %d\n", entry, entry->len, size); */
    if (size + sizeof(*new) + sizeof(*entry) < entry->len) {
        new = (struct hentry *)((u8 *)lbuf + size);
        new->head = entry->head;
        new->len = entry->len - sizeof(*entry) - size;
        new->ref = 0;
        INIT_LIST_HEAD(&new->entry);
        entry->len = sizeof(*entry) + size;
        /* printf("@entry->len %x/%d\n", entry, entry->len); */
        /* printf("@new->len %x/%d\n", new, new->len); */

        lbuf_free(new + 1);
    }
    return lbuf;
}


AT_LBUF_CODE
int lbuf_empty(struct lbuff_head *head)
{
    if (list_empty(&head->head)) {
        return 1;
    }
    return 0;
}



AT_LBUF_CODE
void lbuf_clear(struct lbuff_head *head)
{
    struct hentry *p, *n;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_for_each_entry_safe(p, n, &head->head, entry) {
        lbuf_free(p + 1);
    }

    OS_EXIT_CRITICAL();
}


AT_LBUF_CODE
void lbuf_push(void *lbuf)
{
    struct hentry *p = __get_entry(lbuf);
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    p->state = 0;
    if (p->ref++ == 0) {
        list_add_tail(&p->entry, &p->head->head);
    }

    OS_EXIT_CRITICAL();
}


AT_LBUF_CODE
bool lbuf_more_data(struct lbuff_head *head)
{
    struct hentry *p;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_for_each_entry(p, &head->head, entry) {
        if (p->state == 0) {        //bug : p->ref
            OS_EXIT_CRITICAL();
            return TRUE;
        }
    }

    OS_EXIT_CRITICAL();
    return FALSE;
}


AT_LBUF_CODE
void *lbuf_pop(struct lbuff_head *head)
{
    struct hentry *p;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();
    /*if (list_empty(&head->head)){
    	return NULL;
    }*/

    list_for_each_entry(p, &head->head, entry) {
        if (p->state == 0) {
            p->state = 1;
            OS_EXIT_CRITICAL();
            return p + 1;
        }
    }

    OS_EXIT_CRITICAL();
    return NULL;
}

void *lbuf_query_used(struct lbuff_head *head)
{
    struct hentry *p;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();

    list_for_each_entry(p, &head->head, entry) {
        if (p->state == 1) {
            OS_EXIT_CRITICAL();
            return p + 1;
        }
    }

    OS_EXIT_CRITICAL();
    return NULL;
}


AT_LBUF_CODE
void lbuf_free(void *lbuf)
{
    struct hfree *p;
    struct hfree *new;
    struct hfree *prev = NULL;
    struct hentry *entry;
    struct lbuff_head *head;
    CPU_SR_ALLOC();

    if (lbuf == NULL) {
        log_error("****lbuf nothing to be free***");
        /* ASSERT(0, "Nothing to free\n"); */
        return;
    }

    entry = __get_entry(lbuf);
    new = (struct hfree *)entry;
    head = entry->head;

    OS_ENTER_CRITICAL();

    __list_del_entry(&entry->entry);
    new->len = entry->len;
    /* printf("\n<<free %x/%d\n", new, new->len); */
    /* printf("new free len %d\n", new->len); */

    list_for_each_entry(p, &head->free, entry) {
        if ((p <= new) && ((u8 *)p + p->len > (u8 *)new)) {
            /* printf("free-err: %x\n", lbuf); */
            goto _exit;
        }
        if (p > new) {
            __list_add(&new->entry, p->entry.prev, &p->entry);
            goto __free;
        }
    }
    list_add_tail(&new->entry, &head->free);

__free:
    list_for_each_entry(p, &head->free, entry) {
        if (prev == NULL) {
            prev = p;
            continue;
        }
        /* printf("free p %x\n", p); */
        /* printf("prev len %x/%d\n", prev, prev->len); */

        if ((u32)prev + prev->len == (u32)p) {
            /* puts("ok\n"); */
            prev->len += p->len;
            __list_del_entry(&p->entry);
        } else {
            prev = p;
        }
    }

_exit:
    OS_EXIT_CRITICAL();
}

int puts(char *out);
void printf_buf(u8 *buf, u32 len);
void lbuf_debug(struct lbuff_head *head, u8 flag)
{
    struct hentry *entry;
    struct hentry *q;
    struct hfree *p;
    struct hfree *new;
    void *ret = NULL;
    CPU_SR_ALLOC();

    OS_ENTER_CRITICAL();
    if (flag) {
        /* puts("\n-----log\n"); */
    } else {
        puts("\n******log\n");
    }
    printf("begin : %x", head);
    printf_buf((u8 *)head, 16);

    list_for_each_entry(q, &head->head, entry) {
        printf("alloc %x - %d\n", q, q->len);
    }

    list_for_each_entry(p, &head->free, entry) {
        printf("free %x - %d\n", p, p->len);
        printf("freenext %x - %x\n", p, p->entry.next);
    }
    if (flag) {
        /* puts("-----log end\n"); */
    } else {
        puts("*****log end\n");
    }
    OS_EXIT_CRITICAL();
}

AT_LBUF_CODE
u32 lbuf_remain_len(struct lbuff_head *head, u32 len)
{
    struct hentry *entry;
    struct hfree *p;
    u32 res = 0;

    CPU_SR_ALLOC();

    len += sizeof(*entry);
    if (len & 0x3) {
        len += 4 - (len & 0x3);
    }

    OS_ENTER_CRITICAL();

    list_for_each_entry(p, &head->free, entry) {
        if (p->len < len) {
            continue;
        } else {
            res = 1;
            break;
        }
    }

    OS_EXIT_CRITICAL();

    return res;
}
