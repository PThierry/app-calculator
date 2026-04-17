/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_CONSOLE_H
#define APP_CONSOLE_H

#include <stdint.h>
#include <stddef.h>

#define CONSOLE_MAX_LINE_LENGTH 128U

int console_init(uint32_t uart_label);
void console_display_prompt(void);
int console_write(const uint8_t *buf, size_t len);
char *console_get_line(void);

#endif
