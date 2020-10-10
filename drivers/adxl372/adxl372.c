/**
 * @file adxl372.c
 * @author UBC Capstone Team 2020/2021
 * @brief Driver for ADXL372 High-g IMU
 */

#include <string.h>
#include "adxl372.h"
#include "adxl372_regs.h"
#include "spi.h"

/**
 * @brief Define register address type
 */
typedef uint8_t reg_addr_t;

/**
 * @brief ADXL372 driver states
 */
typedef enum
{
    ADXL372_STATE_INACTIVE = 0,
    ADXL372_STATE_ACTIVE
} adxl372_state_t;

/**
 * @brief Definition for ADXL372 Driver Instance
 */
typedef struct
{
    adxl372_state_t state;
    const adxl372_cfg_t* cfg;
} adxl_372_t;

/**
 * @brief Driver instance
 */
static adxl_372_t adxl372 = {ADXL372_STATE_INACTIVE, NULL};

/******************************
 * Helper functions
 ******************************/

static void write_reg(reg_addr_t reg_addr, void* tx, size_t txn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = (reg_addr << 1U);

    /* format buffer to send */
    buf[0] = addr;
    memcpy(buf+1, tx, txn);

    spi_transfer(SPI_INSTANCE_1, SPI_DEV_ADXL372, buf, txn+1, NULL, 0);
}

static void read_reg(reg_addr_t reg_addr, void* rx, size_t rxn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = (reg_addr << 1U) | 1U;
    spi_transfer(SPI_INSTANCE_1, SPI_DEV_ADXL372, &addr, 1U, buf, rxn + 1U);

    memcpy(rx, buf+1U, rxn);
}

/**
 * @brief Configure driver to run at ODR specified
 * 
 * @param odr
 */
static void configure_odr(adxl372_odr_t odr)
{
    uint8_t tx = (odr << 5U) & ADXL372_TIMING_ODR_MASK;

    write_reg(ADXL372_TIMING_ADDR, &tx, 1U);
}

/**
 * @brief Configure device mode of operation
 * 
 * @param mode
 */
static void configure_mode(adxl372_mode_t mode)
{
    uint8_t tx = mode & ADXL372_POWER_CTL_MODE_MASK;

    write_reg(ADXL372_POWER_CTL_ADDR, &tx, 1U);
}

/******************************
 * API
 ******************************/

/**
 * @brief Initialize ADXL372 Driver
 * 
 * @param cfg - Driver configurations
 */
void adxl372_init(const adxl372_cfg_t* cfg)
{
    /* Test SPI communication */
    uint8_t devid_ad = 0U;
    read_reg(ADXL372_DEVID_AD_ADDR, &devid_ad, 1U);

    if(devid_ad == ADXL372_DEVID_AD_EXPECTED_VAL)
    {
        /* SPI communication successful, let's configure the IMU */
        adxl372.cfg = cfg;
        configure_odr(cfg->odr);
        configure_mode(cfg->mode);

        adxl372.state = ADXL372_STATE_ACTIVE;
    }
}

/**
 * @brief Read raw linear acceleration data from sensor (values straight from registers)
 * 
 * @param readings - Buffer to store data
 */
void adxl372_read_raw(adxl372_val_raw_t readings[ADXL372_AXES])
{
    uint8_t buf[ADXL372_AXES*2U] = {0U};
    uint8_t status = 0U;

    /* wait for data ready */
    do
    {
        read_reg(ADXL372_STATUS_ADDR, &status, 1U);
    } while(!(status & ADXL372_STATUS_DATA_RDY_MASK));

    read_reg(ADXL372_XDATA_H_ADDR, buf, ADXL372_AXES*2);

    /* format data, since it was received in big-endian format (ARM is little endian) */
    for(size_t i = 0U ; i < ADXL372_AXES*2 ; i+=2U)
    {
        readings[i/2U] = (((uint16_t)buf[i+1] << 8U) | (uint16_t)buf[i]);
    }
}

/**
 * @brief Get status of ADXL372 driver
 * 
 * @return adxl372_err_t - Driver error type, refer to @ref adxl372_err_t for details
 */
adxl372_err_t adxl372_status(void)
{
    adxl372_err_t err = ADXL372_ERR_OK;

    if(adxl372.state == ADXL372_STATE_INACTIVE)
        err = ADXL372_ERR_UNINIT;
    else
    {
        /* Test SPI communication */
        uint8_t devid_ad = 0U;
        read_reg(ADXL372_DEVID_AD_ADDR, &devid_ad, 1U);

        if(devid_ad != ADXL372_DEVID_AD_EXPECTED_VAL)
            err = ADXL372_ERR_COMM;
    }

    return err;
}