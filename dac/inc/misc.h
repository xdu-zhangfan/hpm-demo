#ifndef MISC_H
#define MISC_H

#include "main.h"

#define MISC_GET_MIN_VALUE(a, b) \
    ((a < b) ? a : b)
#define MISC_GET_MAX_VALUE(a, b) \
    ((a > b) ? a : b)
#define MISC_SET_BIT(val, bit) \
    (val |= ((uint32_t)1 << bit))
#define MISC_RESET_BIT(val, bit) \
    (val &= ~((uint32_t)1 << bit))

#define PI (3.1415927)

void misc_start_time(void);
uint32_t misc_get_end_time(void);
int misc_gen_sine_wave(uint32_t *buffer, uint32_t *buffer_len,
                       unsigned int sample_rate, unsigned int wave_freq, float init_phase,
                       unsigned int amptitude, unsigned int dc_offset);

#endif