/**
 * @file spi.h
 * @author UBC Capstone Team 2020/2021
 * @brief SPI Driver
 */

#ifndef SPIDRV_H
#define SPIDRV_H

#include <stddef.h>
#include "retcodes.h"

/**
 * @brief Defines available SPI instances
 */
typedef enum
{
    SPI_INSTANCE_0 = 0, /*!< SPI Instance 0 */
    SPI_INSTANCE_2,     /*!< SPI Instance 1 */
    SPI_INSTANCE_MAX    /*!< Max number of SPI Instances */
} spi_instance_t;

/**
 * @brief Defines list of devices on SPI busses
 */
typedef enum
{
    SPI_DEV_ICM20649 = 0, /*!< ICM20649 IMU */
    SPI_DEV_ADXL372,      /*!< ADXL372 IMU */
    SPI_DEV_MAX           /*!< Max number of devices using SPI busses */
} spi_devs_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize SPI instances
 */
retcode_t spi_init(void);

/**
 * @brief Trigger a transfer on the SPI bus
 * 
 * @param instance - SPI bus to read from
 * @param dev - Specify device to determine correct CS pin
 * @param txbuf - bytes to transmit
 * @param txn - number of bytes to transmit
 * @param rxbuf - buffer to receive bytes
 * @param rxn - number of bytes to store in rxbuf
 */
void spi_transfer(spi_instance_t instance, spi_devs_t dev, void* txbuf, size_t txn, void* rxbuf, size_t rxn);


// void spi_lock(spi_instance_t instance, spi_devs_t dev, const nrf_drv_spi_config_t* cfg);

#ifdef __cplusplus
}
#endif

#endif /* SPIDRV_H */