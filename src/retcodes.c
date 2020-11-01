/**
 * @file retcodes.c
 * @author your name (you@domain.com)
 * @brief 
 */

#include "retcodes.h"

/**
 * @brief Error code description strings
 * 
 * Make sure to keep this up to date with new codes added
 * in @ref retcode_desc_t
 */
const char* retcodes_desc[RET_CODES] =
{
    /* Everything's OK */
    "SYS OK",

    /* Error codes defined by nRF SDK */
    "NRF_ERROR_SVC_HANDLER_MISSING",
    "NRF_ERROR_SOFTDEVICE_NOT_ENABLED",
    "NRF_ERROR_INTERNAL",
    "NRF_ERROR_NO_MEM",
    "NRF_ERROR_NOT_FOUND",
    "NRF_ERROR_NOT_SUPPORTED",
    "NRF_ERROR_INVALID_PARAM",
    "NRF_ERROR_INVALID_STATE",
    "NRF_ERROR_INVALID_LENGTH",
    "NRF_ERROR_INVALID_FLAGS",
    "NRF_ERROR_INVALID_DATA",
    "NRF_ERROR_DATA_SIZE",
    "NRF_ERROR_TIMEOUT",
    "NRF_ERROR_NULL",
    "NRF_ERROR_FORBIDDEN",
    "NRF_ERROR_INVALID_ADDR",
    "NRF_ERROR_BUSY",

    /* Project-specific error codes */
    "SYS UNKNOWN ERROR",
    "SYS DRIVER UNINITIALIZED",
    "SYS SERIAL COMMUNICATION ERROR",
    "SYS SELF-TEST ERROR",
    "SYS TIMEOUT ERROR"
};