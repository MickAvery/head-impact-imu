/**
 * @file datetime.c
 * @author UBC Capstone Team 2020/2021
 * @brief Datetime library for accurate recording of date and time
 */

#ifndef DATETIME_H
#define DATETIME_H

#include "nrf.h"

/**
 * @brief Defines the states of the datetime module
 */
typedef enum
{
    DATETIME_UNINIT = 0, /*!< Datetime module uninitialized, call datetime_init() first */
    DATETIME_UNSET,      /*!< No datetime has been set, RTC is disabled */
    DATETIME_SET,        /*!< Datetime has been set, RTC is ticking */
    DATETIME_STATES      /*!< Max number of states */
} datetime_state_t;

/**
 * @brief Defines the error types that the module can return
 */
typedef enum
{
    DATETIME_OK = 0,   /*!< No error */
    DATETIME_ERR,      /*!< Unknown error */
    DATETIME_NOTIME,   /*!< Datetime requested when none available */
    DATETIME_ERR_CODES /*!< Max number of error codes */
} datetime_err_code_t;

/**
 * @brief Struct to contain datetime info
 */
typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hr;
    uint8_t  min;
    uint8_t  sec;
    uint16_t msec;
} datetime_t;


#ifdef __cplusplus
extern "C" {
#endif

void datetime_init(void);
datetime_err_code_t datetime_set(datetime_t* datetime_in);
datetime_err_code_t datetime_reset(void);
datetime_err_code_t datetime_get(datetime_t* datetime_out);

#ifdef __cplusplus
}
#endif

#endif /* DATETIME_H */