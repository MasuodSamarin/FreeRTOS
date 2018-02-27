#include <stdarg.h>
#include <string.h>
#include "cpu.h"
#include "icache_interface.h"
#include "spi_interface.h"
#include "vm.h"
#include "os/rtos.h"
#include "winbond_flash.h"
#include "btstack/utils.h"
#include "my_malloc.h"


#define vm_printf(...)	printf
#define vm_puts(...)	//printf
#define vm_put_buf(...)	//printf

#define vm_memset	memset
#define vm_memcpy	memcpy
#define vm_delay	delay


#define VM_ERROR_DECT       			0
#define VM_SPI0_READ        			0
#define VM_WRITE_RAM_WHEN_FULL			1

#define VM_MAX_SIZE						(64L*1024 * 2)//VM最大支持双64K
#define VM_DEFRAG_SIZE					(80)//VM 碎片整理。百分比

#define LABEL_INDEX_LEN_CRC_SIZE        4

typedef struct _VM_WRITE_RAM_LIST {
    //--write_list
    struct list_head list;

    //--vm_data_package
    u8 magic_unit[LABEL_INDEX_LEN_CRC_SIZE];
    u8 data[0];	//data_len define by vm_unit_magic

} VM_WRITE_RAM_LIST;

typedef struct __PROTECT_INFO_HEAD {
    u8 area_id[8];
    u16 data_len;
    u16 rev;
    u16 head_crc;
    u16 data_crc;
} PROTECT_INFO_HEAD;

typedef struct __PROTECT_AREA {
    u32 start_addr;
    u32 len;
} PROTECT_AREA;

static vm_err get_protect_info(void);

extern u32 vm_used_code_start;
extern u32 vm_used_code_end;

//VM_MAGIC, this flag change from 1 to 0
#define VM_MAGIC_SIZE			(sizeof(u32))			//4 byte len
#define VM_MAGIC_UNUSED			((u32)0xFFFFFFFF)	//1111-1111-1111-1111
#define VM_MAGIC_DEFRAGGING		((u32)0xDDEEBB77)	//1101-1110-1011-0111
#define VM_MAGIC_DEFRAG_OVER	((u32)0xDDEEAA55)	//1101-1110-1010-0101
#define VM_MAGIC_USING			((u32)0x55AAAA55)	//0101-1010-1010-0101
#define VM_MAGIC_WAIT_ERASE		((u32)0x00000000)	//0000-0000-0000-0000


#define VM_BLOCK_SIZE        (64*1024)
#define VM_SECTOR_SIZE       (4*1024)


static u32 sfc_base_adr;
#define SPI_CACHE_READ_ADDR(addr)  (void *)(0x1000000+(((u32)addr)  - sfc_base_adr))

struct _VM_DESC {
    u16 v_addr;
    u16 v_len;
};

struct _AREA_RANGE {
    u32 start_addr;
    u32 len;			//byte
    u16 erase_bit_map;	//surport 16 * 4 sector
};

struct _VM_INFO {
    struct _AREA_RANGE area[2];
    struct _VM_DESC idx_desc[VM_INDEX_DES_MAX];
    struct list_head write_list;
    /* OS_MUTEX mutex; */
    u32 last_write_addr;
    u8 using_area;		//0:main_area	1:temp_area
    u8 warning_line;	//percentage
    u8 init_flag;		//vm_init_statu
    // u8 defrag_flag;		//vm_defrag
    u8 volatile mutex;
};

struct _VM_INFO vm_obj;

#define VM_INIT				(vm_obj.init_flag)

#define VM_MUTEX			(vm_obj.mutex)
#define VM_LINE				(vm_obj.warning_line)

#define VM_WRITE_LIST		(vm_obj.write_list)

#define VM_AREA_USING		(vm_obj.using_area)
#define VM_AREA_UNSUED		(!vm_obj.using_area)

#define VM_IDX_ADR(x)		(vm_obj.idx_desc[x].v_addr)
#define VM_IDX_LEN(x)		(vm_obj.idx_desc[x].v_len)

#define VM_AREA_ADR(x)		(vm_obj.area[x].start_addr)
#define VM_AREA_LEN(x)		(vm_obj.area[x].len)

#define VM_AREA_ERASE_INFO(x)		(vm_obj.area[x].erase_bit_map)

#define VM_LAST_ADR			(vm_obj.last_write_addr)	//spi_flash_addr


//static u32 vm_vm_vm;
/*
|---0-3---|---4-15---|--16-23--|--24-31--|---len--|
|_vm_label| _vm_len  |_vm_index| _vm_crc |_vm_data|...
*/

typedef u8 _vm_lab;
typedef u16 _vm_len;
typedef u8 _vm_idx;
typedef u8 _vm_crc;

#define VM_MAX_DATA_LEN                 (_vm_len)0x0FFF
#define VM_LABEL                        (_vm_lab)0x0D

#define READ_BT_8( buffer, pos) (((u8) buffer[pos]))
#define VM_UNIT_LAB(buffer)		((_vm_lab)(READ_BT_8(buffer, 0) & 0x0F))
#define VM_UNIT_LEN(buffer)		((_vm_len)(READ_BT_16(buffer, 0) >> 4))
#define VM_UNIT_IDX(buffer)		((_vm_idx)(READ_BT_8(buffer, 2)))
#define VM_UNIT_CRC(buffer)		((_vm_crc)(READ_BT_8(buffer, 3)))

u16 crc16(const void *ptr, u32 len);
vm_err vm_status(struct _VM_INFO *vm_str_p);

AT_VM_CODE
int vm_bytecmp(u8 *p, u8 ch, u32 num)
{
    while (num-- > 0) {
        if (*p++ != ch) {
            return -1;
        }
    }
    return 0;
}

static void __flash_suspend()
{

}

static void __flash_resume()
{

}


static inline int vm_mutex_init(void)
{
    VM_MUTEX = 1;

    return TRUE;
    /* return os_mutex_create(&VM_MUTEX); */
}
static inline int vm_mutex_enter(void)
{
    if (VM_MUTEX) {
        VM_MUTEX = 0;
        return TRUE;
    } else {
        return FALSE;
    }

    /* return os_mutex_pend(&VM_MUTEX, 0); */
}
static inline int vm_mutex_exit(void)
{
    VM_MUTEX = 1;
    /* return os_mutex_post(&VM_MUTEX); */
    return TRUE;
}


#define VM_PAGE_ALIGN_BACKWARD(ptr) (u8 *)((u32)(ptr) & ~(FLASH_PAGE_SIZE-1))

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
#if 0   //have uncode area
typedef struct U16BIT {
#if 0//????
    u8 b15: 1;
    u8 b14: 1;
    u8 b13: 1;
    u8 b12: 1;
    u8 b11: 1;
    u8 b10: 1;
    u8 b9: 1;
    u8 b8: 1;
    u8 b7: 1;
    u8 b6: 1;
    u8 b5: 1;
    u8 b4: 1;
    u8 b3: 1;
    u8 b2: 1;
    u8 b1: 1;
    u8 b0: 1;
#else
    u8 b0: 1;
    u8 b1: 1;
    u8 b2: 1;
    u8 b3: 1;
    u8 b4: 1;
    u8 b5: 1;
    u8 b6: 1;
    u8 b7: 1;
    u8 b8: 1;
    u8 b9: 1;
    u8 b10: 1;
    u8 b11: 1;
    u8 b12: 1;
    u8 b13: 1;
    u8 b14: 1;
    u8 b15: 1;
#endif
} U16BIT ;
typedef union UBIT16 {
    U16BIT bits;
    u16 val;
} UBIT16;
static UBIT16 cd1;

static u16 do16(void)
{
    UBIT16 ufcc, urp;
    u8 f_xdat ;
    u8 f_datin;
    ufcc.bits = urp.bits = cd1.bits;
    f_datin  = 0x10 ;
    f_xdat = f_datin ^ urp.bits.b15;

    ufcc.val = urp.val << 1;
    ufcc.bits.b12 =  urp.bits.b11 ^ f_xdat;
    ufcc.bits.b5 = urp.bits.b4 ^ f_xdat ;
    ufcc.bits.b0 = f_xdat ;

    urp.bits = ufcc.bits;
    cd1.bits = ufcc.bits ;
    return ufcc.val & 0x00ff;
}
static void doe(u16 k, void *pBuf, u32 lenIn)
{
    u8 *p = (u8 *)pBuf;
    cd1.val = k;
    *p++ ^= k;
    while (--lenIn) {
        *p++ ^= do16();
    }
}

static u16 get_e(u32 page, u32 delay_cnt)
{
    u8 addr;
    u16 rdat;

    page = page ? 1 : 2;

    for (rdat = 0, addr = 0; addr < 16; addr++) {
        EFUSE_CON = (page << 4) | addr;
        EFUSE_CON |= BIT(9);//READ ENABLE

        delay(delay_cnt);//80 @6Mhtc 153us --- 120us per bit

        rdat >>= 1;

        if ((EFUSE_CON & BIT(15)) != 0) {
            rdat |= BIT(15);
        }

        EFUSE_CON &= ~BIT(9);
    }

    return rdat;
}
#endif
/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/


/*----------------------------------------------------------------------------*/
/**@brief  VM代码加载到cache中
   @param  void
   @return none
   @note   VM_CODE大小必须小于cache大小，否则出现cache溢出
*/
/*----------------------------------------------------------------------------*/
AT_VM_CODE
static void load_vm_code_to_cache(void)
{
    u32 i;
    volatile u8 tmp;

    //putbyte('#');put_u32hex0((u32)&VM_CODE_START);put_u32hex0((u32)&VM_CODE_END);
    u32 ali_start = ((u32)&vm_used_code_start & ~(CACHE_LINE_COUNT - 1));
    u32 ali_end = (u32)&vm_used_code_end + (4 * CACHE_LINE_COUNT);
    /* putbyte('#');put_u32hex((u32)ali_start);put_u32hex((u32)ali_end); */

    for (i = ali_start;  i <  ali_end; i += CACHE_LINE_COUNT) {
        tmp = *(u8 *)i;
    }
}


/*----------------------------------------------------------------------------*/
/**@brief  VM初始化检测
   @param  void
   @return TURE: 已经初始化，FALSE：还没初始化
   @note   static u8 vm_init_check(void)
*/
/*----------------------------------------------------------------------------*/
static u8 vm_init_check(void)
{
    if (VM_INIT) {
        return TRUE;
    } else {
        vm_puts("err:vm_NOT_INIT\n");
        return FALSE;
    }
}

static u8 vm_cache_check(void)
{
    if ((u32)&vm_used_code_end - (u32)&vm_used_code_start > 4 * 1024) {
        vm_puts("\n VM_CODE_END - VM_CODE_START > 4K \n ");
        return FALSE;
    } else {
        return TRUE;
    }
}
/*----------------------------------------------------------------------------*/
/**@brief  VM句柄检测
   @param  void
   @return TURE: 句柄可用，FALSE：句柄超出范围
   @note   static u8 vm_check_hdl(vm_hdl hdl)
*/
/*----------------------------------------------------------------------------*/
static u8 vm_check_hdl(vm_hdl hdl)
{
    if (hdl < VM_INDEX_DES_MAX) {
        return TRUE;
    } else {
        vm_printf("err:vm index %d is overflow,max is %d\n", hdl, VM_INDEX_DES_MAX - 1);
        return FALSE;
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  进入VM临界区
   @param  void
   @return none
   @note   与退出VM临界区配对使用
*/
/*----------------------------------------------------------------------------*/
AT_VM_CODE
static void enter_vm(void)
{
    OS_ENTER_CRITICAL();

    load_vm_code_to_cache();

    __icache_drv->suspend();

    __spi0_drv->resume();

    spiflash_continuous_read_mode_reset();
}

/*----------------------------------------------------------------------------*/
/**@brief  退出VM临界区
   @param  void
   @return none
   @note   与进入VM临界区配对使用
*/
/*----------------------------------------------------------------------------*/
AT_VM_CODE
static void exit_vm(void)
{
    spiflash_recover_continuous_read_mode();

    //icache resum
    __icache_drv->resume();

//    u32 cpu_sr = vm_decl->cpu_sr;
    OS_EXIT_CRITICAL();

    /*spi0_cs(1);*/
}


/*----------------------------------------------------------------------------*/
/**@brief  读内部flash函数
   @param  buf：缓存buf，add：flash的相对地址，len：需要读取的长度，单位byte
   @return none
   @note   static void flash_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
#if VM_SPI0_READ
_NOINLINE_ AT_VM_CODE
static void flash_read(u8 *buf, u32 addr, u32 len)
{
    _vm_decl vm_decl;
    enter_vm();

    spiflash_read(buf, addr, len);

    exit_vm();
}
#endif // VM_SPI0_READ

/*----------------------------------------------------------------------------*/
/**@brief  读内部flash函数
   @param  buf：缓存buf，add：flash的相对地址，len：需要读取的长度，单位byte
   @return none
   @note   static void flash_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
static inline void vm_cache_read(u8 *buf, u32 addr, u32 len)
{
    /* vm_puts("---vm_read---\n"); */
    memcpy(buf, (const void *)SPI_CACHE_READ_ADDR(addr), len);
    /* vm_put_buf(buf,len); */
}

/*----------------------------------------------------------------------------*/
/**@brief  读内部flash函数
   @param  buf：缓存buf，add：flash的相对地址，len：需要读取的长度，单位byte
   @return none
   @note   static void flash_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
void cache_read(u8 *buf, u32 addr, u32 len)
{
    memcpy(buf, (const void *)SPI_CACHE_READ_ADDR(addr), len);
}
/*
void cache_read(u8 *buf, u32 addr, u32 len)
{
#if 1//have uncode area
    memcpy(buf, (const void *)SPI_CACHE_READ_ADDR(addr), len);
#else
    u8 tmp_buf[CACHE_LINE_COUNT];
    u32 addr_align = (addr &~ (CACHE_LINE_COUNT-1));
    u32 t_len = addr - addr_align;
    u32 len2;

    if(addr < sfc_base_adr)
    {
        flash_read(buf, addr,len);
        return;
    }

    while(len > 0)
    {
        memcpy(tmp_buf, (const void *)SPI_CACHE_READ_ADDR(addr_align), CACHE_LINE_COUNT);

        doe(( ((u32)( SPI_CACHE_READ_ADDR(addr_align) )>>2)) ^ vm_vm_vm, tmp_buf, CACHE_LINE_COUNT);

        len2 = len>(CACHE_LINE_COUNT-t_len)? (CACHE_LINE_COUNT-t_len): len;
        memcpy(buf, tmp_buf + t_len, len2);

        buf += len2;
        len -= len2;
        t_len = 0;
        addr_align += CACHE_LINE_COUNT;
    }
#endif // 1
}

static void sdfile_dec_read(u8 *buf, u32 addr, u32 len)
{
    u8 tmp_buf[CACHE_LINE_COUNT];
    u32 addr_align = (addr &~ (CACHE_LINE_COUNT-1));
    u32 t_len = addr - addr_align;
    u32 len2;
    void (*raw_read)(u8 *, u32 , u32 );

//    if(addr < sfc_base_adr)
//        raw_read = flash_read;
//    else
        raw_read = cache_read;

    while(len > 0)
    {
        raw_read(tmp_buf, addr_align, CACHE_LINE_COUNT);
//putbyte('@');put_u32hex0(addr); put_buf(tmp_buf, CACHE_LINE_COUNT);
        doe( vm_vm_vm, tmp_buf, CACHE_LINE_COUNT);

        len2 = len>(CACHE_LINE_COUNT-t_len)? (CACHE_LINE_COUNT-t_len): len;

        memcpy(buf, tmp_buf + t_len, len2);

        buf += len2;
        len -= len2;
        t_len = 0;
        addr_align += CACHE_LINE_COUNT;
    }
}
*/

/*----------------------------------------------------------------------------*/
/**@brief  擦除内部flash函数
   @param  erase：选择擦除方式（chip、block、sector），address：擦除地址，waite：是否等待擦除完成
   @return none
   @note   void _vm_eraser(FLASH_ERASER eraser, u32 address, u8 wait_ok)
*/
/*----------------------------------------------------------------------------*/
_NOINLINE_ AT_VM_CODE
static void _vm_eraser(u8 eraser, u32 address, u8 wait_ok)
{
    __flash_suspend();
    enter_vm();

    spiflash_eraser(eraser, address, 1);

    if (eraser == BLOCK_ERASER) { //VM_MAIN_AREA_ERASE
        __icache_drv->flush((void *)SPI_CACHE_READ_ADDR(address), VM_BLOCK_SIZE);
    } else {	//VM_TEMP_AREA_ERASE
        __icache_drv->flush((void *)SPI_CACHE_READ_ADDR(address), VM_SECTOR_SIZE);
    }

    exit_vm();
    __flash_resume();
}

/*----------------------------------------------------------------------------*/
/**@brief  擦除VM区域函数
   @param  vm_area_p：VM区域
   @return none
   @note   static void _vm_area_erase(_AREA_RANGE * vm_area_p)
*/
/*----------------------------------------------------------------------------*/
//wait_ok:0 only send erase cmd, 1:waite till cmd finish
AT_VM_CODE
static bool _vm_area_erase(struct _AREA_RANGE *vm_area_p)
{
    u32 erase_addr = vm_area_p->start_addr;
    u32 erase_ali = (erase_addr + VM_BLOCK_SIZE - 1) & ~(VM_BLOCK_SIZE - 1);
    u16 num = vm_area_p->len / VM_SECTOR_SIZE;
    u16 cnt;

    vm_printf("--func = %s--\n", __FUNCTION__);

    /* if((num == 16)&&(erase_addr == erase_ali)) */
    /* { */
    /* vm_printf("_vm_eraser_block = %x---\n",erase_addr); */
    /* if(false == sfc_erase(BLOCK_ERASER, erase_addr)) */
    /* { */
    /* return false; */
    /* } */
    /* } */
    /* else if(num <= 16) */
    {
        for (cnt = 0; cnt < num; cnt++) {
            vm_printf("_vm_eraser_sector = %x---\n", erase_addr);
            if (false == sfc_erase(SECTOR_ERASER, erase_addr)) {
                return false;
            }
            erase_addr += VM_SECTOR_SIZE;
        }
    }
    /* else */
    /* { */
    /* vm_puts("_vm_area_len_NOT_surport = "); */
    /* vm_put_u16hex0(num); */
    /* while(1); */
    /* } */
    vm_area_p->erase_bit_map = 0;
    return true;
}

/*----------------------------------------------------------------------------*/
/**@brief  写flash函数
   @param  write_addr：写地址，buf：写的buf，buf_len：写的数据长度，单位byte
   @return none
   @note   void _vm_write(u32 write_addr, const u8 *buf, u32 buf_len)
*/
/*----------------------------------------------------------------------------*/
_NOINLINE_ AT_VM_CODE
static void _vm_write(const u8 *buf, u32 write_addr, u32 buf_len)
{
    /* u8 _write_buf[FLASH_PAGE_SIZE+CACHE_LINE_COUNT]; */
    u8 *_write_buf = malloc(FLASH_PAGE_SIZE + CACHE_LINE_COUNT);
    if (!_write_buf) {
        vm_puts("---vm_write_malloc_err---\n");
        while (1);
    }
    u8 *write_buf = CACHE_ADDR_ALIGN(_write_buf);

    u32 write_addr_align, w_buf_remain_len;
    u32  t_len;

    /* vm_puts("---_vm_write---\n"); */
    /* vm_printf("w_adr = %x\n",write_addr); */
    /* vm_put_buf((void *)buf, buf_len); */

    memset(_write_buf, 0x00, FLASH_PAGE_SIZE + CACHE_LINE_COUNT);

    while (1) {
        write_addr_align = (u32)VM_PAGE_ALIGN_BACKWARD(write_addr);

        if (write_addr_align != write_addr) {
            sfc_read(write_buf, write_addr_align, FLASH_PAGE_SIZE);
        } else {
            vm_memset(write_buf, 0xff, FLASH_PAGE_SIZE);
        }

        w_buf_remain_len = FLASH_PAGE_SIZE - (write_addr - write_addr_align);

        t_len = (buf_len > w_buf_remain_len) ? w_buf_remain_len : buf_len;
        vm_memcpy(write_buf + (write_addr - write_addr_align), buf, t_len);

#if 0 //VM_ERROR_DECT == 1
        if ((write_addr_align < block_addr) || (write_addr_align + FLASH_PAGE_SIZE > block_addr + VM_BLOCK_SIZE)) {
            if ((write_addr_align < sector_addr) || (write_addr_align + FLASH_PAGE_SIZE > sector_addr + VM_SECTOR_SIZE)) {
                vm_puts("\n\n ----_vm_write overflow ---error addr = ");
                vm_put_u32hex0(write_addr_align);
                vm_puts("\n");
                while (1);
            }
        }
#endif
        __flash_suspend(1);
        enter_vm();

        spiflash_write(write_buf, write_addr_align, FLASH_PAGE_SIZE);
        __icache_drv->flush((u8 *)SPI_CACHE_READ_ADDR(write_addr_align), FLASH_PAGE_SIZE);

        exit_vm();
        __flash_resume(0);

        write_addr += t_len;
        buf_len -= t_len;
        if (buf_len == 0) {
            break;
        }
        buf += t_len;
    }

    free(_write_buf);
    /* vm_puts("---vm_write_ok---\n"); */
}
/*
int _vm_enc_write(u32 addr, u8 *buf, u32 len)
{
    if((addr%FLASH_PAGE_SIZE) || (len%FLASH_PAGE_SIZE))
        return -1;

    u8 *pbuf = buf;
    u32 addr2 = addr;
    u32 t_len = len;

    while(t_len > 0)
    {
        doe(( ((u32)( SPI_CACHE_READ_ADDR(addr2) )>>2)) ^ vm_vm_vm, pbuf, CACHE_LINE_COUNT);

        pbuf += CACHE_LINE_COUNT;
        t_len -= CACHE_LINE_COUNT;
        addr2 += CACHE_LINE_COUNT;
    }

    sfc_write(buf,addr,len);
    return 0;
}
*/

/*----------------------------------------------------------------------------*/
/**@brief  VM写数据临时链表检测函数
   @param  hdl：要检测链表中是否有对应句柄的数据
   @return NULL：无有效数据，NOT_NULL：链表入口
   @note   VM_WRITE_RAM_LIST * vm_write_ram_list_check(vm_hdl hdl)
*/
/*----------------------------------------------------------------------------*/
VM_WRITE_RAM_LIST *vm_write_ram_list_check(vm_hdl hdl)
{
    VM_WRITE_RAM_LIST *ram_space;

    if (list_empty(&VM_WRITE_LIST)) {
        /* vm_puts("ram_space_list NULL\n"); */
        return NULL;
    }

    /* vm_printf("head = 0x%x\n",&vm_obj.write_list); */
    /* vm_printf("next = 0x%x\n",vm_obj.write_list.next); */
    /* vm_printf("prev = 0x%x\n",vm_obj.write_list.prev); */

    list_for_each_entry(ram_space, &VM_WRITE_LIST, list) {
        /* vm_puts("list_for_each_entry\n"); */
        if (hdl == VM_UNIT_IDX(ram_space->magic_unit)) {
            /* vm_printf("ram_space_list = 0x%x\n",ram_space); */
            return ram_space;
        }
    }

    /* vm_puts("ram_space_list NULL\n"); */
    return NULL;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM数据拷贝函数
   @param  dst_addr：目标地址（相对flash地址）。src_addr：源地址（相对flash地址）
   @return void
   @note   static void vm_data_copy(u32 dst_addr, u32 src_addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
static void vm_data_copy(u32 dst_addr, u32 src_addr, u32 len)
{
#if 1
    sfc_write((const u8 *)SPI_CACHE_READ_ADDR(src_addr), dst_addr, len);
#else
    u32 t_len;
    u8 buf[FLASH_PAGE_SIZE];

    while (1) {
        t_len = len > FLASH_PAGE_SIZE ? FLASH_PAGE_SIZE : len;
        if (t_len == 0) {
            break;
        }

        sfc_read(buf, src_addr, t_len);
        sfc_write(buf, dst_addr, t_len);
        len -= t_len;
        dst_addr += t_len;
        src_addr += t_len;
    }

#endif
}

/**
 * @brief vm_renew_all_index_last_updated_addr
 *
         VM区域扫描函数，主要查找出最后写地址和各个index的最后更新地址
 *
 * @param vm_area_p：要扫描的区域，此区域必须成为当前使用区域
 *
 * @return
 */
static u32 vm_renew_all_index_last_updated_addr(struct _AREA_RANGE *vm_area_p)
{
    u8 data_buf[LABEL_INDEX_LEN_CRC_SIZE];
    /* u8 *pdata = (u8 *)(data_buf + LABEL_INDEX_LEN_CRC_SIZE); */

    u32 label_addr = vm_area_p->start_addr +  VM_MAGIC_SIZE;
    u32 test_addr;
    u32 offset_addr = vm_area_p->start_addr;
    u32 magic_addr_end = label_addr + vm_area_p->len;

    _vm_lab vm_lab;
    _vm_len vm_len;
    _vm_idx vm_idx;
    _vm_crc vm_crc;
    _vm_crc crc_c;

    vm_printf("--func = %s--\n", __FUNCTION__);
    vm_printf("start_addr = 0x%x\n", vm_area_p->start_addr);
    vm_put_buf((void *)SPI_CACHE_READ_ADDR(vm_area_p->start_addr), 128);

    while (1) {
        sfc_read(data_buf, label_addr, LABEL_INDEX_LEN_CRC_SIZE);
        vm_put_buf(data_buf, LABEL_INDEX_LEN_CRC_SIZE);

        vm_lab = VM_UNIT_LAB(data_buf);
        vm_len = VM_UNIT_LEN(data_buf);
        vm_idx = VM_UNIT_IDX(data_buf);
        vm_crc = VM_UNIT_CRC(data_buf);

        if (vm_len && (vm_lab == VM_LABEL)) {
            crc_c = (_vm_crc)crc16((const void *)SPI_CACHE_READ_ADDR(label_addr + LABEL_INDEX_LEN_CRC_SIZE), vm_len);
            if (vm_crc == crc_c) {
                VM_IDX_ADR(vm_idx) = label_addr - offset_addr;
                VM_IDX_LEN(vm_idx) = LABEL_INDEX_LEN_CRC_SIZE + vm_len;

                label_addr += LABEL_INDEX_LEN_CRC_SIZE + vm_len;

                if (label_addr + LABEL_INDEX_LEN_CRC_SIZE >= magic_addr_end) {
                    vm_puts("vm_find_the_end_of_vm_area\n");
                    break;
                }
            } else {	//crc_err
                vm_puts("vm_find_all_index_last_updated_addr CRC_ERROR, maybe power down cause.\n");
                vm_printf("vm_crc = 0x%x\n", vm_crc);
                vm_printf("crc_c = 0x%x\n", crc_c);
                break;
            }
        } else {
            vm_puts("vm_len == 0  or vm_lab_err\n");
            vm_printf("vm_len = 0x%x\n", vm_len);
            vm_printf("vm_lab = 0x%x\n", vm_lab);
            break;
        }
    }

    return label_addr;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM整理函数
   @param  void
   @return 错误返回
   @note   整理后会切换使用区域，内部调用，整理过程需要考虑断电情况
*/
/*----------------------------------------------------------------------------*/
static vm_err _vm_defrag(u8 level)
{
    u32 vm_magic;
    vm_hdl hdl;

    printf("\n\n\n\n\n\n\n\n\n\n\n\n vm_defrag MMMMMMMMMMMM\n");

    //----------------check_befor_defrag----------------------------------///
    if (VM_AREA_ERASE_INFO(VM_AREA_UNSUED)) { //area_need_erased
        vm_puts("unused_area_need_erase_fisrt\n");
        return VM_DEFRAG_ERR;
    }

    printf("--func = %s--\n", __FUNCTION__);
    vm_put_buf((void *)SPI_CACHE_READ_ADDR(VM_AREA_ADR(VM_AREA_USING)), 128);

    //----------------start_defrag----------------------------------///
    VM_LAST_ADR = VM_AREA_ADR(VM_AREA_UNSUED) + VM_MAGIC_SIZE;

    vm_magic = VM_MAGIC_DEFRAGGING;
    sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_UNSUED), VM_MAGIC_SIZE);

    for (hdl = 0; hdl < VM_INDEX_DES_MAX; hdl++) {
        if (VM_IDX_LEN(hdl)) {
            if (VM_LAST_ADR + VM_IDX_LEN(hdl) + LABEL_INDEX_LEN_CRC_SIZE > VM_AREA_ADR(VM_AREA_UNSUED) + VM_AREA_LEN(VM_AREA_UNSUED)) {
                vm_puts("\n vmm_remove_last_updated_types-VM_WRITE_OVERFLOW---ERROR2\n");
                vm_status(&vm_obj);
                while (1);
                return VM_WRITE_OVERFLOW;
            }


#if VM_WRITE_RAM_WHEN_FULL
            VM_WRITE_RAM_LIST *ram_space;
            ram_space = vm_write_ram_list_check(hdl);
            if (ram_space) {	//use data in list first
                vm_puts("\n vm_write_ram_list_check\n");
                vm_printf("hdl = %d\n", hdl);
                vm_printf("VM_IDX_LEN = %x\n", VM_IDX_LEN(hdl));
                vm_printf("VM_LAST_ADR = %x\n", VM_LAST_ADR);
                vm_put_buf(ram_space->magic_unit, VM_IDX_LEN(hdl));

                sfc_write(ram_space->magic_unit, VM_LAST_ADR, VM_IDX_LEN(hdl));
                VM_IDX_ADR(hdl) = VM_LAST_ADR - VM_AREA_ADR(VM_AREA_UNSUED);
                VM_LAST_ADR += VM_IDX_LEN(hdl);

                list_del(&ram_space->list);
                free(ram_space);
            } else
#endif
                if (VM_IDX_ADR(hdl)) { //clean_data_in_flash
                    vm_puts("\n vm_data_copy\n");
                    vm_printf("hdl = %d\n", hdl);
                    vm_printf("VM_IDX_LEN = %x\n", VM_IDX_LEN(hdl));
                    vm_printf("VM_LAST_ADR = %x\n", VM_LAST_ADR);
                    vm_printf("VM_IDX_ADR(hdl) = %x\n", VM_IDX_ADR(hdl));
                    vm_put_buf((void *)SPI_CACHE_READ_ADDR(VM_AREA_ADR(VM_AREA_USING) + VM_IDX_ADR(hdl)), VM_IDX_LEN(hdl));

                    vm_data_copy(VM_LAST_ADR, VM_AREA_ADR(VM_AREA_USING) + VM_IDX_ADR(hdl), VM_IDX_LEN(hdl));
                    VM_IDX_ADR(hdl) = VM_LAST_ADR - VM_AREA_ADR(VM_AREA_UNSUED);
                    VM_LAST_ADR += VM_IDX_LEN(hdl);
                }
        }
    }

    vm_puts("\n--------vm_data_copy_over---------\n");
    vm_put_buf((void *)SPI_CACHE_READ_ADDR(VM_AREA_ADR(VM_AREA_UNSUED) + 0), VM_LAST_ADR - VM_AREA_ADR(VM_AREA_UNSUED));

    vm_puts("\n defrag_over_resume_magic\n");
    //----------------defrag_over_resume_magic----------------------------------///
    vm_magic = VM_MAGIC_DEFRAG_OVER;
    sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_UNSUED), VM_MAGIC_SIZE);

    if (level) {
        _vm_area_erase(&vm_obj.area[VM_AREA_USING]);	//this function will wast a log time to erase vm_erea_using
    } else {
        /* sfc_erase(SECTOR_ERASER, VM_AREA_ADR(VM_AREA_USING));	//make sure vm_area_using become unused */
        /* VM_AREA_ERASE_INFO(VM_AREA_USING) = (u16)~BIT(0);			//only sector don't need erase */

        vm_magic = VM_MAGIC_WAIT_ERASE;
        sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_USING), VM_MAGIC_SIZE);
        VM_AREA_ERASE_INFO(VM_AREA_USING) = (u16)(0xffff >> (16 - (VM_AREA_LEN(VM_AREA_UNSUED) / VM_SECTOR_SIZE)));			//all area_need_erase
        vm_printf("---VM_AREA_ERASE_INFO(VM_AREA_USING) = %x---\n", VM_AREA_ERASE_INFO(VM_AREA_USING));
    }

    //switch vm_area
    VM_AREA_USING = VM_AREA_UNSUED;

    vm_magic = VM_MAGIC_USING;
    sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_USING), VM_MAGIC_SIZE);

    //----------------defrag_finish----------------------------------///
    vm_printf("---VM_AREA_USING= %d---\n", VM_AREA_USING);

    printf("\n\n\n vm_defrag OKOKOKOKOKOK\r\n");
    return VM_ERR_NONE;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM整理函数
   @param  void
   @return 错误返回
   @note   VM整理接口
*/
/*----------------------------------------------------------------------------*/
/* vm_err vm_defrag(void) */
/* { */
/* vm_err ret; */

/* if(FALSE==vm_init_check()) */
/* { */
/* return VM_NOT_INIT; */
/* } */

/* vm_mutex_enter(); */
/* [> VM_LAST_ADR = vm_renew_all_index_last_updated_addr(&vm_obj.area[VM_AREA_USING]); <] */
/* ret = _vm_defrag(1); */
/* vm_mutex_exit(); */

/* return ret; */
/* } */


/**
 * @brief vm_area_check
 *
 *        VM区域检测函数
 *
 * @param vm_area_p：VM区域
 * @param addr：要检查的地址
 * @param len：要检查的长度
 *
 * @return
 */
static int vm_area_check(struct _AREA_RANGE *vm_area_p, u32 addr, u32 len)
{
    if (vm_bytecmp(SPI_CACHE_READ_ADDR(addr), 0xff, len)) {
        vm_printf("---addr = 0x%x---\n", addr);
        /* vm_put_buf((void *)SPI_CACHE_READ_ADDR(addr), len); */
        return FALSE;//area have some data leftover
    } else {
        return TRUE;//all area is clean
    }
}

/*----------------------------------------------------------------------------*/
/**@brief  VM_WARNING_LINE检测函数
   @param  void：默认检测单前使用区域
   @return FALSE：未到达警告线，TRUE：以到达警告
   @note   用于整理VM前检测
*/
/*----------------------------------------------------------------------------*/
static bool vm_warning_line_check(void)
{
    u32 warning_adr = VM_LINE * VM_AREA_LEN(VM_AREA_USING);
    warning_adr /= 100;

    /* vm_printf("--func = %s--\n", __FUNCTION__); */
    vm_printf("VM_LAST_ADR = %x		", VM_LAST_ADR);
    vm_printf("VM_AREA_ADR(VM_AREA_USING)= %x		", VM_AREA_ADR(VM_AREA_USING));
    vm_printf("warning_adr = %x\n", warning_adr);

    if (VM_LAST_ADR > (VM_AREA_ADR(VM_AREA_USING) + warning_adr)) {
        return TRUE;//area used arrived to warning line and need to be erase
    } else {
        return FALSE;//area have enought sapce
    }
}

static void vm_run_mode_and_keyt_init(u32 mode)
{
    flash_ioctrl(SET_READ_CMD_MODE, mode);
    flash_mode_init();

//    vm_vm_vm = get_e(0,800);
//        vm_puts("\n vm_vm_vm = ");put_u32hex0(vm_vm_vm);vm_puts("\n\n");
}

static vm_err vm_reset(void)
{
    vm_printf("--func = %s--\n", __FUNCTION__);

    u32 vm_magic = VM_MAGIC_USING;

    _vm_area_erase(&vm_obj.area[0]);
    _vm_area_erase(&vm_obj.area[1]);

    VM_AREA_USING = 0;

    sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_USING), VM_MAGIC_SIZE);
    VM_LAST_ADR = VM_AREA_ADR(VM_AREA_USING) + VM_MAGIC_SIZE;

    memset(&vm_obj.idx_desc, 0x00, sizeof(struct _VM_DESC));

    return VM_ERR_NONE;
}



/*----------------------------------------------------------------------------*/
/**@brief  VM初始化函数，所有VM操作之前都必须先初始化
   @param  vm_addr：VM起始地址
   @param  vm_len： VM总大小空间
   @param  spi_base：SFC映射的基地址
   @param  mode：spi模式
   @return none
   @note   vm_err vm_init(u32 vm_addr, u32 spi_base, u8 mode, bool need_defrag_when_write_full)
*/
/*----------------------------------------------------------------------------*/
#define AREA_A	0
#define AREA_B	1
vm_err vm_init(u32 vm_addr, u32 vm_len, u32 spi_base, u32 mode, bool need_defrag_when_write_full)
{
    vm_err ret = VM_ERR_NONE;

    u32 vm_magic;
    u32 vm_magic_a;
    u32 vm_magic_b;
    u32 remove_len;

    /* u32 vm_len = (16UL*1024);//test */

    vm_printf("--func = %s--\n", __FUNCTION__);

    ret = get_protect_info();
    if (VM_ERR_NONE != ret) {
        vm_puts("vm_init_err1\n");
        return ret;
    }
    if (TRUE == vm_init_check()) { //already_init
        vm_puts("vm_init_err2\n");
        return VM_INIT_ALREADY;
    }
    if (FALSE == vm_cache_check()) { //code_size_bigger_than_cache_size
        vm_puts("vm_init_err3\n");
        return VM_WRITE_OVERFLOW;
    }

    /* vm_addr = (vm_addr + VM_SECTOR_SIZE - 1) &~(VM_SECTOR_SIZE-1); */
    vm_addr += vm_len;

    printf("--vm_len = 0x%x--\n", vm_len);
    vm_len = (vm_len & ~(VM_SECTOR_SIZE - 1));
    if (vm_len < (2 * VM_SECTOR_SIZE)) {
        return VM_ERR_INIT;
    }
    if (vm_len > VM_MAX_SIZE) {
        vm_len = VM_MAX_SIZE;
    }
    printf("--vm_len_ali = 0x%x--\n", vm_len);

    vm_addr -= vm_len;//VM_AREA located at last addr
    printf("--vm_adr_ali = 0x%x--\n", vm_addr);

    //all_vm_info_check_ok

    sfc_base_adr = spi_base;
    vm_run_mode_and_keyt_init(mode);

    memset(&vm_obj, 0x00, sizeof(struct _VM_INFO));
    vm_len >>= 1;
    VM_AREA_LEN(AREA_A) = vm_len;
    VM_AREA_ADR(AREA_A) = vm_addr;
    VM_AREA_LEN(AREA_B) = vm_len;
    VM_AREA_ADR(AREA_B) = vm_addr + vm_len;
    VM_LINE = VM_DEFRAG_SIZE;

#if VM_WRITE_RAM_WHEN_FULL
    INIT_LIST_HEAD(&VM_WRITE_LIST);
#endif

    vm_printf("vm_area_a adr = %x\n", VM_AREA_ADR(AREA_A));
    vm_printf("vm_area_a len = %x\n", VM_AREA_LEN(AREA_A));
    vm_printf("vm_area_b adr = %x\n", VM_AREA_ADR(AREA_B));
    vm_printf("vm_area_b len = %x\n", VM_AREA_LEN(AREA_B));

    /* vm_eraser(); */
    /* vm_puts("---vm_eraser_ok----\n"); */
    /* while(1); */

    sfc_read((u8 *)&vm_magic_a, VM_AREA_ADR(AREA_A), VM_MAGIC_SIZE);
    sfc_read((u8 *)&vm_magic_b, VM_AREA_ADR(AREA_B), VM_MAGIC_SIZE);
    vm_printf("vm_area_a magic = %x\n", vm_magic_a);
    vm_printf("vm_area_b magic = %x\n", vm_magic_b);

    switch (vm_magic_a) {
    case VM_MAGIC_USING://area_a_data is complete
        if (vm_magic_b == VM_MAGIC_DEFRAG_OVER) {
            vm_magic = VM_AREA_USING;
            sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(AREA_B), VM_MAGIC_SIZE);
            VM_AREA_USING = AREA_B;
        } else {
            VM_AREA_USING = AREA_A;
        }
        break;

    case VM_MAGIC_DEFRAG_OVER://area_a_data is complete
        //set_area_a as using
        vm_magic = VM_AREA_USING;
        sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(AREA_A), VM_MAGIC_SIZE);
        VM_AREA_USING = AREA_A;
        break;

    case VM_MAGIC_UNUSED:
    case VM_MAGIC_DEFRAGGING:
    case VM_MAGIC_WAIT_ERASE:
        if (vm_magic_b == VM_MAGIC_USING) {
            VM_AREA_USING = AREA_B;
        } else if (vm_magic_b == VM_MAGIC_UNUSED) {
            vm_puts("---vm_area_all_unused---\nvm_eraser\n");
            vm_reset();
        } else {
            vm_puts("VM_MAGIC_ERR_A-1\n");
            vm_reset();
            /* while(1); */
        }
        break;
    default:
        vm_puts("---vm_area_all_err---\nvm_eraser\n");
        vm_eraser();
        break;
    }
    vm_printf("---VM_AREA_USING  = %x---\n", VM_AREA_USING);


    //check_unused_area_first
    if (FALSE == vm_area_check(&vm_obj.area[VM_AREA_UNSUED], VM_AREA_ADR(VM_AREA_UNSUED), VM_AREA_LEN(VM_AREA_UNSUED))) {
        vm_puts("vm_check_erase_err:unused\n");
        _vm_area_erase(&vm_obj.area[VM_AREA_UNSUED]);
    }

    //check_using_area_last
    VM_LAST_ADR = vm_renew_all_index_last_updated_addr(&vm_obj.area[VM_AREA_USING]);

    if (FALSE == vm_area_check(&vm_obj.area[VM_AREA_USING], VM_LAST_ADR, VM_AREA_ADR(VM_AREA_USING) + VM_AREA_LEN(VM_AREA_USING) - VM_LAST_ADR)) {
        vm_puts("vm_check_erase_err:using\n");
        _vm_defrag(1);
    }
    vm_printf("---VM_LAST_ADR= %x---\n", VM_LAST_ADR);

    if (vm_warning_line_check()) {
        vm_puts("vm_warning_line_check:arrived_warning_line\n");
        _vm_defrag(1);
    }

    vm_mutex_init();

    VM_INIT = 1;
    return ret;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM区域擦除函数
   @param  void
   @return none
   @note   vm_err vm_eraser(void)
*/
/*----------------------------------------------------------------------------*/
vm_err vm_eraser(void)
{
    vm_printf("--func = %s--\n", __FUNCTION__);

    if (FALSE == vm_init_check()) {
        return VM_NOT_INIT;
    }

    vm_mutex_enter();

    u32 vm_magic = VM_MAGIC_USING;

    _vm_area_erase(&vm_obj.area[0]);
    _vm_area_erase(&vm_obj.area[1]);

    VM_AREA_USING = 0;

    sfc_write((const u8 *)&vm_magic, VM_AREA_ADR(VM_AREA_USING), VM_MAGIC_SIZE);
    VM_LAST_ADR = VM_AREA_ADR(VM_AREA_USING) + VM_MAGIC_SIZE;

    memset(&vm_obj.idx_desc, 0x00, sizeof(struct _VM_DESC) * VM_INDEX_DES_MAX);

    vm_mutex_exit();

    return VM_ERR_NONE;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM申请
   @param  index：vm申请序号
   @param  data_len：申请的数据长度
   @return handle
   @note   s32 vm_open(u8 index, u16 data_len)
*/
/*----------------------------------------------------------------------------*/
s32 vm_open(u8 index, u16 data_len)
{
    vm_hdl hdl;

    if (vm_check_hdl(index) == FALSE) {
        return VM_INDEX_ERR;
    }

    if (0 == data_len) {
        return VM_DATA_LEN_ERR;
    }

    vm_mutex_enter();
    hdl = index;

    if (VM_IDX_LEN(hdl)) {
        if (VM_IDX_LEN(hdl) - LABEL_INDEX_LEN_CRC_SIZE != data_len) {
            vm_printf("vm_index[%d] data_len Not compatible, maybe flash was Used in other Program = %d, new len = %d\n", index, VM_IDX_LEN(hdl) - LABEL_INDEX_LEN_CRC_SIZE, data_len);
        }
    }

    VM_IDX_LEN(hdl) = data_len + LABEL_INDEX_LEN_CRC_SIZE;

    vm_mutex_exit();

    return hdl;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM关闭
   @param  index：vm申请序号
   @return handle
   @note   vm_err vm_close(vm_hdl hdl)
*/
/*----------------------------------------------------------------------------*/
vm_err vm_close(vm_hdl hdl)
{
    if (vm_check_hdl(hdl) == FALSE) {
        return VM_INDEX_ERR;
    }
    vm_mutex_enter();

    /* vm_idx_desc[hdl].v_len = 0;//reset len */
    /* VM_IDX_LEN(hdl) = 0;//reset len */

    vm_mutex_exit();
    return VM_ERR_NONE;
}


s32 vm_write(vm_hdl hdl, const void *data_buf)
{
    s32 ret;
    u8 flag_buf[LABEL_INDEX_LEN_CRC_SIZE];
    vm_printf("--func = %s--\n", __FUNCTION__);

    if (FALSE == vm_init_check()) {
        return VM_NOT_INIT;
    }

    if (vm_check_hdl(hdl) == FALSE) {
        return VM_INDEX_ERR;
    }

    vm_mutex_enter();

    if (VM_IDX_LEN(hdl) < LABEL_INDEX_LEN_CRC_SIZE) {
        vm_printf("vm_write len error->0x%x\n", VM_IDX_LEN(hdl));
        vm_mutex_exit();
        return VM_INDEX_ERR;
    }

    _vm_len vm_len = VM_IDX_LEN(hdl) - LABEL_INDEX_LEN_CRC_SIZE;
    _vm_crc data_crc = (_vm_crc)crc16(data_buf, vm_len);

    flag_buf[0] = VM_LABEL | (u8)(vm_len << 4);
    flag_buf[1] = (u8)(vm_len >> 4);
    flag_buf[2] = hdl;
    flag_buf[3] = data_crc;

    if (VM_LAST_ADR + LABEL_INDEX_LEN_CRC_SIZE + vm_len <= VM_AREA_ADR(VM_AREA_USING) + VM_AREA_LEN(VM_AREA_USING) - LABEL_INDEX_LEN_CRC_SIZE) {
        sfc_write(flag_buf, VM_LAST_ADR, LABEL_INDEX_LEN_CRC_SIZE);
        sfc_write(data_buf, VM_LAST_ADR + LABEL_INDEX_LEN_CRC_SIZE, vm_len);
        VM_IDX_ADR(hdl) = VM_LAST_ADR - VM_AREA_ADR(VM_AREA_USING);

        vm_printf("vm_write idx:%d		idx_len:0x%x	idx_adr:0x%x\n", hdl, VM_IDX_ADR(hdl), VM_IDX_LEN(hdl));
        vm_put_buf((void *)flag_buf, LABEL_INDEX_LEN_CRC_SIZE);
        vm_put_buf((void *)data_buf, vm_len);

        VM_LAST_ADR += LABEL_INDEX_LEN_CRC_SIZE + vm_len;
        ret = vm_len;
    } else { //block overflow
        vm_puts("block write overflow:\n");
        /* vm_status(&vm_obj); */

#if VM_WRITE_RAM_WHEN_FULL
        VM_WRITE_RAM_LIST *ram_space;
        ram_space = vm_write_ram_list_check(hdl);
        if (ram_space == NULL) {
            vm_printf("ram_space_malloc = %d\n", sizeof(VM_WRITE_RAM_LIST) + vm_len);
            ram_space = malloc(sizeof(VM_WRITE_RAM_LIST) + vm_len);
            if (!ram_space) {
                vm_puts("vm_write_ram_malloc_err\n");
                while (1);
                ret = VM_WRITE_OVERFLOW;
                goto EXIT;
            }
            memset(ram_space, 0x00, sizeof(VM_WRITE_RAM_LIST) + vm_len);
            list_add(&ram_space->list, &VM_WRITE_LIST);
        }

        /* memset(ram_space,0x00,sizeof(VM_WRITE_RAM_LIST)+ vm_len); */
        memcpy(ram_space->magic_unit, flag_buf, LABEL_INDEX_LEN_CRC_SIZE);
        memcpy(ram_space->data, data_buf, vm_len);
        vm_puts("vm_write_ram_buf:\n");
        vm_put_buf(ram_space->magic_unit, LABEL_INDEX_LEN_CRC_SIZE + vm_len);

        ret = vm_len;
#else
        ret = VM_WRITE_OVERFLOW;
#endif
    }

EXIT:
    vm_mutex_exit();

    return ret;
}

s32 vm_read(vm_hdl hdl, void *data_buf)
{
    s32 ret;
    u8 flag_buf[LABEL_INDEX_LEN_CRC_SIZE];
    vm_printf("--func = %s--\n", __FUNCTION__);

    if (FALSE == vm_init_check()) {
        vm_puts("VM_NOT_INIT\n");
        return VM_NOT_INIT;
    }

    if (vm_check_hdl(hdl) == FALSE) {
        vm_puts("VM_INDEX_ERR\n");
        return VM_INDEX_ERR;
    }

    vm_mutex_enter();

    if (VM_IDX_LEN(hdl) < LABEL_INDEX_LEN_CRC_SIZE) {
        /* vm_printf("vm_write len error->0x%x\n", vm_idx_desc[hdl].v_len); */
        vm_puts("VM_INDEX_ERR\n");
        ret = VM_INDEX_ERR;
        goto EXIT;
    }

    if (VM_IDX_ADR(hdl) == 0) {
        /* vm_printf("vm_write len error->0x%x\n", vm_idx_desc[hdl].v_len); */
        vm_puts("VM_READ_NO_INDEX\n");
        ret = VM_READ_NO_INDEX;
        goto EXIT;
    }

#if VM_WRITE_RAM_WHEN_FULL
    VM_WRITE_RAM_LIST *ram_space;
    ram_space = vm_write_ram_list_check(hdl);

    if (ram_space) { //clean_data_in_list
        vm_puts("read_from_ram\n");
        ret = VM_UNIT_LEN(ram_space->magic_unit);
        if (ret + LABEL_INDEX_LEN_CRC_SIZE != VM_IDX_LEN(hdl)) { //数据长度不匹配，读取失败
            vm_printf("vm_idx_len error->0x%x\n", VM_IDX_LEN(hdl));
            vm_printf("vm_ret_len error->0x%x\n", ret);
            ret = VM_READ_DATA_ERR;
            goto EXIT;
        }
        memcpy(data_buf, ram_space->data, ret);
    } else
#endif
    {
        vm_puts("read_from_flash\n");
        sfc_read(flag_buf, VM_IDX_ADR(hdl) + VM_AREA_ADR(VM_AREA_USING), LABEL_INDEX_LEN_CRC_SIZE);
        ret = VM_UNIT_LEN(flag_buf);

        if (ret + LABEL_INDEX_LEN_CRC_SIZE != VM_IDX_LEN(hdl)) { //数据长度不匹配，读取失败
            vm_printf("vm_idx_len error->0x%x\n", VM_IDX_LEN(hdl));
            vm_printf("vm_ret_len error->0x%x\n", ret);
            ret = VM_READ_DATA_ERR;
            goto EXIT;
        }

        sfc_read(data_buf, VM_IDX_ADR(hdl) + VM_AREA_ADR(VM_AREA_USING) + LABEL_INDEX_LEN_CRC_SIZE, ret);
    }

EXIT:
    vm_mutex_exit();
    return ret;
}

vm_err vm_status(struct _VM_INFO *vm_str_p)
{
    printf("---VM_AREA_USINGE = %d---\n", VM_AREA_USING);
    printf("---VM_AREA_ADR = 0x%x---\n", VM_AREA_ADR(VM_AREA_USING));
    printf("---VM_AREA_ADR = 0x%x---\n", VM_AREA_LEN(VM_AREA_USING));

    if (!vm_str_p) {
        return VM_READ_DATA_ERR;
    }
    if (vm_str_p != &vm_obj) {
        /* memcpy(vm_str_p, &vm_obj, sizeof(_VM_INFO)); */
    }

    return VM_ERR_NONE;
}


//level: 0:erase_and_return, 1:erase_all_area
//return : not clean num
static vm_err vm_erase_check(u8 level)
{
    u8 bit_cnt;
    u8 bit_size;
    u8 need_clean_num;

    u8 find_flag = 0;

    if (FALSE == vm_init_check()) {
        return VM_NOT_INIT;
    }

    vm_mutex_enter();

    bit_size = VM_AREA_LEN(VM_AREA_UNSUED) / VM_SECTOR_SIZE;
    if (level) { //erase_vm_area
        if (VM_AREA_ERASE_INFO(VM_AREA_UNSUED)) { //area_need_erased
            _vm_area_erase(&vm_obj.area[VM_AREA_UNSUED]);
        }
        need_clean_num = 0;
        goto EXIT;
    } else {	//find 1 sector and erase
        if (VM_AREA_ERASE_INFO(VM_AREA_UNSUED)) { //area_need_erased
            need_clean_num = 0;
            for (bit_cnt = 0; bit_cnt < bit_size; bit_cnt++) {
                if (VM_AREA_ERASE_INFO(VM_AREA_UNSUED) & BIT(bit_cnt)) {
                    if (find_flag == 0) { //find it
                        find_flag = 1;
                        if (true == sfc_erase(SECTOR_ERASER, VM_AREA_ADR(VM_AREA_UNSUED) + VM_SECTOR_SIZE * bit_cnt)) {
                            VM_AREA_ERASE_INFO(VM_AREA_UNSUED) &= ~BIT(bit_cnt);
                        } else {
                        }
                    } else {
                        need_clean_num++;	//count_sector_unclean_in_area
                    }
                }
            }
        } else {
            need_clean_num = 0;
        }
    }

EXIT:
    vm_mutex_exit();
    return need_clean_num;
}

/*----------------------------------------------------------------------------*/
/**@brief  VM整理函数检测函数，检测是否需要defrag
   @param  level：0：低优先级整理，以最快速度整理VM区域，1：高优先级处理VM区域
   @return NON
   @note   static vm_err vm_defrag_check(u8 level)
*/
/*----------------------------------------------------------------------------*/
static vm_err vm_defrag_check(u8 level)
{
    vm_err ret = VM_ERR_NONE;

    if (FALSE == vm_init_check()) {
        return VM_NOT_INIT;
    }

    vm_mutex_enter();
    if (TRUE == vm_warning_line_check()) { // need to be defraged
        ret = _vm_defrag(level);
    }

    if (!list_empty(&VM_WRITE_LIST)) {	// some clean data in list
        ret = _vm_defrag(level);
    }
    vm_mutex_exit();

    return ret;
}


/*----------------------------------------------------------------------------*/
/**@brief  VM检测函数
   @param  level：0：以最快速度整理VM区域，但不一定全区域整理，1：整理全VM区域
   @return NON
   @note   外部必须定时通过此接口查询VM是否需要整理。
*/
/*----------------------------------------------------------------------------*/
void vm_check_all(u8 level)
{
    s32 ret;

    ret = vm_erase_check(level);

    if (0 == ret) { //all unused atea is clean
        ret = vm_defrag_check(level);
    } else {
        vm_puts("------------VM_AREA_NOT_CLEAN------------\n");
    }

    /* return ret; */
}




//-------------------------flash_protect_function------------------//
static vm_err get_protect_info(void)
{
    PROTECT_AREA *prct_area;
    PROTECT_INFO_HEAD *head_info;
    u32 i;

    vm_puts("------------get_protect_info------------\n");

    return VM_ERR_NONE;//just for test

    head_info = (PROTECT_INFO_HEAD *)0x40000;

    /* printf("data_len = %d\n", head_info->data_len); */
    /* printf_buf((void*)head_info, sizeof(PROTECT_INFO_HEAD)); */
    /* printf_buf((void*)head_info+sizeof(PROTECT_INFO_HEAD), head_info->data_len); */

    if (head_info->head_crc != crc16(head_info, 10)) {
        return VM_ERR_PROTECT;
    }

    prct_area = (PROTECT_AREA *)((u32)head_info + sizeof(PROTECT_INFO_HEAD));
    if (head_info->data_crc != crc16(prct_area, head_info->data_len)) {
        return VM_ERR_PROTECT;
    }

    /* for(i = 0; i < (head_info->data_len / sizeof(PROTECT_AREA)); i++) */
    /* { */
    /* printf("prct_area->start_addr = %x\n", prct_area->start_addr); */
    /* printf("prct_area->len = %x\n", prct_area->len); */
    /* prct_area++; */
    /* } */

    return VM_ERR_NONE;
}

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  addr：擦地址，flash相对地址
   @return VM_ERR_NONE：检查通过，other：区域禁止操作
   @note   static vm_err protect_check(u32 addr , u32 len)
*/
/*----------------------------------------------------------------------------*/
static vm_err protect_check(u32 addr, u32 len)
{
    PROTECT_AREA *prct_area;
    PROTECT_INFO_HEAD *head_info;
    u32 i;

    vm_puts("--protect_check--\n");

    return VM_ERR_NONE;//just for test

    head_info = (PROTECT_INFO_HEAD *)0x40000;

    /* if(head_info->head_crc!=crc16(head_info,10)) */
    /* { */
    /* return VM_ERR_PROTECT;  */
    /* } */

    prct_area = (PROTECT_AREA *)((u32)head_info + sizeof(PROTECT_INFO_HEAD));
    for (i = 0; i < (head_info->data_len / sizeof(PROTECT_AREA)); i++) {
        /* printf("prct_area->start_addr = %x\n", prct_area->start_addr); */
        /* printf("prct_area->len = %x\n", prct_area->len); */

        /* printf("start_addr = %x\n", addr); */
        /* printf("len = %x\n", len); */

        /* if(head_info->data_crc!=crc16(prct_area,head_info->data_len)) */
        if (((prct_area->start_addr <= addr) && (addr < prct_area->start_addr + prct_area->len))
            || ((prct_area->start_addr <= addr + len) && (addr + len < prct_area->start_addr + prct_area->len))) {
            vm_puts("get_protect_info_err\n");
            return VM_ERR_PROTECT;
        }
        prct_area++;
    }

    return VM_ERR_NONE;
}

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  cmd：擦方式
   @param  addr：擦地址，flash相对地址
   @return 0：擦错误，1：擦成功
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
bool sfc_erase(u8 cmd, u32 addr)
{
    vm_err ret = VM_ERR_NONE;
    /*
       -----protect-code-----
    */
    if (cmd == BLOCK_ERASER) {
        ret = protect_check(addr, VM_BLOCK_SIZE);
    } else if (cmd == SECTOR_ERASER) {
        ret = protect_check(addr, VM_SECTOR_SIZE);
    } else {
        return false;
    }

    if (ret != VM_ERR_NONE) {
        return false;
    }

    _vm_eraser(cmd, addr, 1);
    return true;
}

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  buf：写内容
   @param  addr：写地址，flash相对地址
   @param  len：写长度，单位byte
   @return 0：写错误，1：写成功len
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
u32 sfc_write(const u8 *buf, u32 addr, u32 len)
{
    vm_err ret = VM_ERR_NONE;
    /*
       -----protect-code-----
    */
    ret = protect_check(addr, len);
    if (ret != VM_ERR_NONE) {
        return false;
    }

    _vm_write(buf, addr, len);
    return len;
}

/*----------------------------------------------------------------------------*/
/**@brief  内部flash
   @param  buf：读内容
   @param  addr：读地址，flash相对地址
   @param  len：读长度，单位byte
   @return 0：读错误，1：读成功
   @note   u32 sfc_read(u8 *buf, u32 addr, u32 len)
*/
/*----------------------------------------------------------------------------*/
u32 sfc_read(u8 *buf, u32 addr, u32 len)
{
    vm_err ret = VM_ERR_NONE;
    /*
       -----protect-code-----
    */
    /* ret = protect_check(addr, len); */
    /* if(ret != VM_ERR_NONE) */
    /* { */
    /* return VM_ERR_PROTECT;  */
    /* } */

    vm_cache_read(buf, addr, len);
    return len;
}

