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
#define SPI0_ICM20649_CS_PIN  3
#define SPI0_MISO_PIN         9
#define SPI0_MOSI_PIN         10
#define SPI0_CLK_PIN          11

/* ADXL372 and MT25QL256 shared SPI bus pin mappings */
#define SPI1_ADXL372_CS_PIN      8
#define SPI1_MT25QL256ABA_CS_PIN 15
#define SPI1_MOSI_PIN            12
#define SPI1_MISO_PIN            16
#define SPI1_CLK_PIN             13

/*******************************
 * @brief I2C pin mappings
 *******************************/

#define I2C_SDA                 15
#define I2C_SCL                 16

#ifdef __cplusplus
}
#endif

#endif /* PIN_MAP_REV2_H */
