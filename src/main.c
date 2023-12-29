#define MAIN_C

#include "main.h"
#include "interrupts.h"
#include "err_print.h"
#include "init.h"
#include "config.h"
#include "dma.h"

SDRAM_DATATYPE *sdram = SDRAM_BASEADDR;

FATFS fatfs_sd_disk;
const TCHAR fatfs_driver_num_buffer[FATFS_DRIVER_NUM_BUFFER_LENGTH] = {DEV_SD + '0', ':', '/', '\0'};
BYTE fatfs_work_buffer[FATFS_WORK_BUFFER_LENGTH] = {0};
FIL config_file;
unsigned char *hmi_buffer;

int main(void)
{
    hpm_stat_t stat;
    board_init();
    board_init_led_pins();

    init_hmi_uart();

    init_fatfs();
    init_config();
    init_test();

    printf("----------------------------------------------------------------------\n");

    board_timer_create(LED_FLASH_PERIOD_IN_MS, timer_callback_func);

    rtc_config_time(HPM_RTC, 0);

    hmi_buffer = malloc(sizeof(char) * HMI_BUFFER_SIZE);
    if (hmi_buffer == NULL)
    {
        printf("Failed to alloc memory for HMI buffer.\n");
        while (1)
            ;
    }
    memset(hmi_buffer, 0, sizeof(char) * HMI_BUFFER_SIZE);

    stat = uart_rx_trigger_dma(HMI_UART_DMA_CONTROLLER,
                               HMI_UART_RX_DMA_CHN,
                               HMI_UART,
                               core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)hmi_buffer),
                               HMI_BUFFER_SIZE);
    if (stat != status_success)
    {
        printf("uart rx trigger dma failed\n");
    }

    while (1)
    {
        __asm("nop");
    }
}
