/**
 * @file mt25q.c
 * @author UBC Capstone Team 2020/2021
 * @brief MT25Q flash storage driver
 */

#include "mt25q.h"
#include "mt25q_reg.h"

/**
 * @brief Initialize MT25Q driver with configurations
 * 
 * @param cfg - Driver configurations
 * @return sysret_t - Driver status
 */
sysret_t mt25q_init(mt25q_cfg_t* cfg)
{
    sysret_t ret = RET_ERR;

    /* TODO */

    return ret;
}

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
sysret_t mt25q_page_program(uint32_t address, uint8_t* buf)
{
    sysret_t ret = RET_ERR;

    /* TODO */

    return ret;
}

/**
 * @brief Set device bits to 0xFF. Needless to say this is irreversible.
 * 
 * @return sysret_t - Driver status
 */
sysret_t mt25q_bulk_erase(void)
{
    sysret_t ret = RET_ERR;

    /* TODO */

    return ret;
}

/**
 * @brief Check MT25Q driver status
 * 
 * @return sysret_t - Driver status
 */
sysret_t mt25q_test(void)
{
    sysret_t ret = RET_ERR;

    /* TODO */

    return ret;
}