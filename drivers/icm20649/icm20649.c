/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

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
 * @brief Write txn bytes to reg_addr
 * 
 * @param reg_addr - Register to write to
 * @param tx       - Bytes to write
 * @param txn      - Number of bytes to write
 */
static void write_reg(reg_addr_t reg_addr, void* tx, size_t txn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */

    /* format buffer to send */
    buf[0] = reg_addr;
    memcpy(buf+1, tx, txn);

    spi_transfer(SPI_INSTANCE_0, SPI_DEV_ICM20649, buf, txn+1, NULL, 0);
}

/**
 * @brief Select a USER BANK
 * 
 * @param usr_bank - USER BANK to select
 */
static void set_usr_bank(icm20649_usr_bank_t usr_bank)
{
    static icm20649_usr_bank_t current_usr_bank = ICM20649_USR_BANK_0;

    if(usr_bank != current_usr_bank)
    {
        uint8_t tx = (uint8_t)usr_bank;
        write_reg(ICM20649_REG_BANK_SEL_ADDR, &tx, 1U);
        current_usr_bank = usr_bank;
    }
}

/**
 * @notapi
 * @brief Set accelerometer configurations
 * 
 * @param cfg - Config settings
 */
static void config_accel(icm20649_cfg_t* cfg)
{
    uint8_t tx = 0U;

    set_usr_bank(ICM20649_USR_BANK_2);

    /* TODO: handle DLPF configs */

    /* set fullscale */
    tx = ICM20649_ACCEL_FS_SEL_SET(cfg->accel_fs);
    write_reg(ICM20649_ACCEL_CONFIG_ADDR, &tx, 1U);
}

/**
 * @notapi
 * @brief Set gyroscope configurations
 * 
 * @param cfg - Config settings
 */
static void config_gyro(icm20649_cfg_t* cfg)
{
    uint8_t tx = 0U;

    set_usr_bank(ICM20649_USR_BANK_2);

    /* TODO: handle DLPF configs */

    /* set fullscale */
    tx = ICM20649_GYRO_FS_SEL_SET(cfg->gyro_fs);
    write_reg(ICM20649_GYRO_CONFIG_1_ADDR, &tx, 1U);
}

/**
 * @notapi
 * @brief Reset device and restore default settings
 */
static void reset_dev(void)
{
    uint8_t tx = ICM20649_DEVICE_RESET_MASK;
    set_usr_bank(ICM20649_USR_BANK_0);
    write_reg(ICM20649_PWR_MGMT_1_ADDR, &tx, 1U);
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

    if(!check_who_am_i())
        ret = RET_SERIAL_ERR;
    else
    {
        uint8_t tx;

        icm20649_handle.cfg = cfg;

        /* reset device */
        reset_dev();

        /* clock select */
        set_usr_bank(ICM20649_USR_BANK_0);
        tx = ICM20649_CLKSEL_SET(0x01U); /* From datasheet : CLKSEL[2:0] should be set to 1~5 to achieve full gyroscope performance */
        write_reg(ICM20649_PWR_MGMT_1_ADDR, &tx, 1U);

        /* configure accelerometer */
        config_accel(cfg);

        /* configure gyroscope */
        config_gyro(cfg);

        /* update driver */
        icm20649_handle.state = ICM20649_STATE_RUNNING;
    }

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
    /* TODO: implement self-test procedure */
    else
        ret = RET_OK;

    return ret;
}