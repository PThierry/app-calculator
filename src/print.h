/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_PRINT_H
#define APP_PRINT_H

#include <stdint.h>
#include "calculator.h"

int print_error(uint32_t uart_label);
int print_result(uint32_t uart_label, const struct calculator_result *result);

#endif
