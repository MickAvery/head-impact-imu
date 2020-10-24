/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

#include <ctype.h>
#include <string.h>
#include "icm20649.h"
#include "icm20649_regs.h"
#include "spi.h"

typedef uint8_t reg_addr_t;

/**
 * @brief ICM20649 driver state definitions
 */
typedef enum
{
    ICM20649_STATE_UNINIT = 0,
    ICM20649_STATE_RUNNING
} icm20649_states_t;

/**
 * @brief ICM20649 driver handle
 */
typedef struct
{
    icm20649_states_t state; /*!< Driver state */
} icm20649_t;

/**
 * @brief Driver handle singleton
 */
static icm20649_t icm20649_handle = {
    ICM20649_STATE_UNINIT
};

/**************************************
 * local functions
 **************************************/

/**
 * @notapi
 * @brief Read rxn bytes from reg_addr, save to rx buffer
 * 
 * @param reg_addr - Register to read from
 * @param rx       - Buffer to write to
 * @param rxn      - Number of bytes to read
 */
static void read_reg(reg_addr_t reg_addr, void* rx, size_t rxn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = reg_addr | 0x80U;
    spi_transfer(SPI_INSTANCE_0, SPI_DEV_ICM20649, &addr, 1U, buf, rxn + 1U);

    (void)memcpy(rx, buf+1U, rxn);
}

/**
 * @notapi
 * @brief Check if WHOAMI register returns expected value
 *        If not, error suggests faulty SPI connection
 * 
 * @return true if WHOAMI register returns expected value, false otherwise
 */
static bool check_who_am_i(void)
{
    uint8_t whoami = 0U;
    read_reg(ICM20649_WHO_AM_I_ADDR, &whoami, 1U);

    return whoami == ICM20649_WHO_AM_I_VAL;
}

/**************************************
 * API
 **************************************/

/**
 * @brief Initialize ICM20649 driver
 * 
 * @param cfg - Driver configurations
 * @return retcode_t - Driver status
 */
retcode_t icm20649_init(icm20649_cfg_t* cfg)
{
    retcode_t ret = RET_ERR;

    return ret;
}

/**
 * @brief Test serial communication and initiate device self-test
 * 
 * @return retcode_t - Driver status
 * @retval RET_SERIAL_ERR - serial communication error
 * @retval RET_SELFTEST_ERR - device self-test failed
 */
retcode_t icm20649_test(void)
{
    retcode_t ret = RET_ERR;

    if(!check_who_am_i())
        ret = RET_SERIAL_ERR;
    else if(icm20649_handle.state == ICM20649_STATE_UNINIT)
        ret = RET_DRV_UNINIT;
    else
        ret = RET_OK;

    return ret;
}