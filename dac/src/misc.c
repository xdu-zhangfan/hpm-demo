#include "misc.h"

static uint64_t __misc_delta_time;
void misc_start_time(void)
{
    __misc_delta_time = hpm_csr_get_core_mcycle();
}
uint32_t misc_get_end_time(void)
{
    __misc_delta_time = hpm_csr_get_core_mcycle() - __misc_delta_time;
    return __misc_delta_time;
}

int misc_gen_sine_wave(uint32_t *buffer, uint32_t *buffer_len,
                       unsigned int sample_rate, unsigned int wave_freq, float init_phase,
                       unsigned int amptitude, unsigned int dc_offset)
{
    for (int i = 0; i < buffer_len; ++i)
    {
        buffer[i] = amptitude * sin(init_phase + PI * wave_freq * i / sample_rate) + dc_offset;
    }

    return 0;
}