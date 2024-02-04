#ifndef MISC_H
#define MISC_H

#include "main.h"

#define GET_MIN_VALUE(a, b) \
    ((a < b) ? a : b)
#define GET_MAX_VALUE(a, b) \
    ((a > b) ? a : b)

void misc_start_time(void);
uint32_t misc_get_end_time(void);

#endif