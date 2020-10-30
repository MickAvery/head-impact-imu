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
#include "i2c.h"
#include "adxl372.h"
#include "icm20649.h"

/**
 * @brief ADXL372 config
 */
static const adxl372_cfg_t adxl372_cfg = {
    ADXL372_ODR_6400HZ,
    ADXL372_MODE_FULLBAND,
    ADXL372_BW_3200HZ
};

/**
 * @brief ICM20649 config
 */
static icm20649_cfg_t icm20649_cfg = {
    ICM20649_GYRO_FS_2000dps,
    false, /* disable gyro DLPF */

    ICM20649_ACCEL_FS_30g,
    false, /* enable gyro DLPF */

    100 /* sensor read timeout in ms */
};

/**
 * @brief Initialize hardware resources used throught the system
 * 
 * @return retcode_t RET_OK if all's well
 */
static retcode_t sys_init(void)
{
    retcode_t ret = RET_ERR;

    if((ret = nrf_drv_clock_init()) == RET_OK)
    {
        nrf_drv_clock_lfclk_request(NULL);
        ret = app_timer_init();
    }

    return ret;
}

/**
 * @brief Main firmware entry point
 * 
 * @return int 
 */
int main(void)
{
    /* initialize system hardware */
    (void)sys_init();

    /* initialize system modules */
    spi_init();
    (void)i2c_init();
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