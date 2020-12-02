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
    STATE_UNINIT = 0, /*!< State machine uninitialized */
    STATE_INIT,       /*!< State machine initialization, transitions to IDLE after */
    STATE_IDLE        /*!< Idle state */
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