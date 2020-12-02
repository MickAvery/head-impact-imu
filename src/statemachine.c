/**
 * @file statemachine.c
 * @author UBC Capstone Team 2020/2021
 * @brief State machine to define device behaviour
 */

#include "statemachine.h"
#include "nrf_log.h"

/**
 * @brief Keep track of system state
 */
static statemachine_states_t state = STATE_UNINIT;

/**
 * @brief Initialize system state machine module
 * 
 * @return sysret_t Module status
 */
sysret_t statemachine_init(void)
{
    NRF_LOG_DEBUG("STATE = INIT");
    state = STATE_INIT;
    return RET_OK;
}

/**
 * @brief Return current system state
 * 
 * @return statemachine_states_t Current state
 */
statemachine_states_t statemachine_getstate(void)
{
    return state;
}

/**
 * @brief State machine iteration, meant to be called in the main loop
 */
void statemachine_process(void)
{
    switch( state )
    {
        case STATE_INIT:
            NRF_LOG_DEBUG("INIT -> IDLE");
            state = STATE_IDLE;
            break;

        case STATE_IDLE:
            /* TODO */
            break;

        case STATE_WAIT_FOR_TRIGGER:
            /* TODO */
            break;

        case STATE_DATALOGGING:
            /* TODO */
            break;

        case STATE_LOW_POWER:
            /* TODO */
            break;

        case STATE_FILE_TRANSFER:
            /* TODO */
            break;

        case STATE_FW_UPDATE:
            /* TODO */
            break;

        default:
            break;
    }
}