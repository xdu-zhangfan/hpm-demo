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

    printf("----------------------------------------------------------------------\n");

    return;
}
