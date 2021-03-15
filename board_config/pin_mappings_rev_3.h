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

#define RX_PIN_NUMBER  19 /* P0.19 */
#define TX_PIN_NUMBER  20 /* P0.20 */

/*******************************
 * @brief SPI pin mappings
 *******************************/

/* ICM206649 SPI pin mappings */
#define SPI0_PERIPH           0
#define SPI0_ICM20649_CS_PIN  6 /* P0.06 */
#define SPI0_MISO_PIN         3 /* P0.03 */
#define SPI0_MOSI_PIN         4 /* P0.04 */
#define SPI0_CLK_PIN          5 /* P0.05 */

/* ADXL372 and MT25QL256 shared SPI bus pin mappings */
#define SPI2_PERIPH              2
#define SPI2_ADXL372_CS_PIN      22 /* P0.22 */
#define SPI2_MT25Q_CS_PIN        16 /* P0.16 */
#define SPI2_MOSI_PIN            12 /* P0.12 */
#define SPI2_MISO_PIN            11 /* P0.11 */
#define SPI2_CLK_PIN             15 /* P0.15 */

/*******************************
 * @brief I2C pin mappings
 *******************************/

#define I2C1_PERIPH               1 /*!< I2C1 used */
#define I2C1_SDA                 18 /* P0.18 */
#define I2C1_SCL                 17 /* P0.17 */

#ifdef __cplusplus
}
#endif

#endif /* PIN_MAP_REV2_H */
