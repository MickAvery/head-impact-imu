/**
 * @file statemachine.c
 * @author UBC Capstone Team 2020/2021
 * @brief State machine to define device behaviour
 */

#include "statemachine.h"
#include "network.h"
#include "nrf_log.h"

/**
 * @brief Request codes from app
 */
typedef enum
{
    REQ_GET_ALL_INFO = 0,
    REQ_SYNC_DATETIME,
    REQ_UPDATE_CONFIGS
} requests_t;

/**
 * @brief Device state and configurations, values below are default values
 */
static dev_config_t dev_state =
{
    .datalog_en        = false,
    .datalog_mode      = DATALOG_TRIGGER,
    .trigger_on        = TRIGGER_ON_LIN_ACCELERATION,
    .trigger_threshold = 0x7FFFFFFF,
    .sample_rate       = SAMPLE_RATE_100_HZ,
    .accel_fs          = ICM20649_ACCEL_FS_30g,
    .gyro_fs           = ICM20649_GYRO_FS_4000dps,
    .state             = STATE_UNINIT
};

/**
 * @brief Initialize system state machine module
 * 
 * @return sysret_t Module status
 */
sysret_t statemachine_init(void)
{
    NRF_LOG_DEBUG("STATE = INIT");
    dev_state.state = STATE_INIT;
    return RET_OK;
}

/**
 * @brief Return current system state
 * 
 * @return statemachine_states_t Current state
 */
statemachine_states_t statemachine_getstate(void)
{
    return dev_state.state;
}

/**
 * @brief Set/Stop datalogging
 * 
 * @param set - If true, datalogging will start. If false, it will stop
 */
void statemachine_set_datalogging(bool set)
{
    dev_state.datalog_en = set;
}

/**
 * @brief Handles incoming bytes coming from mobile app
 * 
 * @param data - received data
 * @param size - number of data bytes received
 */
void statemachine_ble_data_handler(uint8_t* data, size_t size)
{
    ASSERT(data != NULL);
    ASSERT(size > 0);

    uint8_t request = data[0];

    switch( request )
    {
        case REQ_GET_ALL_INFO:
            NRF_LOG_DEBUG("REQ_GET_ALL_INFO");
            uint8_t  resp[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
            uint16_t len     = 4;
            network_send_response(resp, &len);
            break;

        case REQ_SYNC_DATETIME:
            break;

        case REQ_UPDATE_CONFIGS:
            break;

        default:
            break;
    }
}

/**
 * @brief State machine iteration, meant to be called in the main loop
 */
void statemachine_process(void)
{
    switch( dev_state.state )
    {
        case STATE_INIT:
            NRF_LOG_DEBUG("INIT -> IDLE");
            dev_state.state = STATE_IDLE;
            break;

        case STATE_IDLE:

            if(dev_state.datalog_en)
            {
                NRF_LOG_DEBUG("IDLE -> WAIT_FOR_TRIGGER");
                dev_state.state = STATE_WAIT_FOR_TRIGGER;
            }

            break;

        case STATE_WAIT_FOR_TRIGGER:

            if(!dev_state.datalog_en)
            {
                NRF_LOG_DEBUG("WAIT_FOR_TRIGGER -> IDLE");
                dev_state.state = STATE_IDLE;
            }

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