/**
 * @file retcodes.h
 * @author UBC Capstone Team 2020/2021
 * @brief System-wide API return codes
 */

#ifndef RETCODES_H
#define RETCODES_H

#include <stdint.h>
#include "nrf_error.h"

typedef uint32_t retcode_t;

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

extern const char* retcodes_desc[RET_CODES];

#endif /* RETCODES_H */