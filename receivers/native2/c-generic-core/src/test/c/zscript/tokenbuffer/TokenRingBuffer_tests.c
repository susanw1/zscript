/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>

#include <zscript/tokenbuffer/TokenRingBuffer_defs.h>

// handy test instances used by most tests
#define TEST_BUF_LEN        16

uint8_t testBufferSpace[TEST_BUF_LEN];
ZStok_TokenBuffer testRingBuffer;
Zs_TokenWriter testWriter;

void setup() {
    zstok_initTokenBuffer(&testRingBuffer, testBufferSpace, TEST_BUF_LEN);
    testWriter = zstok_getTokenWriter(&testRingBuffer);
}

void assertEquals(const char *msg, int expected, int actual) {
    if (expected != actual) {
        fprintf (stderr, "*********** Expected = %d, actual = %d: %s\n", expected, actual, msg);
        exit(1);
    }
}

void assertStartsWith(const char *msg, uint8_t contents[], zstok_bufsz_t len) {
    (void)msg;
    for (zstok_bufsz_t i = 0; i < len; i++) {
        uint8_t c = testRingBuffer.data[i];
        if (c != contents[i]) {
            fprintf (stderr, "*********** Expected=%02x(%c), actual=%02x(%c); pos=%d: %s\n", c, c, contents[i], contents[i], i, msg);
            exit(1);
        }
    }
}

void verifyBufferState1(bool tokenComplete, int availableWrite) {
    assertEquals("TokenComplete", zstok_isTokenComplete(testWriter), tokenComplete);
    assertEquals("AvailableWrite", zstok_checkAvailableCapacity(testWriter, availableWrite), true);
    assertEquals("AvailableWrite+1", zstok_checkAvailableCapacity(testWriter, availableWrite + 1), false);

    //assertThat(writer.checkAvailableCapacity(availableWrite)).as("AvailableWrite").isTrue();
    //assertThat(writer.checkAvailableCapacity(availableWrite + 1)).as("AvailableWrite+1").isFalse();
}

void verifyBufferState2(bool tokenComplete, int availableWrite, int currentTokenKey, bool inNibble, int tokenLength) {
    verifyBufferState1(tokenComplete, availableWrite);
    if (!zstok_isTokenComplete(testWriter)) {
        assertEquals("CurrentWriteTokenKey", zstok_getCurrentWriteTokenKey(testWriter), currentTokenKey);
        assertEquals("CurrentWriteTokenLength", zstok_getCurrentWriteTokenLength(testWriter), tokenLength);
        assertEquals("InNibble", zstok_isInNibble(testWriter), inNibble);
    }
}

void shouldInitialize() {
    setup();

    // assertEquals("");
}

void shouldTokenizeNumericFieldWithNoValue() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    uint8_t exp1[] = { 'A', 0, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithNoValue", exp1, sizeof exp1);
    verifyBufferState2(false, 13, 'A', false, 0);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 13);
}


int main() {
    shouldInitialize();
    shouldTokenizeNumericFieldWithNoValue();
}
