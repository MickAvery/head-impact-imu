/**
 * @file retcodes.h
 * @author UBC Capstone Team 2020/2021
 * @brief System-wide API return codes
 */

#ifndef RETCODES_H
#define RETCODES_H

/**
 * @brief System return codes
 */
typedef enum
{
    RET_OK = 0,       /*!< No errors reported, system ok */
    RET_ERR,          /*!< Generic error */
    RET_DRV_UNINIT,   /*!< Driver uninitialized */
    RET_SERIAL_ERR,   /*!< Serial communication error */
    RET_SELFTEST_ERR, /*!< Device selftest failed */
    RET_TIMEOUT,      /*!< Device timeout */
    RET_CODES         /*!< Max number of retcodes */
} retcode_t;

#endif /* RETCODES_H */