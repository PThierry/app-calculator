/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include "console.h"
#include "stm32_usart_driver.h"

static uint32_t g_console_uart_label;
static char g_console_line[CONSOLE_MAX_LINE_LENGTH + 1U];

int console_init(uint32_t uart_label)
{
    g_console_uart_label = uart_label;
    return 0;
}

void console_display_prompt(void)
{
    static const uint8_t prompt[] = "> ";

    (void)stm32_usart_write(g_console_uart_label, prompt, sizeof(prompt) - 1U);
}

char *console_get_line(void)
{
    size_t line_len = 0U;

    for (;;) {
        uint8_t c;

        if (stm32_usart_read(g_console_uart_label, &c, 1U) != 0) {
            continue;
        }

        if (c == (uint8_t)'\r') {
            continue;
        }

        if (c == (uint8_t)'\n') {
            static const uint8_t newline[] = "\r\n";
            (void)stm32_usart_write(g_console_uart_label, newline, sizeof(newline) - 1U);
            g_console_line[line_len] = '\0';
            return g_console_line;
        }

        if ((c == (uint8_t)'\b') || (c == 0x7FU)) {
            if (line_len > 0U) {
                static const uint8_t erase[] = "\b \b";
                line_len--;
                (void)stm32_usart_write(g_console_uart_label, erase, sizeof(erase) - 1U);
            }
            continue;
        }

        if (line_len < CONSOLE_MAX_LINE_LENGTH) {
            g_console_line[line_len++] = (char)c;
            (void)stm32_usart_write(g_console_uart_label, &c, 1U);
        }
    }
}
