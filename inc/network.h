/**
 * @file network.h
 * @author UBC Capstone Team 2020/2021
 * @brief Wireless network module
 */

#ifndef NETWORK_H
#define NETWORK_H

#include "retcodes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize wireless network module
 * 
 * @return sysret_t Module status
 */
sysret_t network_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NETWORK_H */