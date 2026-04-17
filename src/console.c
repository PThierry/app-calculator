/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sentry.h>
#include <merlin/platform/driver.h>
#include "console.h"
#include "stm32_usart_driver.h"

static uint32_t g_console_uart_label;
static char g_console_line[CONSOLE_MAX_LINE_LENGTH + 1U];

#define CONSOLE_IRQ_EVENT_BYTES  (sizeof(exchange_event_t) + sizeof(uint32_t))
#define CONSOLE_IO_RETRY_COUNT   4U

static int console_wait_and_service_usart_irq(void)
{
    uint8_t event_buf[CONSOLE_IRQ_EVENT_BYTES];
    exchange_event_t *event = (exchange_event_t *)event_buf;

    /* polling mode, acknowledge at device level only */
    stm32_usart_acknowledge_irq(0);

    return 0;
}

int console_write(const uint8_t *buf, size_t len)
{
    size_t attempt = 0U;

    if (buf == NULL || len == 0U) {
        return -1;
    }

    while (attempt < CONSOLE_IO_RETRY_COUNT) {
        if (stm32_usart_write(g_console_uart_label, buf, len) == 0 &&
            stm32_usart_flush(g_console_uart_label) == 0) {
            return 0;
        }
        printf("console_write: attempt %u failed, retrying\n", attempt + 1U);

        (void)console_wait_and_service_usart_irq();
        attempt++;
    }

    return -1;
}

int console_init(uint32_t uart_label)
{
    g_console_uart_label = uart_label;
    return 0;
}

void console_display_prompt(void)
{
    static const uint8_t prompt[] = "> ";

    (void)console_write(prompt, sizeof(prompt) - 1U);
}

char *console_get_line(void)
{
    size_t line_len = 0U;

    for (;;) {
        uint8_t c;

        if (stm32_usart_read(g_console_uart_label, &c, 1U) != 0) {
            (void)console_wait_and_service_usart_irq();
            continue;
        }
        //printf("read %x from console\n", (uint32_t)c);

        /* support backspace for char correction */
        if (c == 0x8U) {
            if (line_len > 0U) {
                line_len--;
            }
            continue;
        }

        /* support carriage return for line termination */
        if (c == (uint8_t)'\r') {
            static const uint8_t newline[] = "\r\n";
            (void)console_write(newline, sizeof(newline) - 1U);
            g_console_line[line_len] = '\0';
            return g_console_line;
        }

        if ((c == (uint8_t)'\b') || (c == 0x7FU)) {
            if (line_len > 0U) {
                static const uint8_t erase[] = "\b \b";
                line_len--;
                (void)console_write(erase, sizeof(erase) - 1U);
            }
            continue;
        }

        if (line_len < CONSOLE_MAX_LINE_LENGTH) {
            g_console_line[line_len++] = (char)c;
            (void)console_write(&c, 1U);
        }
    }
}
