/**
 * @file datalog.h
 * @author UBC Capstone Team 2020/2021
 * @brief Datalog API, library related to
 *        logging sensor data to onboard flash
 */

#ifndef DATALOG_H
#define DATALOG_H

#include "retcodes.h"
#include "configs.h"
#include "datetime.h"

#define DATALOG_ROW_MAX_SIZE           30U   /*!< Max datalog row size in bytes */
#define DATALOG_DATETIME_AVAILABLE     0x08U /*!< Datalog row datetime presence mask */
#define DATALOG_GYRO_AVAILABLE         0x04U /*!< Datalog row gyroscope data presence mask */
#define DATALOG_LOW_G_ACCEL_AVAILABLE  0x02U /*!< Datalog row low-g accelerometer data presence mask */
#define DATALOG_HIGH_G_ACCEL_AVAILABLE 0x01U /*!< Datalog row high-g accelerometer data presence mask */

/**
 * @brief Datalogger state
 */
typedef enum
{
    DATALOG_STOPPED = 0, /* Datalogger idle */
    DATALOG_START        /* Datalogger ready to log data */
} datalog_state_t;

/**
 * @brief Start datalogging session.
 * 
 * @note This erases preexisting datalogs to make space
 *       for new ones
 * @param dev_metadata Update device metadata to reflect erasing preexisting datalog
 * @return sysret_t
 */
sysret_t datalog_start(metadata_t* dev_metadata);

/**
 * @brief Log data to flash
 * 
 * If any of the inputs are null, then that information
 * is not included in the datalog row and their existence
 * is logged in the 1B row header.
 * 
 * @param dt Datetime
 * @param gyro Gyroscope raw data
 * @param low_g_accel Low-G Accelerometer raw data
 * @param high_g_accel High-G Accelerometer raw data
 * @return sysret_t 
 */
sysret_t datalog_log(
    datetime_t* dt,
    int16_t gyro[3U],
    int16_t low_g_accel[3U],
    int16_t high_g_accel[3U]);

/**
 * @brief Stop datalogging, save datalog information to flash
 * 
 * @param dev_metadata Update device metadata before saving to flash
 * @return sysret_t 
 */
sysret_t datalog_stop(metadata_t* dev_metadata);

/**
 * @brief Erase current datalog information from flash
 * 
 * @param dev_metadata Update device metadata before saving to flash
 * @return sysret_t 
 */
sysret_t datalog_erase(metadata_t* dev_metadata);

#endif /* DATALOG_H */