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

typedef struct
{
    vcnl4040_cfg_t* cfg;
    vcnl4040_state_t state;
} vcnl4040_t;

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

    /* TODO */

    return ret;
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

    /* TODO */

    return ret;
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
    else if((rx[0] != VCNL4040_ID_LSB) || (rx[1] != VCNL4040_ID_MSB))
        return RET_SERIAL_ERR;

    /**
     * Check if driver has been initialized
     */
    if(vcnl4040.state == VCNL4040_STATE_UNINIT)
        return RET_DRV_UNINIT;

    /* only get here when all tests passed */
    return RET_OK;
}