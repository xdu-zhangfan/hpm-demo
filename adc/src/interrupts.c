#include "interrupts.h"
#include "time.h"
#include "err_print.h"
#include "misc.h"

extern uint32_t seq_buff[];
extern uint8_t seq_adc_channel[];

void timer_callback_func(void)
{
    board_led_toggle();

    pwm_start_counter(BOARD_APP_ADC16_HW_TRIG_SRC);

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

    float *array = (float *)sdram;
    for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES; ++i)
    {
        array[i] = dma_data[i].result;
    }

    const uint32_t conv_core_length = 256;
    float conv_core[conv_core_length];
    for (int i = 0; i < conv_core_length; ++i)
    {
        conv_core[i] = GET_MIN_VALUE(i, conv_core_length - 1);
    }

    float *conv_result = &(array[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES]);
    misc_start_time();
    hpm_dsp_conv_f32(conv_core, conv_core_length, array, APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES, conv_result);
    printf("Convolution time: %d us\n", misc_get_end_time() / 200);

    uint32_t buffer_butwidth = 12;
    printf("Start algorithm: %d\n", buffer_butwidth);
    uint32_t max_index = 0;

    misc_start_time();
    hpm_dsp_rifft_f32(array, buffer_butwidth);
    for (int i = 0; i < 5; ++i)
    {
        array[i] = 0;
    }
    hpm_dsp_max_f32(array, APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES, &max_index);
    printf("FFT time: %d us\n", misc_get_end_time() / 200);

    printf("Max Frequency: %d, %f KHz\n", max_index, 1.0 * max_index * APP_ADC16_SAMPLE_RATE_KHZ / APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES);

    // for (int i = 0; i < APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES - 1; i++)
    // {
    //     printf("%d, %04d\n", i, array[i]);
    // }

    return;
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_adc16)