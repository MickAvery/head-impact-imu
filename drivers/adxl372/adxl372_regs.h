/**
 * @file adxl372_regs.h
 * @author UBC Capstone Team 2020/2021
 * @brief Register definitions for ADXL372 High-g Accelerometer
 */

#ifndef ADXL372_REG_H
#define ADXL372_REG_H

#define ADXL372_DEVID_AD_ADDR          0x00U /*!< Address of Analog Devices ID Register [READ-ONLY] */
#define ADXL372_DEVID_AD_EXPECTED_VAL  0xADU /*!< Expected value stored in Analog Devices ID Register */

#define ADXL372_DEVID_MST_ADDR         0x01U /*!< Address of Devices MEMS ID Register [READ-ONLY] */
#define ADXL372_DEVID_MST_EXPECTED_VAL 0x1DU /*!< Expected value stored in Devices MEMS ID Register */

#define ADXL372_PARTID_ADDR            0x02U /*!< Address of Device ID Register [READ-ONLY] */
#define ADXL372_PARTID_EXPECTED_VAL    0xFAU /*!< Expected value stored in Device ID register */

#endif /* ADXL372_REG_H */