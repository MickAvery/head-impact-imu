/**
 * @file icm20649_regs.h
 * @author UBC Capstone Team 2020/2021
 * @brief ICM20649 Register Definitions
 */

#ifndef ICM20649_REGS_H
#define ICM20649_REGS_H

/**
 * @brief Before attempting to read any registers, set the USER BANK first
 */
#define ICM20649_REG_BANK_SEL_ADDR       0x7FU /*!<  */
#define ICM20649_REG_BANK_SEL_USR_BANK_0 0x00U /*!<  */
#define ICM20649_REG_BANK_SEL_USR_BANK_1 0x10U /*!<  */
#define ICM20649_REG_BANK_SEL_USR_BANK_2 0x20U /*!<  */
#define ICM20649_REG_BANK_SEL_USR_BANK_3 0x30U /*!<  */

/*************************************
 * @brief USER BANK 0 REGISTERS
 *************************************/

#define ICM20649_WHO_AM_I_ADDR      0x00U
#define ICM20649_WHO_AM_I_VAL       0xE1U

#define ICM20649_PWR_MGMT_1_ADDR    0x06U
#define ICM20649_CLKSEL_MASK        0x07U
#define ICM20649_CLKSEL_SET(clk)    (clk & ICM20649_CLKSEL_MASK)
#define ICM20649_DEVICE_RESET_MASK  0x80U

#define ICM20649_ACCEL_XOUT_H_ADDR 0x2DU
#define ICM20649_ACCEL_XOUT_L_ADDR 0x2EU
#define ICM20649_ACCEL_YOUT_H_ADDR 0x2FU
#define ICM20649_ACCEL_YOUT_L_ADDR 0x30U
#define ICM20649_ACCEL_ZOUT_H_ADDR 0x31U
#define ICM20649_ACCEL_ZOUT_L_ADDR 0x32U

#define ICM20649_GYRO_XOUT_H_ADDR  0x33U
#define ICM20649_GYRO_XOUT_L_ADDR  0x34U
#define ICM20649_GYRO_YOUT_H_ADDR  0x35U
#define ICM20649_GYRO_YOUT_L_ADDR  0x36U
#define ICM20649_GYRO_ZOUT_H_ADDR  0x37U
#define ICM20649_GYRO_ZOUT_L_ADDR  0x38U

#define ICM20649_DATA_RDY_STATUS_ADDR 0x74U
#define ICM20649_RAW_DATA_RDY_MASK    0x0FU

/*************************************
 * @brief USER BANK 2 REGISTERS
 *************************************/

#define ICM20649_GYRO_CONFIG_1_ADDR   0x01U
#define ICM20649_GYRO_FCHOICE_MASK    0x01U /*!< Set this bit to enable gyro DLPF */
#define ICM20649_GYRO_FS_SEL_MASK     0x06U
#define ICM20649_GYRO_FS_SEL_SET(fs)  ((fs << 1U) & ICM20649_GYRO_FS_SEL_MASK)
#define ICM20649_GYRO_DLPCFG_MASK     0x38U
#define ICM20649_GYRO_DLPCFG_SET(cfg) ((cfg << 3U) & ICM20649_GYRO_DLPCFG_MASK)

#define ICM20649_ACCEL_CONFIG_ADDR    0x14U
#define ICM20649_ACCEL_FCHOICE_MASK
#define ICM20649_ACCEL_FS_SEL_MASK
#define ICM20649_ACCEL_FS_SEL_SET
#define ICM20649_ACCEL_DLPFCFG_MASK
#define ICM20649_ACCEL_DLPFCFG_SET

#endif /* ICM20649_REGS_H */