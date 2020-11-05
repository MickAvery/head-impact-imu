/**
 * @file mt25q.c
 * @author UBC Capstone Team 2020/2021
 * @brief MT25Q flash storage driver
 */

#include <string.h>
#include "mt25q.h"
#include "mt25q_reg.h"
#include "spi.h"
#include "app_timer.h"

/**
 * @brief Define SPI command type
 */
typedef uint8_t cmd_t;

/**
 * @brief MT25Q driver states
 */
typedef enum
{
    MT25Q_STATE_UNINIT = 0, /*!< Driver uninitialized */
    MT25Q_STATE_RUNNING     /*!< Driver running */
} mt25q_state_t;

/**
 * @brief Write types (i.e. reasons why write enable set)
 */
typedef enum
{
    WRITE_PROG = 0,
    WRITE_ERASE
} mt25q_writetype_t;

/**
 * @brief MT25Q driver instance definition
 */
typedef struct
{
    mt25q_cfg_t* cfg;    /*!< Driver configurations */
    mt25q_state_t state; /*!< Driver state */
} mt25q_t;

/**
 * @brief MT25Q driver singleton
 */
static mt25q_t mt25q = {
    .cfg = NULL,
    .state = MT25Q_STATE_UNINIT
};

/**************************************
 * timer objects to detect
 * ERASE and PROGRAM timeouts
 **************************************/

/**
 * @brief ERASE timer handle
 */
APP_TIMER_DEF(timeout_timer);

/**
 * @notapi
 * @brief on ERASE or PROGRAM timeout, set return code appropariately
 * @param p_ctx - this should be pointer to return code
 */
static void timeout_handler(void* p_ctx)
{
    /* when timer times out, set retcode */
    *((sysret_t*)p_ctx) = RET_TIMEOUT;
}

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
    uint8_t buf[1U + 4U + FLASH_PAGE_SIZE] = {0U}; /*  */

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
 * @brief 
 * 
 * @return sysret_t 
 */
static sysret_t wait_for_write_complete(mt25q_writetype_t write_type)
{
    sysret_t ret = RET_ERR;
    uint32_t timeout_ms = (write_type == WRITE_PROG) ? mt25q.cfg->timeout_ms : BULK_ERASE_TIME_MS;

    ret = app_timer_start(
        timeout_timer,
        APP_TIMER_TICKS(timeout_ms),
        &ret);

    do
    {
        /* poll WIP flag in status register */
        uint8_t rx = 0U;
        ret = read_reg(MT25Q_READ_STATUS_REG_CMD, &rx, 1U);

        if(ret != RET_OK)
            break;
        else if(!(rx & MT25Q_WRITE_IN_PROGRESS))
        {
            ret = RET_OK;
            break;
        }
    } while(ret != RET_TIMEOUT);

    (void)app_timer_stop(timeout_timer);

    return ret;
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

    /* Enable 4-Byte addressing mode */
    ret = enable_4byte_addr_mode();
    SYSRET_CHECK(ret);

    /* configure timer to detect ERASE timeout */
    ret = app_timer_create(
        &timeout_timer,
        APP_TIMER_MODE_SINGLE_SHOT,
        timeout_handler);
    SYSRET_CHECK(ret);

    mt25q.cfg = cfg;
    mt25q.state = MT25Q_STATE_RUNNING;

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
 * @param n - Number of bytes to write
 * @return sysret_t - Driver status
 */
sysret_t mt25q_page_program(uint32_t address, uint8_t* buf, size_t n)
{
    sysret_t ret = RET_ERR;

    /* enable write for PROGRAM command */
    ret = write_enable();
    SYSRET_CHECK(ret);

    ret = spi_flash_transfer(
        SPI_INSTANCE_2, SPI_DEV_MT25Q,
        MT25Q_4B_PAGE_PROG_CMD, address,
        buf, n,
        NULL, 0U);
    SYSRET_CHECK(ret);

    /* wait for PROGRAM to complete */
    ret = wait_for_write_complete(WRITE_PROG);

    return ret;
}

/**
 * @brief Read from flash starting at address
 * 
 * @param address - Address to read from
 * @param buf - Buffer to store bytes
 * @param n - Number of bytes to read
 * @return sysret_t - Driver status
 */
sysret_t mt25q_read(uint32_t address, uint8_t* buf, size_t n)
{
    return spi_flash_transfer(
        SPI_INSTANCE_2, SPI_DEV_MT25Q,
        MT25Q_4B_READ_CMD, address,
        NULL, 0U,
        buf, n+1);
}

/**
 * @brief Set device bits to 0xFF. Needless to say this is irreversible.
 * 
 * @return sysret_t - Driver status
 */
sysret_t mt25q_bulk_erase(void)
{
    sysret_t ret = RET_ERR;

    /* enable write for ERASE command */
    ret = write_enable();
    SYSRET_CHECK(ret);

    /* send BULK ERASE command */
    ret = write_reg(MT25Q_BULK_ERASE_CMD, NULL, 0U);
    SYSRET_CHECK(ret);

    /* wait for ERASE to complete */
    ret = wait_for_write_complete(WRITE_ERASE);

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

    /* Check SPI communication */
    ret = check_id();
    SYSRET_CHECK(ret);

    /* Check driver state */
    if(mt25q.state == MT25Q_STATE_UNINIT)
        ret = RET_DRV_UNINIT;
    else
        ret = RET_OK;

    return ret;
}