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
 * Implement functions here
 */
static void hello_cmd(nrf_cli_t const* p_cli, size_t argc, char** argv)
{
    ASSERT(p_cli);
    ASSERT(p_cli->p_ctx && p_cli->p_iface && p_cli->p_name);

    nrf_cli_fprintf(p_cli, NRF_CLI_VT100_COLOR_DEFAULT,
        "\n"
        "******************\n"
        "\tHello World!\n"
        "******************\n"
        "\n");
}

/**
 * Register the functions, pair them to their command names using NRF5's API
 */
NRF_CLI_CMD_REGISTER(hello, NULL, "Test shell interface", hello_cmd);

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
 * @brief 
 * 
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
 * @brief 
 * 
 */
void shell_process(void)
{
    nrf_cli_process(&cli_uart);
}