/**
* Copyright (c) 2018 makerdiary
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above
*   copyright notice, this list of conditions and the following
*   disclaimer in the documentation and/or other materials provided
*   with the distribution.

* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef IMU_PCB_REV1_H
#define IMU_PCB_REV1_H 

#ifdef __cplusplus
extern "C" {
#endif

/*******************************
 * @brief UART pin mappings
 *******************************/

#define RX_PIN_NUMBER  23
#define TX_PIN_NUMBER  24

/*******************************
 * @brief SPI pin mappings
 *******************************/

/* ICM206649 SPI0 pin mappings */
#define SPI0_PERIPH           0
#define SPI0_ICM20649_CS_PIN  3
#define SPI0_MISO_PIN         0
#define SPI0_MOSI_PIN         1
#define SPI0_CLK_PIN          2

/* SPI2 bus pin mappings */
#define SPI2_PERIPH              2
#define SPI2_MOSI_PIN            10
#define SPI2_MISO_PIN            9
#define SPI2_CLK_PIN             11

/* ADXL372-specific SPI2 bus pin mappings */
#define SPI2_ADXL372_CS_PIN      8
#define SPI2_ADXL372_MOSI_PIN    SPI2_MOSI_PIN
#define SPI2_ADXL372_MISO_PIN    SPI2_MISO_PIN
#define SPI2_ADXL372_CLK_PIN     SPI2_CLK_PIN

/* MT25Q-specific SPI2 bus pin mappings */
#define SPI2_MT25Q_CS_PIN     15
#define SPI2_MT25Q_MOSI_PIN   12
#define SPI2_MT25Q_MISO_PIN   16
#define SPI2_MT25Q_CLK_PIN    13

/*******************************
 * @brief I2C pin mappings
 *******************************/

#define I2C1_PERIPH               1 /*!< I2C1 used */
#define I2C1_SDA                 20
#define I2C1_SCL                 19

#ifdef __cplusplus
}
#endif

#endif // IMU_PCB_REV1_H
