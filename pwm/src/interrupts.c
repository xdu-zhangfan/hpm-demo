#include "interrupts.h"
#include "time.h"
#include "err_print.h"

void timer_callback_func(void)
{
    board_led_toggle();

    return;
}

void isr_adc16(void)
{
    uint32_t status = adc16_get_status_flags(BOARD_APP_ADC16_BASE);

    printf("ADC16 INT.\n");

    return;
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_ADC16_IRQn, isr_adc16)