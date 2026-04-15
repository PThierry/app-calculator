/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "calculator.h"

struct parser {
    const char *p;
};

static void skip_spaces(struct parser *parser)
{
    while (*parser->p != '\0' && isspace((unsigned char)*parser->p)) {
        parser->p++;
    }
}

static int parse_expr(struct parser *parser, struct calculator_result *result);

static int parse_number(struct parser *parser, uint32_t *value)
{
    uint64_t acc = 0U;
    bool has_digit = false;

    skip_spaces(parser);
    while (isdigit((unsigned char)*parser->p)) {
        has_digit = true;
        acc = (acc * 10U) + (uint64_t)(*parser->p - '0');
        if (acc > UINT32_MAX) {
            return -1;
        }
        parser->p++;
    }

    if (!has_digit) {
        return -1;
    }

    *value = (uint32_t)acc;
    return 0;
}

static int parse_factor(struct parser *parser, struct calculator_result *result)
{
    skip_spaces(parser);

    if (*parser->p == '(') {
        parser->p++;
        if (parse_expr(parser, result) != 0) {
            return -1;
        }

        skip_spaces(parser);
        if (*parser->p != ')') {
            return -1;
        }
        parser->p++;
        return 0;
    }

    if (parse_number(parser, &result->value) != 0) {
        return -1;
    }
    result->has_remainder = false;
    result->remainder = 0U;
    return 0;
}

static int parse_term(struct parser *parser, struct calculator_result *result)
{
    struct calculator_result rhs;

    if (parse_factor(parser, result) != 0) {
        return -1;
    }

    for (;;) {
        uint64_t tmp;
        char op;

        skip_spaces(parser);
        op = *parser->p;
        if (op != '*' && op != '/') {
            return 0;
        }
        parser->p++;

        if (parse_factor(parser, &rhs) != 0) {
            return -1;
        }

        if (op == '*') {
            tmp = (uint64_t)result->value * (uint64_t)rhs.value;
            if (tmp > UINT32_MAX) {
                return -1;
            }
            result->value = (uint32_t)tmp;
            result->has_remainder = false;
            result->remainder = 0U;
            continue;
        }

        if (rhs.value == 0U) {
            return -1;
        }

        result->remainder = result->value % rhs.value;
        result->value = result->value / rhs.value;
        result->has_remainder = true;
    }
}

static int parse_expr(struct parser *parser, struct calculator_result *result)
{
    struct calculator_result rhs;

    if (parse_term(parser, result) != 0) {
        return -1;
    }

    for (;;) {
        uint64_t tmp;
        char op;

        skip_spaces(parser);
        op = *parser->p;
        if (op != '+' && op != '-') {
            return 0;
        }
        parser->p++;

        if (parse_term(parser, &rhs) != 0) {
            return -1;
        }

        if (op == '+') {
            tmp = (uint64_t)result->value + (uint64_t)rhs.value;
            if (tmp > UINT32_MAX) {
                return -1;
            }
            result->value = (uint32_t)tmp;
            result->has_remainder = false;
            result->remainder = 0U;
            continue;
        }

        if (result->value < rhs.value) {
            return -1;
        }

        result->value -= rhs.value;
        result->has_remainder = false;
        result->remainder = 0U;
    }
}

int calculator_eval_line(const char *line, struct calculator_result *result)
{
    struct parser parser;

    if (line == NULL || result == NULL) {
        return -1;
    }

    parser.p = line;
    if (parse_expr(&parser, result) != 0) {
        return -1;
    }

    skip_spaces(&parser);
    if (*parser.p != '\0') {
        return -1;
    }

    return 0;
}
