#include "string.h"
#include "stdarg.h"
#include "spi_interface.h"
#include "winbond_flash.h"




struct spiflash_command {
    u8 write_cmd_mode;
    u8 read_cmd_mode;
};

static struct spiflash_command spiflash_command_t  = {
    WINBOND_PAGE_PROGRAM_QUAD,
    WINBOND_FAST_READ_MODE,
};

AT_WINBOND_FLASH_CODE
void spiflash_wait_ok(void)
{
    u32 timeout = FLASH_TIMEOUT;

    __spi0_drv->cs(0);

    __spi0_drv->write_byte(WINBOND_READ_SR1);

    while ((__spi0_drv->read_byte() & BIT(0)) && timeout) {
        timeout--;
    }
    if (timeout == 0) {
        /* vm_puts("spiflash_wait_ok timeout!\r\n"); */
        /* vm_putchar('S'); */
        /* vm_putchar('P'); */
        /* vm_putchar('I'); */
        /* vm_putchar('-'); */
        /* vm_putchar('T'); */
        /* vm_putchar('O'); */
    }

    __spi0_drv->cs(1);
}


AT_WINBOND_FLASH_CODE
void spiflash_send_write_enable(void)
{
    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_WRITE_ENABLE);
    __spi0_drv->cs(1);
}

AT_WINBOND_FLASH_CODE
void spiflash_continuous_read_mode_reset(void)
{
    if ((spiflash_command_t.read_cmd_mode == WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE) ||
        (spiflash_command_t.read_cmd_mode == WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE)) {
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(0xff);   //Quad I/O  operation
        __spi0_drv->cs(1);

        __spi0_drv->cs(0);
        __spi0_drv->write_byte(0xff);
        __spi0_drv->write_byte(0xff);   //Dual I/O  operation
        __spi0_drv->cs(1);
    }
}

AT_WINBOND_FLASH_CODE
void spiflash_send_addr(u32 addr)
{
    __spi0_drv->write_byte(addr >> 16);
    __spi0_drv->write_byte(addr >> 8);
    __spi0_drv->write_byte(addr);
}


AT_WINBOND_FLASH_CODE
void spiflash_recover_continuous_read_mode(void)
{
    u8 addr_and_mode[4];
    memset(addr_and_mode, 0, sizeof(addr_and_mode));

    if ((spiflash_command_t.read_cmd_mode == WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE) ||
        (spiflash_command_t.read_cmd_mode == WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE)) {

        switch (spiflash_command_t.read_cmd_mode) {
        case WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE:
            addr_and_mode[3] = WINBOND_CONTINUOUS_READ_ENHANCE_MODE;
            __spi0_drv->cs(0);
            __spi0_drv->write_byte(WINBOND_FAST_READ_DUAL_IO);
            __spi0_drv->ioctrl(SET_2BIT_MODE, addr_and_mode, 4);
            __spi0_drv->cs(1);
            break;

        case WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE:
            addr_and_mode[3] = WINBOND_CONTINUOUS_READ_ENHANCE_MODE;
            __spi0_drv->cs(0);
            __spi0_drv->write_byte(WINBOND_FAST_READ_QUAD_IO);
            __spi0_drv->write(addr_and_mode, sizeof(addr_and_mode));
            __spi0_drv->cs(1);
            break;
        default:
            break;
        }
    }
}


/* #define PEOTECT_SIZE         (72*1024)       //VM的（64+4）KByte 和 cfg的4KByte */
/* #define VM_ASSERT(a) \ */
/* do { \ */
/* if(!(a)){ \ */
/* vm_putbyte('E'); vm_putbyte('R'); vm_putbyte('R'); \ */
/* JL_POWER->CON |= BIT(4);\ */
/* } \ */
/* }while(0); */

/* extern u32 flash_protect_addr; */

AT_WINBOND_FLASH_CODE
s32 spiflash_write(u8 *buf, u32 addr, u32 len)
{
    /* VM_ASSERT(flash_protect_addr); */
    /* VM_ASSERT(addr >= flash_protect_addr);                     //基地址保护 */
    /* VM_ASSERT((addr + len) <= (flash_protect_addr+PEOTECT_SIZE));   //末地址保护 */

    spiflash_send_write_enable();

    __spi0_drv->cs(0);

    __spi0_drv->write_byte(spiflash_command_t.write_cmd_mode);

    spiflash_send_addr(addr) ;

    __spi0_drv->write(buf, FLASH_PAGE_SIZE);

    __spi0_drv->cs(1);

    spiflash_wait_ok();

    return len;
}

AT_WINBOND_FLASH_CODE
void spiflash_eraser(FLASH_ERASER eraser, u32 address, u8 wait_ok)
{
    u8 eraser_cmd ;

    if (eraser == BLOCK_ERASER) {
        eraser_cmd = WINBOND_BLOCK_ERASE;
    } else {
        eraser_cmd = WINBOND_SECTOR_ERASE;
    }

    spiflash_send_write_enable();

    __spi0_drv->cs(0);
    __spi0_drv->write_byte(eraser_cmd);
    spiflash_send_addr(address);
    __spi0_drv->cs(1);

    if (wait_ok) {
        spiflash_wait_ok();
    }
}

AT_WINBOND_FLASH_CODE
void spiflash_eraser_suspend(void)
{
    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_ERASE_SUSPEND);
    __spi0_drv->cs(1);

    //suspend will done after about 2us
    spiflash_wait_ok();
}

AT_WINBOND_FLASH_CODE
void spiflash_eraser_resume(void)
{
    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_ERASE_RESUME);
    __spi0_drv->cs(1);

    /* spiflash_wait_ok(); */
}

#if 0
AT_WINBOND_FLASH_CODE
static void spiflash_write_protect(u8 pro_cmd)
{
    u8 status_reg1, status_reg2;

    spiflash_send_write_enable();

    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_READ_SR1);
    status_reg1 = __spi0_drv->read_byte();
    __spi0_drv->cs(1);

    status_reg1 &= ~0x7c;
    status_reg1 |= pro_cmd;

    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_READ_SR2);
    status_reg2 = __spi0_drv->read_byte();
    __spi0_drv->cs(1);

    //meybe cause something wrong? keep it
    status_reg2 &= ~(BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6));

    __spi0_drv->cs(0);
    __spi0_drv->write_byte(WINBOND_WRITE_SR);
    __spi0_drv->write_byte(status_reg1);
    __spi0_drv->write_byte(status_reg2);
    __spi0_drv->cs(1);

    spiflash_wait_ok();
}
#endif


s32 spiflash_read(u8 *buf, u32 addr, u16 len)
{
//    static u8 con_mode = 0;
    u8 addr_and_mode[4];

    switch (spiflash_command_t.read_cmd_mode) {
    case WINBOND_READ_DATA_MODE:
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_READ_DATA);
        spiflash_send_addr(addr);
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_MODE:
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_FAST_READ);
        spiflash_send_addr(addr);
        __spi0_drv->read_byte(); //DUMMY_BYTE
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_DUAL_IO_NORMAL_READ_MODE:
        addr_and_mode[0] = (u8)(addr >> 16);
        addr_and_mode[1] = (u8)(addr >> 8);
        addr_and_mode[2] = (u8)addr;
        addr_and_mode[3] = WINBOND_CONTINUOUS_READ_NORMAL_MODE;
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_FAST_READ_DUAL_IO);
        __spi0_drv->ioctrl(SET_2BIT_MODE, addr_and_mode, 4);
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE:
        addr_and_mode[0] = (u8)(addr >> 16);
        addr_and_mode[1] = (u8)(addr >> 8);
        addr_and_mode[2] = (u8)addr;
        addr_and_mode[3] = WINBOND_CONTINUOUS_READ_ENHANCE_MODE;
        __spi0_drv->cs(0);
//            if(!con_mode)
        {
//                con_mode = 1;
            __spi0_drv->write_byte(WINBOND_FAST_READ_DUAL_IO);
        }
        __spi0_drv->ioctrl(SET_2BIT_MODE, addr_and_mode, 4);
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_DUAL_OUTPUT_MODE:
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_FAST_READ_DUAL_OUTPUT);
        spiflash_send_addr(addr);
        __spi0_drv->read(addr_and_mode, 2);	//8 dummy clocks
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_QUAD_OUTPUT_MODE:
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_FAST_READ_QUAD_OUTPUT);
        spiflash_send_addr(addr);
        __spi0_drv->read(addr_and_mode, 4); //8 dummy clocks
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_QUAD_IO_NORMAL_READ_MODE:
        addr_and_mode[0] = (u8)(addr >> 16);
        addr_and_mode[1] = (u8)(addr >> 8);
        addr_and_mode[2] = (u8)addr;
        addr_and_mode[3] = WINBOND_CONTINUOUS_READ_NORMAL_MODE;
        __spi0_drv->cs(0);
        __spi0_drv->write_byte(WINBOND_FAST_READ_QUAD_IO);
        __spi0_drv->write(addr_and_mode, sizeof(addr_and_mode));
        __spi0_drv->read(addr_and_mode, 2);  //4 dummy clocks
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;

    case WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE:
        addr_and_mode[0] = (u8)(addr >> 16);
        addr_and_mode[1] = (u8)(addr >> 8);
        addr_and_mode[2] = (u8)addr;
        addr_and_mode[3] = WINBOND_CONTINUOUS_READ_ENHANCE_MODE;
        __spi0_drv->cs(0);
//            if(!con_mode)
        {
//                con_mode = 1;
            __spi0_drv->write_byte(WINBOND_FAST_READ_QUAD_IO);
        }
        __spi0_drv->write(addr_and_mode, sizeof(addr_and_mode));
        __spi0_drv->read(addr_and_mode, 2);  //4 dummy clocks
        __spi0_drv->read(buf, len);
        __spi0_drv->cs(1);
        break;
    default:
        break;
    }
    return len;
}


AT_WINBOND_FLASH_CODE
void flash_mode_init(void)
{
    switch (spiflash_command_t.read_cmd_mode) {
    case WINBOND_READ_DATA_MODE:
    case WINBOND_FAST_READ_MODE:
        __spi0_drv->ioctrl(SET_WIRE_MODE, SPI_ODD_MODE);
        spiflash_command_t.write_cmd_mode = WINBOND_PAGE_PROGRAM;
        break;

    case WINBOND_FAST_READ_DUAL_IO_NORMAL_READ_MODE:
    case WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE:
    case WINBOND_FAST_READ_DUAL_OUTPUT_MODE:
        __spi0_drv->ioctrl(SET_WIRE_MODE, SPI_DUAL_MODE);
        spiflash_command_t.write_cmd_mode = WINBOND_PAGE_PROGRAM;
        break;

    case WINBOND_FAST_READ_QUAD_IO_NORMAL_READ_MODE:
    case WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE:
    case WINBOND_FAST_READ_QUAD_OUTPUT_MODE:
        __spi0_drv->ioctrl(SET_WIRE_MODE, SPI_QUAD_MODE);
        spiflash_command_t.write_cmd_mode = WINBOND_PAGE_PROGRAM_QUAD;
        break;
    }
}

AT_WINBOND_FLASH_CODE
u8 boot_parm_2_mode(u32 parm)
{
    u8 mode;

    if (((parm & 3) == 0) || ((parm & 3) == 1)) {
        //spi2_parm = SPI_ODD_MODE;
        mode = WINBOND_READ_DATA_MODE;
    } else if ((parm & 3) == 2) {
        //spi2_parm = SPI_DUAL_MODE;
        if (parm & BIT(3)) {
            mode = WINBOND_FAST_READ_DUAL_OUTPUT_MODE;
        } else {
            if (parm & BIT(2)) {
                mode = WINBOND_FAST_READ_DUAL_IO_CONTINUOUS_READ_MODE;
            } else {
                mode = WINBOND_FAST_READ_DUAL_IO_NORMAL_READ_MODE;
            }
        }
    } else {
        if (parm & BIT(3)) {
            mode = WINBOND_FAST_READ_QUAD_OUTPUT_MODE;
        } else {
            if (parm & BIT(2)) {
                mode = WINBOND_FAST_READ_QUAD_IO_CONTINUOUS_READ_MODE;
            } else {
                mode = WINBOND_FAST_READ_QUAD_IO_NORMAL_READ_MODE;
            }
        }
    }
    return mode;
}

AT_WINBOND_FLASH_CODE
void flash_ioctrl(int ctrl, ...)
{
    va_list argptr;
    va_start(argptr, ctrl);

    switch (ctrl) {
    case SET_READ_CMD_MODE:
        spiflash_command_t.read_cmd_mode = boot_parm_2_mode(va_arg(argptr, int));
        break;
    }

    va_end(argptr);
}
