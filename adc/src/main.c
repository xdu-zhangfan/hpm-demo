#define MAIN_C

#include "main.h"
#include "interrupts.h"
#include "err_print.h"
#include "init.h"
#include "config.h"

SDRAM_DATATYPE *sdram = SDRAM_BASEADDR;

FATFS fatfs_sd_disk;
TCHAR fatfs_driver_num_buffer[FATFS_DRIVER_NUM_BUFFER_LENGTH] = {DEV_SD + '0', ':', '/', '\0'};
BYTE fatfs_work_buffer[FATFS_WORK_BUFFER_LENGTH] = {0};
FIL config_file;

ATTR_PLACE_AT_NONCACHEABLE_WITH_ALIGNMENT(ADC_SOC_DMA_ADDR_ALIGNMENT)
uint32_t seq_buff[APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES];
uint8_t seq_adc_channel[APP_ADC16_CH_NUM] = {BOARD_APP_ADC16_CH_1};

int main(void)
{
    board_init();
    board_init_led_pins();

    init_fatfs();
    init_config();
    init_test();

    init_adc();

    printf("----------------------------------------------------------------------\n");

    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_callback_func);

    while (1)
    {
        __asm("nop");
    }
}
