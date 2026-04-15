/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include "print.h"
#include "stm32_usart_driver.h"

static int print_bytes(uint32_t uart_label, const char *text, size_t length)
{
    return stm32_usart_write(uart_label, (const uint8_t *)text, length);
}

static int print_cstr(uint32_t uart_label, const char *text)
{
    size_t length = 0U;

    while (text[length] != '\0') {
        length++;
    }

    return print_bytes(uart_label, text, length);
}

static int print_u32(uint32_t uart_label, uint32_t value)
{
    char digits[10];
    size_t length = 0U;

    if (value == 0U) {
        return print_bytes(uart_label, "0", 1U);
    }

    while (value > 0U) {
        digits[length++] = (char)('0' + (value % 10U));
        value /= 10U;
    }

    while (length > 0U) {
        if (print_bytes(uart_label, &digits[length - 1U], 1U) != 0) {
            return -1;
        }
        length--;
    }

    return 0;
}

int print_error(uint32_t uart_label)
{
    return print_cstr(uart_label, "Erreur\r\n");
}

int print_result(uint32_t uart_label, const struct calculator_result *result)
{
    if (result == NULL) {
        return -1;
    }

    if (print_cstr(uart_label, "Resultat: ") != 0) {
        return -1;
    }

    if (print_u32(uart_label, result->value) != 0) {
        return -1;
    }

    if (result->has_remainder) {
        if (print_cstr(uart_label, " (reste : ") != 0) {
            return -1;
        }

        if (print_u32(uart_label, result->remainder) != 0) {
            return -1;
        }

        if (print_cstr(uart_label, ")") != 0) {
            return -1;
        }
    }

    return print_cstr(uart_label, "\r\n");
}
