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
    if(ret != RET_OK)
        return ret;

    nrf_drv_twi_enable(&i2c_handler);
    if(ret != RET_OK)
        return ret;

    return ret;
}

/**
 * @brief Perform an I2C transfer, transmit followed by a receive
 * 
 * If txn is > 0, a transmit occurs. If rxn is > 0, a receive occurs.
 * 
 * @param slave_addr - slave device address
 * @param txbuf      - transmit buffer
 * @param txn        - number of bytes to transmit
 * @param rxbuf      - receive buffer
 * @param rxn        - number of bytes to receive
 * @return retcode_t Driver status
 */
retcode_t i2c_transceive(i2c_addr_t slave_addr, uint8_t* txbuf, size_t txn, uint8_t* rxbuf, size_t rxn)
{
    retcode_t ret = RET_ERR;
    bool repeated_start = rxbuf && (rxn > 0); /* set repeated start condition on transmit if read requested */

    /**
     * attempt transmit
     **/
    if(txbuf && (txn > 0))
    {
        ret = nrf_drv_twi_tx(
            &i2c_handler,
            slave_addr,
            (uint8_t const*)txbuf,
            (uint8_t)txn,
            repeated_start);

        if(ret != RET_OK)
            return ret;
    }

    /**
     * attempt receive
     **/
    if(rxbuf && (rxn > 0))
    {
        ret = nrf_drv_twi_rx(&i2c_handler,
            slave_addr,
            (uint8_t*)rxbuf,
            (uint8_t)rxn);

        if(ret != RET_OK)
            return ret;
    }

    return RET_OK;
}