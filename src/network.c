/**
 * @file network.c
 * @author UBC Capstone Team 2020/2021
 * @brief Wireless network module
 */

#include <string.h>
#include "network.h"
#include "statemachine.h"
#include "app_timer.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_cli_ble_uart.h"
#include "nrf_log.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_nus.h"

/************************************************
 * MACROS and GLOBAL OBJECTS
 * some unknown, copied from example, not sure
 * what they're for
 * 
 * TODO: maybe put these in a network_params.h file
 ************************************************/

#define DEVICE_NAME                     "SimpLab-IMU" /*!< Name of device to be included in advertising data. */

#define APP_ADV_INTERVAL                600   /*!< The advertising interval (in units of 0.625 ms. This value corresponds to 375 ms). */
#define APP_ADV_DURATION                0     /*!< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_OBSERVER_PRIO           3 /*!< Application's BLE observer priority. You shouldn't need to modify this value. (NOT SURE WHAT THIS IS FOR) */
#define APP_BLE_CONN_CFG_TAG            1 /*!< A tag identifying the SoftDevice BLE configuration (NOT SURE WHAT THIS IS FOR) */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS) /*!< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS) /*!< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                /*!< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)  /*!< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)  /*!< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000) /*!< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                      /*!< Number of attempts before giving up the connection parameter negotiation. */

BLE_NUS_DEF(nus_instance, NRF_SDH_BLE_TOTAL_LINK_COUNT); /*!< BLE NUS service instance. */
NRF_BLE_GATT_DEF(gatt_instance);                         /*!< GATT module instance */
BLE_ADVERTISING_DEF(advertising_instance);               /*!< Advertising module instance */

static uint16_t conn_handle = BLE_CONN_HANDLE_INVALID; /*!< Handle of the current connection. */

/**
 * @brief Universally unique service identifiers (UUIDs)
 * 
 * Set these appropriately depending on the advertised services
 */
static ble_uuid_t advertised_uuids[] =
{
    {BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT, BLE_UUID_TYPE_BLE},
    {BLE_UUID_NUS_SERVICE, BLE_UUID_TYPE_BLE}
};

/**
 * @brief UUIDs!
 */
/* 32A2xxxx-ED70-480B-A945-866522F66758 in reverse byte order! */
#define CUSTOM_BASE_UUID              { 0x58, 0x67, 0xF6, 0x22, 0x65, 0x86, 0x45, 0xA9, 0x0B, 0x48, 0x70, 0xED, 0x00, 0x00, 0xA2, 0x32 } /*!< Custom base UUID for custom services, generated with https://www.uuidgenerator.net/ */
/* 32A20001-ED70-480B-A945-866522F66758 */
#define CUSTOM_SERVICE_UUID           0x0001 /*!< Custom 16-bit service UUID, based on custom UUID */
/* 32A20002-ED70-480B-A945-866522F66758 */
#define CUSTOM_TX_CHARACTERISTIC_UUID 0x0002 /*!< Custom 16-bit TX characteristic UUID, based on custom UUID */
/* 32A20003-ED70-480B-A945-866522F66758 */
#define CUSTOM_RX_CHARACTERISTIC_UUID 0x0003 /*!< Custom 16-bit RX characteristic UUID, based on custom UUID */
/* 32A20004-ED70-480B-A945-866522F66758 */
#define DEVCONF_CHARACTERISTIC_UUID   0x0004 /*!< Custom 16-bit RX characteristic UUID, based on custom UUID */


/**
 * @brief Characteristic UUIDs
 */
static ble_uuid_t tx_char_uuid;
static ble_uuid_t rx_char_uuid;
static ble_uuid_t dev_conf_char_uuid;

/**
 * @brief Custom SimpL Service handler
 */
static ble_simpl_service_t simpl_service = {
    .conn_handle  = BLE_CONN_HANDLE_INVALID,
    .service_uuid = { CUSTOM_SERVICE_UUID, BLE_UUID_TYPE_VENDOR_BEGIN }
};

/**
 * BLE configuration for CLI
 */
NRF_CLI_BLE_UART_DEF(cli_ble_transport, &gatt_instance, 64, 32); /* TODO: magic numbers */
NRF_CLI_DEF(cli_ble,
            "SimPLab:~$ ",
            &cli_ble_transport.transport,
            '\n',
            16U); /* TODO: magic number, but this is log queue size */

/**
 * @brief Module states definitions
 */
typedef enum
{
    NETWORK_UNINIT = 0,            /*!< Network uninitialized */
    NETWORK_INIT,                  /*!< Module initialized but unconnected */
    NETWORK_CONNECTED,             /*!< Module connected */
    NETWORK_CONNECTED_COMM_STARTED /*!< Module connected and communicating */
} network_state_t;

/* Module state */
static network_state_t network_state = NETWORK_UNINIT;

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
        case BLE_GAP_EVT_CONNECTED:
        {
            NRF_LOG_DEBUG("BLE CONNECTED");

            /* Get connection handle */
            conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            /* Initialize Nordic UART Service (NUS) */
            // nrf_cli_ble_uart_config_t config = { .conn_handle = conn_handle };

            // err = nrf_cli_init(&cli_ble, &config,
            //                         false, /* colored prints disabled */
            //                         true,  /* CLI to be used as logger backend */
            //                         NRF_LOG_SEVERITY_DEBUG);
            // APP_ERROR_CHECK(err);

            (void)sd_ble_gatts_sys_attr_set(conn_handle, NULL, 0, 0);

            // ble_gatts_value_t test_gatts;
            // uint8_t testbuf[4] = {0xde, 0xad, 0xbe, 0xef};
            // uint16_t len = 4;

            // test_gatts.len     = 4;
            // test_gatts.offset  = 0;
            // test_gatts.p_value = testbuf;
            // sd_ble_gatts_value_set(conn_handle, simpl_service.dev_conf_char_handles.value_handle, &test_gatts);
            // sd_ble_gatts_hvx();

            // ble_gatts_hvx_params_t hvx_params =
            // {
            //     .handle = simpl_service.dev_conf_char_handles.value_handle,
            //     .offset = 0,
            //     .p_data = testbuf,
            //     .p_len  = &len,
            //     .type   = BLE_GATT_HVX_NOTIFICATION
            // };

            // uint32_t test = sd_ble_gatts_hvx(conn_handle, (const ble_gatts_hvx_params_t*)&hvx_params);
            // NRF_LOG_DEBUG("HVX - 0x%X", test);

            /* Set connection handle for custom service */
            simpl_service.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

            network_state = NETWORK_CONNECTED;
            break;
        }

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_DEBUG("BLE DISCONNECTED");

            /* Reset the connection handle */
            conn_handle = BLE_CONN_HANDLE_INVALID;

            /* Uninitialize Nordic UART Service (NUS) */
            // (void)nrf_cli_stop(&cli_ble);
            // (void)nrf_cli_uninit(&cli_ble);

            /* Reset connection handle for custom service */
            simpl_service.conn_handle = BLE_CONN_HANDLE_INVALID;

            network_state = NETWORK_INIT;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("BLE_GAP_EVT_PHY_UPDATE_REQUEST");
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

        case BLE_GATTS_EVT_WRITE:
            NRF_LOG_DEBUG(
                "BLE_GATTS_EVT_WRITE - 0x%X -- 0x%X",
                p_ble_evt->evt.gatts_evt.params.write.uuid.uuid,
                p_ble_evt->evt.gatts_evt.params.write.uuid.type);

            ble_uuid_t incoming_uuid = p_ble_evt->evt.gatts_evt.params.write.uuid;

            /* check if writing to RX characteristic */
            if( incoming_uuid.type == rx_char_uuid.type && incoming_uuid.uuid == rx_char_uuid.uuid )
            {
                /* App writes to RX characteristic */
                statemachine_ble_data_handler(
                    (uint8_t*)p_ble_evt->evt.gatts_evt.params.write.data,
                    p_ble_evt->evt.gatts_evt.params.write.len);
            }

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
static void on_connect_params_event_handler(ble_conn_params_evt_t* p_evt)
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
static void on_advertising_event_handler(ble_adv_evt_t ble_adv_evt)
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
 * @return sysret_t 
 */
static sysret_t ble_stack_init(void)
{
    sysret_t ret = RET_ERR;
    uint32_t ram_start = 0U;

    ret = nrf_sdh_enable_request();
    SYSRET_CHECK(ret);

    /**
     * Configure BLE stack with default settings.
     * Fetch start address of the application RAM.
     */
    ret = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    SYSRET_CHECK(ret);

    /**
     * Enable BLE stack
     */
    ret = nrf_sdh_ble_enable(&ram_start);
    SYSRET_CHECK(ret);

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
 * @return sysret_t 
 */
static sysret_t gap_params_init(void)
{
    sysret_t ret = RET_ERR;
    ble_gap_conn_sec_mode_t sec_mode;
    ble_gap_conn_params_t   gap_conn_params;

    /**
     * Set advertised device name
     */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    ret = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t*)DEVICE_NAME, strlen(DEVICE_NAME));
    SYSRET_CHECK(ret);

    /**
     * Use an appearance value matching the application's use case
     */
    ret = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS_ACT);
    SYSRET_CHECK(ret);

    /**
     * Set GAP peripheral preferred connection parameters
     */
    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT ;

    ret = sd_ble_gap_ppcp_set(&gap_conn_params);
    SYSRET_CHECK(ret);

    return RET_OK;
}

/**
 * @notapi
 * @brief Initialize GATT library
 * 
 * @return sysret_t 
 */
static sysret_t gatt_init(void)
{
    return nrf_ble_gatt_init(&gatt_instance, NULL);
}

/**
 * @notapi
 * @brief Initialize BLE services
 * 
 * @return sysret_t 
 */
static sysret_t services_init(void)
{
    sysret_t ret = RET_OK;

    /* setup UUID objects */
    ble_uuid128_t base_uuid = { .uuid128 = CUSTOM_BASE_UUID };
    tx_char_uuid.uuid       = CUSTOM_TX_CHARACTERISTIC_UUID;
    tx_char_uuid.type       = BLE_UUID_TYPE_VENDOR_BEGIN;
    rx_char_uuid.uuid       = CUSTOM_RX_CHARACTERISTIC_UUID;
    rx_char_uuid.type       = BLE_UUID_TYPE_VENDOR_BEGIN;
    dev_conf_char_uuid.uuid = DEVCONF_CHARACTERISTIC_UUID;
    dev_conf_char_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    /* Add custom UUIDs to BLE stack */
    ret = sd_ble_uuid_vs_add(&base_uuid, &simpl_service.service_uuid.type);
    SYSRET_CHECK(ret);

    /* Add custom service.
     * For more information, see:
     * https://devzone.nordicsemi.com/nordic/short-range-guides/b/bluetooth-low-energy/posts/ble-services-a-beginners-tutorial
     */
    ret = sd_ble_gatts_service_add(
        BLE_GATTS_SRVC_TYPE_PRIMARY,
        &simpl_service.service_uuid,
        &simpl_service.service_handle);
    SYSRET_CHECK(ret);

    /* Add characteristics to the custom service.
     * For more information, see:
     * https://devzone.nordicsemi.com/nordic/short-range-guides/b/bluetooth-low-energy/posts/ble-characteristics-a-beginners-tutorial
     */

    /* Characteristic # 1 : TX characteristic */
    ble_gatts_char_md_t tx_char_md;
    memset(&tx_char_md, 0, sizeof(tx_char_md));
    tx_char_md.char_props.notify = 1; /* allow mobile app to be notified by this characteristic */

    ble_gatts_attr_md_t tx_attr_md;
    memset(&tx_attr_md, 0, sizeof(tx_attr_md));
    tx_attr_md.vloc = BLE_GATTS_VLOC_STACK; /* attribute is to be stored in the SoftDevice stack */

    ble_gatts_attr_t    tx_char_attr_val;
    memset(&tx_char_attr_val, 0, sizeof(tx_char_attr_val));
    tx_char_attr_val.p_uuid    = &tx_char_uuid;
    tx_char_attr_val.p_attr_md = &tx_attr_md;

    ret = sd_ble_gatts_characteristic_add(simpl_service.service_handle, &tx_char_md, &tx_char_attr_val, &simpl_service.tx_char_handles);
    SYSRET_CHECK(ret);

    /* Characteristic # 2 : RX characteristic */
    ble_gatts_char_md_t rx_char_md;
    memset(&rx_char_md, 0, sizeof(rx_char_md));
    rx_char_md.char_props.write = 1; /* allow mobile app to write to this characteristic */
    // rx_char_md.char_props.write_wo_resp = 1; /* allow mobile app to write to this characteristic without response */

    ble_gatts_attr_md_t rx_attr_md;
    memset(&rx_attr_md, 0, sizeof(rx_attr_md));
    rx_attr_md.vloc = BLE_GATTS_VLOC_STACK; /* attribute is to be stored in the SoftDevice stack */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&rx_attr_md.write_perm); /* set open write permissions */

    ble_gatts_attr_t    rx_char_attr_val;
    memset(&rx_char_attr_val, 0, sizeof(rx_char_attr_val));
    rx_char_attr_val.p_uuid    = &rx_char_uuid;
    rx_char_attr_val.p_attr_md = &rx_attr_md;
    rx_char_attr_val.max_len   = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;

    ret = sd_ble_gatts_characteristic_add(simpl_service.service_handle, &rx_char_md, &rx_char_attr_val, &simpl_service.rx_char_handles);
    SYSRET_CHECK(ret);

    /* Characteristic # 3 : Device Configs characteristic */
    ble_gatts_char_md_t dev_conf_char_md;
    memset(&dev_conf_char_md, 0, sizeof(dev_conf_char_md));
    dev_conf_char_md.char_props.read   = 1;
    dev_conf_char_md.char_props.notify = 1;

    ble_gatts_attr_md_t dev_conf_attr_md;
    memset(&dev_conf_attr_md, 0, sizeof(dev_conf_attr_md));
    dev_conf_attr_md.vloc = BLE_GATTS_VLOC_STACK; /* attribute is to be stored in the SoftDevice stack */
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dev_conf_attr_md.read_perm);

    ble_gatts_attr_t dev_conf_attr_val;
    memset(&dev_conf_attr_val, 0, sizeof(dev_conf_attr_val));
    dev_conf_attr_val.p_uuid    = &dev_conf_char_uuid;
    dev_conf_attr_val.p_attr_md = &dev_conf_attr_md;
    dev_conf_attr_val.max_len   = NRF_SDH_BLE_GATT_MAX_MTU_SIZE;

    ret = sd_ble_gatts_characteristic_add(simpl_service.service_handle, &dev_conf_char_md, &dev_conf_attr_val, &simpl_service.dev_conf_char_handles);
    SYSRET_CHECK(ret);

    return nrf_cli_ble_uart_service_init();
}

/**
 * @notapi
 * @brief Initialize BLE advertising functionality
 * 
 * @return sysret_t 
 */
static sysret_t advertising_init(void)
{
    sysret_t ret = RET_ERR;
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

    init.evt_handler = on_advertising_event_handler;

    /* Initialize BLE advertising */
    ret = ble_advertising_init(&advertising_instance, &init);
    SYSRET_CHECK(ret);

    ble_advertising_conn_cfg_tag_set(&advertising_instance, APP_BLE_CONN_CFG_TAG);

    return RET_OK;
}

/**
 * @notapi
 * @brief Set BLE connection parameters
 * 
 * @return sysret_t 
 */
static sysret_t connection_params_init(void)
{
    ble_conn_params_init_t cp_init = {
        .p_conn_params                  = NULL,
        .first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY,
        .next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY,
        .max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT,
        .start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID,
        .disconnect_on_fail             = false,
        .evt_handler                    = on_connect_params_event_handler,
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
 * @return sysret_t Module status
 */
sysret_t network_init(void)
{
    sysret_t ret = RET_ERR;

    /**
     * Perform all necessary initializations
     */

    ret = ble_stack_init();
    SYSRET_CHECK(ret);

    ret = gap_params_init();
    SYSRET_CHECK(ret);

    ret = gatt_init();
    SYSRET_CHECK(ret);

    ret = services_init();
    SYSRET_CHECK(ret);

    ret = advertising_init();
    SYSRET_CHECK(ret);

    ret = connection_params_init();
    SYSRET_CHECK(ret);

    ret = ble_advertising_start(&advertising_instance, BLE_ADV_MODE_FAST);
    SYSRET_CHECK(ret);

    network_state = NETWORK_INIT;
    return RET_OK;
}

/**
 * On a WRITE REQUEST WITH RESPONSE from the app to the Device Configurations Characteristic,
 * this function sends the response back to the app
 * 
 * @param buf - Response bytes
 * @param len - Response length
 * @return sysret_t Driver status
 */
sysret_t network_set_dev_conf_char_response(uint8_t* buf, uint16_t* len)
{
    ble_gatts_hvx_params_t hvx_params =
    {
        .handle = simpl_service.dev_conf_char_handles.value_handle,
        .offset = 0,
        .p_data = buf,
        .p_len  = len,
        .type   = BLE_GATT_HVX_NOTIFICATION
    };

    return sd_ble_gatts_hvx(conn_handle, (const ble_gatts_hvx_params_t*)&hvx_params);
}

/**
 * @brief Process BLE CLI
 * @note  This function is meant to only be called in shell.c
 */
void network_cli_process(void)
{
    switch(network_state)
    {
        case NETWORK_CONNECTED:
            if(cli_ble_transport.p_cb->service_started)
            {
                // nrf_cli_start(&cli_ble);
                network_state = NETWORK_CONNECTED_COMM_STARTED;
            }

            break;

        case NETWORK_CONNECTED_COMM_STARTED:
            // nrf_cli_process(&cli_ble);
            break;

        default:
            break;
    }
}