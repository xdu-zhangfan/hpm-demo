#ifndef CONFIG_H
#define CONFIG_H

#include "main.h"

#define CONFIG_SYSCONF_FILENAME "sys.conf"
#define CONFIG_SYSLOG_FILENAME_DEFAULT "sys.log"

#define CONFIG_LOG_FILE_NAME_LENGTH FF_MAX_LFN

typedef struct config_SysConfTypeDef
{
    char log_file_name[CONFIG_LOG_FILE_NAME_LENGTH + 1];
    FIL log_file;
} config_SysConfTypeDef;

typedef struct config_ConfTypeDef
{
    config_SysConfTypeDef sys_conf;
} config_ConfTypeDef;

int config_init(void);
int config_write_default_config_file(void);
int config_flush(void);

#endif