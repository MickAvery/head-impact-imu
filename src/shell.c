/**
 * @file shell.c
 * @author UBC Capstone Team 2020/2021
 * @brief Shell interface for user commands
 */

#include "shell.h"
#include "nrf.h"
#include "app_timer.h"
#include "nrf_cli.h"
#include "nrf_cli_uart.h"
#include "nrf_drv_clock.h"

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
static void systest_timer(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    /* TODO: */
    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT, "\n\nTODO\n\n");
}

/**
 * Register the subcommands, pair them to their command names using NRF5's API
 */
NRF_CLI_CREATE_STATIC_SUBCMD_SET(systest_subcmds)
{
    /* NOTE: make sure subcommands are in alphabetical order */
    NRF_CLI_CMD(timer, NULL,  "help string", systest_timer),
    NRF_CLI_SUBCMD_SET_END
};

/**
 * Register the functions, pair them to their command names using NRF5's API
 */
NRF_CLI_CMD_REGISTER(hello, NULL, "Test shell interface", hello_cmd);
NRF_CLI_CMD_REGISTER(sysprop, NULL, "Display status of system peripherals", sysprop_cmd);
NRF_CLI_CMD_REGISTER(systest, &systest_subcmds, "Test system peripherals", NULL);

/**
 * UART configurations for CLI
 */
static nrf_drv_uart_config_t uart_cfg = NRF_DRV_UART_DEFAULT_CONFIG;

NRF_CLI_UART_DEF(cli_uart_transport, 0, 64, 16); /* TODO: magic numbers */
NRF_CLI_DEF(cli_uart,
            "SimplLab:~$ ",
            &cli_uart_transport.transport,
            '\r',
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

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    nrf_drv_clock_lfclk_request(NULL);

    ret = app_timer_init();
    APP_ERROR_CHECK(ret);

    /**
     * Configure the UART peripheral
     */
    uart_cfg.pseltxd = 13; /* TODO: set proper pin! */
    uart_cfg.pselrxd = 12; /* TODO: set proper pin! */
    uart_cfg.hwfc    = NRF_UART_HWFC_DISABLED;
    uart_cfg.baudrate = NRF_UART_BAUDRATE_115200;
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