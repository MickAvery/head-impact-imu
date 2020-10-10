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

#define ADXL372_STATUS_ADDR            0x04U /*!< Address of Status register [READ-ONLY] */
#define ADXL372_STATUS_DATA_RDY_MASK   0x01U /*!< Mask for DATA RDY bit in Status register */

#define ADXL372_XDATA_H_ADDR           0x08U /*!< Address of X Data H Register [READ-ONLY] */
#define ADXL372_XDATA_L_ADDR           0x09U /*!< Address of X Data L Register [READ-ONLY] */
#define ADXL372_YDATA_H_ADDR           0x0AU /*!< Address of Y Data H Register [READ-ONLY] */
#define ADXL372_YDATA_L_ADDR           0x0BU /*!< Address of Y Data L Register [READ-ONLY] */
#define ADXL372_ZDATA_H_ADDR           0x0CU /*!< Address of Z Data H Register [READ-ONLY] */
#define ADXL372_ZDATA_L_ADDR           0x0DU /*!< Address of Z Data L Register [READ-ONLY] */

#define ADXL372_TIMING_ADDR            0x3CU /*!< Address of Timing Register [R/W] */
#define ADXL372_TIMING_ODR_MASK        0xE0U /*!< Mask for ODR bits in Timing register */

#define ADXL372_POWER_CTL_ADDR         0x3FU /*!< Address of Power Control Register */
#define ADXL372_POWER_CTL_MODE_MASK    0x03U /*!< Mask for MODE bits in Power Control Register */

#endif /* ADXL372_REG_H */