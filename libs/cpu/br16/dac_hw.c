#include "dac_interface.h"
#include "mem_heap.h"


struct dac_hw {
    u8 sample_rate;
    u8 dma_size;
    u8 *dma_buffer;
    void (*handler)(void);
};

static struct dac_hw hw;

#define __this      (&hw)

static void *__dac_malloc(size)
{
    void *p;

    p = malloc(size);

    ASSERT(p != NULL, "%s:%s/%s", __FILE__, __LINE__, __func__);

    printf("dac malloc : %x\n", p);
    return p;
}

static void __dac_free(void *p)
{
    printf("dac free : %x\n", p);
    free(p);
}
/*
 *  --------------------HW SFR - digital----------------------------------
 */
static const u16 sample_rate_table[] = {
    44100, 48000, 32000, 32000,
    22050, 24000, 16000, 16000,
    11025, 12000, 8000,  8000,
};

static void __set_sample_rate(u16 sr)
{
    u8 index;

    for (index = 0; index < ARRAY_SIZE(sample_rate_table); index++) {
        if (sr == sample_rate_table[index]) {
            break;
        }
    }
    ASSERT(index != ARRAY_SIZE(sample_rate_table), "%s\n", "Sample Rate not supported!");

    SFR(DAC_CON, 0, 4, index);
}

static void __set_digital_enable(void)
{
    SFR(DAC_CON, 4, 1, 1);
}

static void __set_digital_disable(void)
{
    SFR(DAC_CON, 4, 1, 0);
}

static void __set_digital_ie(void)
{
    SFR(DAC_CON, 5, 1, 0);
}

static void __set_dc_cancelling_fileter(u8 value)
{
    SFR(DAC_CON, 12, 4, value);
}

static void __set_dma_address(int addr)
{
    ASSERT((addr % 3) == 0, "%s\n", "DMA not algin!");

    DAC_ADR = addr;
}

static void __set_dma_size(u16 size)
{
    ASSERT((size & 0x1) == 0, "%s\n", "DMA not 2n!");

    DAC_LEN = size;
}

/*
 *  --------------------HW SFR - analog----------------------------------
 */
static void __set_analog_enable(void)
{
    SFR(DAA_CON0, 0, 1, 1);
}

static void __set_analog_disable(void)
{
    SFR(DAA_CON0, 0, 1, 0);
}

static void __set_current_sel(u8 type)
{
    SFR(DAA_CON4, 0, 3, type);
}

static void __set_mute_enable(void)
{
    SFR(DAA_CON0, 7, 1, 1);
}

static void __set_mute_disable(void)
{
    SFR(DAA_CON0, 7, 1, 0);
}

static void __set_output_channel_en(u8 l_en, u8 r_en)
{
    SFR(DAA_CON0, 4, 1, l_en);
    SFR(DAA_CON0, 5, 1, r_en);
}

static void __set_output_channel_gain(u8 l_gain, u8 r_gain)
{
    SFR(DAA_CON1, 0, 5, l_gain);
    SFR(DAA_CON1, 8, 5, l_gain);
}


static void __dac_out_process(void)
{
    u8 buf_idx;

    buf_idx = DAC_CON & BIT(8);
    buf_idx >>= 8;

    if (__this->handler) {
        __this->handler();
    }
}

static void dac_irq_handler(void)
{
    if (DAC_CON & BIT(5) == 0) {
        return;
    }

    if (DAC_CON & BIT(7)) {
        DAC_CON |= BIT(6);

        __dac_out_process();
    }
}

static void digital_output_init()
{
    __set_dma_size(__this->dma_size);
    __set_dma_address(__this->dma_buffer);
}

static void analog_output_init(u8 ch_l_en, u8 ch_r_en)
{

}

static void __dac_init(void)
{

}

