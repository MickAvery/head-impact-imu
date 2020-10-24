/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

#ifndef ICM20649_H
#define ICM20649_H

#include <stdbool.h>
#include "nrf.h"
#include "retcodes.h"

/**
 * @brief Configurable accelerometer full scale settings
 */
typedef enum
{
    ICM20649_ACCEL_FS_4g,  /*!< accelerometer full scale = +/- 4g */
    ICM20649_ACCEL_FS_8g,  /*!< accelerometer full scale = +/- 8g */
    ICM20649_ACCEL_FS_16g, /*!< accelerometer full scale = +/- 16g */
    ICM20649_ACCEL_FS_30g, /*!< accelerometer full scale = +/- 30g */
    ICM20649_ACCEL_FS_MAX
} icm20649_accel_fs_t;

/**
 * @brief Configurable gyroscope full scale settings
 */
typedef enum
{
    ICM20649_GYRO_FS_500dps,  /*!< gyroscope full scale = +/- 500 dps */
    ICM20649_GYRO_FS_1000dps, /*!< gyroscope full scale = +/- 1000 dps */
    ICM20649_GYRO_FS_2000dps, /*!< gyroscope full scale = +/- 2000 dps */
    ICM20649_GYRO_FS_4000dps, /*!< gyroscope full scale = +/- 4000 dps */
    ICM20649_GYRO_FS_MAX
} icm20649_gyro_fs_t;

/**
 * @brief ICM20649 driver configurations
 */
typedef struct
{
    /* gyroscope configs */
    icm20649_gyro_fs_t gyro_fs;
    bool gyro_dlpf_en;

    /* accelerometer configs */
    icm20649_accel_fs_t accel_fs;
    bool accel_dlpf_en;
} icm20649_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ICM20649 driver
 * 
 * @param cfg - Driver configurations
 * @return retcode_t - Driver status
 */
retcode_t icm20649_init(icm20649_cfg_t* cfg);

/**
 * @brief Test serial communication and initiate device self-test
 * 
 * @return retcode_t - Driver status
 * @retval RET_SERIAL_ERR - serial communication error
 * @retval RET_SELFTEST_ERR - device self-test failed
 */
retcode_t icm20649_test(void);

#ifdef __cplusplus
}
#endif

#endif /* ICM20649_H */