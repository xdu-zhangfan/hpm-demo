#include "interrupts.h"
#include "time.h"
#include "err_print.h"

void timer_callback_func(void)
{
    board_led_toggle();

    

    return;
}