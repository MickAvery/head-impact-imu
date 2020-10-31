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
#include "nrf_cli_uart.h"
#include "nrf_delay.h"
#include "retcodes.h"
#include "datetime.h"
#include "adxl372.h"
#include "icm20649.h"
#include "vcnl4040.h"

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
            4U); /* TODO: magic number, but this is log queue size */

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

    while(rx != ctrl_c)
    {
        adxl372_val_raw_t readings[ADXL372_AXES] = {0U};
        adxl372_read_raw(readings);

        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "%d\t%d\t%d\n",
            readings[ADXL372_X]*100, readings[ADXL372_Y]*100, readings[ADXL372_Z]*100);

        nrf_delay_ms(25U);

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

    while(rx != ctrl_c)
    {
        retcode_t ret;
        int16_t accel[ICM20649_ACCEL_AXES] = {0};
        int16_t gyro[ICM20649_GYRO_AXES] = {0};
        ret = icm20649_read_raw(gyro, accel);

        if(ret == RET_OK)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "%d\t%d\t%d\t%d\t%d\t%d\n",
                accel[ICM20649_ACCEL_X], accel[ICM20649_ACCEL_Y], accel[ICM20649_ACCEL_Z],
                gyro[ICM20649_GYRO_X], gyro[ICM20649_GYRO_Y], gyro[ICM20649_GYRO_Z]);
        }
        else
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "read timeout!\n");

        nrf_delay_ms(25U);

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

    while(rx != ctrl_c)
    {
        retcode_t ret;
        vcnl4040_data_t data;
        ret = vcnl4040_read(&data);

        if(ret == RET_OK)
        {
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
                "%d\n", data);
        }
        else
            nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "error!\n");

        nrf_delay_ms(25U);

        nrf_drv_uart_rx(&cli_uart_transport_uart, &rx, 1U);
    }
}

/**
 * @notapi
 * @brief Enable/Disable CLI echo
 */
static void echo_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    bool command_unrecognized = true;
    bool set = true;

    if(argc >= 2)
    {
        if(strncmp(argv[1], "on", 2) == 0)
        {
            command_unrecognized = false;
            set = true;
        }
        else if(strncmp(argv[1], "off", 3) == 0)
        {
            command_unrecognized = false;
            set = false;
        }
    }

    if(command_unrecognized)
    {
        nrf_cli_help_print(p_cli, NULL, 0);
    }
    else
    {
        p_cli->p_ctx->internal.flag.echo = set ? 1 : 0;
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

    retcode_t adxl372_stat = adxl372_test();
    retcode_t icm20649_stat = icm20649_test();
    retcode_t vcnl4040_stat = vcnl4040_test();

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "\n"
        "/********************\n"
        " * SYSTEM PROPERTIES\n"
        " ********************/\n"
        "\n"
        " > RTC      - [OK]\n"
        " > ADXL372  - [%s]\n"
        " > ICM20649 - [%s]\n"
        " > VCNL4040 - [%s]\n"
        "\n\n",
        retcodes_desc[adxl372_stat],
        retcodes_desc[icm20649_stat],
        retcodes_desc[vcnl4040_stat]);
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

/***************************************************************************************
 * Register the commands, pair them to their command names using NRF5's API
 ***************************************************************************************/

NRF_CLI_CMD_REGISTER(hello, NULL, "Test shell interface", hello_cmd);
NRF_CLI_CMD_REGISTER(datetime, &datetime_subcmds, "Datetime API for setting and getting datetime", NULL);
NRF_CLI_CMD_REGISTER(sensor, &sensor_subcmds, "Print sensor values", NULL);
NRF_CLI_CMD_REGISTER(echo, NULL,
    "Configure CLI echo setting\n"
    "    echo off - turn off CLI echo\n"
    "    echo off - turn on CLI echo\n", echo_cmd);
NRF_CLI_CMD_REGISTER(sysprop, NULL, "Display status of system peripherals", sysprop_cmd);

/******************
 * Start of API
 ******************/

/**
 * @brief Initialize she CLI for user inputs
 */
void shell_init(void)
{
    ret_code_t ret;

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
    APP_ERROR_CHECK(ret);

    /**
     * Start CLI System
     */
    ret = nrf_cli_start(&cli_uart);
    APP_ERROR_CHECK(ret);
}

/**
 * @brief Shell process to be called continuously in main()
 */
void shell_process(void)
{
    nrf_cli_process(&cli_uart);
}