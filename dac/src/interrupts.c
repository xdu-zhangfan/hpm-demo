#include "interrupts.h"
#include "time.h"
#include "err_print.h"
#include "misc.h"

extern uint32_t seq_buff[];
extern uint8_t seq_adc_channel[];

void timer_callback_func(void)
{
    board_led_toggle();

    MISC_RESET_BIT(BOARD_APP_ADC16_BASE->SEQ_DMA_CFG, 13);

    return;
}

void isr_adc16(void)
{
    pwm_stop_counter(BOARD_APP_ADC16_HW_TRIG_SRC);
    BOARD_APP_ADC16_BASE->SEQ_DMA_CFG |= (1 << 13);
    BOARD_APP_ADC16_BASE->SEQ_DMA_CFG &= ~(1 << 13);

    uint32_t status = adc16_get_status_flags(BOARD_APP_ADC16_BASE);
    adc16_clear_status_flags(BOARD_APP_ADC16_BASE, status);

    adc16_seq_dma_data_t *dma_data = (adc16_seq_dma_data_t *)seq_buff;

    misc_start_time();
    float *array = (float *)sdram;
    float sum_value = 0;
    // Convert ADC value
    for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES; ++i)
    {
        array[i] = 3.3 * dma_data[i].result / 65536;
        sum_value += array[i];
    }

    // DC balance
    float average_value = sum_value / APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES;
    for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES; ++i)
    {
        array[i] -= average_value;
    }

    // Add Hamming window
    for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES; ++i)
    {
        float win_v = 0.54 - 0.46 * hpm_dsp_cos_f32((2 * 3.1415926 * i) / (APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES - 1));
        array[i] *= win_v;
    }
    printf("Convert time: %d us\n", misc_get_end_time() / 200);

    uint32_t buffer_butwidth = 11;
    uint32_t max_index = 0;

    misc_start_time();
    int32_t ret = hpm_dsp_rfft_f32(array, buffer_butwidth);
    if (ret != 0)
    {
        printf("Failed to calculate FFT.\n");
        return;
    }
    printf("FFT time: %d us\n", misc_get_end_time() / 200);

    for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES; ++i)
    {
        array[i] = (array[i] < 0) ? (0 - array[i]) : array[i];
    }

    float max_value = hpm_dsp_max_f32(array, APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES / 4, &max_index);

    printf("Max Frequency: %d, %f, %f Hz\n", max_index, max_value,
           1.0 * max_index * ADDA_SAMPLE_RATE / APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES);

    return;
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_adc16)

void isr_dac(void)
{
    uint32_t status;

    status = dac_get_status_flags(BOARD_DAC_BASE);
    dac_set_status_flags(BOARD_DAC_BASE, status);
    // dac_set_buffer_sw_trigger(BOARD_DAC_BASE);

    // printf("DAC int: ");

    if (DAC_IRQ_STS_BUF0_CMPT_GET(status))
    {
        // printf("buf0. ");
    }
    if (DAC_IRQ_STS_BUF1_CMPT_GET(status))
    {
        // printf("buf1. ");
    }
    // printf("\n");

    return;
}
SDK_DECLARE_EXT_ISR_M(BOARD_DAC_IRQn, isr_dac)