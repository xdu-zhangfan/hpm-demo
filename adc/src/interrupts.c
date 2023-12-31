#include "interrupts.h"
#include "time.h"
#include "err_print.h"
#include "dma.h"

void timer_callback_func(void)
{
    hpm_stat_t stat;

    board_led_toggle();

    return;
}

void dma_isr(void)
{
    volatile hpm_stat_t stat_rx_chn, stat_tx_chn;

    stat_rx_chn = dma_check_transfer_status(HMI_UART_DMA_CONTROLLER, HMI_UART_RX_DMA_CHN);
    if (stat_rx_chn & DMA_CHANNEL_STATUS_TC)
    {
        hmi_rx_flag = 1;
    }

    stat_tx_chn = dma_check_transfer_status(HMI_UART_DMA_CONTROLLER, HMI_UART_TX_DMA_CHN);
    if (stat_tx_chn & DMA_CHANNEL_STATUS_TC)
    {
        hmi_tx_flag = 1;
    }

    return;
}
SDK_DECLARE_EXT_ISR_M(HMI_UART_DMA_IRQ, dma_isr)