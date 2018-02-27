/*******************************************************************************************
 *
 *   File Name: vmalloc.c
 *
 *   Version: 1.00
 *
 *   Discription: 虚拟内存管理函数，包括内存分配，释放，地址转换等功能
 *
 *   Author:Bingquan Cai
 *
 *   Email :bingquan_cai@zh-jieli.com
 *
 *   Date:
 *
 *   Copyright:(c)JIELI  2016  @ , All Rights Reserved.
 *
 *   *******************************************************************************************/
#ifndef __vmalloc_c
#define __vmalloc_c
//#include "pmalloc.h"
#include "my_malloc.h"
#include "mmu_tag.h"
#ifndef WIN32
#include "os/rtos.h"
#endif
#include "debug_interface.h"

/* #define DEBUG_ENABLE */
#include "debug_log.h"

#ifdef WIN32
#define OS_CPU_ID  0
#endif

ufat_t vmm_fat[(MAX_SDRAM_SIZE / PAGE_SIZE)] ;
extern u32 SRAM_END_ADDR ;
extern u32 SDRAM_BEGIN_ADDR ;


static ufat_t *get_fat_pos(ufat_t *fat, ufat_t **cend, u32 count);
u32 get_fat_count(ufat_t *fat, ufat_t **end) ;
/* u32 get_phy_addr_by_mmu(ufat_t* fat, u32 vaddr); */

void set_mmu_addr_map(u32 vadd, u32 phyaddr)
{
    u8 *ptr0 = (u8 *)MMU_TAG_ADDR0 ;
    /* u32 *ptr1=MMU_TAG_ADDR1 ; */


    //mem_u32hex(vadd) ;
    //mem_u32hex(phyaddr) ;

    ptr0[(vadd / (PAGE_SIZE))] = (phyaddr / (PAGE_SIZE))  ;
    /* ptr1[(vadd/(PAGE_SIZE))] =  (phyaddr /(PAGE_SIZE))  ; */

    //mem_buf0(0x68100,0x40);
    //mem_buf0(0x6a100,0x40);
}



u32 mmu_init()
{
    memset((u8 *)MMU_TAG_ADDR0, 0xff, MMU_TAG_SIZE);

    memset(vmm_fat, 0x00, sizeof(vmm_fat)) ;

    pmalloc_init((u32)(PAGE_COUNT((u32)&SRAM_END_ADDR))*PAGE_SIZE,
                 (u32)&SDRAM_BEGIN_ADDR + MAX_SDRAM_SIZE) ;

    __debug_drv->on();
    __debug_drv->ioctrl(PC_WR_RAM, MMU_TAG_ADDR0, MMU_TAG_ADDR0 + MMU_TAG_SIZE, 1);
    __debug_drv->ioctrl(PRP_WR_RAM, MMU_TAG_ADDR0, MMU_TAG_ADDR0 + MMU_TAG_SIZE, 1);

    return 0;
}


/*

   虚拟内存申请函数。并且把实际内存映射到虚拟内存中。用于按page 分配内存。

   vadd  --虚拟地址
   size  --需要申请空间大小 。

   return -- 虚拟内存管理指针。


*/
ufat_t  *vmalloc(u32 size)
{
    u32 npage = PAGE_COUNT(size) ;
    u32 i;//size ;//
    u32 saddr;
    ufat_t *ret = 0;
    ufat_t pre = -1, cur ;
    /* u8 *mmu = MMU_TAG_ADDR0 ; */
//    u32 mmu_saddr ;
    /* u32 *ptr1= MMU_TAG_ADDR1 ; */
    /* u8 *ptr0= MMU_TAG_ADDR0 ; */

    /* log_info("vmalloc size = %x \r\n", npage) ; */
    if (!npage) {
        return 0 ;
    }
    /*
       mmu_tag = malloc(sizeof(mmu_tag_t)+MMU_MAX_SIZE/PAGE_SIZE *sizeof(u16) );// size*sizeof(u32));  128 个u16  ，最大支持8M 管理
       if(!mmu_tag)
       {
       return 0 ;
       }
       memset(mmu_tag,0x00 ,sizeof(mmu_tag_t)+MMU_MAX_SIZE/PAGE_SIZE *sizeof(u16)) ;
       mmu_tag->count = size ;
       mmu_tag->appaddr = vaddr/PAGE_SIZE ;
       mmu_saddr = (u8*)((u8*)mmu_tag+sizeof(mmu_tag_t)) ;
       mmu_tag->mmuaddr = mmu_saddr;
       */

    /* pm_mem_curren_pos  = pm_mem_curren_pos_begin ; */
    // DSP0_BF_CON &=~BIT(2) ;
    for (i = 0 ; i < npage ; i++) {
        saddr = (u32)pmalloc(1 * PAGE_SIZE, 0) ;
        /* log_info("pmalloc phy addr 0x%x \r\n" ,saddr) ; */
        if (!saddr) {
            //free(mmu_tag) ;
            log_info("pmalloc err \r\n") ;
            if (ret) {
                vfree(ret);
            }
            return 0 ;
        }
#ifndef WIN32
        /* mmu_saddr = (saddr-SDRAM_BEGIN_ADDR)/PAGE_SIZE ; */
        /* ptr0[((vaddr-MMU_ADDR_BEGIN)/PAGE_SIZE) + i]= mmu_saddr; */
#endif
        cur = (saddr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE ;

        /* log_info("mem page index %x /prev index : %x vmm_fat[pre] : %x\r\n", cur, pre, vmm_fat[pre]) ; */
        //is mark end?
        if ((pre != (ufat_t) - 1) && (vmm_fat[pre] == VMM_FAT_END)) {
            vmm_fat[pre] = cur ;
        }
        //is mark free?
        if (vmm_fat[cur] == 0) {
            vmm_fat[cur] = VMM_FAT_END ;
            pre = cur ;
        }
        //head
        if (!ret) {
            ret = &vmm_fat[cur] ;
            /* log_info("ret %x\n", ret) ; */
        }
        //mem_buf0(vmm_fat,40) ;

    }

    /* mem_buf0(vmm_fat, 32); */
    /* log_info("\r\n vmalloc over ok") ; */
    /* mem_buf0(vmm_fat, sizeof(vmm_fat)) ; */

    //mmu_tag->vmm_fat=ret ;
    //return  mmu_tag;

    return ret ;
}
//AT(.common)
ufat_t *vrealloc(ufat_t *fat, u32 size)
{
    /* u32 cpu_sr ; */
    s32 npage = PAGE_COUNT(size) ;
    u32 i;//size ;//
    u32 saddr, *ret = 0;
    ufat_t pre = (ufat_t) - 1, cur ;
//    u32 mmu_saddr ;
    ufat_t *efat;
    s32 count ;
    ufat_t *end ;

    /* log_info("---------vrealloc begin : size %x\n", size); */
    /* mem_buf0(vmm_fat, sizeof(vmm_fat)) ; */

    if (npage == 0) {
        /* puts_without_lock("npage = 0\n"); */
        return 0 ;
    }

    count = get_fat_count(fat, &end);
    if (npage == count) {
        log_info("npage = count\n");
        return fat ;
    }
    pre = (ufat_t)(end - vmm_fat) ;
    /* log_info("npage %x / count %x / pre %x\n", npage, count, pre); */

    if ((npage - count) > 0) {
        count  = npage - count ;
        for (i = 0 ; i < count ; i++) {
            saddr = (u32)pmalloc(1 * PAGE_SIZE, 0) ;
            /* log_info("pmalloc %x \r\n" ,saddr) ; */
            if (!saddr) {
                //free(mmu_tag) ;
                /* OS_EXIT_CRITICAL() ; */
                //  DSP0_BF_CON |=BIT(2) ;
                /* log_info("pmalloc err \r\n") ; */
                return 0 ;
            }
#ifndef WIN32
            /* mmu_saddr = (saddr-SDRAM_BEGIN_ADDR)/PAGE_SIZE ; */
            /* ptr0[((vaddr-MMU_ADDR_BEGIN)/PAGE_SIZE) + i]= mmu_saddr; */
#endif
            cur = (saddr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE ;
            if ((pre != (ufat_t) - 1) && (vmm_fat[pre] == VMM_FAT_END)) {
                vmm_fat[pre] = cur ;
            }
            if (vmm_fat[cur] == 0) {
                vmm_fat[cur] = VMM_FAT_END ;
                pre = (ufat_t)cur ;
            }
            if (!ret) {
                ret = (u32 *)&vmm_fat[cur] ;
            }
        }
    } else {
        //free not used page, keep the used page
        efat = get_fat_pos(fat, &end, npage) ;
        /* log_info("fat %x / efat %x / end %x \n", fat, efat, end); */
        vfree(efat) ;
        *end = VMM_FAT_END;
    }
    /* log_info("-------------vrealloc over ok\r\n") ; */
    /* mem_buf0(vmm_fat, sizeof(vmm_fat)) ; */
    // OS_EXIT_CRITICAL() ;
    return (ufat_t *)1;
}

//AT(.common)
u32 get_fat_count(ufat_t *fat, ufat_t **end)
{
    u32 i = 0 ;
    *end = fat ;
    do {
        i++ ;
        *end = fat ;
        if ((ufat_t)(*fat) == (ufat_t)VMM_FAT_END) {
            break  ;
        }
        fat = *fat + vmm_fat ;
    } while (1) ;

    return i ;
}

static ufat_t *get_fat_pos(ufat_t *fat, ufat_t **cend, u32 count)
{
    //u16 *end ;
    u32 i = 0 ;
    *cend  = fat ;
    do {
        i++ ;
        fat = *fat + vmm_fat ;
        if (((ufat_t)(*fat) == (ufat_t)VMM_FAT_END) || (i >= count)) {
            break  ;
        }
        *cend =  fat ;
    } while (1) ;

    return fat ;
}

/*

    mod == 0 . 从低地址往高地址填。
    mod == 1 , 从高地址往低地址填。

*/
//AT(.common)
void vfill_tag(ufat_t *fat, u32 vaddr, u8 mod)
{
//    u32 cpu_sr ;
//    u32 saddr ;
    ufat_t *tagbase = (ufat_t *)MMU_TAG_ADDR0 ;
    ufat_t *tag = (ufat_t *)(&tagbase[(vaddr - MMU_ADDR_BEGIN) / PAGE_SIZE]) ;

//    OS_ENTER_CRITICAL() ;
    OS_ENTER_CRITICAL();
    /* DSP_BF_CON &=~(BIT(8)|BIT(10)); */

    __debug_drv->suspend();
    do {
        //mem_u32hex(*tag) ;
        if (mod == 0) {
            *tag++ = ((fat - vmm_fat)) ;// + SDRAM_BEGIN_ADDR/PAGE_SIZE;
        } else {
            *--tag = ((fat - vmm_fat)) ;// + SDRAM_BEGIN_ADDR/PAGE_SIZE;
        }
        if (*fat == VMM_FAT_END) {
            break ;
        }
        fat  = *fat + vmm_fat;
    } while (1);

//   OS_EXIT_CRITICAL() ;
    __debug_drv->resume();
    /* DSP_BF_CON |=(BIT(8)|BIT(10)); */
    OS_EXIT_CRITICAL();
    /* log_info("\r\n mmu tag fill ok") ; */
    /* mem_buf0(tagbase, MMU_TAG_SIZE); */

}
//AT(.common)
void vfree(ufat_t *addr)
{
    u32 saddr ;
    u32 tmp ;
//    u32 cpu_sr ;

    u32 i = 0 ;
    if ((addr < vmm_fat) || (addr >= vmm_fat + ARRAY_SIZE(vmm_fat))) {
        return ;
    }

    OS_ENTER_CRITICAL() ;
    // DSP0_BF_CON &=~BIT(2) ;
    do {
        saddr = ((addr - vmm_fat)) * PAGE_SIZE + (u32)&SDRAM_BEGIN_ADDR ;
        /* mem_u32hex(saddr) ; */
        tmp = *addr ;
        pfree((void *)saddr, 1) ;
        /// ptr1[vaddr+i] = SDRAM_BEGIN_ADDR/RAM_PAGE_SIZE ;  //时地址重新映射到ram开始
        //    ptr0[vaddr+i] = SDRAM_BEGIN_ADDR/RAM_PAGE_SIZE ;
        i++ ;
        if (*addr == VMM_FAT_END) {
            *addr = 0 ;
            break ;
        }

        *addr = 0 ;

        addr  =  vmm_fat + tmp;
        //mem_buf0(vmm_fat,40) ;
    } while (1);
    // DSP0_BF_CON |=BIT(2) ;
    OS_EXIT_CRITICAL() ;
    //mem_buf0(0x68100 ,0x200) ;

    //mem_buf0(vmm_fat ,sizeof(vmm_fat)) ;
    /*
       if(*addr== VMM_FAT_END)
       {
       saddr = ((addr - vmm_fat)) *RAM_PAGE_SIZE+ SDRAM_BEGIN_ADDR ;
     *addr = 0 ;
     pfree(saddr,1) ;
    //mem_u32hex(saddr) ;
    //mem_buf0(vmm_fat,40) ;
    }*/
}
ufat_t *vfree_by_addr(u32 vaddr, u32 size)
{
    int i;// ,j;
    ufat_t *addr, *addro = 0 ;
    u32 tmp ;
    u32 saddr ;
    int cnt = size / PAGE_SIZE ;

    ufat_t *tagbase = (ufat_t *)MMU_TAG_ADDR0 ;
    ufat_t val = tagbase[(vaddr -  MMU_ADDR_BEGIN) / PAGE_SIZE];
//    log_info("vfree by addr %x %x %x %x   ",vaddr ,size, PAGE_SIZE, cnt) ;
    //addro = vmm_fat+ val ;
    OS_ENTER_CRITICAL() ;
    for (i = 0 ; i < sizeof(vmm_fat) / sizeof(ufat_t); i++) {
        if (vmm_fat[i] == val) {
            addro = &vmm_fat[i];
            break ;
        }
    }

    __debug_drv->suspend();
    /* DSP_BF_CON &=~(BIT(8)|BIT(10)); */
    for (i = 0 ; i < cnt ; i++) {
        tagbase[(vaddr -  MMU_ADDR_BEGIN) / PAGE_SIZE + i] = VMM_FAT_END ;
    }
    __debug_drv->resume();
    /* DSP_BF_CON |=(BIT(8)|BIT(10)); */
    addr =  vmm_fat + val ;
    while (cnt--) {

        saddr = ((addr - vmm_fat)) * PAGE_SIZE + (u32)&SDRAM_BEGIN_ADDR ;
        /* mem_u32hex(saddr) ; */
        tmp = *addr ;
//        log_info("pfree ->0x%x, 0x%x\n",cnt,saddr);
        pfree((void *)saddr, 1) ;
        if (*addr == VMM_FAT_END) {
            *addr = 0 ;
            break ;
        }
        *addr = 0 ;
        addr  =  vmm_fat + tmp;

    };


//    log_info("vmm_fat = \r\n") ;
//    log_info(vmm_fat ,sizeof(vmm_fat)) ;
//    log_info("MMU_TAG_ADDR0 \r\n") ;
//    mem_buf0(MMU_TAG_ADDR0 ,0x200) ;


    if (addro) {
        *addro =  addr - vmm_fat;
        OS_EXIT_CRITICAL() ;
        return addro ;
    } else {
        OS_EXIT_CRITICAL() ;
        return addr ;
    }




}
u32 get_phy_addr(u32 vaddr)
{
    u32 addr ;
    u32 *ptr[2] = {(u32 *)MMU_TAG_ADDR0, (u32 *)MMU_TAG_ADDR0};
    if ((vaddr < MMU_ADDR_BEGIN) || (vaddr >= MMU_ADDR_END)) { // mmu 转换地址必须在mmu 虚拟地址区域
        //      log_info("vaadr = %x addr = %x \r\n" ,vaddr ,addr ) ;
        return vaddr ;
    }
    addr = ((ptr[OS_CPU_ID][vaddr / (PAGE_SIZE)]) << PAGE_SHIFT) + (vaddr & (PAGE_SIZE - 1));
    ///log_info("vaadr = %x addr = %x \r\n" ,vaddr ,addr ) ;
    return addr + (u32)&SDRAM_BEGIN_ADDR;
}

u32 get_phy_addr_by_mmu(ufat_t *fat, u32 vdd_start, u32 vaddr)
{
    u32 addr ;
    u32 offset ;
    u32 tmp;
    u32 saddr ;
    /* if( (vaddr < MMU_ADDR_BEGIN) ||(vaddr >= MMU_ADDR_END))  // mmu 转换地址必须在mmu 虚拟地址区域 */
    {
        //       log_info("vaadr = %x addr = %x \r\n" ,vaddr ,addr ) ;
        /* return vaddr ; */
    }

    // u16 *mmu_saddr=mmu->mmuaddr ;
    /* log_info("vaddr = %x \r\n" ,vaddr ) ; */
    vaddr = vaddr - vdd_start;
    offset = vaddr / (PAGE_SIZE) ;
    do { //for(i = 0 ; i < offset  ;i++)
        saddr = (fat - vmm_fat) * PAGE_SIZE + (u32)&SDRAM_BEGIN_ADDR ;
        /* mem_u32hex(saddr) ; */
        tmp = *fat ;
        if (*fat == VMM_FAT_END) {
            break ;
        }
        fat = vmm_fat + tmp;

    } while (offset--);


    addr = saddr + (vaddr & (PAGE_SIZE - 1));
    //vaddr = vaddr&(PAGE_SIZE-1);
    /* log_info("vaadr = %x addr = %x \r\n" ,vaddr ,addr ) ; */
    //addr= addr + vaddr ;
    log_info("vaadr = %x addr = %x \r\n", vaddr, addr) ;
    return  addr ;

}

void *wmalloc(u32 nsize)
{
    u32 addr, i ;
    u32 page ;
//    ufat_t *fat ;
    u32 saddr ;
    ufat_t pre = -1, cur ;

    page = PAGE_COUNT(nsize) ;
    addr = (u32)pmalloc(nsize, 0) ;
    if (!addr) {
        return 0 ;
    }
    for (i = 0 ; i < page; i++) {
        saddr = addr + i * PAGE_SIZE ;
        cur = (saddr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE ;
        if ((pre != (ufat_t) - 1) && (vmm_fat[pre] == VMM_FAT_END)) {
            vmm_fat[pre] = cur ;
        }
        if (vmm_fat[cur] == 0) {
            vmm_fat[cur] = VMM_FAT_END ;
            pre = cur ;
        }


    }
    return (void *)addr ;
}


void wfree(void *ptr)
{
    ufat_t *fat ;
    ufat_t offset = ((u32)ptr - (u32)&SDRAM_BEGIN_ADDR) / PAGE_SIZE ;
    fat = &vmm_fat[offset] ;
    vfree(fat) ;

}


/* void vmm_test()
   {
   ufat_t *ptr ;
   vmm_init() ;
   ptr = vmalloc(0,10) ;
   vfree(ptr) ;
   } */
#endif
