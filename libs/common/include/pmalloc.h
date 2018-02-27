/*********************************************************************************************
    *   Filename        : pmalloc.h

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 15:29

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef __PMALLOC_H
#define __PMALLOC_H

#include "typedef.h"


void *pmalloc(u32 nsize, u32 mode);
void *pmalloc_alin(u32 nsize, u32 alignment_size);
void pfree(void *p, u32 npage);
void pfree_func(void *p, u32 size);

extern u32 pm_usd_map[(MAX_SDRAM_SIZE + (PM_BITMAP_UNIT - 1)) / PM_BITMAP_UNIT]; // 最大32M ram  /64K
#endif

