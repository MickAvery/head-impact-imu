/**
 * @file pin_mappings_rev_2.h
 * @author UBC Capstone Team 2020/2021
 * @brief Pin definitions for PCB Rev 2 
 */

#ifndef PIN_MAP_REV2_H
#define PIN_MAP_REV2_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************
 * @brief UART pin mappings
 *******************************/

#define RX_PIN_NUMBER  19
#define TX_PIN_NUMBER  20

/*******************************
 * @brief SPI pin mappings
 *******************************/

/* ICM206649 SPI pin mappings */
#define SPI0_PERIPH           0
#define SPI0_ICM20649_CS_PIN  3
#define SPI0_MISO_PIN         9
#define SPI0_MOSI_PIN         10
#define SPI0_CLK_PIN          11

/* ADXL372 and MT25QL256 shared SPI bus pin mappings */
#define SPI2_PERIPH              2
#define SPI2_ADXL372_CS_PIN      8
#define SPI2_MT25Q_CS_PIN        15
#define SPI2_MOSI_PIN            12
#define SPI2_MISO_PIN            16
#define SPI2_CLK_PIN             13

/*******************************
 * @brief I2C pin mappings
 *******************************/

#define I2C1_PERIPH               1 /*!< I2C1 used */
#define I2C1_SDA                 23
#define I2C1_SCL                 24

#ifdef __cplusplus
}
#endif

#endif /* PIN_MAP_REV2_H */
