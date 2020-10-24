/**
 * @file icm20649.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Gyro+Accel Driver
 */

#ifndef ICM20649_H
#define ICM20649_H

#include "nrf.h"
#include "retcodes.h"

/**
 * @brief 
 */
typedef struct
{

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