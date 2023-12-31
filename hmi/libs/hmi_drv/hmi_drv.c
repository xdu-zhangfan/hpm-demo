#include "hmi_drv.h"

volatile unsigned char *hmi_buffer;

volatile int hmi_rx_flag = 0;
volatile int hmi_tx_flag = 0;
volatile int hmi_start_time = 0;

volatile int hmi_info_page_id = 0;

int hmi_mainloop(void)
{
    hpm_stat_t stat;

    while (hmi_rx_flag == 0)
        __asm("nop");

    if (hmi_buffer[0] == 0x01)
    {
        hmi_start_time = rtc_get_time(HPM_RTC);

        unsigned int recv_length = 0;
        const unsigned int recv_buffer_size = 256;
        char *recv_buffer = malloc(sizeof(char) * recv_buffer_size);

        // Receive string length from HMI display screen
        hmi_rx_flag = 0;
        memset(hmi_buffer, 0, HMI_DRV_BUFFER_SIZE);
        stat = uart_rx_trigger_dma(HMI_UART_DMA_CONTROLLER,
                                   HMI_UART_RX_DMA_CHN,
                                   HMI_UART,
                                   core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)hmi_buffer),
                                   HMI_DRV_BUFFER_SIZE);
        if (stat != status_success)
        {
            printf("uart rx trigger dma failed\n");
        }
        while (hmi_rx_flag == 0)
        {
            if (rtc_get_time(HPM_RTC) - hmi_start_time > HMI_DRV_DELAY_TIME_SEC)
            {
                free(recv_buffer);
                return 0;
            }
        }

        memcpy(&recv_length, hmi_buffer, sizeof(int));
        printf("Length: %d\n", recv_length);

        // Receive string from HMI display screen
        if (recv_length > 0)
        {
            hmi_rx_flag = 0;
            memset(recv_buffer, 0, sizeof(char) * recv_buffer_size);
            stat = uart_rx_trigger_dma(HMI_UART_DMA_CONTROLLER,
                                       HMI_UART_RX_DMA_CHN,
                                       HMI_UART,
                                       core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)recv_buffer),
                                       recv_length);
            if (stat != status_success)
            {
                printf("uart rx trigger dma failed\n");
            }
            while (hmi_rx_flag == 0)
            {
                if (rtc_get_time(HPM_RTC) - hmi_start_time > HMI_DRV_DELAY_TIME_SEC)
                {
                    free(recv_buffer);
                    return -1;
                }
            }

            const unsigned int send_buffer_size = 256;
            char *send_buffer = malloc(sizeof(char) * send_buffer_size);
            memset(send_buffer, 0, sizeof(char) * send_buffer_size);

            // Reload string to HMI display screen
            sprintf(send_buffer, "t1.txt=\"%s\"\xff\xff\xff", recv_buffer);
            stat = uart_tx_trigger_dma(HMI_UART_DMA_CONTROLLER,
                                       HMI_UART_TX_DMA_CHN,
                                       HMI_UART,
                                       core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)send_buffer),
                                       strlen(send_buffer));
            if (stat != status_success)
            {
                printf("uart rx trigger dma failed\n");
            }
            while (hmi_tx_flag == 0)
            {
                if (rtc_get_time(HPM_RTC) - hmi_start_time > HMI_DRV_DELAY_TIME_SEC)
                {
                    free(send_buffer);
                    free(recv_buffer);
                    return -2;
                }
            }

            free(send_buffer);

            printf("Data: %s\n", recv_buffer);
        }

        free(recv_buffer);
    }

    return 0;
}