/**
 * @file adxl372.h
 * @author UBC Capstone Team 2020/2021
 * @brief Driver for ADXL372 High-g IMU
 */

#ifndef ADXL372_H
#define ADXL372_H

#include "nrf.h"
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
typedef uint16_t adxl372_val_raw_t;

/**
 * @brief Type definition of an axis reading
 */
typedef float32_t adxl372_val_t;

/**
 * @brief Configurations for ADXL372 Driver
 */
typedef struct
{
    adxl372_odr_t odr;
} adxl372_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ADXL372 Driver
 * 
 * @param cfg - Driver configurations
 */
void adxl372_init(const adxl372_cfg_t* cfg);

/**
 * @brief Read raw linear acceleration data from sensor (values straight from registers)
 * 
 * @param readings - Buffer to store data
 */
void adxl372_read_raw(adxl372_val_raw_t readings[ADXL372_AXES]);

/**
 * @brief Get status of ADXL372 driver
 * 
 * @return adxl372_err_t - Driver error type, refer to @ref adxl372_err_t for details
 */
adxl372_err_t adxl372_status(void);

#ifdef __cplusplus
}
#endif

#endif /* ADXL372_H */