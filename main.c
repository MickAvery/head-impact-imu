/**
 * @file main.c
 * @author UBC Capstone Team 2020/2021
 * @brief Firmware entry point
 */

#include "nrf.h"
#include "shell.h"

/**
 * @brief Main firmware entry point
 * 
 * @return int 
 */
int main(void)
{
    /* initialize shell interface for user inputs */
    shell_init();

    while(1)
    {
        /* infinite loop */
    }

    /* we should never get here */
    return 0;
}