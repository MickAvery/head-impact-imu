/**
 * @file adxl372.c
 * @author UBC Capstone Team 2020/2021
 * @brief Driver for ADXL372 High-g IMU
 */

#include <stdbool.h>
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
    adxl372_val_raw_t offsets[ADXL372_AXES];
} adxl_372_t;

/**
 * @brief Driver instance
 */
static adxl_372_t adxl372 = {ADXL372_STATE_INACTIVE, NULL};

/******************************
 * Averaging configs for calibration.
 * TODO: Maybe put these in a header file?
 ******************************/

#define NUM_CAL_AVG_N 128U /*!< When calibrating, this is the number of data points to read and average */
static adxl372_val_raw_t default_setpoint[ADXL372_AXES] = {0, 0, 10}; /*!< Default axis setpoints, units in LSB */

/******************************
 * Helper functions
 ******************************/

/**
 * @notapi
 * @brief Write to sensor register
 * 
 * @param reg_addr 
 * @param tx 
 * @param txn 
 * @return retcode_t - Driver status
 */
static retcode_t write_reg(reg_addr_t reg_addr, void* tx, size_t txn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = (reg_addr << 1U);

    /* format buffer to send */
    buf[0] = addr;
    memcpy(buf+1, tx, txn);

    return spi_transfer(SPI_INSTANCE_2, SPI_DEV_ADXL372, buf, txn+1, NULL, 0);
}

/**
 * @notapi
 * @brief Read from sensor register
 * 
 * @param reg_addr 
 * @param rx 
 * @param rxn 
 * @return retcode_t - Driver status
 */
static retcode_t read_reg(reg_addr_t reg_addr, void* rx, size_t rxn)
{
    retcode_t ret;
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    uint8_t addr = (reg_addr << 1U) | 1U;
    ret = spi_transfer(SPI_INSTANCE_2, SPI_DEV_ADXL372, &addr, 1U, buf, rxn + 1U);

    memcpy(rx, buf+1U, rxn);

    return ret;
}

/**
 * @notapi
 * @brief Configure LPF bandwidth
 * 
 * @param bandwidth 
 * @return retcode_t - Driver status
 */
static retcode_t configure_bandwidth(adxl372_bandwidth_t bandwidth)
{
    if(bandwidth < ADXL372_BW_DISABLE)
    {
        uint8_t tx = bandwidth & ADXL372_BANDWIDTH_MASK;

        return write_reg(ADXL372_MEASURE_ADDR, &tx, 1U);
    }

    return RET_OK;
}

/**
 * @brief Configure driver to run at ODR specified
 * 
 * @param odr
 * @return retcode_t - Driver status
 */
static retcode_t configure_odr(adxl372_odr_t odr)
{
    uint8_t tx = (odr << 5U) & ADXL372_TIMING_ODR_MASK;

    return write_reg(ADXL372_TIMING_ADDR, &tx, 1U);
}

/**
 * @brief Configure device mode of operation
 * 
 * @param mode
 * @param lpf_disable - if true, LPF is disabled
 * @return retcode_t - Driver status
 */
static retcode_t configure_mode(adxl372_mode_t mode, bool lpf_disable)
{
    uint8_t tx = mode & ADXL372_POWER_CTL_MODE_MASK;

    if(lpf_disable)
        tx |= ADXL372_POWER_CTL_LPF_MASK;

    /* disable HPF */
    tx |= ADXL372_POWER_CTL_HPF_MASK;

    return write_reg(ADXL372_POWER_CTL_ADDR, &tx, 1U);
}

/**
 * @brief Reset the device, place in standby mode
 * @return retcode_t - Driver status
 */
static retcode_t reset_dev(void)
{
    uint8_t tx = ADXL372_RESET_VAL;
    return write_reg(ADXL372_POWER_CTL_ADDR, &tx, 1U);
}

/******************************
 * API
 ******************************/

/**
 * @brief Initialize ADXL372 Driver
 * 
 * @param cfg - Driver configurations
 * @return retcode_t - Driver status
 */
retcode_t adxl372_init(const adxl372_cfg_t* cfg)
{
    /* Test SPI communication */
    uint8_t devid_ad = 0U;
    retcode_t ret = read_reg(ADXL372_DEVID_AD_ADDR, &devid_ad, 1U);

    if(devid_ad != ADXL372_DEVID_AD_EXPECTED_VAL)
        ret = RET_SERIAL_ERR;
    else if(ret == RET_OK)
    {
        /* SPI communication successful, let's configure the IMU */

        /**
         * reset sensor registers
         */
        if((ret = reset_dev()) != RET_OK)
            return ret;

        adxl372.cfg = cfg;
        memset(adxl372.offsets, 0, sizeof(adxl372_val_raw_t)*ADXL372_AXES);

        /**
         * configure registers
         */
        if((ret = configure_bandwidth(cfg->bandwidth)) != RET_OK)
            return ret;

        if((ret = configure_odr(cfg->odr)) != RET_OK)
            return ret;

        ret = configure_mode(cfg->mode, cfg->bandwidth == ADXL372_BW_DISABLE);
        if(ret != RET_OK)
            return ret;

        /**
         * update driver state
         */
        adxl372.state = ADXL372_STATE_ACTIVE;
        ret = RET_OK;
    }

    return ret;
}

/**
 * @brief Read raw linear acceleration data from sensor (values straight from registers)
 * 
 * @param readings - Buffer to store data
 * @return retcode_t - Error status if something goes wrong 
 */
retcode_t adxl372_read_raw(adxl372_val_raw_t readings[ADXL372_AXES])
{
    retcode_t ret = RET_DRV_UNINIT;

    if(adxl372.state == ADXL372_STATE_ACTIVE)
    {
        uint8_t buf[ADXL372_AXES*2U] = {0U};
        uint8_t status = 0U;

        /* wait for data ready */
        do
        {
            if((ret = read_reg(ADXL372_STATUS_ADDR, &status, 1U)) != RET_OK)
                return ret;
        } while(!(status & ADXL372_STATUS_DATA_RDY_MASK));

        if((ret = read_reg(ADXL372_XDATA_H_ADDR, buf, ADXL372_AXES*2)) != RET_OK)
            return ret;

        for(size_t i = 0U ; i < ADXL372_AXES*2 ; i+=2U)
        {
            /* format data, since it was received in big-endian format (ARM is little endian) */
            readings[i/2U] = (buf[i] << 8U) | (buf[i+1] & 0xF0U);
            /* convert from 12-bit to 16-bit integer */
            readings[i/2U] /= 16;
            /* trim offset */
            readings[i/2U] -= adxl372.offsets[i/2U];
        }

        ret = RET_OK;
    }

    return ret;
}

/**
 * @brief Get status of ADXL372 driver
 * 
 * @return retcode_t - Driver error code, refer to @ref retcode_desc_t
 */
retcode_t adxl372_test(void)
{
    retcode_t ret = RET_ERR;

    /* Test SPI communication */
    uint8_t devid_ad = 0U;
    ret = read_reg(ADXL372_DEVID_AD_ADDR, &devid_ad, 1U);

    if(ret != RET_OK)
        return ret;
    else if(devid_ad != ADXL372_DEVID_AD_EXPECTED_VAL)
        return RET_SERIAL_ERR;
    else if(adxl372.state == ADXL372_STATE_INACTIVE)
        return RET_DRV_UNINIT;
    else
        ret = RET_OK;

    return ret;
}

/**
 * @brief Calibrate sensor to correct offset
 * 
 * @param setpoint - Expected values at every axis
 *                   If NULL then default setpoint is used where device is assumed
 *                   to be at rest with the Z-axis completely perpendicular to the
 *                   X-Y plane [0, 0, 1g]
 * @return retcode_t - Driver status
 */
retcode_t adxl372_calibrate(adxl372_val_raw_t setpoint[ADXL372_AXES])
{
    retcode_t ret = RET_ERR;

    /* if NULL, set setpoint to default */
    setpoint  = (setpoint == NULL) ? default_setpoint : setpoint;

    if(adxl372.state == ADXL372_STATE_ACTIVE)
    {
        adxl372_val_raw_t axes[ADXL372_AXES] = {0};

        /* get datapoints */
        for(size_t i = 0 ; i < NUM_CAL_AVG_N ; i++)
        {
            adxl372_val_raw_t vals[ADXL372_AXES];
            ret = adxl372_read_raw(vals);

            if(ret != RET_OK)
                return ret;

            axes[ADXL372_X] += vals[ADXL372_X];
            axes[ADXL372_Y] += vals[ADXL372_Y];
            axes[ADXL372_Z] += vals[ADXL372_Z];
        }

        /* get average, write offset to register */
        for(size_t i = 0 ; i < ADXL372_AXES ; i++)
        {
            axes[i] /= NUM_CAL_AVG_N;

            adxl372.offsets[i] = axes[i] - setpoint[i];
        }

        ret = RET_OK;
    }

    return ret;
}