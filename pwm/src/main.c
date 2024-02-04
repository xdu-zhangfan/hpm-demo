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

int main(void)
{
    board_init();
    board_init_led_pins();
    init_gpio();

    init_adc();

    init_fatfs();
    init_config();
    init_test();

    printf("----------------------------------------------------------------------\n");

    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_callback_func);

    gpio_write_pin(HPM_GPIO0, MOTO_ENA_PINGROUP, MOTO_ENA_PINNUM, 1);
    gpio_write_pin(HPM_GPIO0, MOTO_DIR_PINGROUP, MOTO_DIR_PINNUM, 0);

    int s = 250;

    for (int i = 0;; ++i)
    {
        gpio_toggle_pin(HPM_GPIO0, MOTO_PUL_PINGROUP, MOTO_PUL_PINNUM);
        if (i % s == 0)
        {
            gpio_toggle_pin(HPM_GPIO0, MOTO_DIR_PINGROUP, MOTO_DIR_PINNUM);
        }

        board_delay_ms(100);
    }
}
