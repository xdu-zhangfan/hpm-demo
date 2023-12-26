#include "config.h"
#include "err_print.h"

static config_ConfTypeDef __config_sys_configs;

static FRESULT __config_open(void)
{
    char *config_file_name = malloc(sizeof(char) * (FF_MAX_LFN + 1));
    memset(config_file_name, 0, sizeof(char) * (FF_MAX_LFN + 1));
    sprintf(config_file_name, "%s%s", fatfs_driver_num_buffer, CONFIG_SYSCONF_FILENAME);

    FRESULT fresult = f_open(&config_file, config_file_name, FA_WRITE | FA_READ);
    free(config_file_name);

    return fresult;
}

int config_init(void)
{
    FRESULT fresult;

    memset(&__config_sys_configs, 0, sizeof(config_ConfTypeDef));

    fresult = __config_open();
    if (fresult != FR_OK)
    {
        printf("Failed to open configuration file, cause: %s\n", show_error_string(fresult));

        if (fresult == FR_NO_FILE)
        {
            if (config_write_default_config_file() == 0)
            {
                printf("Initialized configuration file.\n");

                return 0;
            }
            else
            {
                printf("Failed to initialize configuration file.\n");

                return -1;
            }
        }
        else
        {
            return -2;
        }
    }

    unsigned int br;
    f_read(&config_file, &__config_sys_configs, sizeof(__config_sys_configs), &br);
    if (br != sizeof(__config_sys_configs))
    {
        f_close(&config_file);
        config_write_default_config_file();
    }

    return 0;
}

int config_write_default_config_file(void)
{
    char *config_file_name = malloc(sizeof(char) * (FF_MAX_LFN + 1));
    memset(config_file_name, 0, sizeof(char) * (FF_MAX_LFN + 1));
    sprintf(config_file_name, "%s%s", fatfs_driver_num_buffer, CONFIG_SYSCONF_FILENAME);

    FRESULT fresult = f_open(&config_file, config_file_name, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    free(config_file_name);
    if (fresult != FR_OK)
    {
        return fresult + 10;
    }

    memset(&__config_sys_configs, 0, sizeof(config_ConfTypeDef));
    sprintf(__config_sys_configs.sys_conf.log_file_name, "%s%s", fatfs_driver_num_buffer, CONFIG_SYSLOG_FILENAME_DEFAULT);

    unsigned int bw = 0;
    fresult = f_write(&config_file, &__config_sys_configs, sizeof(config_ConfTypeDef), &bw);
    if ((fresult != FR_OK) || (bw < sizeof(config_ConfTypeDef)))
    {
        return 1;
    }

    config_flush();

    return 0;
}

int config_flush(void)
{
    f_close(&config_file);

    if (__config_open() != FR_OK)
    {
        return -1;
    }

    return 0;
}