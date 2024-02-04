#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "time.h"

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_clock_drv.h"
#include "hpm_rtc_drv.h"
#include "hpm_adc16_drv.h"
#include "hpm_pwm_drv.h"
#include "hpm_trgm_drv.h"
#include "hpm_trgmmux_src.h"
#include "hpm_gpio_drv.h"
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

#define MOTO_ENA_PINGROUP GPIO_OE_GPIOC
#define MOTO_ENA_PINNUM 12
#define MOTO_DIR_PINGROUP GPIO_OE_GPIOC
#define MOTO_DIR_PINNUM 11
#define MOTO_PUL_PINGROUP GPIO_OE_GPIOA
#define MOTO_PUL_PINNUM 6

extern sd_card_t g_sd;
extern SDRAM_DATATYPE *sdram;

extern FATFS fatfs_sd_disk;
extern TCHAR fatfs_driver_num_buffer[];
extern BYTE fatfs_work_buffer[];
extern char fatfs_log_file_name[];
extern FIL config_file;

#endif