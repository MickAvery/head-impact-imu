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
 * @brief NRF52 chip is limited to 255-byte transfers at a time, see:
 *        https://devzone.nordicsemi.com/f/nordic-q-a/49699/nrf52832-spi-master-s-max-transfer-size-at-once-is-255
 */
#define NRF52_MAX_SPIM_TRANSFER_SIZE 255U

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
    SPI0_ICM20649_CS_PIN, SPI2_ADXL372_CS_PIN, SPI2_MT25Q_CS_PIN
};

/*********************************
 * Helper functions
 *********************************/

/**
 * @brief Lock SPI2 peripheral to specific sensor SPI bus
 * 
 * @note Why do we need this function?
 * 
 * A peculiar hardware design choice of REV1 is that even though the ADXL372 and MT25Q
 * both use SPI2 with 9MHz, they're on different SPI busses (i.e. different pins).
 * 
 * Normally this wouldn't be an issue: just use SPI0, SPI1, and SPI2 right?
 * 
 * Well as it turns out, since we're already using I2C1 for the proximity sensor and
 * the I2C1 configuration register is mapped to that of SPI1, using I2C1 means
 * we lose the ability to use SPI1. We're left with SPI0 and SPI2 to share among
 * three SPI busses. And since ADXL372 and MT25Q both want to use 9MHz, those
 * two busses need to share the same SPI2 peripheral. Hence the need to "lock" SPI2
 * to that specific IC.
 * 
 * @param dev - Device that wants to lock SPI2
 * @return sysret_t - Driver status 
 */
static sysret_t spi2_lock(spi_devs_t dev)
{
    sysret_t ret = RET_OK;

#if defined(PCB_REV_1) || defined(PCB_REV_2)
    /* who currently holds lock SPI2 peripheral? (default=ADXL372) */
    static spi_devs_t cur_dev_lock_owner = SPI_DEV_ADXL372;
    
    if(cur_dev_lock_owner != dev)
    {
        nrf_drv_spi_uninit(&spi2);

        spi2_cfg.sck_pin   = (dev == SPI_DEV_ADXL372) ? SPI2_ADXL372_CLK_PIN  : SPI2_MT25Q_CLK_PIN;
        spi2_cfg.mosi_pin  = (dev == SPI_DEV_ADXL372) ? SPI2_ADXL372_MOSI_PIN : SPI2_MT25Q_MOSI_PIN;
        spi2_cfg.miso_pin  = (dev == SPI_DEV_ADXL372) ? SPI2_ADXL372_MISO_PIN : SPI2_MT25Q_MISO_PIN;
        spi2_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
        spi2_cfg.frequency = NRF_DRV_SPI_FREQ_8M;

        ret = nrf_drv_spi_init(&spi2, &spi2_cfg, NULL, NULL);
        SYSRET_CHECK(ret);

        cur_dev_lock_owner = dev;
    }

#endif /* defined(PCB_REV_1) || defined(PCB_REV_2) */

    return ret;
}

/*********************************
 * helper functions
 *********************************/

/**
 * @notapi
 * @brief Switch integer byte order
 */
static uint32_t htonl(uint32_t in)
{
    return ((in & 0xFF000000U) >> 24) | ((in & 0x00FF0000U) >> 8) | ((in & 0x0000FF00U) << 8) | ((in & 0x000000FFU) << 24);
}

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
    ret = nrf_drv_spi_init(&spi0, &spi0_cfg, NULL, NULL);
    if(ret != RET_OK)
        return ret;

    /* Initialize SPI-1 instance */
    spi2_cfg.sck_pin   = SPI2_CLK_PIN;
    spi2_cfg.mosi_pin  = SPI2_MOSI_PIN;
    spi2_cfg.miso_pin  = SPI2_MISO_PIN;
    spi2_cfg.ss_pin    = NRF_DRV_SPI_PIN_NOT_USED;
    spi2_cfg.frequency = NRF_DRV_SPI_FREQ_8M;
    ret = nrf_drv_spi_init(&spi2, &spi2_cfg, NULL, NULL);
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
    ASSERT(instance < SPI_INSTANCE_MAX);
    ASSERT(dev < SPI_DEV_MAX);

    sysret_t ret = RET_ERR;

    nrf_drv_spi_t const * const spi = (instance == SPI_INSTANCE_0) ? &(spi0) : &(spi2);
    uint8_t pin = cs_pins[dev];

    /**
     * @note see function doc above to know why we do this...
     */
    if(instance == SPI_INSTANCE_2)
    {
        ret = spi2_lock(dev);
        SYSRET_CHECK(ret);
    }

    /**
     * Initiate transfer, manually reset and set CS pin
     */
    nrf_gpio_pin_clear(pin);
    ret = nrf_drv_spi_transfer(spi, txbuf, txn, rxbuf, rxn);
    nrf_gpio_pin_set(pin);

    return ret;
}

/**
 * @brief Flash-specific SPI bus transfer, specifying address
 * 
 * @param instance - SPI bus to read from
 * @param dev - Specify device to determine correct CS pin
 * @param cmd - Command to send to flash chip
 * @param addr - Address to reference from flash chip
 * @param txbuf - bytes to transmit
 * @param txn - number of bytes to transmit
 * @return sysret_t - Module status
 */
sysret_t spi_flash_transmit(
    spi_instance_t instance, spi_devs_t dev,
    uint8_t cmd, uint32_t addr,
    uint8_t* txbuf, size_t txn)
{
    ASSERT(instance < SPI_INSTANCE_MAX);
    ASSERT(dev < SPI_DEV_MAX);

    sysret_t ret = RET_ERR;
    addr = htonl(addr);

    nrf_drv_spi_t const * const spi = (instance == SPI_INSTANCE_0) ? &(spi0) : &(spi2);
    uint8_t pin = cs_pins[dev];

    /**
     * @note see function doc above to know why we do this...
     */
    if(instance == SPI_INSTANCE_2)
    {
        ret = spi2_lock(dev);
        SYSRET_CHECK(ret);
    }

    /**
     * Initiate transfer, manually reset and set CS pin
     */
    nrf_gpio_pin_clear(pin);
    if((ret = nrf_drv_spi_transfer(spi, &cmd, 1U, NULL, 0U)) != RET_OK) {
        /* failed to send command */
    } else if((ret = nrf_drv_spi_transfer(spi, (uint8_t*)&addr, sizeof(addr), NULL, 0U)) != RET_OK) {
        /* failed to send address */
    } else {

        for(size_t i = 0 ; i < txn ; i += NRF52_MAX_SPIM_TRANSFER_SIZE)
        {

            uint8_t size = NRF52_MAX_SPIM_TRANSFER_SIZE;

            if( (txn - i) < NRF52_MAX_SPIM_TRANSFER_SIZE )
                size = txn - i;

            if((ret = nrf_drv_spi_transfer(spi, &txbuf[i], size, NULL, 0)) != RET_OK)
                break;
        }

    }
    nrf_gpio_pin_set(pin);

    return ret;
}

/**
 * @brief Flash-specific SPI bus transfer, specifying address
 * 
 * @param instance - SPI bus to read from
 * @param dev - Specify device to determine correct CS pin
 * @param cmd - Command to send to flash chip
 * @param addr - Address to reference from flash chip
 * @param rxbuf - buffer to receive bytes
 * @param rxn - number of bytes to store in rxbuf
 * @return sysret_t - Module status
 */
sysret_t spi_flash_receive(
    spi_instance_t instance, spi_devs_t dev,
    uint8_t cmd, uint32_t addr,
    uint8_t* rxbuf, size_t rxn)
{
    ASSERT(instance < SPI_INSTANCE_MAX);
    ASSERT(dev < SPI_DEV_MAX);

    sysret_t ret = RET_ERR;
    addr = htonl(addr);

    nrf_drv_spi_t const * const spi = (instance == SPI_INSTANCE_0) ? &(spi0) : &(spi2);
    uint8_t pin = cs_pins[dev];

    /**
     * @note see function doc above to know why we do this...
     */
    if(instance == SPI_INSTANCE_2)
    {
        ret = spi2_lock(dev);
        SYSRET_CHECK(ret);
    }

    /**
     * Initiate transfer, manually reset and set CS pin
     */
    nrf_gpio_pin_clear(pin);
    if((ret = nrf_drv_spi_transfer(spi, &cmd, 1U, NULL, 0U)) != RET_OK) {
        /* failed to send command */
    } else if((ret = nrf_drv_spi_transfer(spi, (uint8_t*)&addr, sizeof(addr), NULL, 0U)) != RET_OK) {
        /* failed to send address */
    } else {

        for(size_t i = 0 ; i < rxn ; i += NRF52_MAX_SPIM_TRANSFER_SIZE)
        {

            uint8_t size = NRF52_MAX_SPIM_TRANSFER_SIZE;

            if( (rxn - i) < NRF52_MAX_SPIM_TRANSFER_SIZE )
                size = rxn - i;

            if((ret = nrf_drv_spi_transfer(spi, NULL, 0, (uint8_t*)&rxbuf[i], size)) != RET_OK)
                break;
        }

    }
    nrf_gpio_pin_set(pin);

    return ret;
}