/**
 * @file statemachine.c
 * @author UBC Capstone Team 2020/2021
 * @brief State machine to define device behaviour
 */

#include "nrf_delay.h"
#include "statemachine.h"
#include "datetime.h"
#include "network.h"
#include "configs.h"
#include "mt25q.h"
#include "app_timer.h"
#include "nrf_log.h"

/**
 * @brief Request codes from app
 */
typedef enum
{
    REQ_GET_ALL_INFO = 0,
    REQ_SET_CONFIGS,
    REQ_SET_DATETIME,
    REQ_START_DATALOG,
    REQ_STOP_DATALOG,
    REQ_LOG_DOWNLOAD
} requests_t;

/**
 * @brief Device state and configurations, values below are default values
 */
static statemachine_t state_machine =
{
    .log_download_requested = false,
    .state = STATE_UNINIT
};

/**************************************
 * Variables and configurations related
 * to the datalog timer
 **************************************/

/**
 * @brief datalog timer handle
 */
APP_TIMER_DEF(datalog_timer);

/**
 * @brief Set by @ref datalog_timer_handler() on timer alarm
 */
static volatile bool its_time_to_log_data = false;

/**
 * @notapi
 * @brief Signify to state machine that it's time for another
 *        round of datalogging
 */
static void datalog_timer_handler(void* p_ctx)
{
    (void)p_ctx;
    its_time_to_log_data = true;
}

/**************************************
 * API
 **************************************/

/**
 * @brief Initialize system state machine module
 * 
 * @return sysret_t Module status
 */
sysret_t statemachine_init(void)
{
    NRF_LOG_DEBUG("STATE = INIT");
    state_machine.state = STATE_INIT;
    return RET_OK;
}

/**
 * @brief Return current system state
 * 
 * @return statemachine_states_t Current state
 */
statemachine_states_t statemachine_getstate(void)
{
    return state_machine.state;
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

    uint16_t len = 0U;
    uint8_t request = data[0];
    sysret_t ret = RET_ERR;

    NRF_LOG_DEBUG("request = %d | size = %d", request, size);

    switch( request )
    {
        case REQ_GET_ALL_INFO:
            NRF_LOG_DEBUG("REQ_GET_ALL_INFO");
            len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE - 3; // TODO: make this into a better macro
            network_set_dev_conf_char_response(GLOBAL_CONFIGS.configs_bytes, &len);
            break;

        case REQ_SET_CONFIGS:
            NRF_LOG_DEBUG("REQ_SET_CONFIGS", size);

            /* TODO: make sure configs can only be set when IDLE */

            /* set header */
            GLOBAL_CONFIGS.device_configs.header = CONFIGS_FRAME_HEADER;

            /* save configurations */
            memcpy(
                GLOBAL_CONFIGS.configs_bytes + sizeof(CONFIGS_FRAME_HEADER) + 1,
                &data[1],
                size-1
            );

            /* save configurations */
            ret = configs_save(&GLOBAL_CONFIGS);
            NRF_LOG_DEBUG("CONFIGS_SAVE = %d", ret);

            /* update characteristic attribute */
            len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;
            ret = network_set_dev_conf_char_response(GLOBAL_CONFIGS.configs_bytes, &len);
            NRF_LOG_DEBUG("ATT_UPDATE = %d", ret);

            break;

        case REQ_SET_DATETIME:
            NRF_LOG_DEBUG("REQ_SET_DATETIME");

            // TODO: maybe make datetime_t a union?
            datetime_t dt;
            memcpy(&dt.year, &data[1], sizeof(dt.year));
            dt.month = data[3];
            dt.day   = data[4];
            dt.hr    = data[5];
            dt.min   = data[6];
            dt.sec   = data[7];
            memcpy(&dt.usec, &data[8], sizeof(dt.usec));

            datetime_reset();
            datetime_set(&dt);

            break;

        case REQ_START_DATALOG:
            NRF_LOG_DEBUG("REQ_START_DATALOG");

            GLOBAL_CONFIGS.device_configs.datalog_en = true;

            break;

        case REQ_STOP_DATALOG:
            NRF_LOG_DEBUG("REQ_STOP_DATALOG");

            GLOBAL_CONFIGS.device_configs.datalog_en = false;

            break;

        case REQ_LOG_DOWNLOAD:
            NRF_LOG_DEBUG("REQ_LOG_DOWNLOAD");

            state_machine.log_download_requested = true;

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
    sysret_t ret;

    switch( state_machine.state )
    {
        case STATE_INIT:
            NRF_LOG_DEBUG("INIT -> IDLE");

            /* read existing configs from memory */
            ret = configs_get(&GLOBAL_CONFIGS);

            if(ret == RET_OK)
            {
                NRF_LOG_DEBUG("EXISTING METADATA IN FLASH");
            }
            else if(ret == RET_ERR)
            {
                NRF_LOG_DEBUG("NO EXISTING METADATA IN FLASH");
            }
            else
            {
                NRF_LOG_DEBUG("FAILED TO READ CONFIGS - %d", ret);
            }

            (void)app_timer_create(
                &datalog_timer,
                APP_TIMER_MODE_REPEATED,
                datalog_timer_handler
            );

            GLOBAL_CONFIGS.device_configs.datalog_en = false;

            state_machine.state = STATE_IDLE;
            break;

        case STATE_IDLE:

            if(GLOBAL_CONFIGS.device_configs.datalog_en)
            {
                NRF_LOG_DEBUG("IDLE -> WAIT_FOR_TRIGGER");
                state_machine.state = STATE_WAIT_FOR_TRIGGER;
            }
            else if(state_machine.log_download_requested)
            {
                NRF_LOG_DEBUG("IDLE -> FILE_TRANSFER");
                state_machine.state = STATE_FILE_TRANSFER;
            }

            break;

        case STATE_WAIT_FOR_TRIGGER:

            if(!GLOBAL_CONFIGS.device_configs.datalog_en)
            {
                NRF_LOG_DEBUG("WAIT_FOR_TRIGGER -> IDLE");
                state_machine.state = STATE_IDLE;
            }
            else if(GLOBAL_CONFIGS.device_configs.datalog_mode == CONFIGS_DATALOG_MODE_CONTINUOUS)
            {
                NRF_LOG_DEBUG("WAIT_FOR_TRIGGER -> DATALOGGING");

                /* start timer */
                (void)app_timer_start(
                    datalog_timer,
                    APP_TIMER_TICKS(1000), // TODO: get this value programatically
                    NULL
                );

                state_machine.state = STATE_DATALOGGING;
            }

            break;

        case STATE_DATALOGGING:

            if(!GLOBAL_CONFIGS.device_configs.datalog_en)
            {
                NRF_LOG_DEBUG("DATALOGGING -> WAIT_FOR_TRIGGER");
                (void)app_timer_stop(datalog_timer);
                state_machine.state = STATE_WAIT_FOR_TRIGGER;
            }

            if(its_time_to_log_data)
            {
                NRF_LOG_DEBUG(".");
                its_time_to_log_data = false;
            }

            break;

        case STATE_LOW_POWER:
            /* TODO */
            break;

        case STATE_FILE_TRANSFER:
        {
            uint16_t len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE - 3; // TODO: make this into a better macro
            uint8_t tx[NRF_SDH_BLE_GATT_MAX_MTU_SIZE - 3] = {0xde, 0xad, 0xbe, 0xef};
            size_t failed = 0;
            size_t success = 0;
            sysret_t ret;

            for(int i = 0 ; i < 256 ; )
            {
                ret = network_transmit_file_packet(tx, len);
                if(ret == RET_OK)
                {
                    i++;
                    success++;
                }
                else
                {
                    failed++;
                }
            }

            NRF_LOG_DEBUG("filetx = 0x%X | success = %d | failed = %d", ret, success, failed);

            NRF_LOG_DEBUG("FILE_TRANSFER -> IDLE");
            state_machine.log_download_requested = false;
            state_machine.state = STATE_IDLE;
            break;
        }

        case STATE_FW_UPDATE:
            /* TODO */
            break;

        default:
            break;
    }
}