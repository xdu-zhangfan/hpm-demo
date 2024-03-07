#define MAIN_C

#include "main.h"
#include "interrupts.h"
#include "err_print.h"
#include "init.h"
#include "config.h"
#include "misc.h"

SDRAM_DATATYPE *sdram = SDRAM_BASEADDR;

FATFS fatfs_sd_disk;
TCHAR fatfs_driver_num_buffer[FATFS_DRIVER_NUM_BUFFER_LENGTH] = {DEV_SD + '0', ':', '/', '\0'};
BYTE fatfs_work_buffer[FATFS_WORK_BUFFER_LENGTH] = {0};
FIL config_file;

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint32_t seq_buff[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
uint8_t seq_adc_channel[APP_ADC16_CH_NUM] = {BOARD_APP_ADC16_CH_1};

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(DAC_SOC_BUFF_ALIGNED_SIZE)
uint32_t dac_buffer0[DAC_BUFFER_LEN];
ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(DAC_SOC_BUFF_ALIGNED_SIZE)
uint32_t dac_buffer1[DAC_BUFFER_LEN];

int main(void)
{
    board_init();
    board_init_led_pins();

    /* Initialize buffer */
    memset(seq_buff, 0, APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES * sizeof(uint32_t));
    memset(dac_buffer0, 0, DAC_BUFFER_LEN * sizeof(uint32_t));
    memset(dac_buffer0, 0, DAC_BUFFER_LEN * sizeof(uint32_t));
    misc_gen_sine_wave(dac_buffer0, DAC_BUFFER_LEN, ADDA_SAMPLE_RATE, ADDA_SAMPLE_RATE / 20, 0, 1024, 2048);
    misc_gen_sine_wave(dac_buffer1, DAC_BUFFER_LEN, ADDA_SAMPLE_RATE, ADDA_SAMPLE_RATE / 20, PI, 1024, 2048);
    printf(" buf0: %08x, [%d, %d, %d, %d, %d]\n", (uint32_t)dac_buffer0, dac_buffer0[0], dac_buffer0[DAC_BUFFER_LEN * 1 / 4],
           dac_buffer0[DAC_BUFFER_LEN / 2], dac_buffer0[DAC_BUFFER_LEN * 3 / 4], dac_buffer0[DAC_BUFFER_LEN - 1]);
    printf(" buf1: %08x, [%d, %d, %d, %d, %d]\n", (uint32_t)dac_buffer1, dac_buffer1[0], dac_buffer1[DAC_BUFFER_LEN * 1 / 4],
           dac_buffer1[DAC_BUFFER_LEN / 2], dac_buffer1[DAC_BUFFER_LEN * 3 / 4], dac_buffer1[DAC_BUFFER_LEN - 1]);

    init_fatfs();
    init_config();
    init_test();

    init_trigger();
    init_dac();
    init_adc();
    
    printf("----------------------------------------------------------------------\n");

    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_callback_func);

    while (1)
    {
        __asm("nop");
    }
}
