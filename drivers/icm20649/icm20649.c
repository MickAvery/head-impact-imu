/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

#include <string.h>
#include "icm20649.h"
#include "icm20649_regs.h"
#include "spi.h"
#include "app_timer.h"
#include "nrf_assert.h"

typedef uint8_t reg_addr_t;

/**************************************
 * driver objects
 **************************************/

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
    icm20649_cfg_t* cfg;
    icm20649_states_t state; /*!< Driver state */
} icm20649_t;

/**
 * @brief Driver handle singleton
 */
static icm20649_t icm20649_handle = {
    NULL,
    ICM20649_STATE_UNINIT
};

/**************************************
 * timer objects to detect
 * data ready timeout
 **************************************/

/**
 * @brief timer handle
 */
APP_TIMER_DEF(gp_timer);

/**
 * @notapi
 * @brief on timeout, set return code appropariately
 * 
 * @param p_ctx - this should be pointer to return code
 */
static void timeout_handler(void* p_ctx)
{
    /* when timer times out, set retcode */
    *((sysret_t*)p_ctx) = RET_TIMEOUT;
}

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
static sysret_t read_reg(reg_addr_t reg_addr, void* rx, size_t rxn)
{
    sysret_t ret = RET_ERR;
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = reg_addr | 0x80U;
    ret = spi_transfer(SPI_INSTANCE_0, SPI_DEV_ICM20649, &addr, 1U, buf, rxn + 1U);

    (void)memcpy(rx, buf+1U, rxn);
    return ret;
}

/**
 * @notapi
 * @brief Write txn bytes to reg_addr
 * 
 * @param reg_addr - Register to write to
 * @param tx       - Bytes to write
 * @param txn      - Number of bytes to write
 */
static sysret_t write_reg(reg_addr_t reg_addr, void* tx, size_t txn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */

    /* format buffer to send */
    buf[0] = reg_addr;
    memcpy(buf+1, tx, txn);

    return spi_transfer(SPI_INSTANCE_0, SPI_DEV_ICM20649, buf, txn+1, NULL, 0);
}

/**
 * @notapi
 * @brief Select a USER BANK
 * 
 * @param usr_bank - USER BANK to select
 */
static sysret_t set_usr_bank(icm20649_usr_bank_t usr_bank)
{
    sysret_t ret = RET_OK;
    static icm20649_usr_bank_t current_usr_bank = ICM20649_USR_BANK_MAX;

    if(usr_bank != current_usr_bank)
    {
        uint8_t tx = (uint8_t)usr_bank;
        ret = write_reg(ICM20649_REG_BANK_SEL_ADDR, &tx, 1U);

        if(ret == RET_OK)
            current_usr_bank = usr_bank;
    }

    return ret;
}

/**
 * @notapi
 * @brief Set accelerometer configurations
 * 
 * @param cfg - Config settings
 */
static sysret_t config_accel(icm20649_cfg_t* cfg)
{
    sysret_t ret = RET_ERR;
    uint8_t tx = 0U;

    ret = set_usr_bank(ICM20649_USR_BANK_2);
    SYSRET_CHECK(ret);

    /* TODO: handle DLPF configs */

    /* set fullscale */
    tx = ICM20649_ACCEL_FS_SEL_SET(cfg->accel_fs);
    return write_reg(ICM20649_ACCEL_CONFIG_ADDR, &tx, 1U);
}

/**
 * @notapi
 * @brief Set gyroscope configurations
 * 
 * @param cfg - Config settings
 */
static sysret_t config_gyro(icm20649_cfg_t* cfg)
{
    sysret_t ret = RET_ERR;
    uint8_t tx = 0U;

    ret = set_usr_bank(ICM20649_USR_BANK_2);
    SYSRET_CHECK(ret);

    /* TODO: handle DLPF configs */

    /* set fullscale */
    tx = ICM20649_GYRO_FS_SEL_SET(cfg->gyro_fs);
    return write_reg(ICM20649_GYRO_CONFIG_1_ADDR, &tx, 1U);
}

/**
 * @notapi
 * @brief Wait for data ready signal from sensor
 * 
 * @retval RET_OK - data ready to read
 * @retval RET_TIMEOUT - timeout reached when waiting for data ready
 */
static sysret_t wait_data_rdy(void)
{
    sysret_t ret = RET_ERR;
    uint8_t rx = 0U;

    /* set USR BANK */
    ret = set_usr_bank(ICM20649_USR_BANK_0);
    SYSRET_CHECK(ret);

    /* start timer to detect timeout */
    ret = app_timer_start(
        gp_timer,
        APP_TIMER_TICKS(icm20649_handle.cfg->timeout),
        &ret);
    SYSRET_CHECK(ret);

    do
    {
        ret = read_reg(ICM20649_DATA_RDY_STATUS_ADDR, &rx, 1U);

        if(ret != RET_OK)
            break;
        else if((rx & ICM20649_DATA_RDY_MASK_NO_FIFO) > 0U)
        {
            ret = RET_OK;
            break;
        }
    } while(ret != RET_TIMEOUT);

    /* stop timer */
    (void)app_timer_stop(gp_timer);

    return ret;
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
    (void)set_usr_bank(ICM20649_USR_BANK_0);
    (void)read_reg(ICM20649_WHO_AM_I_ADDR, &whoami, 1U);

    return whoami == ICM20649_WHO_AM_I_VAL;
}

/**************************************
 * API
 **************************************/

/**
 * @brief Initialize ICM20649 driver
 * 
 * @param cfg - Driver configurations
 * @return sysret_t - Driver status
 */
sysret_t icm20649_init(icm20649_cfg_t* cfg)
{
    ASSERT(cfg);
    sysret_t ret = RET_ERR;

    if(!check_who_am_i())
        ret = RET_SERIAL_ERR;
    else
    {
        uint8_t tx;

        icm20649_handle.cfg = cfg;

        /* set usr bank */
        ret = set_usr_bank(ICM20649_USR_BANK_0);
        SYSRET_CHECK(ret);

        /* set USER_CTRL, disable all */
        tx = 0x00U;
        ret = write_reg(ICM20649_USER_CTRL_ADDR, &tx, 1U);
        SYSRET_CHECK(ret);

        /* set LP_CONFIG, disable duty cycle modes */
        tx = 0x00U;
        ret = write_reg(ICM20649_LP_CONFIG_ADDR, &tx, 1U);
        SYSRET_CHECK(ret);

        /* clock select */
        tx = ICM20649_CLKSEL_SET(0x01U); /* From datasheet : CLKSEL[2:0] should be set to 1~5 to achieve full gyroscope performance */
        ret = write_reg(ICM20649_PWR_MGMT_1_ADDR, &tx, 1U);
        SYSRET_CHECK(ret);

        /* enable sensors */
        tx = 0U;
        ret = write_reg(ICM20649_PWR_MGMT_2_ADDR, &tx, 1U);
        SYSRET_CHECK(ret);

        /* configure accelerometer */
        ret = config_accel(cfg);
        SYSRET_CHECK(ret);

        /* configure gyroscope */
        ret = config_gyro(cfg);
        SYSRET_CHECK(ret);

        /* configure timer to detect read timeout */
        ret = app_timer_create(
            &gp_timer,
            APP_TIMER_MODE_SINGLE_SHOT,
            timeout_handler);
        SYSRET_CHECK(ret);

        /* update driver */
        icm20649_handle.state = ICM20649_STATE_RUNNING;
        ret = RET_OK;
    }

    return ret;
}

/**
 * @brief Read raw gyroscope and accelerometer sensor data (straight from registers)
 * 
 * @param gyro  - Buffer to store raw gyroscope readings
 * @param accel - Buffer to store raw accelerometer readings
 * @return sysret_t 
 */
sysret_t icm20649_read_raw(int16_t gyro[ICM20649_GYRO_AXES], int16_t accel[ICM20649_ACCEL_AXES])
{
    ASSERT(gyro && accel);
    sysret_t ret = RET_ERR;

    if(icm20649_handle.state == ICM20649_STATE_RUNNING)
    {
        /* wait for data ready */
        ret = wait_data_rdy();
        SYSRET_CHECK(ret);

        /* set USR BANK to read from correct regs */
        set_usr_bank(ICM20649_USR_BANK_0);

        /* read from sensor output regs */
        ret = read_reg(ICM20649_GYRO_XOUT_H_ADDR, gyro, ICM20649_GYRO_AXES*2U);
        SYSRET_CHECK(ret);

        ret = read_reg(ICM20649_ACCEL_XOUT_H_ADDR, accel, ICM20649_ACCEL_AXES*2U);
        SYSRET_CHECK(ret);

        /* switch byte order */
        for(size_t i = 0U ; i < ICM20649_GYRO_AXES ; i++)
        {
            gyro[i] = ((gyro[i] & 0x00FFU) << 8U) | ((gyro[i] & 0xFF00U) >> 8U);
            accel[i] = ((accel[i] & 0x00FFU) << 8U) | ((accel[i] & 0xFF00U) >> 8U);
        }

        ret = RET_OK;
    }
    else
        ret = RET_DRV_UNINIT;

    return ret;
}

/**
 * @brief Test serial communication and initiate device self-test
 * 
 * @return sysret_t - Driver status
 * @retval RET_SERIAL_ERR - serial communication error
 * @retval RET_SELFTEST_ERR - device self-test failed
 */
sysret_t icm20649_test(void)
{
    sysret_t ret = RET_ERR;

    if(!check_who_am_i())
        ret = RET_SERIAL_ERR;
    else if(icm20649_handle.state == ICM20649_STATE_UNINIT)
        ret = RET_DRV_UNINIT;
    /* TODO: implement self-test procedure */
    else
        ret = RET_OK;

    return ret;
}