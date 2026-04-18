/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright 2023 Ledger SAS
 */

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <uapi.h>
#include <types.h>
#include <merlin/platform/driver.h>
#include "console.h"
#include "stm32_usart_driver.h"

/* DTS sentry,label value for the usart3 node */
#define USART3_LABEL 0x103U

int main(void)
{
    uint8_t rx_char;
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

    /* kernel input event buffer */
    const uint8_t event_buf_size = sizeof(exchange_event_t) + sizeof(uint32_t);
    uint8_t event_buf[event_buf_size];
    exchange_event_t *event = (exchange_event_t *)event_buf;
    uint32_t *IRQn = (uint32_t *)(&event->data[0]);

    printf("Calculator hello!\n");

    if (stm32_usart_probe(USART3_LABEL) != 0) {
        goto err;
    }

    if (stm32_usart_init(USART3_LABEL, &uart3_cfg) != 0) {
        goto err;
    }
    printf("Usart initialized\n");

    if (console_init(USART3_LABEL) != 0) {
        goto err;
    }
    printf("Console initialized\n");

    if (console_display_prompt() != 0) {
        goto err;
    }

    for (;;) {
        /* wait for event with 20 ms timeout*/
        if (__sys_wait_for_event(EVENT_TYPE_ALL, 20) != STATUS_OK) {
            continue;
        }
        /* get back IRQ event from kernel */
        copy_from_kernel(event_buf, event_buf_size);


        /* get back IRQn, encoded 32 bits, from data tab field and ask merlin to dispatch
         * driver ISR auto get back character from USART, device IRQ event is acknowledged and
         * NVIC IRQ line is cleared in the driver ISR, then process the character in console */
         */
        merlin_platform_driver_irq_displatch(*IRQn);

        if (stm32_usart_read(USART3_LABEL, &rx_char, 1U) != 0) {
            continue;
        }

        (void)console_process_rx_char(rx_char);
    }

err:
    return -1;
}
