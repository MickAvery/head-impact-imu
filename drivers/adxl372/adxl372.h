/**
 * @file adxl372.h
 * @author UBC Capstone Team 2020/2021
 * @brief Driver for ADXL372 High-g IMU
 */

#ifndef ADXL372_H
#define ADXL372_H

#include "nrf.h"
#include "retcodes.h"
#include "arm_math.h"

/**
 * @brief Configurable operational data rates of ADXL372
 */
typedef enum
{
    ADXL372_ODR_400HZ = 0, /*!< ODR = 400 Hz */
    ADXL372_ODR_800HZ,     /*!< ODR = 800 Hz */
    ADXL372_ODR_1600HZ,    /*!< ODR = 1600 Hz */
    ADXL372_ODR_3200HZ,    /*!< ODR = 3200 Hz */
    ADXL372_ODR_6400HZ     /*!< ODR = 6400 Hz */
} adxl372_odr_t;

/**
 * @brief Configurable bandwidth for LPF
 */
typedef enum
{
    ADXL372_BW_200HZ = 0, /*!< 200 Hz Bandwidth */
    ADXL372_BW_400HZ,     /*!< 400 Hz Bandwidth */
    ADXL372_BW_800HZ,     /*!< 800 Hz Bandwidth */
    ADXL372_BW_1600HZ,    /*!< 1600 Hz Bandwidth */
    ADXL372_BW_3200HZ,    /*!< 3200 Hz Bandwidth */
    ADXL372_BW_DISABLE,   /*!< Disable LPF */
    ADXL372_BW_MAX = ADXL372_BW_DISABLE
} adxl372_bandwidth_t;

/**
 * @brief Configurable driver modes
 */
typedef enum
{
    ADXL372_MODE_STANDBY = 0, /*!< Standby mode */
    ADXL372_MODE_WAKEUP,      /*!< Wakeup mode */
    ADXL372_MODE_INSTANT_ON,  /*!< Instant on mode */
    ADXL372_MODE_FULLBAND     /*!< Full bandwidth measurement mode */
} adxl372_mode_t;

/**
 * @brief ADXL372 Driver error definitions
 */
typedef enum
{
    ADXL372_ERR_OK = 0, /*!< Driver active */
    ADXL372_ERR_UNINIT, /*!< Driver uninitialized */
    ADXL372_ERR_COMM    /*!< Serial communication error */
} adxl372_err_t;

/**
 * @brief Axis indeces/offsets when reading sensor values
 */
typedef enum
{
    ADXL372_X = 0, /*!< X axis */
    ADXL372_Y,     /*!< Y axis */
    ADXL372_Z,     /*!< Z axis */
    ADXL372_AXES,  /*!< Number if axes */
} adxl372_axis_t;

/**
 * @brief Type definition of a raw axis reading
 */
typedef int16_t adxl372_val_raw_t;

/**
 * @brief Configurations for ADXL372 Driver
 */
typedef struct
{
    adxl372_odr_t odr;
    adxl372_mode_t mode;
    adxl372_bandwidth_t bandwidth;
} adxl372_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ADXL372 Driver
 * 
 * @param cfg - Driver configurations
 * @return retcode_t - Driver status
 */
retcode_t adxl372_init(const adxl372_cfg_t* cfg);

/**
 * @brief Read raw linear acceleration data from sensor (values straight from registers)
 * 
 * @param readings - Buffer to store data
 * @return adxl372_err_t - Error status if something goes wrong 
 */
adxl372_err_t adxl372_read_raw(adxl372_val_raw_t readings[ADXL372_AXES]);

/**
 * @brief Get status of ADXL372 driver
 * 
 * @return adxl372_err_t - Driver error type, refer to @ref adxl372_err_t for details
 */
adxl372_err_t adxl372_status(void);

/**
 * @brief Calibrate sensor to correct offset
 * 
 * @param setpoint - Expected values at every axis
 *                   If NULL then default setpoint is used where device is assumed
 *                   to be at rest with the Z-axis completely perpendicular to the
 *                   X-Y plane [0, 0, 1g]
 * @return adxl372_err_t - Error status if something goes wrong
 */
adxl372_err_t adxl372_calibrate(adxl372_val_raw_t setpoint[ADXL372_AXES]);

#ifdef __cplusplus
}
#endif

#endif /* ADXL372_H */