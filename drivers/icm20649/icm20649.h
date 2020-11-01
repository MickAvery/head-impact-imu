/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

#ifndef ICM20649_H
#define ICM20649_H

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "retcodes.h"

/**
 * @brief ICM20649 accelerometer axis indexes
 */
typedef enum
{
    ICM20649_ACCEL_X = 0, /*!< X axis */
    ICM20649_ACCEL_Y,     /*!< Y axis */
    ICM20649_ACCEL_Z,     /*!< Z axis */
    ICM20649_ACCEL_AXES   /*!< Max number of accelerometer axes */
} icm20649_accel_axes_t;

/**
 * @brief ICM20649 gyroscope axis indexes
 */
typedef enum
{
    ICM20649_GYRO_X = 0, /*!< X axis */
    ICM20649_GYRO_Y,     /*!< Y axis */
    ICM20649_GYRO_Z,     /*!< Z axis */
    ICM20649_GYRO_AXES   /*!< Max number of gyroscope axes */
} icm20649_gyro_axes_t;

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
    icm20649_gyro_fs_t gyro_fs;   /*!< set gyroscope fullscale */
    bool gyro_dlpf_en;            /*!< if true, gyroscope DLPF enabled */

    /* accelerometer configs */
    icm20649_accel_fs_t accel_fs; /*!< set accelerometer fullscale */
    bool accel_dlpf_en;           /*!< if true, accelerometer DLPF enabled */

    uint32_t timeout;             /*!< sensor read timeout in ms */
} icm20649_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize ICM20649 driver
 * 
 * @param cfg - Driver configurations
 * @return sysret_t - Driver status
 */
sysret_t icm20649_init(icm20649_cfg_t* cfg);

/**
 * @brief Read raw gyroscope and accelerometer sensor data (straight from registers)
 * 
 * @param gyro  - Buffer to store raw gyroscope readings
 * @param accel - Buffer to store raw accelerometer readings
 * @return sysret_t 
 */
sysret_t icm20649_read_raw(int16_t gyro[ICM20649_GYRO_AXES], int16_t accel[ICM20649_ACCEL_AXES]);

/**
 * @brief Test serial communication and initiate device self-test
 * 
 * @return sysret_t - Driver status
 * @retval RET_SERIAL_ERR - serial communication error
 * @retval RET_SELFTEST_ERR - device self-test failed
 */
sysret_t icm20649_test(void);

#ifdef __cplusplus
}
#endif

#endif /* ICM20649_H */