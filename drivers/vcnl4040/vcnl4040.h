/**
 * @file vcnl4040.h
 * @author UBC Capstone Team 2020/2021
 * @brief VCNL4040 proximity sensor driver
 */

#ifndef VCNL4040_H
#define VCNL4040_H

#include <stdint.h>
#include <stdbool.h>
#include "retcodes.h"

/**
 * @brief VCNL4040 sensor data definition
 */
typedef uint16_t vcnl4040_data_t;

/**
 * @brief PS IRED on / off duty ratio setting
 */
typedef enum
{
    VCNL4040_PS_DUTY_1_40 = 0, /*!< 1 / 40 */
    VCNL4040_PS_DUTY_1_80,     /*!< 1 / 80 */
    VCNL4040_PS_DUTY_1_160,    /*!< 1 / 160 */
    VCNL4040_PS_DUTY_1_320     /*!< 1 / 320 */
} vcnl4040_ps_duty_t;

/**
 * @brief PS integration time setting
 */
typedef enum
{
    VCNL4040_PS_IT_1T = 0, /*!< 1   T */
    VCNL4040_PS_IT_1_5T,   /*!< 1.5 T */
    VCNL4040_PS_IT_2T,     /*!< 2   T */
    VCNL4040_PS_IT_2_5T,   /*!< 2.5 T */
    VCNL4040_PS_IT_3T,     /*!< 3   T */
    VCNL4040_PS_IT_3_5T,   /*!< 3.5 T */
    VCNL4040_PS_IT_4T,     /*!< 4   T */
    VCNL4040_PS_IT_8T      /*!< 8   T */
} vcnl4040_ps_it_t;

/**
 * @brief PS number of output bits
 */
typedef enum
{
    VCNL4040_PS_HD_12_BITS = 0, /*!< PS output is 12 bits */
    VCNL4040_PS_HD_16_BITS      /*!< PS output is 16 bits */
} vcnl4040_ps_hd_t;

/**
 * @brief Smart persistence setting
 */
#define VCNL4040_SMART_PERSISTENCE_ENABLE  (true)  /*!< Enable smart persistence */
#define VCNL4040_SMART_PERSISTENCE_DISABLE (false) /*!< Disable smart persistence */

/**
 * @brief LED current selection setting
 */
typedef enum
{
    VCNL4040_LED_CURRENT_50mA = 0, /*!< LED current 50 mA */
    VCNL4040_LED_CURRENT_75mA,     /*!< LED current 75 mA */
    VCNL4040_LED_CURRENT_100mA,    /*!< LED current 100 mA */
    VCNL4040_LED_CURRENT_120mA,    /*!< LED current 120 mA */
    VCNL4040_LED_CURRENT_140mA,    /*!< LED current 140 mA */
    VCNL4040_LED_CURRENT_160mA,    /*!< LED current 160 mA */
    VCNL4040_LED_CURRENT_180mA,    /*!< LED current 180 mA */
    VCNL4040_LED_CURRENT_200mA     /*!< LED current 200 mA */
} vcnl4040_led_current_t;

/**
 * @brief VCNL4040 driver configuration
 */
typedef struct
{
    vcnl4040_ps_duty_t     ps_duty;              /*!< PS IRED on / off duty ratio */
    vcnl4040_ps_it_t       ps_it;                /*!< PS integration time */
    vcnl4040_ps_hd_t       ps_out_bits;          /*!< PS output bits */
    bool                   smart_persistence_en; /*!< Smart persistence enable */
    vcnl4040_led_current_t led_curr;             /*!< LED current */
    uint32_t               timeout;              /*!< Serial read timeout in ms */
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