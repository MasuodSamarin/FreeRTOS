/*********************************************************************************************
    *   Filename        : pmalloc.c

    *   Description     :

    *   Author          : Bingquan

    *   Email           : bingquan_cai@zh-jieli.com

    *   Last modifiled  : 2016-07-12 15:29

    *   Copyright:(c)JIELI  2011-2016  @ , All Rights Reserved.
*********************************************************************************************/
#include "typedef.h"
#include "mmu_tag.h"
#include "pmalloc.h"
#ifndef WIN32
#include "os/rtos.h"
#endif

/* #define DEBUG_ENABLE */
#include "debug_log.h"

//u32 pm_usd_map[(MAX_SDRAM_SIZE/PAGE_SIZE)/32];  // 最大32M ram  /64K
u32 pm_usd_map[(MAX_SDRAM_SIZE + (PM_BITMAP_UNIT - 1)) / PM_BITMAP_UNIT]; // 最大32M ram  /64K

u16 pm_mem_curren_pos ;
u16 pm_mem_curren_pos_begin ;
extern u32 SDRAM_BEGIN_ADDR ;
/*
   page 管理初始化，
   saddr  -- 参与管理的内存开始地址。
   eaddr  -- 参与管理的内存结束地址
   return -- 0 -- 失败
   1 -- 成功

*/

#define RAM_S       0x8600
#define RAM_E       0x18000-512
#define RAM_SIZE    0x18000-512
//#define PAGE_SIZE   512       //cpu.h已经声明
#define RAM_USD

static int reverse_1(int idx)
{
    int i;
    int x;
    /* log_info("reverse =%x \r\n", idx); */
    if (0 == idx) {
        return -1;
    }
    for (i = 31, x = -1; i >= idx; i--) {
        x &= ~BIT(i);

        /* log_info("i =%x %x\r\n", i, x); */
    }
    return x;
}

u32 pmalloc_init(u32 saddr, u32 eaddr)
{
    u32 head_bit_idx, tail_bit_idx;
    u8 head_word_usd_bit, tail_word_usd_bit;
    u32 total_bit, total_word;
    u32 word;

    log_info("saddr = %x eaddr =%x \r\n", saddr, eaddr) ;
    head_bit_idx = (saddr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE;
    tail_bit_idx = (eaddr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE;
    //bit map cnt
    total_bit = tail_bit_idx - head_bit_idx;

    log_info("head_bit_idx = %x tail_bit_idx =%x total_bit =%x \r\n", head_bit_idx, tail_bit_idx, total_bit) ;
    memset(pm_usd_map, 0, sizeof(pm_usd_map));
    //usd bit in word 32
    head_word_usd_bit = head_bit_idx & 0x1f;
    tail_word_usd_bit = tail_bit_idx & 0x1f;
    //bit map cnt - head usd cnt
    /* total_bit -= (31 - head_word_usd_bit); */
    //bit map cnt - tail usd cnt
    /* total_bit -= (31 - tail_word_usd_bit); */

    log_info("head_word_usd_bit = %x tail_word_usd_bit =%x total_bit =%x \r\n", head_word_usd_bit, tail_word_usd_bit, total_bit) ;
    total_word = (total_bit + 32 - 1) / 32 ;
    log_info("total_word =%x \r\n", total_word) ;
    pm_mem_curren_pos_begin =  head_bit_idx;
    pm_mem_curren_pos  = pm_mem_curren_pos_begin ;
    //skip head usd bit
    //fix head usd bit in word
    word = (head_bit_idx / 32);
    if (head_word_usd_bit) {
        pm_usd_map[word] = -(1 << head_word_usd_bit); //head_word_usd_bit
        log_info("head =%x \r\n", pm_usd_map[word]) ;
        word++;
    }
    for (; word < tail_bit_idx / 32; word++) {
        pm_usd_map[word] = -1;
        log_info("middle =%x \r", pm_usd_map[word]) ;

    }
    //fix tail usd bit in word
    if (tail_word_usd_bit) {
        word = (tail_bit_idx / 32);
        pm_usd_map[word] = reverse_1(tail_word_usd_bit);//tail_word_usd_bit
        log_info("tail =%x \r\n", pm_usd_map[word]) ;
    }
    log_info_hexdump(pm_usd_map, sizeof(pm_usd_map) + 8);
    //while(1);
    return 0;
}

/*
   设置ram 的使用状态
   addr   -- ram 地址 。
   status   0 -- ram 被使用
   1 -- ram 被释放

*/
//AT(.common)
static void set_ram_status(u32 addr, u32 status)
{
    u32 pos ;
    pos = (addr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE;
    if (status) {
        pm_usd_map[pos / 32] |= BIT(pos & 0x1f) ;
    } else {
        pm_usd_map[pos / 32] &= ~BIT(pos & 0x1f) ;
    }

}
/*

   获取一下ram 的状态 。
 *addr   -- 当前获取到的ram 的地址。
 return  0 -- 当前获取到的ram 地址被使用。
 1 -- 当前获取到的ram 地址没有被使用。
*/
static u32 get_next_ram(u32 *addr)
{
    u32 ret ;
    static u8 retry = 0 ;
    *addr = pm_mem_curren_pos * PAGE_SIZE + (u32)&SDRAM_BEGIN_ADDR ;
    //log_info("pos = %x \r\n" ,pm_mem_curren_pos) ;
    //mem_u32hex0(*addr) ;


    ret = pm_usd_map[pm_mem_curren_pos / 32] & BIT((pm_mem_curren_pos & 0x1f)) ;
    pm_mem_curren_pos++;
    if (pm_mem_curren_pos >= ((MAX_SDRAM_SIZE / PAGE_SIZE))) {
        // 此时认为分配内存失败，内存分配完毕。
        /* log_info("分配内存失败，内存分配完毕\r\n") ; */
        pm_mem_curren_pos =  pm_mem_curren_pos_begin;

        retry ++ ;
        if (retry > 2) {
            retry = 0;
            *addr = 0 ;
            return 0;
        }


    }

    //log_info("get next ram %x \r\n",ret) ;
    return  ret;

}

/*
   申请page *64K 大小的连续的ram 空间

   page -- 申请的page 数目
   mode -- 保留

   return -- 申请到的ram 空间地址
*/
//AT(.common)
void *pmalloc(u32 nsize, u32 mode)
{
    u32 addr = -1, i, begin = 0 ;
    u32 npage = PAGE_COUNT(nsize);
    /* pm_mem_curren_pos  = pm_mem_curren_pos_begin ; */
    /* mem_puts("\n----"); */
    /* mem_puts(__func__); */
    /* log_info_hexdump0(pm_usd_map,sizeof(pm_usd_map)) ; */
    OS_ENTER_CRITICAL();
    for (i = 0 ; i < npage ;) {
        if (get_next_ram(&addr)) {
            if (begin == 0) {
                begin = addr ;
            }
            i++;
        } else {
            i = 0 ;
            begin = 0 ;
        }
        //log_info("get ram addr = %x ",addr) ;
        if (addr == 0) {
            //return 0 ;
            begin = 0;
            goto end;
        }

    }
    for (i = 0 ; i < npage ; i++) {
        set_ram_status(begin + i * PAGE_SIZE, 0) ;
    }
end:
    OS_EXIT_CRITICAL();
    //deg_printf("pmalloc begin %x \r\n",begin ) ;
    /* log_info_hexdump0(pm_usd_map, sizeof(pm_usd_map)) ; */
    return (void *)begin ;
}

//add pmalloc_alin,alloc addr align at alignment_size
void *pmalloc_alin(u32 nsize, u32 alignment_size)
{
    u32 addr = -1, i, begin = 0 ;
    u32 npage = PAGE_COUNT(nsize);
    u8 alin_flag = 0;
    /* pm_mem_curren_pos  = pm_mem_curren_pos_begin ; */
    /* log_info_hexdump0(pm_usd_map,sizeof(pm_usd_map)) ; */
    OS_ENTER_CRITICAL();
    for (i = 0 ; i < npage ;) {
        if ((get_next_ram(&addr)) &&
            ((addr % alignment_size) == 0 || alin_flag)) {
            alin_flag = 1;
            if (begin == 0) {
                begin = addr ;
            }
            i++;
        } else {
            i = 0 ;
            begin = 0 ;
            alin_flag = 0;
        }

        //log_info("get ram addr = %x ",addr) ;
        if (addr == 0) {
            //return 0 ;
            begin = 0;
            alin_flag = 0;
            goto end;
        }
    }
    for (i = 0 ; i < npage ; i++) {
        set_ram_status(begin + i * PAGE_SIZE, 0) ;
    }
end:
    OS_EXIT_CRITICAL();
    return (void *)begin ;
}

/*

*/
//AT(.common)
void pfree(void *p, u32 npage)
{
    u32 i ;
    u32 pos;

    OS_ENTER_CRITICAL();

    for (i = 0 ; i < npage ; i++) {
        set_ram_status((u32)p + i * PAGE_SIZE, 1) ;
        /* pm_mem_curren_pos--; */
    }
    pos = ((u32)p - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE;
    if (pos < pm_mem_curren_pos) {
        pm_mem_curren_pos = pos;
    }
    //log_info_hexdump0(pm_usd_map, sizeof(pm_usd_map)) ;
    OS_EXIT_CRITICAL();
}

void pfree_func(void *p, u32 size)
{
    pfree(p, (size + PAGE_SIZE - 1) / PAGE_SIZE);
}


/* void pmalloc_test()
   {
   u32 addr ;
   pmalloc_init(SDRAM_BEGIN_ADDR+0x20000 ,SDRAM_BEGIN_ADDR*2) ;
   log_info_hexdump0(pm_usd_map ,sizeof(pm_usd_map)) ;
   addr = pmalloc(10,0) ;
   log_info_hexdump0(pm_usd_map ,sizeof(pm_usd_map)) ;
   pfree(addr,10) ;
   log_info_hexdump0(pm_usd_map ,sizeof(pm_usd_map)) ;

   } */

