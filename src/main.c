/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2023 Ledger SAS
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <uapi.h>
#include <platform.h>
#include <types.h>
#include "calculator.h"
#include "console.h"
#include "stm32_usart_driver.h"

/* DTS sentry,label value for the usart3 node */
#define USART3_LABEL 0x103U

int main(void)
{
    char output[96];
    const struct usart_config uart3_cfg = {
        .baudrate     = 115200U,
        .mode         = USART_MODE_ASYNCHRONOUS,
        .parity       = USART_PARITY_NONE,
        .stop_bits    = USART_STOP_BITS_1,
        .word_length  = USART_WORD_LENGTH_8,
        .flow_control = USART_FLOW_CONTROL_NONE,
        .tx_enable    = true,
        .rx_enable    = true,
    };

    if (stm32_usart_probe(USART3_LABEL) != 0) {
        goto err;
    }

    if (stm32_usart_init(USART3_LABEL, &uart3_cfg) != 0) {
        goto err;
    }

    if (console_init(USART3_LABEL) != 0) {
        goto err;
    }
    /* test: acknowledge USART3 global interrupt */
    __sys_irq_acknowledge(63U); /* USART3 global interrupt */

    console_display_prompt();

    for (;;) {
        int len;
        char *line;
        struct calculator_result calc;

        line = console_get_line();
        if (line == NULL) {
            continue;
        }

        if (calculator_eval_line(line, &calc) != 0) {
            len = snprintf(output, sizeof(output), "Erreur\r\n");
        } else if (calc.has_remainder) {
            len = snprintf(output, sizeof(output), "Resultat: %lu (reste : %lu)\r\n",
                           (unsigned long)calc.value, (unsigned long)calc.remainder);
        } else {
            len = snprintf(output, sizeof(output), "Resultat: %lu\r\n",
                           (unsigned long)calc.value);
        }

        if (len > 0) {
            (void)console_write((const uint8_t *)output, (size_t)len);
        }

        console_display_prompt();
    }

err:
    return -1;
}
