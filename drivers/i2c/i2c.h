/**
 * @file i2c.h
 * @author UBC Capstone Team 2020/2021
 * @brief I2C driver
 */

#ifndef I2C_H
#define I2C_H

#include "retcodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize I2C driver
 * 
 * @return retcode_t Driver status, RET_OK if all's well
 */
retcode_t i2c_init(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */