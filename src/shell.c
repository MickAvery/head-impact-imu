/**
 * @file shell.c
 * @author UBC Capstone Team 2020/2021
 * @brief Shell interface for user commands
 */

#include <string.h>
#include "shell.h"
#include "custom_board.h"
#include "nrf.h"
#include "nrf_cli.h"
#include "nrf_cli_rtt.h"
#include "nrf_cli_uart.h"
#include "nrf_delay.h"
#include "datetime.h"
#include "adxl372.h"
#include "icm20649.h"
#include "vcnl4040.h"
#include "mt25q.h"

/**
 * @brief Default delay between sensor stream readouts in ms
 * 
 */
static uint32_t default_stream_delay_ms = 0U;

/**
 * @brief The number of arguments that the datetime_set() command expects:
 *        YYYY, MM, DD, HH, MM, SS, sss
 */
typedef enum
{
    SHELL_SET_CMD = 0, /*!< By default, the first string in argv is always the command string */
    SHELL_YEAR,
    SHELL_MONTH,
    SHELL_DAY,
    SHELL_HOUR,
    SHELL_MIN,
    SHELL_SEC,
    SHELL_USEC,
    SHELL_DATETIME_NUMARGS
} shell_dt_args_t;

/**
 * UART configurations for CLI
 */
static nrf_drv_uart_config_t uart_cfg = NRF_DRV_UART_DEFAULT_CONFIG;

NRF_CLI_UART_DEF(cli_uart_transport, 0, 64, 64); /* TODO: magic numbers */
NRF_CLI_DEF(cli_uart,
            "SimPLab:~$ ",
            &cli_uart_transport.transport,
            '\n',
            16U); /* TODO: magic number, but this is log queue size */

/**
 * RTT configurations for CLI
 */
NRF_CLI_RTT_DEF(cli_rtt_transport);
NRF_CLI_DEF(cli_rtt,
            "SimPLab:~$ ",
            &cli_rtt_transport.transport,
            '\n',
            16U); /* TODO: magic number, but this is log queue size */

/**
 * @notapi
 * @brief The mother of all C programs 
 */
static void hello_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "\n"
        "************\n"
        "Hello World!\n"
        "************\n"
        "\n");
}

/**
 * @notapi
 * @brief Set system datetime
 */
static void datetime_set_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    /* Printing help if needed */
    if ((argc < SHELL_DATETIME_NUMARGS) || nrf_cli_help_requested(p_cli))
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        datetime_t dt = {
            (uint16_t)atoi(argv[SHELL_YEAR]),
            (uint8_t)atoi(argv[SHELL_MONTH]),
            (uint8_t)atoi(argv[SHELL_DAY]),
            (uint8_t)atoi(argv[SHELL_HOUR]),
            (uint8_t)atoi(argv[SHELL_MIN]),
            (uint8_t)atoi(argv[SHELL_SEC]),
            (uint32_t)atoi(argv[SHELL_USEC])
        };

        datetime_set(&dt);
    }
}

/**
 * @notapi
 * @brief Get system datetime
 */
static void datetime_get_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    datetime_t dt;

    if(datetime_get(&dt) == DATETIME_OK) {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "%04d-%02d-%02d %02d:%02d:%02d.%06d\n",
            dt.year, dt.month, dt.day, dt.hr, dt.min, dt.sec, dt.usec);
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "\nUnable to get datetime information\n");
    }
}

/**
 * @notapi
 * @brief Calibrate ADXL372
 */
static void adxl372_calibrate_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    adxl372_err_t ret = adxl372_calibrate(NULL);

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "%s\n",
        ret == ADXL372_ERR_OK ? "ADXL372 sensor calibrated" : "ADXL372 sensor calibration failed");
}

/**
 * @notapi
 * @brief Continuously print out ADXL372 sensor readings
 */
static void adxl372_stream_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    uint8_t rx = 0U;
    uint8_t ctrl_c = 0x03U;
    uint32_t delay = default_stream_delay_ms;

    /* determine if user specified custom delay */
    if(argc > 1)
        delay = (uint32_t)atoi(argv[1]);

    while(rx != ctrl_c)
    {
        adxl372_val_raw_t readings[ADXL372_AXES] = {0U};
        datetime_t dt;

        (void)datetime_get(&dt);
        (void)adxl372_read_raw(readings);

        /* print datetime */
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "%02d:%02d:%02d.%03d\t",
            dt.hr, dt.min, dt.sec, dt.usec/1000U);

        /* print sensor values */
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "%d\t%d\t%d\n",
            readings[ADXL372_X]*100, readings[ADXL372_Y]*100, readings[ADXL372_Z]*100);

        if(delay > 0)
            nrf_delay_ms(delay);

        nrf_drv_uart_rx(&cli_uart_transport_uart, &rx, 1U);
    }
}

/**
 * @notapi
 * @brief Continuously print out ICM20649 sensor readings
 */
static void icm20649_stream_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    uint8_t rx = 0U;
    uint8_t ctrl_c = 0x03U;
    uint32_t delay = default_stream_delay_ms;

    /* determine if user specified custom delay */
    if(argc > 1)
        delay = (uint32_t)atoi(argv[1]);

    while(rx != ctrl_c)
    {
        int16_t accel[ICM20649_ACCEL_AXES] = {0};
        int16_t gyro[ICM20649_GYRO_AXES] = {0};
        datetime_t dt;

        sysret_t dt_ret = datetime_get(&dt);
        sysret_t sensor_ret = icm20649_read_raw(gyro, accel);

        if(dt_ret == RET_OK && sensor_ret == RET_OK)
        {
            /* print datetime */
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "%02d:%02d:%02d.%03d\t",
                dt.hr, dt.min, dt.sec, dt.usec/1000U);

            /* print sensor values */
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "%d\t%d\t%d\t%d\t%d\t%d\n",
                accel[ICM20649_ACCEL_X], accel[ICM20649_ACCEL_Y], accel[ICM20649_ACCEL_Z],
                gyro[ICM20649_GYRO_X], gyro[ICM20649_GYRO_Y], gyro[ICM20649_GYRO_Z]);
        }
        else
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "Error : datetime [%s] sensor [%s]\n",
                retcodes_desc[dt_ret], retcodes_desc[sensor_ret]);

        if(delay > 0)
            nrf_delay_ms(delay);

        nrf_drv_uart_rx(&cli_uart_transport_uart, &rx, 1U);
    }
}

/**
 * @notapi
 * @brief Continuously print out ICM20649 sensor readings
 */
static void vcnl4040_stream_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    uint8_t rx = 0U;
    uint8_t ctrl_c = 0x03U;
    uint32_t delay = default_stream_delay_ms;

    /* determine if user specified custom delay */
    if(argc > 1)
        delay = (uint32_t)atoi(argv[1]);

    while(rx != ctrl_c)
    {
        sysret_t ret;
        vcnl4040_data_t data;
        ret = vcnl4040_read(&data);

        if(ret == RET_OK)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "%d\n", data);
        }
        else
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "error!\n");

        if(delay > 0)
            nrf_delay_ms(delay);

        nrf_drv_uart_rx(&cli_uart_transport_uart, &rx, 1U);
    }
}

/**
 * @notapi
 * @brief Erase entire contents of external flash storage 
 */
static void storage_erase_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Storage erase in progress...\n");
    sysret_t ret = mt25q_bulk_erase();
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Storage erase status - [%s]\n", retcodes_desc[ret]);
}

/**
 * @notapi
 * @brief Storage test - write to every page in external flash storage, read back and verify written value 
 */
static void storage_pp_test_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    uint32_t expected = 0xDEADBEEFU; /* write this to every page in flash */

    sysret_t ret;

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "\n"
        "Storage PAGE PROGRAM test\n"
        "- Write 0x%08X to every page in external flash storage...\n"
        "\n", expected);

    for(size_t i = 0U ; i < FLASH_CAPACITY ; i += FLASH_PAGE_SIZE)
    {
        uint32_t tx = expected;
        uint32_t rx = 0U;

        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Page %d | Addr 0x%08X : ", i/FLASH_PAGE_SIZE, i);

        ret = mt25q_page_program(i, (uint8_t*)&tx, sizeof(tx));
        if(ret != RET_OK)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Write error... - [%s]\n", retcodes_desc[ret]);
            break;
        }

        ret = mt25q_read(i, (uint8_t*)&rx, sizeof(rx));
        if(ret != RET_OK)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Read error... - [%s]\n", retcodes_desc[ret]);
            break;
        }

        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "Write [0x%08X] | Read [0x%08X] ", expected, rx);

        if(expected != rx)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "- WRITE/READ FAILED");
        }

        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\n");
    }
}

/**
 * @notapi
 * @brief Print out status of system peripherals
 */
static void sysprop_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    sysret_t datetime_stat = datetime_test();
    sysret_t adxl372_stat = adxl372_test();
    sysret_t icm20649_stat = icm20649_test();
    sysret_t vcnl4040_stat = vcnl4040_test();
    sysret_t mt25q_stat = mt25q_test();

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "\n"
        "/********************\n"
        " * SYSTEM PROPERTIES\n"
        " ********************/\n"
        "\n"
        " > RTC      - [%s]\n"
        " > ADXL372  - [%s]\n"
        " > ICM20649 - [%s]\n"
        " > VCNL4040 - [%s]\n"
        " > MT25Q    - [%s]\n"
        "\n\n",
        retcodes_desc[datetime_stat],
        retcodes_desc[adxl372_stat],
        retcodes_desc[icm20649_stat],
        retcodes_desc[vcnl4040_stat],
        retcodes_desc[mt25q_stat]);
}

/***************************************************************************************
 * Register the systest subcommands, pair them to their command names using NRF5's API
 ***************************************************************************************/

NRF_CLI_CREATE_STATIC_SUBCMD_SET(datetime_subcmds)
{
    NRF_CLI_CMD(get, NULL, "help string", datetime_get_cmd),
    NRF_CLI_CMD(set, NULL,
        "Set system datetime.\n"
        "Input is in format \"YYYY MM DD HH MM SS ffffff\"\n"
        "  - where HH is in 24-hour format\n"
        "  - where S is seconds and s is milliseconds\n",
        datetime_set_cmd),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(adxl372_subcmds)
{
    NRF_CLI_CMD(calibrate, NULL, "Calibrate ADXL372 High-g sensor", adxl372_calibrate_cmd),
    NRF_CLI_CMD(stream, NULL, "Stream sensor data from ADXL372 High-g sensor", adxl372_stream_cmd),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(icm20649_subcmds)
{
    NRF_CLI_CMD(calibrate, NULL, "Calibrate ICM20649 sensor", NULL),
    NRF_CLI_CMD(stream, NULL, "Stream sensor data from ICM20649 sensor", icm20649_stream_cmd),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(vcnl4040_subcmds)
{
    NRF_CLI_CMD(stream, NULL, "Stream sensor data from VCNL4040 sensor", vcnl4040_stream_cmd),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(sensor_subcmds)
{
    NRF_CLI_CMD(adxl372, &adxl372_subcmds, "ADXL372 subcommands", NULL),
    NRF_CLI_CMD(icm20649, &icm20649_subcmds, "ICM20649 subcommands", NULL),
    NRF_CLI_CMD(vcnl4040, &vcnl4040_subcmds, "VCNL4040 subcommands", NULL),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(storage_subcmds)
{
    NRF_CLI_CMD(erase, NULL, "Erase entire storage", storage_erase_cmd),
    NRF_CLI_CMD(pp_test, NULL, "Storage page program test", storage_pp_test_cmd),
    NRF_CLI_SUBCMD_SET_END
};

/***************************************************************************************
 * Register the commands, pair them to their command names using NRF5's API
 ***************************************************************************************/

NRF_CLI_CMD_REGISTER(hello, NULL, "Test shell interface", hello_cmd);
NRF_CLI_CMD_REGISTER(datetime, &datetime_subcmds, "Datetime API for setting and getting datetime", NULL);
NRF_CLI_CMD_REGISTER(sensor, &sensor_subcmds, "Sensor values and configurations", NULL);
NRF_CLI_CMD_REGISTER(storage, &storage_subcmds, "Storage properties and testing", NULL);
NRF_CLI_CMD_REGISTER(sysprop, NULL, "Display status of system peripherals", sysprop_cmd);

/******************
 * Start of API
 ******************/

#include "nrf_log.h"

/**
 * @brief Initialize she CLI for user inputs
 * @return sysret_t - Module status
 */
sysret_t shell_init(void)
{
    sysret_t ret;

    NRF_LOG_INIT(NULL);

    /**
     * Configure the UART peripheral
     */
    uart_cfg.pseltxd  = TX_PIN_NUMBER;
    uart_cfg.pselrxd  = RX_PIN_NUMBER;
    uart_cfg.hwfc     = NRF_UART_HWFC_DISABLED;
    uart_cfg.baudrate = NRF_UART_BAUDRATE_921600;
    ret = nrf_cli_init(&cli_uart,
                       &uart_cfg,
                       false, /* colored prints disabled */
                       true,  /* CLI to be used as logger backend */
                       NRF_LOG_SEVERITY_INFO);
    SYSRET_CHECK(ret);

    /**
     * Configure RTT peripheral 
     * 
     */
    ret = nrf_cli_init(&cli_rtt,
                       NULL,
                       false, /* colored prints disabled */
                       true,  /* CLI to be used as logger backend */
                       NRF_LOG_SEVERITY_INFO);
    SYSRET_CHECK(ret);

    /**
     * Start CLI System
     */
    ret = nrf_cli_start(&cli_rtt);
    ret = nrf_cli_start(&cli_uart);
    SYSRET_CHECK(ret);

    return RET_OK;
}

/**
 * @brief Shell process to be called continuously in main()
 */
void shell_process(void)
{
    nrf_cli_process(&cli_rtt);
    nrf_cli_process(&cli_uart);
}