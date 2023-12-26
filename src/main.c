#define MAIN_C

#include "main.h"
#include "interrupts.h"
#include "err_print.h"
#include "init.h"
#include "config.h"

SDRAM_DATATYPE *sdram = SDRAM_BASEADDR;

FATFS fatfs_sd_disk;
const TCHAR fatfs_driver_num_buffer[FATFS_DRIVER_NUM_BUFFER_LENGTH] = {DEV_SD + '0', ':', '/', '\0'};
BYTE fatfs_work_buffer[FATFS_WORK_BUFFER_LENGTH] = {0};
FIL config_file;

int main(void)
{
    board_init();
    board_init_led_pins();

    init_fatfs();
    init_config();
    init_test();

    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_callback_func);

    while (1)
    {
    }
}
