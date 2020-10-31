/**
 * @file network.c
 * @author UBC Capstone Team 2020/2021
 * @brief Wireless network module
 */

#include <string.h>
#include "network.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"

/************************************************
 * MACROS and GLOBAL OBJECTS
 * some unknown, copied from example, not sure
 * what they're for
 * 
 * TODO: maybe put these in a network_params.h file
 ************************************************/

#define DEVICE_NAME                     "SimpLab-IMU" /*!< Name of device to be included in advertising data. */

#define APP_ADV_INTERVAL                300   /*!< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                18000 /*!< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_OBSERVER_PRIO           3 /*!< Application's BLE observer priority. You shouldn't need to modify this value. (NOT SURE WHAT THIS IS FOR) */
#define APP_BLE_CONN_CFG_TAG            1 /*!< A tag identifying the SoftDevice BLE configuration (NOT SURE WHAT THIS IS FOR) */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS) /*!< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS) /*!< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                /*!< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)  /*!< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /*!< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000) /*!< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                      /*!< Number of attempts before giving up the connection parameter negotiation. */

NRF_BLE_GATT_DEF(gatt_instance);           /*!< GATT module instance */
BLE_ADVERTISING_DEF(advertising_instance); /*!< Advertising module instance */

static uint16_t conn_handle = BLE_CONN_HANDLE_INVALID; /*!< Handle of the current connection. */

/**
 * @brief Universally unique service identifiers (UUIDs)
 * 
 * Set these appropriately depending on the advertised services
 */
static ble_uuid_t advertised_uuids[] =
{
    {BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT, BLE_UUID_TYPE_BLE}
};

/************************************************
 * EVENT HANDLERS
 ************************************************/

/**
 * @notapi
 * @brief BLE events handler
 * 
 * @param p_ble_evt - Bluetooth stack event
 * @param p_context 
 */
static void ble_event_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    ret_code_t err = NRF_SUCCESS;

    switch(p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            break;

        case BLE_GAP_EVT_CONNECTED:
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };

            err = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err);
            break;
        }

        case BLE_GATTC_EVT_TIMEOUT:
            /* Disconnect on GATT Client timeout event */
            err = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            /* Disconnect on GATT Server timeout event */
            err = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                        BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err);
            break;

        default:
            break;
    }
}

/**
 * @notapi
 * @brief Connection parameters module handler
 * 
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 * 
 * @note All this function does is to disconnect. This could have been done by simply
 *       setting the disconnect_on_fail config parameter, but instead we use the event
 *       handler mechanism to demonstrate its use.
 * 
 * @param p_evt 
 */
static void on_connect_params_event(ble_conn_params_evt_t* p_evt)
{
    ret_code_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}

/**
 * @notapi
 * @brief Connection Parameters error handler
 *
 * @param nrf_error - Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief Advertising events handler
 * 
 * @details Called in advertising events which are passed to the application
 * 
 * @param ble_adv_evt - Advertising event
 */
static void on_advertising_event(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            break;
        case BLE_ADV_EVT_IDLE:
            break;
        default:
            break;
    }
}

/************************************************
 * HELPER FUNCTIONS
 ************************************************/

/**
 * @notapi
 * @brief Initialize BLE stack
 * 
 * @return retcode_t 
 */
static retcode_t ble_stack_init(void)
{
    retcode_t ret = RET_ERR;
    uint32_t ram_start = 0U;

    if((ret = nrf_sdh_enable_request()) != RET_OK)
        return ret;

    /**
     * Configure BLE stack with default settings.
     * Fetch start address of the application RAM.
     */
    if((ret = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start)) != RET_OK)
        return ret;

    /**
     * Enable BLE stack
     */
    if((ret = nrf_sdh_ble_enable(&ram_start)) != RET_OK)
        return ret;

    /**
     * Register handler for BLE events
     */
    NRF_SDH_BLE_OBSERVER(ble_observer, APP_BLE_OBSERVER_PRIO, ble_event_handler, NULL);

    return RET_OK;
}

/**
 * @notapi
 * @brief Set GAP parameters
 * 
 * @return retcode_t 
 */
static retcode_t gap_params_init(void)
{
    retcode_t ret = RET_ERR;
    ble_gap_conn_sec_mode_t sec_mode;
    ble_gap_conn_params_t   gap_conn_params;

    /**
     * Set advertised device name
     */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    ret = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t*)DEVICE_NAME, strlen(DEVICE_NAME));
    if(ret != RET_OK)
        return ret;

    /**
     * Use an appearance value matching the application's use case
     */
    ret = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT);
    if(ret != RET_OK)
        return ret;

    /**
     * Set GAP peripheral preferred connection parameters
     */
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT ;

    ret = sd_ble_gap_ppcp_set(&gap_conn_params);
    if(ret != RET_OK)
        return ret;

    return RET_OK;
}

/**
 * @notapi
 * @brief Initialize GATT library
 * 
 * @return retcode_t 
 */
static retcode_t gatt_init(void)
{
    return nrf_ble_gatt_init(&gatt_instance, NULL);
}

/**
 * @notapi
 * @brief Initialize BLE services
 * 
 * @return retcode_t 
 */
static retcode_t services_init(void)
{
    retcode_t ret = RET_ERR;

    /* TODO */
    ret = RET_OK;

    return ret;
}

/**
 * @notapi
 * @brief Initialize BLE advertising functionality
 * 
 * @return retcode_t 
 */
static retcode_t advertising_init(void)
{
    retcode_t ret = RET_ERR;
    ble_advertising_init_t init;

    (void)memset(&init, 0, sizeof(ble_advertising_init_t));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(advertised_uuids) / sizeof(advertised_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = advertised_uuids;

    init.config.ble_adv_fast_enabled     = true;
    init.config.ble_adv_fast_interval    = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout     = APP_ADV_DURATION;

    init.evt_handler = on_advertising_event;

    /* Initialize BLE advertising */
    ret = ble_advertising_init(&advertising_instance, &init);
    if(ret != RET_OK)
        return ret;

    ble_advertising_conn_cfg_tag_set(&advertising_instance, APP_BLE_CONN_CFG_TAG);

    return RET_OK;
}

/**
 * @notapi
 * @brief Set BLE connection parameters
 * 
 * @return retcode_t 
 */
static retcode_t connection_params_init(void)
{
    ble_conn_params_init_t cp_init = {
        .p_conn_params                  = NULL,
        .first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY,
        .next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY,
        .max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT,
        .start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID,
        .disconnect_on_fail             = false,
        .evt_handler                    = on_connect_params_event,
        .error_handler                  = conn_params_error_handler
    };

    return ble_conn_params_init(&cp_init);
}

/************************************************
 * API
 ************************************************/

/**
 * @brief Initialize wireless network module
 * 
 * @return retcode_t Module status
 */
retcode_t network_init(void)
{
    retcode_t ret = RET_ERR;

    /**
     * Perform all necessary initializations
     */

    if((ret = ble_stack_init()) != RET_OK)
        return ret;

    if((ret = gap_params_init()) != RET_OK)
        return ret;

    if((ret = gatt_init()) != RET_OK)
        return ret;

    if((ret = services_init()) != RET_OK)
        return ret;

    if((ret = advertising_init()) != RET_OK)
        return ret;

    if((ret = connection_params_init()) != RET_OK)
        return ret;

    if((ret = ble_advertising_start(&advertising_instance, BLE_ADV_MODE_FAST)) != RET_OK)
        return ret;

    return RET_OK;
}