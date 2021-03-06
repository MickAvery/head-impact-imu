/**
 * @file shell.h
 * @author UBC Capstone Team 2020/2021
 * @brief Shell interface for user commands
 */

#ifndef SHELL_H
#define SHELL_H

#include "retcodes.h"

#ifdef __cplusplus
extern "C" {
#endif

sysret_t shell_init(void);
void shell_process(void);

#ifdef __cplusplus
}
#endif

#endif /* SELL_H */