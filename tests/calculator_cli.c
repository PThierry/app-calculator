/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "calculator.h"

int main(void)
{
    char line[256];

    while (fgets(line, sizeof(line), stdin) != NULL) {
        size_t len = strlen(line);
        struct calculator_result result;

        if (len > 0U && line[len - 1U] == '\n') {
            line[len - 1U] = '\0';
        }

        if (calculator_eval_line(line, &result) != 0) {
            (void)printf("Erreur\n");
            continue;
        }

        if (result.has_remainder) {
            (void)printf("Resultat: %" PRIu32 " (reste : %" PRIu32 ")\n",
                         result.value, result.remainder);
        } else {
            (void)printf("Resultat: %" PRIu32 "\n", result.value);
        }
    }

    return 0;
}
