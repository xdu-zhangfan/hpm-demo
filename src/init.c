#include "init.h"
#include "err_print.h"
#include "config.h"

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
        printf("Failed to mount SD card, cause: %s\n", show_error_string(fresult));
    }

    fresult = f_chdrive(fatfs_driver_num_buffer);
    if (fresult == FR_NO_FILESYSTEM)
    {
        printf("There is no File system available, making file system...\n");
        printf("Formatting the SD card, depending on the SD card capacity, the formatting process may take a long time\n");
        fresult = f_mkfs(fatfs_driver_num_buffer, NULL, fatfs_work_buffer, FATFS_WORK_BUFFER_LENGTH * sizeof(BYTE));
        if (fresult != FR_OK)
        {
            printf("Making File system failed, cause: %s\n", show_error_string(fresult));
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
            printf("Failed to mount SD card, cause: %s\n", show_error_string(fresult));
        }
    }

    return;
}

void init_hmi_uart(void)
{
    uart_config_t config = {0};

    HPM_IOC->PAD[IOC_PAD_PZ06].PAD_CTL = IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(0) | IOC_PAD_PAD_CTL_KE_SET(0);
    HPM_IOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_UART6_TXD;
    HPM_BIOC->PAD[IOC_PAD_PZ06].FUNC_CTL = IOC_PZ06_FUNC_CTL_SOC_PZ_06;

    HPM_IOC->PAD[IOC_PAD_PZ07].PAD_CTL = IOC_PAD_PAD_CTL_SPD_SET(3) | IOC_PAD_PAD_CTL_PE_SET(0) | IOC_PAD_PAD_CTL_KE_SET(0);
    HPM_IOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_UART6_RXD;
    HPM_BIOC->PAD[IOC_PAD_PZ07].FUNC_CTL = IOC_PZ07_FUNC_CTL_SOC_PZ_07;

    clock_set_source_divider(HMI_UART_CLK_NAME, clk_src_pll0_clk1, 1);
    clock_add_to_group(HMI_UART_CLK_NAME, 0);

    uart_default_config(HMI_UART, &config);
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(HMI_UART_CLK_NAME);
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;

    config.baudrate = 9600;

    config.word_length = word_length_8_bits;
    config.parity = parity_none;
    config.num_of_stop_bits = stop_bits_1;

    hpm_stat_t stat = uart_init(HMI_UART, &config);
    if (stat != status_success)
    {
        printf("failed to initialize UART\n");
        while (1)
        {
        }
    }
    printf("Initialized UART successfully.\n");
    printf("UART frequency: %d\n", clock_get_frequency(HMI_UART_CLK_NAME));

    intc_m_enable_irq_with_priority(HMI_UART_DMA_IRQ, 1);
    dmamux_config(HMI_UART_DMAMUX_CONTROLLER, HMI_UART_RX_DMAMUX_CHN, HMI_UART_RX_DMA_REQ, true);
    dmamux_config(HMI_UART_DMAMUX_CONTROLLER, HMI_UART_TX_DMAMUX_CHN, HMI_UART_TX_DMA_REQ, true);

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
