#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_rtc_drv.h"
#include "hpm_uart_drv.h"
#ifdef CONFIG_HAS_HPMSDK_DMAV2
#include "hpm_dmav2_drv.h"
#else
#include "hpm_dma_drv.h"
#endif
#include "hpm_dmamux_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_common.h"

#include "hpm_math.h"
#include "hpm_sdmmc_sd.h"
#include "diskio.h"
#include "ff.h"

#define SDRAM_DATATYPE uint32_t
#define SDRAM_BASEADDR ((SDRAM_DATATYPE *)BOARD_SDRAM_ADDRESS)
#define SDRAM_LENGTH (BOARD_SDRAM_SIZE / sizeof(SDRAM_DATATYPE))

#define FATFS_DRIVER_NUM_BUFFER_LENGTH 4
#define FATFS_WORK_BUFFER_LENGTH FF_MAX_LFN

#define LED_FLASH_PERIOD_IN_MS 500

#define HMI_BUFFER_SIZE 4

#define HMI_UART HPM_UART6
#define HMI_UART_BASE HPM_UART6_BASE
#define HMI_UART_IRQ IRQn_UART6
#define HMI_UART_CLK_NAME clock_uart6

#define HMI_UART_DMA_IRQ BOARD_APP_HDMA_IRQ
#define HMI_UART_DMA_CONTROLLER BOARD_APP_HDMA
#define HMI_UART_DMAMUX_CONTROLLER BOARD_APP_DMAMUX

#define HMI_UART_RX_DMA_CHN (0U)
#define HMI_UART_RX_DMAMUX_CHN DMA_SOC_CHN_TO_DMAMUX_CHN(HMI_UART_DMA_CONTROLLER, HMI_UART_RX_DMA_CHN)
#define HMI_UART_RX_DMA_REQ HPM_DMA_SRC_UART6_RX

#define HMI_UART_TX_DMA_CHN (1U)
#define HMI_UART_TX_DMAMUX_CHN DMA_SOC_CHN_TO_DMAMUX_CHN(HMI_UART_DMA_CONTROLLER, HMI_UART_TX_DMA_CHN)
#define HMI_UART_TX_DMA_REQ HPM_DMA_SRC_UART6_TX

#ifndef MAIN_C

extern sd_card_t g_sd;
extern SDRAM_DATATYPE *sdram;

extern FATFS fatfs_sd_disk;
extern TCHAR fatfs_driver_num_buffer[];
extern BYTE fatfs_work_buffer[];
extern char fatfs_log_file_name[];
extern FIL config_file;

extern unsigned char *hmi_buffer;
#endif

#endif