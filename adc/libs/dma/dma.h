#ifndef DMA_H
#define DMA_H

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_uart_drv.h"
#ifdef CONFIG_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"

hpm_stat_t uart_tx_trigger_dma(DMA_Type *dma_ptr,
                               uint8_t ch_num,
                               UART_Type *uart_ptr,
                               uint32_t src,
                               uint32_t size);
hpm_stat_t uart_rx_trigger_dma(DMA_Type *dma_ptr,
                               uint8_t ch_num,
                               UART_Type *uart_ptr,
                               uint32_t dst,
                               uint32_t size);

#endif