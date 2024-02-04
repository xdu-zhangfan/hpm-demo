#include "init.h"
#include "err_print.h"
#include "config.h"

void init_gpio(void)
{
    HPM_IOC->PAD[IOC_PAD_PA06].FUNC_CTL = IOC_PA06_FUNC_CTL_GPIO_A_06;
    HPM_IOC->PAD[IOC_PAD_PC11].FUNC_CTL = IOC_PC11_FUNC_CTL_GPIO_C_11;
    HPM_IOC->PAD[IOC_PAD_PC12].FUNC_CTL = IOC_PC12_FUNC_CTL_GPIO_C_12;

    gpio_set_pin_output_with_initial(HPM_GPIO0, GPIO_OE_GPIOA, 6, 0);
    gpio_set_pin_output_with_initial(HPM_GPIO0, GPIO_OE_GPIOC, 11, 0);
    gpio_set_pin_output_with_initial(HPM_GPIO0, GPIO_OE_GPIOC, 12, 0);

    return;
}

void init_fatfs(void)
{
    FRESULT fresult;

    fresult = f_mount(&fatfs_sd_disk, fatfs_driver_num_buffer, 1);
    if (fresult == FR_OK)
    {
        printf("SD card has been mounted successfully\n");
    }
    else
    {
        printf("Failed to mount SD card, cause: %s\n", err_print_error_string(fresult));
    }

    fresult = f_chdrive(fatfs_driver_num_buffer);
    if (fresult == FR_NO_FILESYSTEM)
    {
        printf("There is no File system available, making file system...\n");
        printf("Formatting the SD card, depending on the SD card capacity, the formatting process may take a long time\n");
        fresult = f_mkfs(fatfs_driver_num_buffer, NULL, fatfs_work_buffer, FATFS_WORK_BUFFER_LENGTH * sizeof(BYTE));
        if (fresult != FR_OK)
        {
            printf("Making File system failed, cause: %s\n", err_print_error_string(fresult));
            return;
        }
        else
        {
            printf("Making file system is successful\n");
        }

        fresult = f_mount(&fatfs_sd_disk, fatfs_driver_num_buffer, 1);
        if (fresult == FR_OK)
        {
            printf("SD card has been mounted successfully\n");
        }
        else
        {
            printf("Failed to mount SD card, cause: %s\n", err_print_error_string(fresult));
        }
    }

    return;
}

void init_config(void)
{
    int res = 0;

    res = config_init();
    if (res != 0)
    {
        printf("Failed to initialize system configuration file.\n");
        while (1)
            ;
    }

    return;
}

void init_test(void)
{
    printf("Testing SDRAM... ");
    for (unsigned int i = 0; i < SDRAM_LENGTH; ++i)
    {
        sdram[i] = i;
        if (sdram[i] != i)
        {
            printf("Failed.\n");
            while (1)
                ;
        }
    }
    memset(sdram, (unsigned int)0, SDRAM_LENGTH * sizeof(SDRAM_DATATYPE));
    printf("OK\n");

    printf("Testing malloc()... ");
    int *array = malloc(8 * sizeof(int));
    if (array == NULL)
    {
        printf("Failed\n");
        while (1)
            ;
    }
    free(array);
    printf("OK (0x%08x)\n", array);

    return;
}

void init_adc(void)
{
    board_init_adc16_pins();
    board_init_adc16_clock(BOARD_APP_ADC16_BASE, true);

    adc16_config_t cfg;
    hpm_stat_t stat;

    /* initialize an ADC instance */
    adc16_get_default_config(&cfg);

    cfg.res = adc16_res_16_bits;
    cfg.conv_mode = adc16_conv_mode_period;
    cfg.adc_clk_div = adc16_clock_divider_4;
    cfg.sel_sync_ahb = (clk_adc_src_ahb0 == clock_get_source(BOARD_APP_ADC16_CLK_NAME)) ? true : false;

    stat = adc16_init(BOARD_APP_ADC16_BASE, &cfg);
    if (stat != status_success)
    {
        printf("%s initialization failed, cause: %s\n", BOARD_APP_ADC16_NAME, err_print_error_string(stat));
        while (1)
            ;
    }
    printf("Initialized %s.\n", BOARD_APP_ADC16_NAME);

    adc16_channel_config_t ch_cfg;

    /* get a default channel config */
    adc16_get_channel_default_config(&ch_cfg);

    /* initialize an ADC channel */
    ch_cfg.ch = BOARD_APP_ADC16_CH_1;
    ch_cfg.sample_cycle = 20;

    stat = adc16_init_channel(BOARD_APP_ADC16_BASE, &ch_cfg);
    if (stat != status_success)
    {
        printf("%s channel initialization failed, cause: %s\n", BOARD_APP_ADC16_NAME, err_print_error_string(stat));
        while (1)
            ;
    }
    printf("Initialized %s channel.\n", BOARD_APP_ADC16_NAME);

    adc16_prd_config_t prd_cfg;

    prd_cfg.ch = BOARD_APP_ADC16_CH_1;
    prd_cfg.prescale = 22;    /* Set divider: 2^22 clocks */
    prd_cfg.period_count = 5; /* 104.86ms when AHB clock at 200MHz is ADC clock source */

    stat = adc16_set_prd_config(BOARD_APP_ADC16_BASE, &prd_cfg);
    if (stat != status_success)
    {
        printf("%s period mode initialization failed, cause: %s\n", BOARD_APP_ADC16_NAME, err_print_error_string(stat));
        while (1)
            ;
    }
    printf("Initialized %s period mode.\n", BOARD_APP_ADC16_NAME);

    intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 2);

    return;
}