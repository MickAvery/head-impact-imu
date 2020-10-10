/**
 * @file spi.c
 * @author UBC Capstone Team 2020/2021
 * @brief SPI Driver
 */

#include "spi.h"
#include "custom_board.h"
#include "nrf_gpio.h"

/**
 * @brief SPI instances used by system
 */
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
static const nrf_drv_spi_t spi1 = NRF_DRV_SPI_INSTANCE(1);

/**
 * @brief SPI instance configurations
 */
static nrf_drv_spi_config_t spi0_cfg = NRF_DRV_SPI_DEFAULT_CONFIG;
static nrf_drv_spi_config_t spi1_cfg = NRF_DRV_SPI_DEFAULT_CONFIG;

/**
 * @brief CS pin mappings
 */
static const uint8_t cs_pins[SPI_DEV_MAX] = {
    SPI0_ICM20649_CS_PIN, SPI1_ADXL372_CS_PIN
};

/*********************************
 * API
 *********************************/

/**
 * @brief Initialize SPI instances
 */
void spi_init(void)
{
    ret_code_t ret;

    /* Initialize SPI-0 instance */
    spi0_cfg.sck_pin   = SPI0_CLK_PIN;
    spi0_cfg.mosi_pin  = SPI0_MOSI_PIN;
    spi0_cfg.miso_pin  = SPI0_MISO_PIN;
    spi0_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
    spi0_cfg.frequency = NRF_DRV_SPI_FREQ_4M;
    ret = nrf_drv_spi_init(&(spi0), &(spi0_cfg), NULL, NULL);
    APP_ERROR_CHECK(ret);

    /* Initialize SPI-1 instance */
    spi1_cfg.sck_pin   = SPI1_CLK_PIN;
    spi1_cfg.mosi_pin  = SPI1_MOSI_PIN;
    spi1_cfg.miso_pin  = SPI1_MISO_PIN;
    spi1_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
    spi1_cfg.frequency = NRF_DRV_SPI_FREQ_8M;
    ret = nrf_drv_spi_init(&(spi1), &(spi1_cfg), NULL, NULL);
    APP_ERROR_CHECK(ret);

    /* Initialize CS GPIO pins */
    for(size_t i = 0 ; i < SPI_DEV_MAX ; i++)
    {
        nrf_gpio_cfg_output(cs_pins[i]);
        nrf_gpio_pin_set(cs_pins[i]);
    }
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
 */
void spi_transfer(
    spi_instance_t instance, spi_devs_t dev,
    void* txbuf, size_t txn, void* rxbuf, size_t rxn)
{
    ASSERT(txbuf != NULL && rxbuf != NULL);
    ASSERT(instance < SPI_INSTANCE_MAX);
    ASSERT(dev < SPI_DEV_MAX);

    nrf_drv_spi_t const * const spi = (instance == SPI_INSTANCE_0) ? &(spi0) : &(spi1);
    uint8_t pin = cs_pins[dev];

    /**
     * Initiate transfer, manually reset and set CS pin
     */
    nrf_gpio_pin_clear(pin);
    ret_code_t ret = nrf_drv_spi_transfer(spi, txbuf, txn, rxbuf, rxn);
    nrf_gpio_pin_set(pin);

    APP_ERROR_CHECK(ret);
}