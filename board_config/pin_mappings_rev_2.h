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
#define SPI1_ICM20649_CS_PIN  3
#define SPI1_MISO_PIN         9
#define SPI1_MOSI_PIN         10
#define SPI1_CLK_PIN          11

/* ADXL372 and MT25QL256 shared SPI bus pin mappings */
#define SPI2_ADXL372_CS_PIN      8
#define SPI2_MT25QL256ABA_CS_PIN 15
#define SPI2_MOSI_PIN            12
#define SPI2_MISO_PIN            16
#define SPI2_SCK_PIN             13

/*******************************
 * @brief I2C pin mappings
 *******************************/

#define I2C_SDA                 15
#define I2C_SCL                 16

#ifdef __cplusplus
}
#endif

#endif /* PIN_MAP_REV2_H */
