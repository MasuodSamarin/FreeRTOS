/*********************************************************************************************
    *   Filename        : my_malloc.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2017-05-16 19:24

    *   Copyright:(c)JIELI  2011-2017  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _MY_MALLOC_H_
#define _MY_MALLOC_H_

#include "typedef.h"

u32 mmu_init() ;
ufat_t *vmalloc(u32 size) ;
ufat_t *vrealloc(ufat_t *fat, u32 size) ;
void vfree(ufat_t *fat) ;
u32 get_phy_addr(u32 vaddr) ;
u32 get_phy_addr_by_mmu(ufat_t *fat, u32 vddr_start, u32 vaddr) ;

extern void *sbrk(void *addr, int incr);
extern void *malloc(unsigned int size);
extern void *calloc(unsigned int count, unsigned int size);
extern void *realloc(void *rmem, unsigned int newsize);
extern void  free(void *mem);
extern void *pmalloc(u32 nsize, u32 mode);
extern void pfree(void *p, u32 npage);
extern ufat_t *vrealloc(ufat_t *fat, u32 size);
extern void mem_heap_stats(void);

#endif
