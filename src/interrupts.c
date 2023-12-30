#include "interrupts.h"
#include "time.h"
#include "err_print.h"
#include "dma.h"

void timer_callback_func(void)
{
    hpm_stat_t stat;

    board_led_toggle();

    // const unsigned int send_buffer_size = 256;
    // char *send_buffer = malloc(sizeof(char) * send_buffer_size);
    // memset(send_buffer, 0, sizeof(char) * send_buffer_size);

    // sprintf(send_buffer, "t1.txt=\"hello\"\xff\xff\xff");
    // printf("Send buffer (Length %d): \"%s\"\n", strlen(send_buffer), send_buffer);
    // stat = uart_tx_trigger_dma(HMI_UART_DMA_CONTROLLER,
    //                            HMI_UART_TX_DMA_CHN,
    //                            HMI_UART,
    //                            core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)send_buffer),
    //                            strlen(send_buffer));

    // if (stat != status_success)
    // {
    //     printf("uart tx trigger dma failed\n");
    // }

    // free(send_buffer);

    return;
}

void dma_isr(void)
{
    volatile hpm_stat_t stat_rx_chn, stat_tx_chn;

    stat_rx_chn = dma_check_transfer_status(HMI_UART_DMA_CONTROLLER, HMI_UART_RX_DMA_CHN);
    if (stat_rx_chn & DMA_CHANNEL_STATUS_TC)
    {
        printf("UART DMA RX: %02x %02x %02x %02x\n", hmi_buffer[0], hmi_buffer[1], hmi_buffer[2], hmi_buffer[3]);

        if (!((hmi_buffer[0] == 0x1a) &&
              (hmi_buffer[1] == 0xff) &&
              (hmi_buffer[2] == 0xff) &&
              (hmi_buffer[3] == 0xff)))
        {
            hmi_rx_flag = 1;
        }
    }

    stat_tx_chn = dma_check_transfer_status(HMI_UART_DMA_CONTROLLER, HMI_UART_TX_DMA_CHN);
    // if (stat_tx_chn & DMA_CHANNEL_STATUS_TC)
    // {
    //     printf("TX\n");
    // }

    printf("hmi_rx_flag: %d\n", hmi_rx_flag);

    return;
}
SDK_DECLARE_EXT_ISR_M(HMI_UART_DMA_IRQ, dma_isr)