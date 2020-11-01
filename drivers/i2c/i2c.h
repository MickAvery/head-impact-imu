/**
 * @file i2c.h
 * @author UBC Capstone Team 2020/2021
 * @brief I2C driver
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stddef.h>
#include "retcodes.h"

/**
 * @brief Slave address definition
 */
typedef uint8_t i2c_addr_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize I2C driver
 * 
 * @return sysret_t Driver status, RET_OK if all's well
 */
sysret_t i2c_init(void);

/**
 * @brief Perform an I2C transfer, transmit followed by a receive
 * 
 * If txn is > 0, a transmit occurs. If rxn is > 0, a receive occurs.
 * 
 * @param slave_addr - slave device address
 * @param txbuf      - transmit buffer
 * @param txn        - number of bytes to transmit
 * @param rxbuf      - receive buffer
 * @param rxn        - number of bytes to receive
 * @return sysret_t Driver status
 */
sysret_t i2c_transceive(i2c_addr_t slave_addr, uint8_t* txbuf, size_t txn, uint8_t* rxbuf, size_t rxn);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */