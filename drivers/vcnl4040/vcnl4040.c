/**
 * @file vcnl4040.c
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor driver
 */

#include <stddef.h>
#include "vcnl4040.h"
#include "vcnl4040_regs.h"
#include "i2c.h"

/**
 * @brief VCNL4040 driver state
 */
typedef enum
{
    VCNL4040_STATE_UNINIT = 0, /*!< Driver uninitialized */
    VCNL4040_STATE_RUNNING     /*!< VCNL4040 sensor running */
} vcnl4040_state_t;

/**
 * @brief VCNL4040 driver instance definition
 */
typedef struct
{
    vcnl4040_cfg_t* cfg;
    vcnl4040_state_t state;
} vcnl4040_t;

/**
 * @brief VCNL4040 driver singleton
 */
static vcnl4040_t vcnl4040 =
{
    .cfg   = NULL,
    .state = VCNL4040_STATE_UNINIT
};

/**
 * @brief Configure and initialize VCNL4040 driver
 * 
 * @param cfg Driver configuration
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_init(vcnl4040_cfg_t* cfg)
{
    retcode_t ret = RET_ERR;
    uint8_t tx[VCNL4040_TX_NUMBYTES] = {0U};
    uint8_t conf1, conf2, conf3, ms;

    /* check input */
    if(!cfg)
        return ret;

    /**
     * set CONF1 & CONF2
     */
    conf1 = VCNL4040_DUTY_SET(cfg->ps_duty) | VCNL4040_IT_SET(cfg->ps_it);
    conf2 = VCNL4040_OUT_BITS_SET(cfg->ps_out_bits);
    tx[VCNL4040_TX_CMD] = VCNL4040_PS_CONF1_CONF2_ADDR;
    tx[VCNL4040_TX_LSB] = conf1;
    tx[VCNL4040_TX_MSB] = conf2;

    ret = i2c_transceive(VCNL4040_SLAVE_ADDR, tx, VCNL4040_TX_NUMBYTES, NULL, 0);

    if(ret != RET_OK)
        return ret;

    /**
     * set CONF3 & MS
     */
    conf3 = VCNL4040_SMART_PERS_SET(cfg->smart_persistence_en);
    ms = VCNL4040_LED_I_SET(cfg->led_curr);
    tx[VCNL4040_TX_CMD] = VCNL4040_PS_CONF3_MS_ADDR;
    tx[VCNL4040_TX_LSB] = conf3;
    tx[VCNL4040_TX_MSB] = ms;

    ret = i2c_transceive(VCNL4040_SLAVE_ADDR, tx, VCNL4040_TX_NUMBYTES, NULL, 0);

    if(ret != RET_OK)
        return ret;

    /**
     * update driver state
     */
    vcnl4040.cfg = cfg;
    vcnl4040.state = VCNL4040_STATE_RUNNING;

    return RET_OK;
}

/**
 * @brief Read data from VCNL4040 sensor
 * 
 * @param data - Buffer to store data
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_read(vcnl4040_data_t* data)
{
    retcode_t ret = RET_ERR;
    uint8_t tx;

    /* check input */
    if(!data)
        return ret;

    /* check state */
    if(vcnl4040.state != VCNL4040_STATE_RUNNING)
        return VCNL4040_STATE_UNINIT;

    /**
     * Read from output registers
     */
    tx = VCNL4040_PS_DATA_ADDR;
    ret = i2c_transceive(VCNL4040_SLAVE_ADDR, &tx, 1U, (uint8_t*)data, sizeof(vcnl4040_data_t));

    if(ret != RET_OK)
        return ret;

    return RET_OK;
}

/**
 * @brief Get driver state and test serial communication
 * 
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_test(void)
{
    retcode_t ret = RET_ERR;

    /**
     * read ID register to verify I2C communication
     */
    uint8_t tx = VCNL4040_ID_ADDR;
    uint8_t rx[2U] = {0U};

    ret = i2c_transceive(VCNL4040_SLAVE_ADDR, &tx, 1U, rx, 2U);

    if(ret != RET_OK)
        return ret;
    else if(rx[VCNL4040_RX_LSB] != VCNL4040_ID_LSB)
        return RET_SERIAL_ERR;
    else if(rx[VCNL4040_RX_MSB] != VCNL4040_ID_MSB)
        return RET_SERIAL_ERR;

    /**
     * Check if driver has been initialized
     */
    if(vcnl4040.state == VCNL4040_STATE_UNINIT)
        return RET_DRV_UNINIT;

    /* only get here when all tests passed */
    return RET_OK;
}