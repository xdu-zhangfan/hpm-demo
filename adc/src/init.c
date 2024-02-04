#include "init.h"
#include "err_print.h"
#include "config.h"
#include "board.h"

extern uint32_t seq_buff[];
extern uint8_t seq_adc_channel[];

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

    const test_word_length = 1024;
    printf("Testing malloc()... ");
    unsigned int *array = malloc(test_word_length * 4);
    if (array == NULL)
    {
        printf("Failed\n");
        while (1)
            ;
    }

    srand(3258);
    for (int i = 0; i < test_word_length; ++i)
    {
        array[i] = rand();
    }

    struct timeval start_time = rtc_get_timeval(HPM_RTC_BASE);
    for (int i = 0; i < test_word_length; ++i)
    {
        sdram[i] = array[i];
    }
    struct timeval ending_time = rtc_get_timeval(HPM_RTC_BASE);

    free(array);
    printf("OK (0x%08x)\n", array);
    printf("Length: %d Words, Time: %d us\n", test_word_length,
           (ending_time.tv_sec * 1000000 + ending_time.tv_usec) -
               (start_time.tv_sec * 1000000 + start_time.tv_usec));

    return;
}

void init_adc(void)
{
    board_init_adc16_pins();
    board_init_adc16_clock(BOARD_APP_ADC16_BASE, true);

    hpm_stat_t stat;

    /* initialize an ADC instance */
    adc16_config_t cfg;
    adc16_get_default_config(&cfg);
    cfg.res = adc16_res_16_bits;
    cfg.conv_mode = adc16_conv_mode_sequence;
    cfg.adc_clk_div = adc16_clock_divider_4;
    cfg.sel_sync_ahb = (clk_adc_src_ahb0 == clock_get_source(BOARD_APP_ADC16_CLK_NAME)) ? true : false;
    cfg.adc_ahb_en = true;
    stat = adc16_init(BOARD_APP_ADC16_BASE, &cfg);
    if (stat != status_success)
    {
        printf("%s initialization failed, cause: %s\n", BOARD_APP_ADC16_NAME, err_print_error_string(stat));
        while (1)
            ;
    }
    intc_m_enable_irq_with_priority(BOARD_APP_ADC16_IRQn, 1);
    printf("Initialized %s.\n", BOARD_APP_ADC16_NAME);

    /* Set a sequence config */
    adc16_seq_config_t seq_cfg;
    seq_cfg.seq_len = APP_ADC16_CH_NUM;
    seq_cfg.restart_en = false;
    seq_cfg.cont_en = true;
    seq_cfg.hw_trig_en = true;
    seq_cfg.sw_trig_en = false;
    for (int i = 0; i < seq_cfg.seq_len; i++)
    {
        seq_cfg.queue[i].seq_int_en = false;
        seq_cfg.queue[i].ch = seq_adc_channel[i];
    }
    seq_cfg.queue[seq_cfg.seq_len - 1].seq_int_en = true;

    /* Trigger source initialization */
    pwm_cmp_config_t pwm_cmp_cfg;
    pwm_output_channel_t pwm_output_ch_cfg;
    pwm_set_reload(BOARD_APP_ADC16_HW_TRIG_SRC, 0, 200000 / APP_ADC16_SAMPLE_RATE_KHZ - 1);
    memset(&pwm_cmp_cfg, 0x00, sizeof(pwm_cmp_config_t));
    pwm_cmp_cfg.enable_ex_cmp = false;
    pwm_cmp_cfg.mode = pwm_cmp_mode_output_compare;
    pwm_cmp_cfg.update_trigger = pwm_shadow_register_update_on_shlk;
    pwm_cmp_cfg.cmp = 1;
    pwm_config_cmp(BOARD_APP_ADC16_HW_TRIG_SRC, 8, &pwm_cmp_cfg);
    pwm_issue_shadow_register_lock_event(BOARD_APP_ADC16_HW_TRIG_SRC);
    pwm_output_ch_cfg.cmp_start_index = 8; /* start channel */
    pwm_output_ch_cfg.cmp_end_index = 8;   /* end channel */
    pwm_output_ch_cfg.invert_output = false;
    pwm_config_output_channel(BOARD_APP_ADC16_HW_TRIG_SRC, 8, &pwm_output_ch_cfg);

    /* Trigger mux initialization */
    trgm_output_t trgm_output_cfg;
    trgm_output_cfg.invert = false;
    trgm_output_cfg.type = trgm_output_same_as_input;
    trgm_output_cfg.input = BOARD_APP_ADC16_HW_TRGM_IN;
    trgm_output_config(BOARD_APP_ADC16_HW_TRGM, BOARD_APP_ADC16_HW_TRGM_OUT_SEQ, &trgm_output_cfg);

    /* Initialize a sequence */
    adc16_dma_config_t dma_cfg;
    adc16_set_seq_config(BOARD_APP_ADC16_BASE, &seq_cfg);
    dma_cfg.start_addr = (uint32_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)seq_buff);
    dma_cfg.buff_len_in_4bytes = APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES;
    dma_cfg.stop_en = true;
    dma_cfg.stop_pos = APP_ADC16_SEQ_DMA_BUFF_LEN_IN_4BYTES - 1;
    adc16_init_seq_dma(BOARD_APP_ADC16_BASE, &dma_cfg);

    /* Enable sequence complete interrupt */
    adc16_enable_interrupts(BOARD_APP_ADC16_BASE, adc16_event_seq_dma_abort);

    return;
}