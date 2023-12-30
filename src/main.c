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
int hmi_rx_flag = 0;

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
        while (hmi_rx_flag == 0)
            __asm("nop");

        const unsigned int send_buffer_size = 256;
        char *send_buffer = malloc(sizeof(char) * send_buffer_size);

        unsigned int recv_length = 0;
        const unsigned int recv_buffer_size = 256;
        char *recv_buffer = malloc(sizeof(char) * recv_buffer_size);

        memset(send_buffer, 0, sizeof(char) * send_buffer_size);
        sprintf(send_buffer, "prints t0.txt_maxl,0\xff\xff\xff");
        printf("Send buffer (Length %d): \"%s\"\n", strlen(send_buffer), send_buffer);
        uart_send_data(HMI_UART, send_buffer, strlen(send_buffer));

        memset(recv_buffer, 0, sizeof(char) * recv_buffer_size);
        stat = uart_receive_data(HMI_UART, recv_buffer, sizeof(recv_length));
        if (stat != status_success)
        {
            printf("Failed to recive length: %d\n", stat);
        }
        memcpy(&recv_length, recv_buffer, sizeof(int));
        printf("Length: %d\n", recv_length);

        memset(send_buffer, 0, sizeof(char) * send_buffer_size);
        sprintf(send_buffer, "prints t0.txt,%d\xff\xff\xff", recv_length);
        printf("Send buffer (Length %d): \"%s\"\n", strlen(send_buffer), send_buffer);

        memset(recv_buffer, 0, sizeof(char) * recv_buffer_size);
        stat = uart_receive_data(HMI_UART, recv_buffer, recv_length);
        if (stat != status_success)
        {
            printf("Failed to recive data: %d\n", stat);
        }
        printf("Data: %s\n", recv_buffer);

        free(send_buffer);
        free(recv_buffer);

        stat = uart_rx_trigger_dma(HMI_UART_DMA_CONTROLLER,
                                   HMI_UART_RX_DMA_CHN,
                                   HMI_UART,
                                   core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)hmi_buffer),
                                   HMI_BUFFER_SIZE);
        if (stat != status_success)
        {
            printf("uart rx trigger dma failed\n");
        }

        hmi_rx_flag = 0;
        printf("hmi_rx_flag: %d\n", hmi_rx_flag);
    }
}
