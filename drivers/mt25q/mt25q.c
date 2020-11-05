/**
 * @file mt25q.c
 * @author UBC Capstone Team 2020/2021
 * @brief MT25Q flash storage driver
 */

#include <string.h>
#include "mt25q.h"
#include "mt25q_reg.h"
#include "spi.h"

/**
 * @brief Define SPI command type
 */
typedef uint8_t cmd_t;

/*********************************
 * Helper functions
 *********************************/

/**
 * @notapi
 * @brief Send command to flash without expecting reply
 * @return sysret_t - Driver status
 */
static sysret_t write_reg(cmd_t command, void* tx, size_t txn)
{
    uint8_t buf[32U] = {0U}; /* TODO: magic number */

    /* format buffer to send */
    buf[0] = command;
    memcpy(buf+1, tx, txn);

    return spi_transfer(SPI_INSTANCE_2, SPI_DEV_MT25Q, buf, txn+1, NULL, 0);
}

/**
 * @notapi
 * @brief Send command to flash, expecting a reply
 * @return sysret_t - Driver status
 */
static sysret_t read_reg(cmd_t command, void* rx, size_t rxn)
{
    sysret_t ret;
    uint8_t buf[32U] = {0U}; /* TODO: magic number */
    ret = spi_transfer(SPI_INSTANCE_2, SPI_DEV_MT25Q, &command, 1U, buf, rxn + 1U);

    memcpy(rx, buf+1U, rxn);

    return ret;
}

/**
 * @notapi
 * @brief Enable 4byte address mode to use full 256Mb flash capacity
 * 
 * @return sysret_t - Driver status
 */
static inline sysret_t enable_4byte_addr_mode(void)
{
    return write_reg(MT25Q_ENTER_4B_ADDR_CMD, NULL, 0);
}

/**
 * @notapi
 * @brief Enable PROGRAM, ERASE, and WRITE commands
 * 
 * @return sysret_t - Driver status
 */
static inline sysret_t write_enable(void)
{
    return write_reg(MT25Q_WRITE_ENABLE_CMD, NULL, 0);
}

/**
 * @notapi
 * @brief Read from Device ID data tables, verify their contents to check proper SPI communication
 * 
 * @return sysret_t - Driver status
 */
static sysret_t check_id(void)
{
    sysret_t ret = RET_ERR;

    /* TODO */
    uint8_t rx[MT25Q_DEV_ID_NUMBYTES] = {0};
    uint8_t expected[MT25Q_DEV_ID_NUMBYTES] = {
        MT25Q_MANUFACTURER_ID_EXPECTED,
        MT25Q_MEM_TYPE_EXPECTED,
        MT25Q_MEM_CAP_EXPECTED };

    ret = read_reg(MT25Q_READ_ID_CMD, rx, MT25Q_DEV_ID_NUMBYTES);
    SYSRET_CHECK(ret);

    if(memcmp(rx, expected, MT25Q_DEV_ID_NUMBYTES) != 0)
        ret = RET_SERIAL_ERR;
    // if(rx[0] != expected[0])
    //     ret = RET_SERIAL_ERR;
    // else if(rx[1] != expected[1])
    //     ret = RET_SERIAL_ERR;
    // else if(rx[2] != expected[2])
    //     ret = RET_SERIAL_ERR;
    // else
    //     ret = RET_OK;

    return ret;
}

/*********************************
 * API
 *********************************/

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
    ret = enable_4byte_addr_mode();

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
    write_enable();

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
    ret = check_id();

    return ret;
}