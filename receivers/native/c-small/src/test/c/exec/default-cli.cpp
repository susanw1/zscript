/*
 * default-cli.c
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#include <stdio.h>

#include <stdbool.h>

#include <stdint.h>
#include <stdlib.h>

#define ZCODE_BIG_FIELD_SIZE 10
#define ZCODE_FIELD_COUNT 10
typedef uint16_t ZcodeBigFieldAddress_t;

#include <zcode/modules/outer-core/ZcodeOuterCoreModule.h>
#include <zcode/modules/core/ZcodeCoreModule.h>

#include "../support/ZcodeLocalChannel.h"

#include <zcode/Zcode.h>

#include <zcode/ZcodeCCode.h>

int main(void) {

    int ch;
    bool hasChar = false;
    ZcodeInitialise();
    while (true) {
        ZcodeRunNext();
        if (!zcode.slot.state.waitingOnRun && hasChar) {
            if (ZcodeAcceptByte((char) ch)) {
                hasChar = false;
            }
        }
        while (!zcode.slot.state.waitingOnRun && !hasChar && (ch = getchar()) != EOF) {
            if (!ZcodeAcceptByte((char) ch)) {
                hasChar = true;
                break;
            }
            if (ch == '\n') {
                break;
            }
        }
    }
    return 0;
}

