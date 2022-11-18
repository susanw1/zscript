/*
 * default-cli.c
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#include <stdio.h>

#include <zcode/modules/outer-core/ZcodeOuterCoreModule.h>
#include <zcode/modules/core/ZcodeCoreModule.h>

#include "../support/ZcodeLocalChannel.h"

#include <zcode/Zcode.h>

#include <zcode/ZcodeCCode.h>

int main(void) {
    Zcode z;
    printf("%li", sizeof(z));

    int ch;
    bool hasChar = false;
    ZcodeInitialise(&z);
    while (true) {
        ZcodeRunNext(&z);
        if (hasChar) {
            if (ZcodeAcceptByte(&z, (char) ch)) {
                hasChar = false;
            }
        }
        while (!hasChar && (ch = getchar()) != EOF) {
            if (!ZcodeAcceptByte(&z, (char) ch)) {
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

