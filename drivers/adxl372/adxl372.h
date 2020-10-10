/**
 * @file adxl372.h
 * @author UBC Capstone Team 2020/2021
 * @brief Driver for ADXL372 High-g IMU
 */

#ifndef ADXL372_H
#define ADXL372_H

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
 * @brief Get status of ADXL372 driver
 * 
 * @return adxl372_err_t - Driver error type, refer to @ref adxl372_err_t for details
 */
adxl372_err_t adxl372_status(void);

#ifdef __cplusplus
}
#endif

#endif /* ADXL372_H */