/*********************************************************************************************
    *   Filename        : mem_heap.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 15:31

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include <string.h>
#include "mem_heap.h"
#include "printf.h"
#include "pmalloc.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"

typedef unsigned char u8_t;
typedef signed char err_t;
typedef unsigned long u32_t, mem_ptr_t;
typedef unsigned int mem_size_t;
void *sbrk(void *addr, int incr);
#define MEM_HEAP_NO_SYS                      0     //0:使用操作系统,1:不使用操作系统

#define HAVE_REALLOC                         1

#define MEM_HEAP_ERROR_CHECK                 0
#define MEM_HEAP_CLR_MAGIC                   (u8_t)0xff
#define MEM_HEAP_OVERFLOW_MAGIC              0x53555aca

/** All allocated blocks will be MEM_HEAP_MIN_SIZE bytes big, at least!
 * MEM_HEAP_MIN_SIZE can be overridden to suit your needs. Smaller values save space,
 * larger values could prevent too small blocks to fragment the RAM too much. */
#define MEM_HEAP_MIN_SIZE                    MEM_HEAP_ALIGNMENT

#define MEM_HEAP_STATS                       1   //统计功能


/** print debug message only if debug message type is enabled...
 */
#define MEM_HEAP_PLATFORM_ASSERT(x) \
    do \
    {   printf("Assertion \"%s\" failed at line %d in %s\n", x, __LINE__, __FILE__);while(1); \
    } while(0)

#define MEM_HEAP_PLATFORM_DIAG(x)  do {printf x;} while(0)

//#define MEM_HEAP_NOASSERT

/** flag for LWIP_DEBUGF to enable that debug message */
#define MEM_HEAP_DBG_ON                     0x80U
/** flag for LWIP_DEBUGF to disable that debug message */
#define MEM_HEAP_DBG_OFF                    0x00U
/** flag for LWIP_DEBUGF to halt after printing this debug message */
#define MEM_HEAP_DBG_HALT                   0x08U

#define MEM_HEAP_DEBUG                      (MEM_HEAP_DBG_ON)

#define CHK_SBRK_PTR(ptr) do{       \
    if(ptr == (void*)-1)    \
    {                       \
        MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("%s,%d sbrk ret == NULL\n",__FUNCTION__,__LINE__ ));   \
        MEM_STATS_INC(err);     \
        MEM_HEAP_UNPROTECT();   \
        return (void*)0;        \
    }                       \
}while(0)
/*
   ------------------------------------
   ---------- MEM_HEAP options ----------
   ------------------------------------
*/
#define MEM_HEAP_MALLOC_THRESHOLD         10*1024

/**
 * MEM_HEAP_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_HEAP_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_HEAP_ALIGNMENT 2
 */
#define MEM_HEAP_ALIGNMENT                   16

#define SBRK_PAGE_ALIGNMENT   PAGE_SIZE
#define SBRK_PAGE_ALIGN_SIZE(size) (((size) + SBRK_PAGE_ALIGNMENT - 1) & ~(SBRK_PAGE_ALIGNMENT-1))
#define SBRK_PTR_ALIGN_FORWARD(ptr) (u8_t *)(((mem_ptr_t)(ptr)) & ~(mem_ptr_t)(SBRK_PAGE_ALIGNMENT-1))
#define SBRK_PTR_ALIGN_BACKWARD(ptr) (u8_t *)(((mem_ptr_t)(ptr) + SBRK_PAGE_ALIGNMENT - 1) & ~(mem_ptr_t)(SBRK_PAGE_ALIGNMENT-1))

#ifdef MEM_HEAP_DEBUG

#define MEM_HEAP_DEBUGF(debug, message) do { \
                               if ((debug) & MEM_HEAP_DBG_ON) { \
                                 MEM_HEAP_PLATFORM_DIAG(message); \
                                 if ((debug) & MEM_HEAP_DBG_HALT) { \
                                   while(1); \
                                 } \
                               } \
                             } while(0)
#else  /* MEM_HEAP_DEBUG */
#define MEM_HEAP_DEBUGF(debug, message)
#endif /* MEM_HEAP_DEBUG */

#ifndef MEM_HEAP_NOASSERT
#define MEM_HEAP_ASSERT(message, assertion) do { if(!(assertion)) \
  MEM_HEAP_PLATFORM_ASSERT(message); } while(0)
#else  /* MEM_HEAP_NOASSERT */
#define MEM_HEAP_ASSERT(message, assertion)
#endif /* MEM_HEAP_NOASSERT */


#if MEM_HEAP_STATS

struct stats_mem {
#ifdef MEM_HEAP_DEBUG
    const char *name;
#endif /* MEM_HEAP_DEBUG */
    mem_size_t avail;
    mem_size_t used;
    mem_size_t max;
    u32_t err;
    u32_t illegal;
};

struct stats_ {
    struct stats_mem mem;
};

#define STATS_INC(x) ++mem_heap.mem_stats.x
#define STATS_DEC(x) --mem_heap.mem_stats.x
#define STATS_INC_USED(x, y) do { mem_heap.mem_stats.x.used += y; \
                                if (mem_heap.mem_stats.x.max < mem_heap.mem_stats.x.used) { \
                                    mem_heap.mem_stats.x.max = mem_heap.mem_stats.x.used; \
                                } \
                             } while(0)

#define MEM_STATS_AVAIL(x, y) mem_heap.mem_stats.mem.x = y
#define MEM_STATS_INC(x) STATS_INC(mem.x)
#define MEM_STATS_INC_USED(x, y) STATS_INC_USED(mem, y)
#define MEM_STATS_DEC_USED(x, y) mem_heap.mem_stats.mem.x -= y

#else
#define MEM_STATS_AVAIL(x, y)
#define MEM_STATS_INC(x)
#define MEM_STATS_INC_USED(x, y)
#define MEM_STATS_DEC_USED(x, y)
#endif


#if !MEM_HEAP_NO_SYS

#include "os/rtos.h"

typedef OS_EVENT sys_mutex_t; // type of mutex

/* Protect the heap only by using a semaphore */
#define MEM_HEAP_WAIT_MUTEX_TIMEOUT    0
#define MEM_HEAP_DECL_PROTECT()   u16   oserr
#define MEM_HEAP_PROTECT()     os_mutex_pend(&mem_heap.mem_mutex,MEM_HEAP_WAIT_MUTEX_TIMEOUT)
#define MEM_HEAP_UNPROTECT()   os_mutex_post(&mem_heap.mem_mutex)

#else /* !MEM_HEAP_NO_SYS */
#define sys_mutex_new()   0
#define MEM_HEAP_DECL_PROTECT()
#define MEM_HEAP_PROTECT()
#define MEM_HEAP_UNPROTECT()
#endif

/**
 * The heap is made up as a list of structs of this type.
 * This does not have to be aligned since for getting its size,
 * we only use the macro SIZEOF_STRUCT_MEM, which automatically alignes.
 */
#define MEM_USED   0x5a5aeaae
#define MEM_UNUSED 0xa5abcdef
struct mem {
#if MEM_HEAP_ERROR_CHECK == 1
    u32_t check_before;
#endif
    /** index of the next struct */
    struct mem *next;
    /** index of the previous struct */
    struct mem *prev;
    /** index of mem size */
    u32_t size;
    /** MEM_USED: this area is used; MEM_UNUSED: this area is unused */
    u32_t used;
#if MEM_HEAP_ERROR_CHECK == 1
    u32_t check_after;
#endif
};

/* some alignment macros: we define them here for better source code layout */
#define MIN_SIZE_ALIGNED     MEM_HEAP_ALIGN_SIZE(MEM_HEAP_MIN_SIZE)
#define SIZEOF_STRUCT_MEM    MEM_HEAP_ALIGN_SIZE(sizeof(struct mem))
#define MEM_SIZE_ALIGNED     MEM_HEAP_ALIGN_SIZE(MEM_HEAP_SIZE)

/** Calculate memory size for an aligned buffer - returns the next highest
 * multiple of MEM_HEAP_ALIGNMENT (e.g. MEM_HEAP_ALIGN_SIZE(3) and
 * MEM_HEAP_ALIGN_SIZE(4) will both yield 4 for MEM_HEAP_ALIGNMENT == 4).
 */
#define MEM_HEAP_ALIGN_SIZE(size) (((size) + MEM_HEAP_ALIGNMENT - 1) & ~(MEM_HEAP_ALIGNMENT-1))

/** Align a memory pointer to the alignment defined by MEM_HEAP_ALIGNMENT
 * so that ADDR % MEM_HEAP_ALIGNMENT == 0
 */
#define MEM_HEAP_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_HEAP_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_HEAP_ALIGNMENT-1)))

struct _mem_heap {
    /** pointer to the heap (ram_heap): for alignment, ram is now a pointer instead of an array */
    struct mem *ram;
    /** the last entry, always used! */
    struct mem *ram_end;
    /** pointer to the lowest free block, this is used for faster search */
//    struct mem *lfree;
    /** record total mem_heap size */
//    mem_ptr_t mem_heap_size;
#if !MEM_HEAP_NO_SYS
    sys_mutex_t mem_mutex;
#endif
#if MEM_HEAP_STATS
    struct stats_ mem_stats;
#endif
    u8_t mem_init;
};
static struct _mem_heap mem_heap = {
    .mem_init = 0,
};

#define MEM_VER  "MEM 100 "
char *get_mem_lib_version(void)
{
    return MEM_VER;
}

#if MEM_HEAP_STATS
void mem_stats_init(void)
{
#ifdef MEM_HEAP_DEBUG
    mem_heap.mem_stats.mem.name = "MEM_HEAP";
#endif
}

void malloc_stats(void)
{
    MEM_HEAP_PLATFORM_DIAG(("\nMEM_HEAP %s\n\t", mem_heap.mem_stats.mem.name));
    MEM_HEAP_PLATFORM_DIAG(("avail: %u\n\t", (u32_t)mem_heap.mem_stats.mem.avail));
    MEM_HEAP_PLATFORM_DIAG(("used: %u\n\t", (u32_t)mem_heap.mem_stats.mem.used));
    MEM_HEAP_PLATFORM_DIAG(("max: %u\n\t", (u32_t)mem_heap.mem_stats.mem.max));
    MEM_HEAP_PLATFORM_DIAG(("err: %u\n", (u32_t)mem_heap.mem_stats.mem.err));
}

void get_malloc_stats(void *arg)
{
    struct stats_mem *mem_sta;

    if (arg != NULL) {
        mem_sta = (struct stats_mem *)arg;

        mem_sta->name = mem_heap.mem_stats.mem.name;
        mem_sta->avail = mem_heap.mem_stats.mem.avail;
        mem_sta->used = mem_heap.mem_stats.mem.used;
        mem_sta->max = mem_heap.mem_stats.mem.max;
        mem_sta->err = mem_heap.mem_stats.mem.err;
    }
}

#endif

#if !MEM_HEAP_NO_SYS
static err_t sys_mutex_new(void)
{
    u16 err;
    err = os_mutex_create(&mem_heap.mem_mutex) ;

    MEM_HEAP_ASSERT("sys_mutex_new ", err == OS_NO_ERR);
    if (err) {
        return err;
    }

    return 0;
}
#endif

/**
 * "Plug holes" by combining adjacent empty struct mems.
 * After this function is through, there should not exist
 * one empty struct mem pointing to another empty struct mem.
 *
 * @param mem this points to a struct mem which just has been freed
 * @internal this function is only called by mem_free() and mem_trim()
 *
 * This assumes access to the heap is protected by the calling function
 * already.
 */

static struct mem *
plug_holes(struct mem *mem)
{
    struct mem *mem2 = mem;
    struct mem *nmem;
    struct mem *pmem;

//  MEM_HEAP_ASSERT("plug_holes: mem >= ram", (u8_t *)mem >= mem_heap.ram);
//  MEM_HEAP_ASSERT("plug_holes: mem < ram_end", (u8_t *)mem < (u8_t *)mem_heap.ram_end);
    MEM_HEAP_ASSERT("plug_holes: mem->used == MEM_UNUSED", mem->used == MEM_UNUSED);

    /* plug hole forward */
//  MEM_HEAP_ASSERT("plug_holes: mem->next <= ram_end", mem->next <= mem_heap.ram_end);

    nmem = mem->next;
    if (mem != nmem && nmem->used == MEM_UNUSED && (u8_t *)nmem != (u8_t *)mem_heap.ram_end
        && ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)nmem)) {
        /* if mem->next is unused and not end of ram, combine mem and mem->next */
//        if (mem_heap.lfree == nmem)
//        {
//            mem_heap.lfree = mem;
//        }
        mem->next = nmem->next;
        nmem->next->prev = mem;
        mem->size += SIZEOF_STRUCT_MEM + nmem->size;
    }

    /* plug hole backward */
    pmem = mem->prev;
    if (pmem != mem && pmem->used == MEM_UNUSED
        && ((mem_size_t)pmem + SIZEOF_STRUCT_MEM + pmem->size == (mem_size_t)mem)) {
        /* if mem->prev is unused, combine mem and mem->prev */
//        if (mem_heap.lfree == mem)
//        {
//            mem_heap.lfree = pmem;
//        }
        pmem->next = mem->next;
        mem->next->prev = pmem;
        pmem->size += SIZEOF_STRUCT_MEM + mem->size;
        mem2 = pmem;
    }
    return mem2;
}

/**
 * Zero the heap and initialize start, end and lowest-free
 */

static void mem_heap_init(void)
{
    u32 count = 0;
    if (mem_heap.mem_init) {
        return ;
    }

#if MEM_HEAP_STATS
    mem_stats_init();
#endif

    MEM_HEAP_ASSERT("Sanity check alignment", (SIZEOF_STRUCT_MEM & (MEM_HEAP_ALIGNMENT - 1)) == 0);

    struct mem *mem_end_prev;
    mem_heap.ram = sbrk(0, SBRK_PAGE_ALIGNMENT);
    log_info("mem_heap.ram : %x\n", mem_heap.ram);
    mem_end_prev = (struct mem *)sbrk((void *) - 1, SBRK_PAGE_ALIGNMENT);
    log_info("mem_end_prev  : %x\n", mem_end_prev);

    /* initialize the start of the heap */
    mem_heap.ram->next = mem_end_prev;
    mem_heap.ram->prev = NULL;
    mem_heap.ram->size = SBRK_PAGE_ALIGNMENT - SIZEOF_STRUCT_MEM;
    mem_heap.ram->used = MEM_UNUSED;

    /* initialize the end of the heap */
    mem_heap.ram_end = (struct mem *)((mem_size_t)mem_end_prev + SBRK_PAGE_ALIGNMENT - SIZEOF_STRUCT_MEM);
    mem_heap.ram_end->used = MEM_USED;
    mem_heap.ram_end->size = 0;
    mem_heap.ram_end->next = mem_heap.ram_end;
    mem_heap.ram_end->prev = mem_end_prev;
    log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));

    /* initialize the mem_end_prev */
    mem_end_prev->next = mem_heap.ram_end;
    mem_end_prev->prev = mem_heap.ram;
    mem_end_prev->size = SBRK_PAGE_ALIGNMENT - 2 * SIZEOF_STRUCT_MEM;
    mem_end_prev->used = MEM_UNUSED;

#if MEM_HEAP_ERROR_CHECK == 1
    mem_heap.ram->check_before = MEM_HEAP_OVERFLOW_MAGIC;
    mem_heap.ram->check_after = MEM_HEAP_OVERFLOW_MAGIC;
    mem_end_prev->check_before = MEM_HEAP_OVERFLOW_MAGIC;
    mem_end_prev->check_after = MEM_HEAP_OVERFLOW_MAGIC;
    mem_heap.ram_end->check_before = MEM_HEAP_OVERFLOW_MAGIC;
    mem_heap.ram_end->check_after = MEM_HEAP_OVERFLOW_MAGIC;
#endif

//    mem_heap.mem_heap_size = 2*SBRK_PAGE_ALIGNMENT;
    /* initialize the lowest-free pointer to the start of the heap */
//    mem_heap.lfree = mem_heap.ram;
    /* log_info_hexdump(pm_usd_map, sizeof(pm_usd_map)) ; */
    for (u32 i = 0; i < ARRAY_SIZE(pm_usd_map); i++) {
        /* log_info("mem page : %x\n", i); */
        u32 tmp = pm_usd_map[i];
        if (tmp) {
            for (u32 j = 0; j < 32; j++) {
                if (tmp & BIT(0)) {
                    count++;
                }
                tmp >>= 1;
            }
        }
    }


//    MEM_STATS_AVAIL(avail, (mem_size_t)mem_end_prev+SBRK_PAGE_ALIGNMENT-(mem_size_t)mem_heap.ram);
    log_info("free page count : %x\n", count);
    MEM_STATS_AVAIL(avail, count * PAGE_SIZE);
    MEM_STATS_INC_USED(used, 0);
    if (sys_mutex_new() != 0) {
        MEM_HEAP_ASSERT("failed to create mem_mutex", 0);
    }

    /* malloc_stats(); */

    mem_heap. mem_init = 1 ;

    log_info("+mem_heap_init over- .ram = 0x%x, mem_end_prev = 0x%x, .end = 0x%x\n ",
             mem_heap.ram, mem_end_prev, mem_heap.ram_end);
}

static void sbrk_free(struct mem *mem)
{
    struct mem *sbrk_ptr1, *sbrk_ptr2;
    u8_t combine_flag = 0;
    int sbrk_pages = 0;
    sbrk_ptr1 = (struct mem *)SBRK_PTR_ALIGN_BACKWARD(mem);
    sbrk_ptr2 = (struct mem *)(SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size));

    if (sbrk_ptr2 > sbrk_ptr1) {
        sbrk_pages = ((mem_size_t)sbrk_ptr2 - (mem_size_t)sbrk_ptr1) / SBRK_PAGE_ALIGNMENT;
    }
//log_info("+sbrk_pages: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",sbrk_ptr1, sbrk_ptr2, mem, mem->prev ,mem->next);
//log_info("+sbrk_pages_size: 0x%x, 0x%x, 0x%x\n",mem->size, mem->prev->size, mem->next->size);
    if (sbrk_pages > 0) {
        if (mem == mem_heap.ram) { //frist PAGE don't reback
            if (sbrk_pages == 1) {
                return ;
            }

            --sbrk_pages;
            struct mem *new_mem = (struct mem *)((mem_size_t)mem + SBRK_PAGE_ALIGNMENT - SIZEOF_STRUCT_MEM);
            new_mem->next = mem->next;
            new_mem->prev = mem;
            new_mem->used = MEM_UNUSED;
            new_mem->size = mem->size + SIZEOF_STRUCT_MEM - SBRK_PAGE_ALIGNMENT;
#if MEM_HEAP_ERROR_CHECK == 1
            new_mem->check_after = MEM_HEAP_OVERFLOW_MAGIC;
            new_mem->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
            mem->next->prev = new_mem;
            mem->next = new_mem;
            mem->size = SBRK_PAGE_ALIGNMENT - 2 * SIZEOF_STRUCT_MEM;
//            log_info("+new_ram8: 0x%x, 0x%x, 0x%x\n",new_mem->prev, new_mem, new_mem->next);

            mem = new_mem;
            sbrk_ptr1 = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM);
        }

        if ((mem_size_t)sbrk_ptr1 < (mem_size_t)mem + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED) {
            combine_flag = 1;
        }

        if (((mem_size_t)mem->next - (mem_size_t)sbrk_ptr2 >= SIZEOF_STRUCT_MEM)
            && ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)mem->next)) {
            mem->next->prev = sbrk_ptr2;
            sbrk_ptr2->next = mem->next;
            if (combine_flag) {
                sbrk_ptr2->prev = mem->prev;
            } else {
                sbrk_ptr2->prev = mem;
            }
            sbrk_ptr2->size = (mem_size_t)sbrk_ptr2->next - (mem_size_t)sbrk_ptr2 - SIZEOF_STRUCT_MEM;
            sbrk_ptr2->used = MEM_UNUSED;
#if MEM_HEAP_ERROR_CHECK == 1
            sbrk_ptr2->check_after = MEM_HEAP_OVERFLOW_MAGIC;
            sbrk_ptr2->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
            mem->next = sbrk_ptr2;
//    log_info("+new_ram0: 0x%x, 0x%x, 0x%x\n",sbrk_ptr2->prev, sbrk_ptr2, sbrk_ptr2->next);
        } else if ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)sbrk_ptr2) {
            if (combine_flag) {
                mem->next->prev = mem->prev;
            }
        }

//            else if(((mem_size_t)mem->next == (mem_size_t)sbrk_ptr2) && ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)mem->next))
//            {log_info("\n\n ----never happend555\n\n");while(1);}
//            else if((mem_size_t)mem == (mem_size_t)sbrk_ptr2)
//            {log_info("\n\n ----never happend4444\n\n");while(1);}
//            else if((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)sbrk_ptr1)
//            {log_info("\n\n ----never happend3333\n\n");while(1);}
        else {
            log_info("\n\n ----never happend\n\n");
            while (1);
        }
//            if(((mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + (mem_size_t)mem->prev->size != (mem_size_t)mem) && (sbrk_ptr1 != mem))
//            {log_info("\n\n ----never ^^^^^^^^^^happend\n\n");while(1);}

        if (combine_flag) {
            mem->prev->next =  mem->next;
            mem->prev->size += (mem_size_t)sbrk_ptr1 - (mem_size_t)mem;
        } else {
            mem->size = (mem_size_t)sbrk_ptr1 - (mem_size_t)mem - SIZEOF_STRUCT_MEM;
        }
        while (sbrk_pages--) {
            sbrk(sbrk_ptr1, 0 - SBRK_PAGE_ALIGNMENT);
            sbrk_ptr1 = (struct mem *)((u8_t *)sbrk_ptr1 + SBRK_PAGE_ALIGNMENT);
        }
    }
}
/**
 * Put a struct mem back on the heap
 *
 * @param rmem is the data portion of a struct mem as returned by a previous
 *             call to mem_malloc()
 */
void free(void *rmem)
{
    struct mem *mem;
//    MEM_HEAP_DECL_PROTECT();

    if (rmem == (void *)0) {
        return;
    }

    MEM_HEAP_ASSERT("mem_free: sanity check alignment", (((mem_ptr_t)rmem) & (MEM_HEAP_ALIGNMENT - 1)) == 0);

//  MEM_HEAP_ASSERT("mem_free: legal memory", (u8_t *)rmem >= (u8_t *)mem_heap.ram &&
//    (u8_t *)rmem < (u8_t *)mem_heap.ram_end);

//  if ((u8_t *)rmem < (u8_t *)mem_heap.ram || (u8_t *)rmem >= (u8_t *)mem_heap.ram_end) {
//    MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("mem_free: illegal memory\n"));
//    /* protect mem stats from concurrent access */
//    MEM_HEAP_PROTECT();
//    MEM_STATS_INC(illegal);
//    MEM_HEAP_UNPROTECT();
//    return;
//  }
    /* protect the heap from concurrent access */
    MEM_HEAP_PROTECT();

    /* Get the corresponding struct mem ... */
    mem = (struct mem *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
    MEM_STATS_DEC_USED(used, mem->size);
//log_info("FREE--> 0x%x,0x%x,0x%x,0x%x,0x%x\n",mem,mem->prev,mem->next,mem->size, mem->used);
#if MEM_HEAP_ERROR_CHECK == 1
    MEM_HEAP_ASSERT("mem_free: mem->check_before", mem->check_before == MEM_HEAP_OVERFLOW_MAGIC);
    MEM_HEAP_ASSERT("mem_free: mem->check_after", mem->check_after == MEM_HEAP_OVERFLOW_MAGIC);
    memset(rmem, MEM_HEAP_CLR_MAGIC, mem->size);
#endif

    /* ... which has to be in a used state ... */
    MEM_HEAP_ASSERT("mem_free: mem->used", mem->used == MEM_USED);
    /* ... and is now unused. */
    mem->used = MEM_UNUSED;

//    if (mem < mem_heap.lfree)
//    {
//        /* the newly freed struct is now the lowest */
//        mem_heap.lfree = mem;
//    }

//log_info("free mem: 0x%x, 0x%x, 0x%x\n",mem,mem->prev,mem->next);
    /* finally, see if prev or next are free also */
    mem = plug_holes(mem);
//log_info("free mem2: 0x%x, 0x%x, 0x%x\n",mem,mem->prev,mem->next);

    sbrk_free(mem);

    MEM_HEAP_UNPROTECT();
}

/**
 * Adam's mem_malloc() plus solution for bug #17922
 * Allocate a block of memory with a minimum of 'size' bytes.
 *
 * @param size is the minimum size of the requested block in bytes.
 * @return pointer to allocated memory or NULL if no free memory was found.
 *
 * Note that the returned value will always be aligned (as defined by MEM_HEAP_ALIGNMENT).
 */
static void *malloc_from_head(mem_size_t size)
{
    struct mem *mem, *mem2;
    struct mem *sbrk_lfree = 0;
    u8_t sbrk_find = 0;
    struct mem *sbrk_prev_mem;
    u8_t *sbrk_maddr;
    mem_size_t sbrk_msize;

//    MEM_HEAP_DECL_PROTECT();

    /* protect the heap from concurrent access */
    MEM_HEAP_PROTECT();

MALLOC:
    /* Scan through the heap searching for a free block that is big enough,
     * beginning with the lowest free block.
     */
//    for (sbrk_lfree? (mem = sbrk_lfree): (mem = mem_heap.lfree); mem != mem_heap.ram_end; mem = mem->next)
    for (sbrk_lfree ? (mem = sbrk_lfree) : (mem = mem_heap.ram); mem != mem_heap.ram_end; mem = mem->next) {
//        mem_byte('[');mem_u32hex0(mem->prev);mem_u32hex0(mem);mem_u32hex0(mem->next);mem_u32hex0(mem->size);mem_byte(']');
        if ((mem->used == MEM_UNUSED) && (mem->size >= size)) {
            /* mem is not used and at least perfect fit is possible:
             * mem->next - ((mem_ptr_t)mem + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem */
            if (mem->size >= (size + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED)) {
                /* (in addition to the above, we test if another struct mem (SIZEOF_STRUCT_MEM) containing
                 * at least MIN_SIZE_ALIGNED of data also fits in the 'user data space' of 'mem')
                 * -> split large block, create empty remainder,
                 * remainder must be large enough to contain MIN_SIZE_ALIGNED data: if
                 * mem->next - ((mem_ptr_t)mem + (2*SIZEOF_STRUCT_MEM)) == size,
                 * struct mem would fit in but no data between mem2 and mem2->next
                 * @todo we could leave out MIN_SIZE_ALIGNED. We would create an empty
                 *       region that couldn't hold data, but when mem->next gets freed,
                 *       the 2 regions would be combined, resulting in more free memory
                 */
                mem2 = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM + size);
                /* create mem2 struct */
                mem2->used = MEM_UNUSED;
                mem2->size = mem->size - size - SIZEOF_STRUCT_MEM;
                mem2->next = mem->next;
                mem2->prev = mem;
#if MEM_HEAP_ERROR_CHECK == 1
                mem2->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                mem2->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                /* and insert it between mem and mem->next */
                mem->next = mem2;
                mem->used = MEM_USED;
                mem->size = size;
                mem2->next->prev = mem2;
//log_info("+new_ram1: 0x%x, 0x%x, 0x%x, 0x%x\n",mem2->prev, mem2, mem2->next,mem2->size);
            } else {
                /* (a mem2 struct does no fit into the user data space of mem and mem->next will always
                 * be used at this point: if not we have 2 unused structs in a row, plug_holes should have
                 * take care of this).
                 * -> near fit or excact fit: do not split, no mem2 creation
                 * also can't move mem->next directly behind mem, since mem->next
                 * will always be used at this point!
                 */
                mem->used = MEM_USED;
            }
//            if (mem == mem_heap.lfree)
//            {
//                struct mem *cur = mem_heap.lfree;
//                /* Find next free block after mem and update lowest free pointer */
//                while ((cur->used == MEM_USED) && cur != mem_heap.ram_end)
//                {
//                    cur = cur->next;
//                }
//                mem_heap.lfree = cur;
//                MEM_HEAP_ASSERT("mem_malloc: !mem_heap.lfree->used", ((mem_heap.lfree == mem_heap.ram_end) || (mem_heap.lfree->used == MEM_UNUSED)));
//            }


//log_info("MALLOC--> 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",mem,mem->prev,mem->next,size, mem->size, mem->used);
            MEM_STATS_INC_USED(used, mem->size);
            MEM_HEAP_UNPROTECT();
//        MEM_HEAP_ASSERT("mem_malloc: allocated memory not above ram_end.",
//         (mem_ptr_t)mem + SIZEOF_STRUCT_MEM + size <= (mem_ptr_t)mem_heap.ram_end);
            MEM_HEAP_ASSERT("mem_malloc: allocated memory properly aligned.",
                            ((mem_ptr_t)mem + SIZEOF_STRUCT_MEM) % MEM_HEAP_ALIGNMENT == 0);
            MEM_HEAP_ASSERT("mem_malloc: sanity check alignment",
                            (((mem_ptr_t)mem) & (MEM_HEAP_ALIGNMENT - 1)) == 0);

            return (u8_t *)mem + SIZEOF_STRUCT_MEM;
        } else if (mem->used == MEM_USED) {
            if (!sbrk_find) {
                if ((mem_size_t)mem->next - (mem_size_t)mem - SIZEOF_STRUCT_MEM - (mem_size_t)mem->size >=  size + SIZEOF_STRUCT_MEM) {
                    sbrk_find = 1;
                    sbrk_prev_mem = mem;
                    sbrk_maddr = (u8_t *)((mem_ptr_t)mem + SIZEOF_STRUCT_MEM + mem->size);
                    sbrk_msize = size + SIZEOF_STRUCT_MEM;
                }
            }
        } else { //if(mem->used == MEM_UNUSED)
            if (!sbrk_find) {
                if (((mem_size_t)mem->next - (mem_size_t)mem - SIZEOF_STRUCT_MEM >= size) ||
                    ((mem->next->used == MEM_UNUSED) && ((mem_size_t)mem->next - (mem_size_t)mem + mem->next->size) >=  size)) {
                    sbrk_find = 1;
                    sbrk_prev_mem = mem;
                    sbrk_maddr = (u8_t *)((mem_ptr_t)mem + SIZEOF_STRUCT_MEM + mem->size);
                    if ((mem_size_t)mem->next - (mem_size_t)mem - SIZEOF_STRUCT_MEM >= size) {
                        sbrk_msize = size - mem->size;
                    } else {
                        sbrk_msize = (mem_size_t)mem->next - (mem_size_t)mem - SIZEOF_STRUCT_MEM - mem->size;
                    }
                }
            }
        }
    }

//SBRK_MEM:

//    MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, (" +SBRK_MEM\r\n"));

    if (sbrk_find) {
        struct mem *sbrk_ptr;
        mem_size_t mem_heap_incr_size;

        mem_heap_incr_size = SBRK_PAGE_ALIGN_SIZE(sbrk_msize);
        sbrk_ptr = sbrk(sbrk_maddr, mem_heap_incr_size);
        CHK_SBRK_PTR(sbrk_ptr);
        if (sbrk_msize < size) {
            sbrk_prev_mem->size = sbrk_prev_mem->size + mem_heap_incr_size;
            sbrk_lfree = sbrk_prev_mem;
        } else {
            sbrk_ptr->next = sbrk_prev_mem->next;
            sbrk_ptr->prev = sbrk_prev_mem;
            sbrk_ptr->used = MEM_UNUSED;
            sbrk_ptr->size = mem_heap_incr_size - SIZEOF_STRUCT_MEM;
#if MEM_HEAP_ERROR_CHECK == 1
            sbrk_ptr->check_after = MEM_HEAP_OVERFLOW_MAGIC;
            sbrk_ptr->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
            sbrk_ptr->next->prev = sbrk_ptr;
            sbrk_prev_mem->next = sbrk_ptr;
//log_info("+new_ram3: 0x%x, 0x%x, 0x%x\n",sbrk_ptr->prev, sbrk_ptr, sbrk_ptr->next);
            sbrk_lfree = sbrk_ptr;
        }

        sbrk_lfree = plug_holes(sbrk_lfree);

        goto  MALLOC;
    } else {
        MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("malloc_from_tail not find enough memory. could not allocate %d bytes\n", size));
        MEM_STATS_INC(err);
        MEM_HEAP_UNPROTECT();
        return (void *)0;
    }
}

static void *malloc_from_tail(mem_size_t size)
{
    struct mem *mem, *mem2;
    struct mem *sbrk_lfree = 0;
    struct mem *sbrk_ptr;
    u8_t *sbrk_maddr;
    mem_size_t sbrk_msize;

//    MEM_HEAP_DECL_PROTECT();

    /* protect the heap from concurrent access */
    MEM_HEAP_PROTECT();

MALLOC:
    /* Scan through the heap searching for a free block that is big enough,
     * beginning with the lowest free block.
     */
//    for (sbrk_lfree? (mem = sbrk_lfree): (mem = mem_heap.lfree); mem != mem_heap.ram_end; mem = mem->next)
    for (sbrk_lfree ? (mem = sbrk_lfree) : (mem = mem_heap.ram_end->prev); mem != NULL; mem = mem->prev) {
//        mem_byte('[');
//        mem_u32hex0(mem->prev);mem_u32hex0(mem->prev->used);mem_u32hex0(mem->prev->size);
//        mem_u32hex0(mem);mem_u32hex0(mem->used);mem_u32hex0(mem->size);
//        mem_u32hex0(mem->next);mem_u32hex0(mem->next->used);mem_u32hex0(mem->next->size);
//        mem_byte(']');

        if ((mem->used == MEM_UNUSED) && (mem->size >= size)) {
            /* mem is not used and at least perfect fit is possible:
             * mem->next - ((mem_ptr_t)mem + SIZEOF_STRUCT_MEM) gives us the 'user data size' of mem */
            if (mem->size >= (size + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED)) {
                /* (in addition to the above, we test if another struct mem (SIZEOF_STRUCT_MEM) containing
                 * at least MIN_SIZE_ALIGNED of data also fits in the 'user data space' of 'mem')
                 * -> split large block, create empty remainder,
                 * remainder must be large enough to contain MIN_SIZE_ALIGNED data: if
                 * mem->next - ((mem_ptr_t)mem + (2*SIZEOF_STRUCT_MEM)) == size,
                 * struct mem would fit in but no data between mem2 and mem2->next
                 * @todo we could leave out MIN_SIZE_ALIGNED. We would create an empty
                 *       region that couldn't hold data, but when mem->next gets freed,
                 *       the 2 regions would be combined, resulting in more free memory
                 */
                mem2 = (struct mem *)((mem_size_t)mem + mem->size - size);
                /* create mem2 struct */
                mem2->used = MEM_USED;
                mem2->size = size;
                mem2->next = mem->next;
                mem2->prev = mem;
                mem2->next->prev = mem2;
#if MEM_HEAP_ERROR_CHECK == 1
                mem2->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                mem2->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                /* and insert it between mem and mem->next */
                mem->next = mem2;
                mem->used = MEM_UNUSED;
                mem->size = mem->size - SIZEOF_STRUCT_MEM - size;

//log_info("+new_ram6: 0x%x, 0x%x, 0x%x, 0x%x\n",mem2->prev, mem2, mem2->next,mem2->size);
            } else {
                /* (a mem2 struct does no fit into the user data space of mem and mem->next will always
                 * be used at this point: if not we have 2 unused structs in a row, plug_holes should have
                 * take care of this).
                 * -> near fit or excact fit: do not split, no mem2 creation
                 * also can't move mem->next directly behind mem, since mem->next
                 * will always be used at this point!
                 */
                mem->used = MEM_USED;
                mem2 = mem;
            }
//            if (mem == mem_heap.lfree)
//            {
//                struct mem *cur = mem_heap.lfree;
//                /* Find next free block after mem and update lowest free pointer */
//                while ((cur->used == MEM_USED) && cur != mem_heap.ram_end)
//                {
//                    cur = cur->next;
//                }
//                mem_heap.lfree = cur;
//                MEM_HEAP_ASSERT("mem_malloc: !mem_heap.lfree->used", ((mem_heap.lfree == mem_heap.ram_end) || (mem_heap.lfree->used == MEM_UNUSED)));
//            }


//log_info("MALLOC--> 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",mem,mem->prev,mem->next,size, mem->size, mem->used);
            MEM_STATS_INC_USED(used, mem2->size);
            MEM_HEAP_UNPROTECT();
            return (u8_t *)mem2 + SIZEOF_STRUCT_MEM;
        } else if ((mem == mem_heap.ram) || ((mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size == (mem_size_t)mem)) {
            continue;
        } else if (mem->used == MEM_USED) {
            if ((mem->prev->used == MEM_USED)
                && (mem->prev->size != 0)
                && ((mem_size_t)mem - (mem_size_t)mem->prev - SIZEOF_STRUCT_MEM - mem->prev->size >= size + SIZEOF_STRUCT_MEM)) {
                sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem - size - SIZEOF_STRUCT_MEM);
                sbrk_msize = SBRK_PAGE_ALIGN_SIZE(size + SIZEOF_STRUCT_MEM);

                sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                CHK_SBRK_PTR(sbrk_ptr);

                sbrk_ptr->next = mem;
                sbrk_ptr->prev = mem->prev;
                sbrk_ptr->used = MEM_UNUSED;
                sbrk_ptr->size = sbrk_msize - SIZEOF_STRUCT_MEM;
#if MEM_HEAP_ERROR_CHECK == 1
                sbrk_ptr->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                sbrk_ptr->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                mem->prev->next = sbrk_ptr;
                mem->prev = sbrk_ptr;

                sbrk_lfree = sbrk_ptr;
//mem_puts("\n MMMAKR--------0");
                goto MALLOC;
            } else if ((mem->prev->used == MEM_UNUSED)
                       && ((mem_size_t)mem - (mem_size_t)mem->prev - SIZEOF_STRUCT_MEM >= size)) {
                if ((mem_size_t)mem - size - SIZEOF_STRUCT_MEM > (mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size) {
                    sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem - size - SIZEOF_STRUCT_MEM);
                    sbrk_msize = SBRK_PAGE_ALIGN_SIZE(size + SIZEOF_STRUCT_MEM);
                    sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                    CHK_SBRK_PTR(sbrk_ptr);
                    sbrk_ptr->next = mem;
                    sbrk_ptr->prev = mem->prev;
                    sbrk_ptr->used = MEM_UNUSED;
                    sbrk_ptr->size = sbrk_msize - SIZEOF_STRUCT_MEM;
#if MEM_HEAP_ERROR_CHECK == 1
                    sbrk_ptr->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                    sbrk_ptr->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                    mem->prev->next = sbrk_ptr;
                    mem->prev = sbrk_ptr;
//mem_puts("\n MMMAKR--------1");
                    goto MALLOC;
                } else if ((mem_size_t)mem - size - SIZEOF_STRUCT_MEM <= (mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size) {
                    sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size);
                    sbrk_msize = SBRK_PAGE_ALIGN_SIZE((mem_size_t)mem - (mem_size_t)sbrk_maddr);

                    sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                    CHK_SBRK_PTR(sbrk_ptr);
                    mem->prev->used = MEM_UNUSED;
                    mem->prev->size += sbrk_msize;

                    sbrk_lfree = mem->prev;
//mem_puts("\n MMMAKR--------2");

                    goto MALLOC;
                }
            }
        } else { //if(mem->used == MEM_UNUSED)
            if ((mem->prev->used == MEM_USED)
                && (mem->prev->size != 0)
                && ((mem_size_t)mem + mem->size - (mem_size_t)mem->prev - mem->prev->size >= size + SIZEOF_STRUCT_MEM)) {
                sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem + mem->size - size);
                sbrk_msize = SBRK_PAGE_ALIGN_SIZE((mem_size_t)mem - (mem_size_t)sbrk_maddr);

                sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                CHK_SBRK_PTR(sbrk_ptr);
                sbrk_ptr->next = mem->next;
                sbrk_ptr->prev = mem->prev;
                sbrk_ptr->used = MEM_UNUSED;
                sbrk_ptr->size = sbrk_msize + mem->size;
#if MEM_HEAP_ERROR_CHECK == 1
                sbrk_ptr->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                sbrk_ptr->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                mem->prev->next = sbrk_ptr;
                mem->next->prev = sbrk_ptr;

                sbrk_lfree = sbrk_ptr;
//mem_puts("\n MMMAKR--------3");
                goto MALLOC;
            } else if ((mem->prev->used == MEM_UNUSED)
                       && ((mem_size_t)mem + mem->size - (mem_size_t)mem->prev >= size)) {
                if ((mem_size_t)mem + mem->size - size > (mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size) {
                    sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem + mem->size - size);
                    sbrk_msize = SBRK_PAGE_ALIGN_SIZE((mem_size_t)mem - (mem_size_t)sbrk_maddr);
                    sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                    CHK_SBRK_PTR(sbrk_ptr);
                    sbrk_ptr->next = mem->next;
                    sbrk_ptr->prev = mem->prev;
                    sbrk_ptr->used = MEM_UNUSED;
                    sbrk_ptr->size = sbrk_msize + mem->size;
#if MEM_HEAP_ERROR_CHECK == 1
                    sbrk_ptr->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                    sbrk_ptr->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                    mem->prev->next = sbrk_ptr;
                    mem->next->prev = sbrk_ptr;
//mem_puts("\n MMMAKR--------4");
                    goto MALLOC;
                } else if ((mem_size_t)mem + mem->size - size <= (mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size) {
                    sbrk_maddr = SBRK_PTR_ALIGN_FORWARD((mem_size_t)mem->prev + SIZEOF_STRUCT_MEM + mem->prev->size);
                    sbrk_msize = SBRK_PAGE_ALIGN_SIZE((mem_size_t)mem - (mem_size_t)sbrk_maddr);
                    sbrk_ptr = sbrk(sbrk_maddr, sbrk_msize);
                    CHK_SBRK_PTR(sbrk_ptr);
                    mem->prev->used = MEM_UNUSED;
                    mem->prev->size += sbrk_msize + SIZEOF_STRUCT_MEM + mem->size;
                    mem->prev->next = mem->next;
                    mem->next->prev = mem->prev;
                    sbrk_lfree = mem->prev;
//mem_puts("\n MMMAKR--------5");

                    goto MALLOC;
                }
            }
        }
    }
    MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("malloc_from_tail not find enough memory. could not allocate %d bytes\n", size));
    MEM_STATS_INC(err);
    MEM_HEAP_UNPROTECT();
    return (void *)0;
}

void *malloc(mem_size_t size)
{
    void *ret;

    if (size == 0) {
        return (void *)0;
    }

    mem_heap_init();

    /* Expand the size of the allocated memory region so that we can
       adjust for alignment. */
    size = MEM_HEAP_ALIGN_SIZE(size);

    if (size < MIN_SIZE_ALIGNED) {
        size = MIN_SIZE_ALIGNED;
    }

    if (size > MEM_HEAP_MALLOC_THRESHOLD) {
        ret = malloc_from_tail(size);
    } else {
        ret = malloc_from_head(size);
    }

    return ret;
}

/**
 * Contiguously allocates enough space for count objects that are size bytes
 * of memory each and returns a pointer to the allocated memory.
 *
 * The allocated memory is filled with bytes of value zero.
 *
 * @param count number of objects to allocate
 * @param size size of the objects to allocate
 * @return pointer to allocated memory / NULL pointer if there is an error
 */
void *calloc(mem_size_t count, mem_size_t size)
{
    void *p;
    /* allocate 'count' objects of size 'size' */
    p = malloc(count * size);
    if (p) {
        /* zero the memory */
        memset(p, 0, count * size);
    }
    return p;
}

#if HAVE_REALLOC == 1
void *realloc(void *rmem, mem_size_t newsize)
{
#define USE_FIX_MALLOC 1
#if USE_FIX_MALLOC == 0
    mem_heap_init();

    void *new_rmem;
    struct mem *mem = (struct mem *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
    newsize = MEM_HEAP_ALIGN_SIZE(newsize);
    if (newsize < MIN_SIZE_ALIGNED) {
        newsize = MIN_SIZE_ALIGNED;
    }

    if (rmem == (void *)0) {
        rmem = malloc(newsize);
        return rmem;
    }

    if (newsize <= mem->size) {
        return rmem;
    }

    new_rmem = malloc(newsize);
    if (new_rmem != (void *)0) {
        memmove(new_rmem, rmem, mem->size);
        free(rmem);
        rmem = new_rmem;
        return rmem;
    } else {
        MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("mem_realloc: could not allocate %d bytes\n", newsize));
        return (void *)0;
    }
#else

    mem_size_t size;
    struct mem *new_mem;
    struct mem *mem, *mem2;
    void *new_rmem;
    /* use the FREE_PROTECT here: it protects with sem OR SYS_ARCH_PROTECT */
//    MEM_HEAP_DECL_PROTECT();

    /* Expand the size of the allocated memory region so that we can
       adjust for alignment. */
    newsize = MEM_HEAP_ALIGN_SIZE(newsize);

    if (newsize < MIN_SIZE_ALIGNED) {
        /* every data block must be at least MIN_SIZE_ALIGNED long */
        newsize = MIN_SIZE_ALIGNED;
    }

    mem_heap_init();

    if (rmem == (void *)0) {
        rmem = malloc(newsize);
        return rmem;
    }

//  MEM_HEAP_ASSERT("realloc: legal memory", ((u8_t *)rmem >= (u8_t *)mem_heap.ram &&
//   (u8_t *)rmem < (u8_t *)mem_heap.ram_end));
//
//  if ((u8_t *)rmem < (u8_t *)mem_heap.ram || (u8_t *)rmem >= (u8_t *)mem_heap.ram_end) {
//    MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("realloc: illegal memory\n"));
//    /* protect mem stats from concurrent access */
//    MEM_HEAP_PROTECT();
//    MEM_STATS_INC(illegal);
//    MEM_HEAP_UNPROTECT();
//    return rmem;
//  }

    /* protect the heap from concurrent access */
    MEM_HEAP_PROTECT();

    /* Get the corresponding struct mem ... */
    mem = (struct mem *)(void *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);
    /* ... and its offset pointer */
    size = mem->size;
    if (newsize > size) {
        if ((mem->next->used == MEM_UNUSED) &&
            ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)mem->next) &&
            ((mem_size_t)mem->size + SIZEOF_STRUCT_MEM + mem->next->size >= newsize)) {
            new_mem = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM + newsize);
            if ((mem_size_t)mem + 2 * SIZEOF_STRUCT_MEM + mem->size + mem->next->size - (mem_size_t)new_mem  >= SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED) {
//                if(mem_heap.lfree == mem->next)
//                {
//                    mem_heap.lfree = new_mem;
//                }
                log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
                log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
                while (1);
                if (mem_heap.ram_end->prev == mem->next) {
                    mem_heap.ram_end->prev = new_mem;
                }
                mem->next->next->prev = new_mem;
                new_mem->next = mem->next->next;
                new_mem->prev = mem;
                new_mem->used = MEM_UNUSED;
                new_mem->size = mem->size + mem->next->size - newsize;
#if MEM_HEAP_ERROR_CHECK == 1
                new_mem->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                new_mem->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                mem->next = new_mem;
                mem->size = newsize;
            } else {
//                if(mem_heap.lfree == mem->next)
//                {
//                    struct mem *cur = mem->next->next;
//                    while ((cur->used == MEM_USED) && cur != mem_heap.ram_end)
//                    {
//                        cur = cur->next;
//                    }
//                    mem_heap.lfree = cur;
//                }
                log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
                log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
                while (1);
                if (mem_heap.ram_end->prev == mem->next) {
                    mem_heap.ram_end->prev = mem;
                }
                mem->size += SIZEOF_STRUCT_MEM + mem->next->size;
                mem->next->next->prev = mem;
                mem->next = mem->next->next;
            }
            MEM_STATS_DEC_USED(used, size);
            MEM_STATS_INC_USED(used, mem->size);
            MEM_HEAP_UNPROTECT();
            return rmem;
        } else if (((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size < (mem_size_t)mem->next)) {
            u8_t *p1 = (u8_t *)mem + SIZEOF_STRUCT_MEM + mem->size;
            u8_t *p2 = (u8_t *)mem->next;
            new_mem = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM + newsize);
            if ((mem_size_t)mem->next - (mem_size_t)mem - SIZEOF_STRUCT_MEM >= newsize) {
                if ((mem_size_t)p2 - (mem_size_t)new_mem  >= SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED) {
                    p2 = (u8_t *)SBRK_PTR_ALIGN_BACKWARD((mem_size_t)new_mem + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED);
                    p1 = sbrk(p1, p2 - p1);
                    CHK_SBRK_PTR(p1);
//                    if(mem_heap.lfree > new_mem)
//                        mem_heap.lfree = new_mem;

                    mem->next->prev = new_mem;
                    new_mem->next = mem->next;
                    new_mem->prev = mem;
                    new_mem->used = MEM_UNUSED;
                    new_mem->size = (mem_size_t)p2 - (mem_size_t)new_mem - SIZEOF_STRUCT_MEM;
#if MEM_HEAP_ERROR_CHECK == 1
                    new_mem->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                    new_mem->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                    mem->next = new_mem;
                    mem->size = newsize;

                    plug_holes(new_mem);
                } else {
//                    log_info("\n +realloc makk3\n");
                    p1 = sbrk(p1, p2 - p1);
                    CHK_SBRK_PTR(p1);
                    mem->size += (mem_size_t)(p2 - p1);

                    plug_holes(mem);
                }
                MEM_STATS_DEC_USED(used, size);
                MEM_STATS_INC_USED(used, mem->size);
                MEM_HEAP_UNPROTECT();
                return rmem;
            } else if ((mem->next->used == MEM_UNUSED) &&
                       ((mem_size_t)mem->next - (mem_size_t)mem + (mem_size_t)mem->next->size >= newsize)) {
                p1 = sbrk(p1, p2 - p1);
                CHK_SBRK_PTR(p1);
                if ((mem_size_t)mem->next - (mem_size_t)mem + mem->next->size - newsize >= SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED) {
//                    log_info("\n +realloc makk0\n");
//                    if(mem_heap.lfree == mem->next)
//                    {
//                        mem_heap.lfree = new_mem;
//                    }
                    log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
                    log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
                    while (1);
                    if (mem_heap.ram_end->prev == mem->next) {
                        mem_heap.ram_end->prev = new_mem;
                    }
                    mem->next->next->prev = new_mem;
                    new_mem->next = mem->next->next;
                    new_mem->prev = mem;
                    new_mem->used = MEM_UNUSED;
                    new_mem->size = (mem_size_t)mem->next - (mem_size_t)mem + mem->next->size - newsize - SIZEOF_STRUCT_MEM;
#if MEM_HEAP_ERROR_CHECK == 1
                    new_mem->check_after = MEM_HEAP_OVERFLOW_MAGIC;
                    new_mem->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
                    mem->next = new_mem;
                    mem->size = newsize;
                } else {
//                    log_info("\n +realloc makk2\n");
                    //                if(mem_heap.lfree == mem->next)
                    //                {
                    //                    struct mem *cur = mem->next->next;
                    //                    while ((cur->used == MEM_USED) && cur != mem_heap.ram_end)
                    //                    {
                    //                        cur = cur->next;
                    //                    }
                    //                    mem_heap.lfree = cur;
                    //                }
                    log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
                    log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
                    while (1);
                    if (mem_heap.ram_end->prev == mem->next) {
                        mem_heap.ram_end->prev = mem;
                    }
                    mem->size = (mem_size_t)mem->next - (mem_size_t)mem + mem->next->size;
                    mem->next->next->prev = mem;
                    mem->next = mem->next->next;
                }
                MEM_STATS_DEC_USED(used, size);
                MEM_STATS_INC_USED(used, mem->size);
                MEM_HEAP_UNPROTECT();
                return rmem;
            }
        }

        MEM_HEAP_UNPROTECT();
        new_rmem = malloc(newsize);
        if (new_rmem != (void *)0) {
            memmove(new_rmem, rmem, size);
            free(rmem);
            rmem = new_rmem;
            return rmem;
        } else {
            MEM_HEAP_DEBUGF(MEM_HEAP_DEBUG, ("mem_realloc: could not allocate %d bytes\n", (mem_size_t)newsize));
            return (void *)0;
        }
    } else if (newsize < size) {
        mem2 = mem->next;
        if ((mem2->used == MEM_UNUSED) &&
            ((mem_size_t)mem + SIZEOF_STRUCT_MEM + mem->size == (mem_size_t)mem2)) {
            /* The next struct is unused, we can simply move it at little */
            struct mem *next;
            /* remember the old next pointer */
            next = mem2->next;
            /* create new struct mem which is moved directly after the shrinked mem */
            new_mem = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM + newsize);
//            if (mem_heap.lfree == mem2)
//            {
//                mem_heap.lfree = new_mem;
//            }
            log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
            log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
            while (1);
            if (mem_heap.ram_end->prev == mem2) {
                mem_heap.ram_end->prev = new_mem;
            }
            mem2 = new_mem;
#if MEM_HEAP_ERROR_CHECK == 1
            mem2->check_after = MEM_HEAP_OVERFLOW_MAGIC;
            mem2->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
            mem2->used = MEM_UNUSED;
            mem2->size = mem->size - newsize + (mem_size_t)mem->next->size;
            /* restore the next pointer */
            mem2->next = next;
            /* link it back to mem */
            mem2->prev = mem;
            /* link mem to it */
            mem->next = mem2;
            mem->size = newsize;
            /* last thing to restore linked list: as we have moved mem2,
             * let 'mem2->next->prev' point to mem2 again. but only if mem2->next is not
             * the end of the heap */
            log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
            log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
            while (1);
            if (mem2->next != mem_heap.ram_end) { //imposible?
                mem2->next->prev = mem2;
            }
            sbrk_free(mem2);
            MEM_STATS_DEC_USED(used, size);
            MEM_STATS_INC_USED(used, mem->size);
            /* no need to plug holes, we've already done that */
        } else if (newsize + SIZEOF_STRUCT_MEM + MIN_SIZE_ALIGNED <= size) {
            /* Next struct is used but there's room for another struct mem with
             * at least MIN_SIZE_ALIGNED of data.
             * Old size ('size') must be big enough to contain at least 'newsize' plus a struct mem
             * ('SIZEOF_STRUCT_MEM') with some data ('MIN_SIZE_ALIGNED').
             * @todo we could leave out MIN_SIZE_ALIGNED. We would create an empty
             *       region that couldn't hold data, but when mem->next gets freed,
             *       the 2 regions would be combined, resulting in more free memory */
            new_mem = (struct mem *)((mem_size_t)mem + SIZEOF_STRUCT_MEM + newsize);
            mem2 = new_mem;
#if MEM_HEAP_ERROR_CHECK == 1
            mem2->check_after = MEM_HEAP_OVERFLOW_MAGIC;
            mem2->check_before = MEM_HEAP_OVERFLOW_MAGIC;
#endif
//            if (mem2 < mem_heap.lfree)
//            {
//                mem_heap.lfree = mem2;
//            }
            mem2->used = MEM_UNUSED;
            mem2->size = mem->size - newsize - SIZEOF_STRUCT_MEM;
            mem2->next = mem->next;
            mem2->prev = mem;
            mem->next = mem2;
            mem->size = newsize;
            log_info_hexdump(mem_heap.ram_end, sizeof(struct mem));
            log_info("waring : %s - %s - %s\n", __FILE__, __LINE__, __func__);
            while (1);
            if (mem2->next != mem_heap.ram_end) {
                mem2->next->prev = mem2;
            }
            if (mem_heap.ram_end->prev == mem) {
                mem_heap.ram_end->prev = mem2;
            }
            sbrk_free(mem2);
            MEM_STATS_DEC_USED(used, size);
            MEM_STATS_INC_USED(used, mem->size);
            /* the original mem->next is used, so no need to plug holes! */
        }
        /* else {
          next struct mem is used but size between mem and mem2 is not big enough
          to create another struct mem
          -> don't do anyhting.
          -> the remaining space stays unused since it is too small
        } */
        MEM_HEAP_UNPROTECT();
        return rmem;
    } else if (newsize == size) {
        MEM_HEAP_UNPROTECT();
        /* No change in size, simply return */
        return rmem;
    }
    return 0;
#endif
}
#endif

#if MEM_HEAP_ERROR_CHECK == 1
void mem_heap_check(const char *const func, unsigned int line)
{
    MEM_HEAP_DECL_PROTECT();
    MEM_HEAP_PROTECT();

    if (func) {
        mem_byte('{');
        deg_printf("%d", line);
    }

    struct mem *mem;

    for (mem = mem_heap.ram; mem != mem_heap.ram_end->prev; mem = mem->next) {
        if (/* (mem > (u32_t)mem_heap.ram + mem_heap.mem_heap_size + SIZEOF_STRUCT_MEM)
                || (mem < mem_heap.ram)
                || ((u32_t)mem->next > (u32_t)mem_heap.ram + mem_heap.mem_heap_size + SIZEOF_STRUCT_MEM)
                || ((u32_t)mem->next < mem_heap.ram )
                || ((u32_t)mem->prev > (u32_t)mem_heap.ram + mem_heap.mem_heap_size+ SIZEOF_STRUCT_MEM)
                || ((u32_t)mem->prev < mem_heap.ram )
                || */((mem->used != MEM_USED) && (mem->used != MEM_UNUSED))
            || (mem->check_before != MEM_HEAP_OVERFLOW_MAGIC)
            || (mem->check_after  != MEM_HEAP_OVERFLOW_MAGIC)
        ) {
            deg_printf("\n\n\n\n\n\n\n\nmem_heap was maliciously modified:\n"\
                       "mem_heap.ram = 0x%x\n"\
                       "mem = 0x%x\n"\
                       "mem->next = 0x%x\n"\
                       "mem->prev = 0x%x\n"\
                       "mem->used = 0x%x\n"\
                       "mem->check_before = 0x%x\n"\
                       "mem->check_after = 0x%x\n\n\n\n\n\n\n\n\n", \
                       mem_heap.ram, mem, mem->next, mem->prev,
                       mem->used, mem->check_before, mem->check_after);
            while (1);
        }
    }

    if (func) {
        mem_byte('}');
    }

    MEM_HEAP_UNPROTECT();
}
#endif


//#define SBRK_INTERNAL
#ifdef SBRK_INTERNAL
static inline void *sbrk(void *addr, int incr)
{
#define SBRK_PIECE_SIZE 0x180000//1.5M
    static u8_t sbrk_mem_heap[SBRK_PIECE_SIZE + 2 * SBRK_PAGE_ALIGNMENT] AT(.mem_heap);
    u8_t *psbrk;
    psbrk = SBRK_PTR_ALIGN_BACKWARD(sbrk_mem_heap);

    static u8_t *keep_addr = 0;
    static u32_t keep_cnt = 0;
    u8_t *sbrk_ptr = -1;
    int i;

    if (addr == 0) {
        return psbrk;
    } else if (addr == -1) {
        return (u8_t *)SBRK_PTR_ALIGN_FORWARD(&sbrk_mem_heap[sizeof(sbrk_mem_heap) - 1]) - SBRK_PAGE_ALIGNMENT;
    }

    if (((int)addr % SBRK_PAGE_ALIGNMENT) || ((int)incr % SBRK_PAGE_ALIGNMENT)) {
        log_info("\n\n\n\n\n sbrk NOT ALIGNED 0x%x ,0x%x \n\n\n", addr, incr);
        while (1);
    }


    if (incr >= 0) {
//        mem_byte('+');mem_u32hex0(addr);
        if ((u32_t)addr + incr > psbrk + SBRK_PIECE_SIZE) {
            log_info("\n\n\n\n\n sbrk none never happend =0x%x, 0x%x \n\n\n", addr, incr);
            return -1;
        }
        sbrk_ptr = addr;

        if (keep_addr && (sbrk_ptr <= keep_addr) && (sbrk_ptr + incr > keep_addr)) {
            for (i = 0; i < keep_cnt; i++) {
                if (keep_addr[i] != 0x77) {
                    log_info("\n\n\n\n\n sbrk eeeerrrooorrr 0x%x, 0x%x,0x%x ,0x%x \n\n\n", sbrk_ptr, keep_addr, keep_cnt, incr);
                    mem_buf(keep_addr, keep_cnt);
                    while (1);
                }
//                else mem_byte('$');
            }
            keep_addr = keep_cnt = 0;
        }
    } else if (incr < 0) {
        if (keep_addr == 0) {
            //mem_byte('-');mem_u32hex0(addr);mem_u32hex0(0-incr);

            keep_addr = addr;
            keep_cnt = 0 - incr;
            memset(keep_addr, 0x77, keep_cnt);
        }
    }

    return sbrk_ptr;
}
#endif
