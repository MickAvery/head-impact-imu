/**
 * @file vcnl4040_regs.h
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor registers
 */

#ifndef VCNL4040_REGS_H
#define VCNL4040_REGS_H

#define VCNL4040_SLAVE_ADDR           0x60U /*!< VCNL4040 I2C slave address */

/**
 * CONF1 and CONF2 register address and bit definitions
 * 
 * Write to this address expects 2 bytes, the first to CONF1 and the next to CONF2
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
 * Write to this address expects 2 bytes, the first to CONF3 and the next to MS
 */
#define VCNL4040_PS_CONF3_MS_ADDR     0x04U /*!< VCNL4040 CONF3 and MS register address */
#define VCNL4040_SMART_PERS_FLAG      0x10U /*!< PS smart persistence mask */
#define VCNL4040_SMART_PERS_SET(en)   ((en << 4) & VCNL4040_SMART_PERS_FLAG) /*!< enable/disable smart persistence */
#define VCNL4040_LED_I_MASK           0x07U /*!< PS LED_I bits mask */
#define VCNL4040_LED_I_SET(curr)      (curr & VCNL4040_LED_I_MASK) /*!< set LED current setting */

#endif /* VCNL4040_REGS_H */