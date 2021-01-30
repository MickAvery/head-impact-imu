/**
 * @file network.h
 * @author UBC Capstone Team 2020/2021
 * @brief Wireless network module
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "retcodes.h"
#include "ble_gatts.h"

/**
 * @brief BLE custom service definition (name = SimpL Service)
 */
typedef struct
{
    uint16_t                 conn_handle;     /*!< Connection handle upon device connection */
    uint16_t                 service_handle;  /*!< Service handle */
    ble_uuid_t               service_uuid;    /*!< Service UUID */
    ble_gatts_char_handles_t tx_char_handles; /*!< Characteristic handle to be refered to later on */
    ble_gatts_char_handles_t rx_char_handles; /*!< Characteristic handle to be refered to later on */
    ble_gatts_char_handles_t dev_conf_char_handles; /*!< Characteristic handle to be refered to later on */
} ble_simpl_service_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize wireless network module
 * 
 * @return sysret_t Module status
 */
sysret_t network_init(void);

/**
 * On a WRITE REQUEST WITH RESPONSE from the app to the RX characteristic,
 * this function sends the response back to the app
 * 
 * @param buf - Response bytes
 * @param len - Response length
 */
void network_send_response(uint8_t* buf, uint16_t* len);

/**
 * @brief Process BLE CLI
 * @note  This function is meant to only be called in shell.c
 */
void network_cli_process(void);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_H */