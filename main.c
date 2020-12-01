/**
 * @file main.c
 * @author UBC Capstone Team 2020/2021
 * @brief Firmware entry point
 */

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_log.h"
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
#include "vcnl4040.h"
#include "mt25q.h"
#include "network.h"

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
 * @brief VCNL4040 config
 */
static vcnl4040_cfg_t vcnl4040_cfg = {
    .ps_duty              = VCNL4040_PS_DUTY_1_320,
    .ps_it                = VCNL4040_PS_IT_8T,
    .ps_out_bits          = VCNL4040_PS_HD_16_BITS,
    .smart_persistence_en = VCNL4040_SMART_PERSISTENCE_ENABLE,
    .led_curr             = VCNL4040_LED_CURRENT_200mA,
    .timeout              = 100
};

/**
 * @brief MT25Q config
 */
static mt25q_cfg_t mt25q_cfg = {
    .timeout_ms = 100
};

/**
 * @brief Initialize hardware resources used throught the system
 * 
 * @return sysret_t RET_OK if all's well
 */
static sysret_t sys_init(void)
{
    sysret_t ret = RET_ERR;

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

    /**
     * Why the delay?
     * To give enough time for the sensors' power-on sequences
     */
    nrf_delay_ms(1000);

    /* initialize system modules */
    sysret_t shell_status =  shell_init();
    NRF_LOG_INFO("SHELL    - [%s]", retcodes_desc[shell_status]);
    NRF_LOG_INFO("SPI      - [%s]", retcodes_desc[spi_init()]);
    NRF_LOG_INFO("I2C      - [%s]", retcodes_desc[i2c_init()]);
    NRF_LOG_INFO("ADXL372  - [%s]", retcodes_desc[adxl372_init(&adxl372_cfg)]);
    NRF_LOG_INFO("ICM20649 - [%s]", retcodes_desc[icm20649_init(&icm20649_cfg)]);
    NRF_LOG_INFO("VCNL4040 - [%s]", retcodes_desc[vcnl4040_init(&vcnl4040_cfg)]);
    NRF_LOG_INFO("MT25Q    - [%s]", retcodes_desc[mt25q_init(&mt25q_cfg)]);
    NRF_LOG_INFO("Datetime - [%s]", retcodes_desc[datetime_init()]);
    NRF_LOG_INFO("Network  - [%s]", retcodes_desc[network_init()]);

    while(1)
    {
        /* infinite loop */
        shell_process();
    }

    /* we should never get here */
    return 0;
}