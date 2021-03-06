/**
 * @file vcnl4040_regs.h
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor registers
 */

#ifndef VCNL4040_REGS_H
#define VCNL4040_REGS_H

#define VCNL4040_SLAVE_ADDR           0x60U /*!< VCNL4040 I2C slave address */

/**
 * Use these macros when creating tx buffers
 */
#define VCNL4040_TX_CMD               0U /*!< Command index */
#define VCNL4040_TX_LSB               1U /*!< LSB tx index */
#define VCNL4040_TX_MSB               2U /*!< MSB tx index */
#define VCNL4040_TX_NUMBYTES          3U /*!< Max number of tx bytes */

/**
 * Use these macros when indexing from rx buffer
 */
#define VCNL4040_RX_LSB               0U /*!< LSB rx index */
#define VCNL4040_RX_MSB               1U /*!< MSB rx idnex */

/**
 * CONF1 and CONF2 register address and bit definitions
 * 
 * Writes to this address expects 2 bytes, the first to CONF1 and the next to CONF2
 */
#define VCNL4040_PS_CONF1_CONF2_ADDR  0x03U /*!< VCNL4040 CONF1 and CONF2 register address */
#define VCNL4040_DUTY_MASK            0xC0U /*!< PS IRED on / off duty ratio setting mask */
#define VCNL4040_DUTY_SET(duty)       ((duty << 6) & VCNL4040_DUTY_MASK) /*!< set IRED on / off duty ratio setting */
#define VCNL4040_IT_MASK              0x0EU /*!< PS integration time mask */
#define VCNL4040_IT_SET(it)           ((it << 1) & VCNL4040_IT_MASK) /*!< set PS integration time */
#define VCNL4040_OUT_BITS_MASK        0x08U /*!< PS output bits mask */
#define VCNL4040_OUT_BITS_SET(bits)   ((bits << 3) & VCNL4040_OUT_BITS_MASK) /*!< set PS output bits */

/**
 * CONF3 and MS register address and bit definitions
 * 
 * Writes to this address expects 2 bytes, the first to CONF3 and the next to MS
 */
#define VCNL4040_PS_CONF3_MS_ADDR     0x04U /*!< VCNL4040 CONF3 and MS register address */
#define VCNL4040_SMART_PERS_FLAG      0x10U /*!< PS smart persistence mask */
#define VCNL4040_SMART_PERS_SET(en)   ((en << 4) & VCNL4040_SMART_PERS_FLAG) /*!< enable/disable smart persistence */
#define VCNL4040_LED_I_MASK           0x07U /*!< PS LED_I bits mask */
#define VCNL4040_LED_I_SET(curr)      (curr & VCNL4040_LED_I_MASK) /*!< set LED current setting */

/**
 * PS data output register address
 */
#define VCNL4040_PS_DATA_ADDR         0x08U /*!< VCNL4040 PS Data output register address */

/**
 * Device ID register address
 */
#define VCNL4040_ID_ADDR              0x0CU /*!< VCNL4040 ID register address */
#define VCNL4040_ID_LSB               0x86U /*!< expected ID LSB */
#define VCNL4040_ID_MSB               0x01U /*!< expected ID MSB */

#endif /* VCNL4040_REGS_H */