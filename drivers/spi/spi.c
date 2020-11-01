/**
 * @file spi.c
 * @author UBC Capstone Team 2020/2021
 * @brief SPI Driver
 */

#include "spi.h"
#include "custom_board.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"

/**
 * @brief SPI instances used by system
 */
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(SPI0_PERIPH);
static const nrf_drv_spi_t spi2 = NRF_DRV_SPI_INSTANCE(SPI2_PERIPH);

/**
 * @brief SPI instance configurations
 */
static nrf_drv_spi_config_t spi0_cfg = NRF_DRV_SPI_DEFAULT_CONFIG;
static nrf_drv_spi_config_t spi2_cfg = NRF_DRV_SPI_DEFAULT_CONFIG;

/**
 * @brief CS pin mappings
 */
static const uint8_t cs_pins[SPI_DEV_MAX] = {
    SPI0_ICM20649_CS_PIN, SPI2_ADXL372_CS_PIN
};

/*********************************
 * API
 *********************************/

/**
 * @brief Initialize SPI instances
 * 
 * @return sysret_t - Module status
 */
sysret_t spi_init(void)
{
    sysret_t ret = RET_ERR;

    /* Initialize SPI-0 instance */
    spi0_cfg.sck_pin   = SPI0_CLK_PIN;
    spi0_cfg.mosi_pin  = SPI0_MOSI_PIN;
    spi0_cfg.miso_pin  = SPI0_MISO_PIN;
    spi0_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
    spi0_cfg.frequency = NRF_DRV_SPI_FREQ_4M;
    ret = nrf_drv_spi_init(&(spi0), &(spi0_cfg), NULL, NULL);
    if(ret != RET_OK)
        return ret;

    /* Initialize SPI-1 instance */
    spi2_cfg.sck_pin   = SPI2_CLK_PIN;
    spi2_cfg.mosi_pin  = SPI2_MOSI_PIN;
    spi2_cfg.miso_pin  = SPI2_MISO_PIN;
    spi2_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
    spi2_cfg.frequency = NRF_DRV_SPI_FREQ_8M;
    ret = nrf_drv_spi_init(&(spi2), &(spi2_cfg), NULL, NULL);
    if(ret != RET_OK)
        return ret;

    /* Initialize CS GPIO pins */
    for(size_t i = 0 ; i < SPI_DEV_MAX ; i++)
    {
        nrf_gpio_cfg_output(cs_pins[i]);
        nrf_gpio_pin_set(cs_pins[i]);
    }

    return RET_OK;
}

/**
 * @brief Trigger a transfer on the SPI bus
 * 
 * @param instance - SPI bus to read from
 * @param dev - Specify device to determine correct CS pin
 * @param txbuf - bytes to transmit
 * @param txn - number of bytes to transmit
 * @param rxbuf - buffer to receive bytes
 * @param rxn - number of bytes to store in rxbuf
 * @return sysret_t - Module status
 */
sysret_t spi_transfer(
    spi_instance_t instance, spi_devs_t dev,
    void* txbuf, size_t txn, void* rxbuf, size_t rxn)
{
    ASSERT(txbuf != NULL && rxbuf != NULL);
    ASSERT(instance < SPI_INSTANCE_MAX);
    ASSERT(dev < SPI_DEV_MAX);

    sysret_t ret = RET_ERR;

    nrf_drv_spi_t const * const spi = (instance == SPI_INSTANCE_0) ? &(spi0) : &(spi2);
    uint8_t pin = cs_pins[dev];

    /**
     * Initiate transfer, manually reset and set CS pin
     */
    nrf_gpio_pin_clear(pin);
    ret = nrf_drv_spi_transfer(spi, txbuf, txn, rxbuf, rxn);
    nrf_gpio_pin_set(pin);

    return ret;
}