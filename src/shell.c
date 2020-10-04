/**
 * @file shell.c
 * @author UBC Capstone Team 2020/2021
 * @brief Shell interface for user commands
 */

#include "shell.h"
#include "nrf.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "datetime.h"

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
    SHELL_MSEC,
    SHELL_DATETIME_NUMARGS
} shell_dt_args_t;

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
        return;
    } else {
        datetime_t dt = {
            (uint16_t)atoi(argv[SHELL_YEAR]),
            (uint8_t)atoi(argv[SHELL_MONTH]),
            (uint8_t)atoi(argv[SHELL_DAY]),
            (uint8_t)atoi(argv[SHELL_HOUR]),
            (uint8_t)atoi(argv[SHELL_MIN]),
            (uint8_t)atoi(argv[SHELL_SEC]),
            (uint16_t)atoi(argv[SHELL_MSEC])
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
            "%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
            dt.year, dt.month, dt.day, dt.hr, dt.min, dt.sec, dt.msec);
    } else {
        nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
            "\nUnable to get datetime information\n");
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

    /* TODO: */
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\n\nTODO\n\n");
}

/**
 * @notapi
 * @brief System test for timer library
 */
static void systest_datetime(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    /* TODO: */
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\n\nTODO\n\n");
}

/***************************************************************************************
 * Register the systest subcommands, pair them to their command names using NRF5's API
 ***************************************************************************************/

NRF_CLI_CREATE_STATIC_SUBCMD_SET(datetime_subcmds)
{
    NRF_CLI_CMD(get, NULL, "help string", datetime_get_cmd),
    NRF_CLI_CMD(set, NULL,
        "Set system datetime.\n"
        "Input is in format \"YYYY MM DD HH MM SS sss\"\n"
        "  - where HH is in 24-hour format\n"
        "  - where S is seconds and s is milliseconds\n",
        datetime_set_cmd),
    NRF_CLI_SUBCMD_SET_END
};

NRF_CLI_CREATE_STATIC_SUBCMD_SET(systest_subcmds)
{
    /* NOTE: make sure subcommands are in alphabetical order */
    NRF_CLI_CMD(datetime, NULL,  "help string", systest_datetime),
    NRF_CLI_SUBCMD_SET_END
};

/***************************************************************************************
 * Register the commands, pair them to their command names using NRF5's API
 ***************************************************************************************/

NRF_CLI_CMD_REGISTER(hello, NULL, "Test shell interface", hello_cmd);
NRF_CLI_CMD_REGISTER(datetime, &datetime_subcmds, "Datetime API for setting and getting datetime", NULL);
NRF_CLI_CMD_REGISTER(sysprop, NULL, "Display status of system peripherals", sysprop_cmd);
NRF_CLI_CMD_REGISTER(systest, &systest_subcmds, "Test system peripherals", NULL);

/**
 * UART configurations for CLI
 */
static nrf_drv_uart_config_t uart_cfg = NRF_DRV_UART_DEFAULT_CONFIG;

NRF_CLI_UART_DEF(cli_uart_transport, 0, 64, 64); /* TODO: magic numbers */
NRF_CLI_DEF(cli_uart,
            "SimplLab:~$ ",
            &cli_uart_transport.transport,
            '\n',
            4U); /* TODO: magic number */

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
    uart_cfg.pseltxd = 13; /* TODO: set proper pin! */
    uart_cfg.pselrxd = 12; /* TODO: set proper pin! */
    uart_cfg.hwfc    = NRF_UART_HWFC_DISABLED;
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