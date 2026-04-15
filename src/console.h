/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_CONSOLE_H
#define APP_CONSOLE_H

#include <stdint.h>

#define CONSOLE_MAX_LINE_LENGTH 128U

int console_init(uint32_t uart_label);
void console_display_prompt(void);
char *console_get_line(void);

#endif
