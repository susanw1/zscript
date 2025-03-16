/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>

#include <zscript/tokenbuffer/TokenRingBuffer_defs.h>

void assertEquals(const char *msg, int expected, int actual) {
    if (expected != actual) {
        fprintf (stderr, "*********** Expected = %d, actual = %d: %s\n", expected, actual, msg);
        exit(1);
    }
}

void test_initTokenBuffer() {
}

int main() {
    const int BUF_LEN = 128;
    uint8_t buffer[BUF_LEN];

    ZStok_TokenBuffer ringBuffer;
    zstok_initTokenBuffer(&ringBuffer, buffer, BUF_LEN);

    test_initTokenBuffer(&ringBuffer);
}
