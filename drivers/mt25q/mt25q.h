/**
 * @file mt25q.h
 * @author UBC Capstone Team 2020/2021
 * @brief MT25Q flash storage driver
 */

#ifndef MT25Q_H
#define MT25Q_H

#include <stdint.h>
#include "retcodes.h"

/**
 * @brief The following macros contain key characteristics of the flash chip.
 */
#define FLASH_PAGE_SIZE           256U   /*!< Page size in bytes */
#define FLASH_4KB_SUBSECTOR_SIZE  4096U  /*!< 4KB subsector size in bytes */
#define FLASH_32KB_SUBSECTOR_SIZE 32768U /*!< 32KB subsector size in bytes */
#define FLASH_SECTOR_SIZE         65536U /*!< Sector size in bytes */

/**
 * @brief MT25Q driver configurations
 * 
 * @note The Capstone 2020/2021 team decided that the default flash chip settings
 *       are enough to meet project requirements. If any future teams think it
 *       necessary to provide the ability to reconfigure the flash chip, then
 *       feel free to add configuration options here. Then make sure to make the
 *       appropriate serial transmissions in @ref mt25q_init().
 */
typedef struct
{
    uint32_t timeout_ms; /*!< Maximum available time in ms to wait for write to complete */
} mt25q_cfg_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize MT25Q driver with configurations
 * 
 * @param cfg - Driver configurations
 * @return sysret_t - Driver status
 */
sysret_t mt25q_init(mt25q_cfg_t* cfg);

/**
 * @brief Program bytes from buf into flash page starting at address
 * 
 * @note It is the caller's responsibility to make sure that all bytes written
 *       to page at the starting address all fall within the page's boundaries.
 *       Any bytes written beyond the page boundaries wrap around the page address
 *       and overwrite the first bytes of the page
 * 
 * @param address - Starting address to write to
 * @param buf - Bytes to write
 * @return sysret_t - Driver status
 */
sysret_t mt25q_page_program(uint32_t address, uint8_t* buf);

/**
 * @brief Set device bits to 0xFF. Needless to say this is irreversible.
 * 
 * @return sysret_t - Driver status
 */
sysret_t mt25q_bulk_erase(void);

/**
 * @brief Check MT25Q driver status
 * 
 * @return sysret_t - Driver status
 */
sysret_t mt25q_test(void);

#ifdef __cplusplus
}
#endif

#endif /* MT25Q_H */