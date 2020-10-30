/**
 * @file vcnl4040.c
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor driver
 */

#include "vcnl4040.h"
#include "vcnl4040_regs.h"

/**
 * @brief VCNL4040 driver state
 */
typedef enum
{
    VCNL4040_STATE_UNINIT = 0, /*!< Driver uninitialized */
    VCNL4040_STATE_RUNNING     /*!< VCNL4040 sensor running */
} vcnl4040_state_t;

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

    /* TODO */

    return ret;
}