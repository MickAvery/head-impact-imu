/**
 * @file retcodes.h
 * @author UBC Capstone Team 2020/2021
 * @brief System-wide API return codes
 */

#ifndef RETCODES_H
#define RETCODES_H

#include <stdint.h>
#include "nrf_error.h"

/**
 * @brief Project-specific system-wide return code type
 * 
 * Expands the @ref ret_code_t codes that the nRF SDK provides in nrf_errors.h
 * to provide additional project-specific error codes
 */
typedef uint32_t sysret_t;

/**
 * @brief System return codes
 */
typedef enum
{
    RET_OK  = NRF_SUCCESS,      /*!< No errors reported, system ok */
    RET_ERR = NRF_ERROR_BUSY+1, /*!< Generic error */
    RET_DRV_UNINIT,             /*!< Driver uninitialized */
    RET_SERIAL_ERR,             /*!< Serial communication error */
    RET_SELFTEST_ERR,           /*!< Device selftest failed */
    RET_TIMEOUT,                /*!< Device timeout */
    RET_CODES                   /*!< Max number of retcodes */
} retcode_desc_t;

/**
 * @brief Use to check return values of type @ref sysret_t
 * 
 * Project-specific handler to define action when return code
 * is anything but OK
 */
#define SYSRET_CHECK(ret) \
    if(ret != RET_OK)     \
        return ret;

/**
 * @brief Error code description strings
 * 
 * Make sure to keep this up to date with new codes added
 * in @ref retcode_desc_t
 */
extern const char* retcodes_desc[RET_CODES];

#endif /* RETCODES_H */