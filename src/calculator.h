/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_CALCULATOR_H
#define APP_CALCULATOR_H

#include <stdbool.h>
#include <stdint.h>

struct calculator_result {
    uint32_t value;
    bool has_remainder;
    uint32_t remainder;
};

int calculator_eval_line(const char *line, struct calculator_result *result);

#endif
