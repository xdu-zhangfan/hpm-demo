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