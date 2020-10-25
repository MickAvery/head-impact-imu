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
#define ICM20649_REG_BANK_SEL_ADDR       0x7FU /*!< User Bank Select Address */

/**
 * @brief Values to write to REG_BANK_SEL
 */
typedef enum
{
    ICM20649_USR_BANK_0 = 0x00U, /*!< USR Bank 0 */
    ICM20649_USR_BANK_1 = 0x10U, /*!< USR Bank 1 */
    ICM20649_USR_BANK_2 = 0x20U, /*!< USR Bank 2 */
    ICM20649_USR_BANK_3 = 0x30U, /*!< USR Bank 3 */
    ICM20649_USR_BANK_MAX
} icm20649_usr_bank_t;

/*************************************
 * @brief USER BANK 0 REGISTERS
 *************************************/

#define ICM20649_WHO_AM_I_ADDR        0x00U /*!< WHO_AM_I register address */
#define ICM20649_WHO_AM_I_VAL         0xE1U /*!< WHO_AM_I expected value */

#define ICM20649_USER_CTRL_ADDR       0x03U /*!< USER_CTRL register address */

#define ICM20649_LP_CONFIG_ADDR       0x05U /*!< LP_CONFIG register address */

#define ICM20649_PWR_MGMT_1_ADDR      0x06U /*!< PWR_MGMT_1 register address */
#define ICM20649_CLKSEL_MASK          0x07U
#define ICM20649_CLKSEL_SET(clk)      (clk & ICM20649_CLKSEL_MASK)
#define ICM20649_DEVICE_RESET_MASK    0x80U

#define ICM20649_PWR_MGMT_2_ADDR      0x07U /*!< PWM_MGMT_2 register address */

#define ICM20649_ACCEL_XOUT_H_ADDR    0x2DU /*!< ACCEL_XOUT_H register address */
#define ICM20649_ACCEL_XOUT_L_ADDR    0x2EU /*!< ACCEL_XOUT_L register address */
#define ICM20649_ACCEL_YOUT_H_ADDR    0x2FU /*!< ACCEL_YOUT_H register address */
#define ICM20649_ACCEL_YOUT_L_ADDR    0x30U /*!< ACCEL_YOUT_L register address */
#define ICM20649_ACCEL_ZOUT_H_ADDR    0x31U /*!< ACCEL_ZOUT_H register address */
#define ICM20649_ACCEL_ZOUT_L_ADDR    0x32U /*!< ACCEL_ZOUT_L register address */

#define ICM20649_GYRO_XOUT_H_ADDR     0x33U /*!< GYRO_XOUT_H register address */
#define ICM20649_GYRO_XOUT_L_ADDR     0x34U /*!< GYRO_XOUT_L register address */
#define ICM20649_GYRO_YOUT_H_ADDR     0x35U /*!< GYRO_YOUT_H register address */
#define ICM20649_GYRO_YOUT_L_ADDR     0x36U /*!< GYRO_YOUT_L register address */
#define ICM20649_GYRO_ZOUT_H_ADDR     0x37U /*!< GYRO_ZOUT_H register address */
#define ICM20649_GYRO_ZOUT_L_ADDR     0x38U /*!< GYRO_ZOUT_L register address */

#define ICM20649_DATA_RDY_STATUS_ADDR  0x74U /*!< DATA_RDY_STATUS register address */
#define ICM20649_DATA_RDY_MASK         0x0FU
#define ICM20649_DATA_RDY_MASK_NO_FIFO 0x0EU

/*************************************
 * @brief USER BANK 2 REGISTERS
 *************************************/

#define ICM20649_GYRO_CONFIG_1_ADDR   0x01U /*!< GYRO_CONFIG_1 register address */
#define ICM20649_GYRO_FCHOICE_MASK    0x01U /*!< Set this bit to enable gyro DLPF */
#define ICM20649_GYRO_FS_SEL_MASK     0x06U
#define ICM20649_GYRO_FS_SEL_SET(fs)  ((fs << 1U) & ICM20649_GYRO_FS_SEL_MASK)
#define ICM20649_GYRO_DLPCFG_MASK     0x38U
#define ICM20649_GYRO_DLPCFG_SET(cfg) ((cfg << 3U) & ICM20649_GYRO_DLPCFG_MASK)

#define ICM20649_ACCEL_CONFIG_ADDR      0x14U /*!< ACCEL_CONFIG register address */
#define ICM20649_ACCEL_FCHOICE_MASK     0x01U
#define ICM20649_ACCEL_FS_SEL_MASK      0x06U
#define ICM20649_ACCEL_FS_SEL_SET(fs)   ((fs << 1U) & ICM20649_ACCEL_FS_SEL_MASK)
#define ICM20649_ACCEL_DLPFCFG_MASK     0x38U
#define ICM20649_ACCEL_DLPFCFG_SET(cfg) ((cfg << 3U) & ICM20649_ACCEL_DLPFCFG_MASK)

#endif /* ICM20649_REGS_H */