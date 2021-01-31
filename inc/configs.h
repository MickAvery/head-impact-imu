/**
 * @file configs.h
 * @author UBC Capstone Team 2020/2021
 * @brief API for getting and setting device configurations
 */

#ifndef CONFIGS_H
#define CONFIGS_H

#include <stdint.h>
#include "mt25q.h"

/**
 * @brief Expected header of the configurations frame.
 *
 * After attempting to read configurations from flash memory,
 * check existence of header to see if previous configurations
 * were saved.
 */
#define CONFIGS_FRAME_HEADER 0xDEADBEEF

/**
 * @brief Size in memory for a configurations frame
 */
#define CONFIGS_FRAME_SIZE (FLASH_PAGE_SIZE)

/**
 * @brief Definition of a configurations frame
 */
typedef union
{
    struct __attribute__((__packed__)) device_configs
    {
        uint32_t header;
        uint8_t  datalog_mode;
        uint8_t  trigger_on;
        uint8_t  trigger_axis;
        int16_t  threshold_resultant;
        int16_t  threshold_x;
        int16_t  threshold_y;
        int16_t  threshold_z;
        uint8_t  gyro_sampling_rate;
        uint8_t  low_g_sampling_rate;
        uint8_t  high_g_sampling_rate;
    } device_configs;
    uint8_t  configs_bytes[CONFIGS_FRAME_SIZE];
} configs_t;

/**
 * @brief Datalog mode options
 */
typedef enum
{
    CONFIGS_DATALOG_MODE_CONTINUOUS = 0,
    CONFIGS_DATALOG_MODE_TRIGGER,
    CONFIGS_DATALOG_MODE_MAX /*!< not an option */
} configs_datalog_mode_t;

/**
 * @brief Trigger on... options
 */
typedef enum
{
    CONFIGS_TRIGGER_ON_LINEAR_ACC = 0,
    CONFIGS_TRIGGER_ON_ANG_VELOC,
    CONFIGS_TRIGGER_ON_MAX /*!< not an option */
} configs_trigger_on_t;

/**
 * @brief Trigger axis options
 */
typedef enum
{
    CONFIGS_TRIGGER_AXIS_RESULTANT = 0,
    CONFIGS_TRIGGER_AXIS_PER_AXIS,
    CONFIGS_TRIGGER_AXIS_MAX /*!< not an option */
} configs_trigger_axis_t;

/**
 * @brief Gyroscope sampling rate options
 */
typedef enum
{
    CONFIGS_GYRO_SAMPLE_RATE_4500HZ = 0,
    CONFIGS_GYRO_SAMPLE_RATE_2000HZ,
    CONFIGS_GYRO_SAMPLE_RATE_1000HZ,
    CONFIGS_GYRO_SAMPLE_RATE_500HZ,
    CONFIGS_GYRO_SAMPLE_RATE_250HZ,
    CONFIGS_GYRO_SAMPLE_RATE_125HZ,
    CONFIGS_GYRO_SAMPLE_RATE_MAX /*!< not an option */
} configs_gyro_sample_rate_t;

/**
 * @brief Low G Accelerometer sampling rate options
 */
typedef enum
{
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_4500HZ = 0,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_2000HZ,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_1000HZ,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_500HZ,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_250HZ,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_125HZ,
    CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_MAX /*!< not an option */
} configs_low_g_accel_sample_rate_t;

/**
 * @brief High G Accelerometer sampling rate options
 */
typedef enum
{
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_4500HZ = 0,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_2000HZ,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_1000HZ,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_500HZ,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_250HZ,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_125HZ,
    CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_MAX /*!< not an option */
} configs_high_g_accel_sample_rate_t;

/**
 * @brief Global configurations singleton, can be referenced anywhere
 */
extern configs_t GLOBAL_CONFIGS;

/**
 * @brief Configuration option strings for logging
 */
extern char* configs_datalog_mode_strings[CONFIGS_DATALOG_MODE_MAX];
extern char* configs_trigger_on_strings[CONFIGS_TRIGGER_ON_MAX];
extern char* configs_trigger_axis_strings[CONFIGS_TRIGGER_AXIS_MAX];
extern char* configs_gyro_sample_rate_strings[CONFIGS_GYRO_SAMPLE_RATE_MAX];
extern char* configs_low_g_accel_sample_rate_strings[CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_MAX];
extern char* configs_high_g_accel_sample_rate_strings[CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_MAX];

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Attempt to get configurations from persistent memory
 * 
 * @param configs Configurations will be saved here
 * @return sysret_t Driver status
 * @retval RET_ERR if persistent memory doesn't contain configurations
 * @retval RET_OK if configurations are present in persistent memory
 */
sysret_t configs_get(configs_t* configs);

/**
 * @brief Save configurations to persistent memory
 * 
 * @param configs Configurations to save
 * @return sysret_t Driver status
 */
sysret_t configs_save(configs_t* configs);

#ifdef __cplusplus
}
#endif

#endif /* CONFIGS_H */