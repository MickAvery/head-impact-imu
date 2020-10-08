/**
 * @file main.c
 * @author UBC Capstone Team 2020/2021
 * @brief Firmware entry point
 */

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "app_timer.h"
#include "datetime.h"
#include "shell.h"

/**
 * @brief Main firmware entry point
 * 
 * @return int 
 */
int main(void)
{
    /* initialize the LFCLK to be used by multiple peripherals */
    ret_code_t ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    nrf_drv_clock_lfclk_request(NULL);

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    /* initialize system modules */
    datetime_init();
    shell_init();

    while(1)
    {
        /* infinite loop */
        shell_process();
    }

    /* we should never get here */
    return 0;
}