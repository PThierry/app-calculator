/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "calculator.h"
#include "console.h"
#include "stm32_usart_driver.h"

static uint32_t g_console_uart_label;
static char g_console_line[CONSOLE_MAX_LINE_LENGTH + 1U];
static size_t g_console_line_len;

#define CONSOLE_TX_BUFFER_LENGTH 160U
#define CONSOLE_PROMPT           "> "
#define CONSOLE_ERASE_SEQ        "\b \b"
#define CONSOLE_NEWLINE          "\r\n"
#define CONSOLE_ERR_MSG          "Erreur\r\n"
#define CONSOLE_OVERFLOW_MSG     "Ligne trop longue\r\n"

static int console_write_bytes(const uint8_t *buf, size_t len)
{
    if (buf == NULL || len == 0U) {
        return -1;
    }

    for (size_t i = 0U; i < len; i++) {
        if (stm32_usart_write(g_console_uart_label, &buf[i], 1U) != 0) {
            return -1;
        }
    }

    if (stm32_usart_flush(g_console_uart_label) != 0) {
        return -1;
    }

    return 0;
}

static int console_write_literal(const char *literal)
{
    if (literal == NULL) {
        return -1;
    }

    return console_write_bytes((const uint8_t *)literal, strlen(literal));
}

int console_init(uint32_t uart_label)
{
    g_console_uart_label = uart_label;
    g_console_line_len = 0U;
    g_console_line[0] = '\0';
    return 0;
}

int console_display_prompt(void)
{
    return console_write_literal(CONSOLE_PROMPT);
}

int console_process_rx_char(uint8_t c)
{
    char txbuf[CONSOLE_TX_BUFFER_LENGTH];
    struct calculator_result calc;
    int n;

    if ((c == (uint8_t)'\b') || (c == 0x7FU) || (c == 0x8U)) {
        if (g_console_line_len > 0U) {
            g_console_line_len--;
            return console_write_literal(CONSOLE_ERASE_SEQ);
        }
        return 0;
    }

    if (c == (uint8_t)'\n') {
        return 0;
    }

    if (c == (uint8_t)'\r') {
        g_console_line[g_console_line_len] = '\0';

        if (calculator_eval_line(g_console_line, &calc) != 0) {
            n = snprintf(txbuf, sizeof(txbuf), "%s%s", CONSOLE_ERR_MSG, CONSOLE_PROMPT);
        } else if (calc.has_remainder) {
            n = snprintf(txbuf,
                         sizeof(txbuf),
                         "%sResultat: %lu (reste : %lu)\r\n%s",
                         CONSOLE_NEWLINE,
                         (unsigned long)calc.value,
                         (unsigned long)calc.remainder,
                         CONSOLE_PROMPT);
        } else {
            n = snprintf(txbuf,
                         sizeof(txbuf),
                         "%sResultat: %lu\r\n%s",
                         CONSOLE_NEWLINE,
                         (unsigned long)calc.value,
                         CONSOLE_PROMPT);
        }

        g_console_line_len = 0U;

        if (n < 0 || (size_t)n >= sizeof(txbuf)) {
            return -1;
        }

        return console_write_bytes((const uint8_t *)txbuf, (size_t)n);
    }

    if (g_console_line_len >= CONSOLE_MAX_LINE_LENGTH) {
        g_console_line_len = 0U;
        return console_write_literal(CONSOLE_OVERFLOW_MSG CONSOLE_PROMPT);
    }

    g_console_line[g_console_line_len++] = (char)c;

    return console_write_bytes(&c, 1U);
}
