/**
 * @file statemachine.h
 * @author UBC Capstone Team 2020/2021
 * @brief State machine to define device behaviour
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "retcodes.h"

/**
 * @brief State machine states
 */
typedef enum
{
    STATE_UNINIT = 0,       /*!< State machine uninitialized */
    STATE_INIT,             /*!< State machine initialization, transitions to IDLE after */
    STATE_IDLE,             /*!< Idle state */
    STATE_WAIT_FOR_TRIGGER, /*!< Waiting for trigger */
    STATE_DATALOGGING,      /*!< Device is logging data */
    STATE_LOW_POWER,        /*!< Device is in low power mode, some ICs are powered down */

    /* states related to file-streaming */
    STATE_FILE_TRANSFER, /*!< Transfer file from device to app */
    STATE_FW_UPDATE,     /*!< Receive updated firmware from app */

    STATES_NUM /*!< Max number of states */
} statemachine_states_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize system state machine module
 * 
 * @return sysret_t Module status
 */
sysret_t statemachine_init(void);

/**
 * @brief Return current system state
 * 
 * @return statemachine_states_t Current state
 */
statemachine_states_t statemachine_getstate(void);

/**
 * @brief State machine iteration, meant to be called in the main loop
 */
void statemachine_process(void);

#ifdef __cplusplus
}
#endif

#endif /* STATE_MACHINE_H */