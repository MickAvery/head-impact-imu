/**
 * @file i2c.c
 * @author your name (you@domain.com)
 * @author UBC Capstone Team 2020/2021
 * @brief I2C driver
 */

#include "i2c.h"
#include "custom_board.h"
#include "nrf_drv_twi.h"

/**
 * @brief I2C configurations
 */
static const nrf_drv_twi_t i2c_handler = NRF_DRV_TWI_INSTANCE(I2C1_PERIPH);
static nrf_drv_twi_config_t i2c_conf = NRF_DRV_TWI_DEFAULT_CONFIG;

/**
 * @brief Initialize I2C driver
 * 
 * @return retcode_t Driver status, RET_OK if all's well
 */
retcode_t i2c_init(void)
{
    retcode_t ret = RET_ERR;

    i2c_conf.frequency = NRF_DRV_TWI_FREQ_400K;
    i2c_conf.scl = I2C1_SCL;
    i2c_conf.sda = I2C1_SDA;

    ret = nrf_drv_twi_init(&i2c_handler, &i2c_conf, NULL, NULL);

    return ret;
}