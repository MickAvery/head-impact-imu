/**
 * @file statemachine.c
 * @author UBC Capstone Team 2020/2021
 * @brief State machine to define device behaviour
 */

#include "statemachine.h"
#include "datetime.h"
#include "network.h"
#include "mt25q.h"
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
    REQ_STOP_DATALOG
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
 * @brief If this preamble exists as first few bytes in flash memory, then
 *        metadata exists for device
 */
static uint32_t preamble_word = 0xDEADBEEF;

/**
 * @brief Size of metadata in flash,
 *        ideally occupies entire first page in flash
 */
#define METADATA_SIZE (FLASH_PAGE_SIZE)

/**
 * @brief 
 */
union
{
    struct __attribute__((__packed__)) device_configs
    {
        uint32_t preamble;
        uint8_t  datalog_mode;
        uint8_t  trigger_on;
        uint8_t  trigger_axis;
        int16_t  threshold_resultant;
        int16_t  threshold_x;
        int16_t  threshold_y;
        int16_t  threshold_z;
        uint8_t  gyro_sampling_rate;
        uint8_t  low_g_sampling_rate;
        uint8_t  high_g_sampling_rate;
    } device_configs;
    uint8_t  metadata_bytes[METADATA_SIZE];
} metadata;

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

    uint16_t len = 0U;
    uint8_t request = data[0];
    sysret_t ret = RET_ERR;

    NRF_LOG_DEBUG("request = %d | size = %d", request, size);

    switch( request )
    {
        case REQ_GET_ALL_INFO:
            NRF_LOG_DEBUG("REQ_GET_ALL_INFO");
            len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;
            network_send_response(metadata.metadata_bytes, &len);
            break;

        case REQ_SET_CONFIGS:
            NRF_LOG_DEBUG("REQ_SET_CONFIGS", size);

            /* set preamble */
            metadata.device_configs.preamble = preamble_word;

            /* extract configurations */
            // TODO: maybe memcpy to metadata_bytes???
            metadata.device_configs.datalog_mode = data[1];
            metadata.device_configs.trigger_on   = data[2];
            metadata.device_configs.trigger_axis = data[3];

            memcpy(&metadata.device_configs.threshold_resultant, &data[4], sizeof(uint16_t));
            memcpy(&metadata.device_configs.threshold_x, &data[6], sizeof(uint16_t));
            memcpy(&metadata.device_configs.threshold_y, &data[8], sizeof(uint16_t));
            memcpy(&metadata.device_configs.threshold_z, &data[10], sizeof(uint16_t));

            metadata.device_configs.gyro_sampling_rate   = data[12];
            metadata.device_configs.low_g_sampling_rate  = data[13];
            metadata.device_configs.high_g_sampling_rate = data[14];

            /* erase first subsector in flash */
            ret = mt25q_4kB_subsector_erase(0);
            NRF_LOG_DEBUG("FLASH_ERASE = %d", ret);
            /* save configs to flash */
            ret = mt25q_page_program(0, metadata.metadata_bytes, METADATA_SIZE);
            NRF_LOG_DEBUG("FLASH_WRITE = %d", ret);

            /* update characteristic attribute */
            len = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;
            (void)network_set_dev_conf_char_response(metadata.metadata_bytes, &len);

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

            /* readf metadata from flash */
            mt25q_read(0, metadata.metadata_bytes, METADATA_SIZE);

            if(metadata.device_configs.preamble == preamble_word)
            {
                NRF_LOG_DEBUG("EXISTING METADATA IN FLASH");
            }
            else
            {
                NRF_LOG_DEBUG("NO EXISTING METADATA IN FLASH");
            }

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