/**
 * @file vcnl4040.h
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor driver
 */

#ifndef VCNL4040_H
#define VCNL4040_H

#include <stdint.h>
#include "retcodes.h"

/**
 * @brief VCNL4040 sensor data definition
 */
typedef uint16_t vcnl4040_data_t;

/**
 * @brief VCNL4040 driver configuration
 */
typedef struct
{
    uint32_t timeout; /*!< Serial read timeout */
} vcnl4040_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure and initialize VCNL4040 driver
 * 
 * @param cfg Driver configuration
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_init(vcnl4040_cfg_t* cfg);

/**
 * @brief Read data from VCNL4040 sensor
 * 
 * @param data - Buffer to store data
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_read(vcnl4040_data_t* data);

/**
 * @brief Get driver state and test serial communication
 * 
 * @return retcode_t Driver status
 */
retcode_t vcnl4040_test(void);

#ifdef __cplusplus
}
#endif

#endif /* VCNL4040_H */