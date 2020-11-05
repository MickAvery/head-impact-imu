/**
 * @file mt25q_reg.h
 * @author UBC Capstone Team 2020/2021
 * @brief MT25Q flash storage registers and command definitions
 */

#ifndef MT25Q_REGS_H
#define MT25Q_REGS_H

#define MT25Q_READ_ID_CMD              0x9EU /*!< Command to read device ID data table */
#define MT25Q_DEV_ID_NUMBYTES          3U    /*!< Number of bytes to read from device ID data table */
#define MT25Q_MANUFACTURER_ID_EXPECTED 0x20U /*!< Expected manufacturer ID from data table */
#define MT25Q_MEM_TYPE_EXPECTED        0xBAU /*!< Expected memory type from data table */
#define MT25Q_MEM_CAP_EXPECTED         0x19U /*!< Expected memory capacity from data table */

#define MT25Q_READ_STATUS_REG_CMD      0x05U /*!< Command to read status register */
#define MT25Q_WRITE_IN_PROGRESS        0x01U /*!< Write in progress flag in status register */

#define MT25Q_READ_FLAG_STATUS_REG_CMD 0x70U /*!< Command to read flag status register */
#define MT25Q_ERASE_SUCCESSFUL         0x10U /*!< If this bit is clear, ERASE operation successful */

#define MT25Q_WRITE_ENABLE_CMD         0x06U /*!< Command to set write enable latch */
#define MT25Q_WRITE_DISABLE_CMD        0x04U /*!< Command to clear write enable latch */

#define MT25Q_PAGE_PROG_CMD

#define MT25Q_BULK_ERASE_CMD           0xC7U /*!< Command to perfrom BULK ERASE */

#define MT25Q_ENTER_4B_ADDR_CMD        0xB7U /*!< Command to enter 4-Byte address mode */
#define MT25Q_EXIT_4B_ADDR_CMD         0xE9U /*!< Command to exit 4-Byte address mode */

#endif /* MT25Q_REGS_H */