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
#include "spi.h"
#include "adxl372.h"
#include "icm20649.h"

static const adxl372_cfg_t adxl372_cfg = {
    ADXL372_ODR_6400HZ,
    ADXL372_MODE_FULLBAND,
    ADXL372_BW_3200HZ
};

static icm20649_cfg_t icm20649_cfg = {
    ICM20649_GYRO_FS_2000dps,
    false, /* disable gyro DLPF */

    ICM20649_ACCEL_FS_30g,
    false, /* enable gyro DLPF */

    100 /* sensor read timeout in ms */
};

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
    spi_init();
    adxl372_init(&adxl372_cfg);
    (void)icm20649_init(&icm20649_cfg);
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