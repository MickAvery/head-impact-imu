/**
 * @file datalog.c
 * @author UBC Capstone Team 2020/2021
 * @brief Datalog API, library related to
 *        logging sensor data to onboard flash
 */

#include <string.h>
#include "datalog.h"
#include "mt25q.h"
#include "nrf_assert.h"
#include "nrf_log.h"

/**
 * @brief Track state of datalogger
 */
static datalog_state_t datalogger_state = DATALOG_STOPPED;

/**
 * @brief Track size of datalog
 */
static uint32_t datalog_size = 0U;

/**
 * @brief Starting address of datalog in flash
 */
static const uint32_t datalog_base_flash_addr = FLASH_4KB_SUBSECTOR_SIZE;

/*********************************************************
 * 
 * HELPER FUNCTIONS
 * 
 *********************************************************/

/**
 * @notapi
 * @brief Erase contents of existing datalog
 * 
 * @return sysret_t
 */
// static sysret_t erase_datalog(void)
// {
//     sysret_t ret = RET_ERR;

//     for(uint32_t i = FLASH_4KB_SUBSECTOR_SIZE ; i < FLASH_CAPACITY ; i += FLASH_4KB_SUBSECTOR_SIZE)
//     {
//         ret = mt25q_4kB_subsector_erase(i);
//         SYSRET_CHECK(ret);
//     }

//     return ret;
// }

/*********************************************************
 * 
 * API
 * 
 *********************************************************/

/**
 * @brief Start datalogging session.
 * 
 * @note This erases preexisting datalogs to make space
 *       for new ones
 *
 * @param dev_metadata Update device metadata to reflect erasing preexisting datalog
 * @return sysret_t
 */
sysret_t datalog_start(metadata_t* dev_metadata)
{
    ASSERT(dev_metadata);
    sysret_t ret = RET_ERR;

    if(datalogger_state != DATALOG_STOPPED)
        return ret;

    /* clear space on flash for new datalogging session */
    ret = datalog_erase(dev_metadata);
    SYSRET_CHECK(ret);

    datalog_size = 0U;
    datalogger_state = DATALOG_START;

    return ret;
}

/**
 * @brief Log data to flash
 * 
 * If any of the inputs are null, then that information
 * is not included in the datalog row and their existence
 * is logged in the 1B row header.
 * 
 * @param dt Datetime
 * @param gyro Gyroscope raw data
 * @param low_g_accel Low-G Accelerometer raw data
 * @param high_g_accel High-G Accelerometer raw data
 * @return sysret_t 
 */
sysret_t datalog_log(
    datetime_t* dt,
    int16_t gyro[3U],
    int16_t low_g_accel[3U],
    int16_t high_g_accel[3U])
{
    sysret_t ret = RET_ERR;
    uint8_t datalog_row[DATALOG_ROW_MAX_SIZE] = {0U};
    uint8_t row_header = 0U;
    size_t i = 1U; /* row indexer, start at index 1 to skip row header */

    if(datalogger_state != DATALOG_START)
        return ret;

    /* insert datetime info */
    if(dt != NULL)
    {
        (void)memcpy(datalog_row + i, dt, sizeof(datetime_t));
        i += sizeof(datetime_t);
        row_header |= DATALOG_DATETIME_AVAILABLE;
    }

    /* insert gyroscope information */
    if(gyro != NULL)
    {
        (void)memcpy(datalog_row + i, gyro, sizeof(int16_t)*3U);
        i += sizeof(int16_t)*3U;
        row_header |= DATALOG_GYRO_AVAILABLE;
    }

    /* insert low-g accelerometer information */
    if(low_g_accel != NULL)
    {
        (void)memcpy(datalog_row + i, low_g_accel, sizeof(int16_t)*3U);
        i += sizeof(int16_t)*3U;
        row_header |= DATALOG_LOW_G_ACCEL_AVAILABLE;
    }

    /* insert high-g accelerometer information */
    if(high_g_accel != NULL)
    {
        (void)memcpy(datalog_row + i, high_g_accel, sizeof(int16_t)*3U);
        i += sizeof(int16_t)*3U;
        row_header |= DATALOG_HIGH_G_ACCEL_AVAILABLE;
    }

    datalog_row[0] = row_header;

    if(i > 1)
    {
        uint32_t new_size = datalog_size + i;
        uint32_t end_addr = datalog_base_flash_addr + new_size;

        if(end_addr < FLASH_CAPACITY)
        {
            /* save datalog row to flash */

            /* determine if flash write needs to be broken up to
             * prevent wrap-around */
            uint32_t next_page = NEXT_PAGE_ADDR_FROM_CURR(datalog_base_flash_addr + datalog_size);

            if(next_page < end_addr)
            {
                /* write needs to be broken up into two batches to prevent wrap-around */
                uint32_t extra = end_addr - next_page;

                /* write first chunk... */
                ret = mt25q_page_program(
                    datalog_base_flash_addr + datalog_size,
                    datalog_row,
                    i - extra);
                SYSRET_CHECK(ret);

                /* ...then the next */
                ret = mt25q_page_program(
                    datalog_base_flash_addr + datalog_size + (i - extra),
                    datalog_row + (i - extra),
                    extra);
                SYSRET_CHECK(ret);
            }
            else
            {
                /* can write row all at once */
                ret = mt25q_page_program(
                    datalog_base_flash_addr + datalog_size,
                    datalog_row,
                    i);
                SYSRET_CHECK(ret);
            }

            datalog_size = new_size;
        }
    }

    return ret;
}

/**
 * @brief Stop datalogging, save datalog information to flash
 * 
 * @param dev_metadata Update device metadata before saving to flash
 * @return sysret_t 
 */
sysret_t datalog_stop(metadata_t* dev_metadata)
{
    ASSERT(dev_metadata);
    sysret_t ret = RET_ERR;

    if(datalogger_state != DATALOG_START)
        return ret;

    ret = RET_OK;

    /* update metadata if data was logged */
    if(datalog_size > 0)
    {
        dev_metadata->device_metadata.datalog_header = CONFIGS_FRAME_HEADER;
        dev_metadata->device_metadata.datalog_size = datalog_size;
        (void)memcpy(
            &(dev_metadata->device_metadata.datalog_configs),
            &(dev_metadata->device_metadata.current_dev_configs),
            sizeof(configs_t)
        );

        ret = configs_save(dev_metadata);
    }

    return ret;
}

/**
 * @brief Erase current datalog information from flash
 * 
 * @param dev_metadata Update device metadata before saving to flash
 * @return sysret_t 
 */
sysret_t datalog_erase(metadata_t* dev_metadata)
{
    ASSERT(dev_metadata);
#if 0
    sysret_t ret = RET_ERR;

    if(datalogger_state != DATALOG_STOPPED)
        return ret;

    /* update metadata */
    dev_metadata->device_metadata.datalog_header = 0U;
    dev_metadata->device_metadata.datalog_size = 0U;
    ret = configs_save(dev_metadata);
    SYSRET_CHECK(ret);

    /* erase contents of preexisting datalog */
    ret = erase_datalog();

    return ret;
#endif
    return RET_OK;
}