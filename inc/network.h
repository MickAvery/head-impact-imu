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
    uint16_t                 service_handle;
    ble_gatts_char_handles_t custom_val_handles;
    ble_uuid_t               service_uuid;
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
 * @brief Process BLE CLI
 * @note  This function is meant to only be called in shell.c
 */
void network_cli_process(void);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_H */