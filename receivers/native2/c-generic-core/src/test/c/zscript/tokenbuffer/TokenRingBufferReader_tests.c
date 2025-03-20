/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdarg.h>
#include <string.h>

#include <zscript/tokenbuffer/TokenRingBufferWriter_defs.h>
#include <zscript/tokenbuffer/TokenRingBufferReader_defs.h>

#include "../testing/TestTools.h"

// handy test instances used by most tests
#define TEST_BUF_LEN        16
static const uint8_t ERROR_BUFFER_OVERRUN = 0xf1;

uint8_t           testBufferSpace[TEST_BUF_LEN];
ZStok_TokenBuffer testRingBuffer;
ZStok_TokenWriter testWriter;

void setup(void) {
    zs_errno = 0; // Clear error state
    memset(testBufferSpace, 0, TEST_BUF_LEN);
    zstok_initTokenBuffer(&testRingBuffer, testBufferSpace, TEST_BUF_LEN);
    testWriter = zstok_getTokenWriter(&testRingBuffer);
}


void assertBufferContainsAt(const char *msg, ZStok_TokenWriter tbw, zstok_bufsz_t start, uint8_t contents[], zstok_bufsz_t len) {
    checkErrno();
    if (start + len <= tbw.tokenBuffer->bufLen) {
        assertContains(msg, tbw.tokenBuffer->data + start, contents, len);
    } else {
        int l1 = tbw.tokenBuffer->bufLen - start;
        assertContains(msg, tbw.tokenBuffer->data + start, contents, l1);
        assertContains(msg, tbw.tokenBuffer->data, contents + l1, len - l1);
    }
}

void assertStartsWith(const char *msg, ZStok_TokenWriter tbw, uint8_t contents[], zstok_bufsz_t len) {
    assertBufferContainsAt(msg, tbw, 0, contents, len);
}


void verifyBufferState1(bool tokenComplete, int availableWrite) {
    assertEquals("TokenComplete", zstok_isTokenComplete(testWriter), tokenComplete);
    assertEquals("AvailableWrite sz", zstok_getAvailableWrite_priv(testWriter), availableWrite);
    assertEquals("AvailableCapacity", zstok_checkAvailableCapacity(testWriter, availableWrite), true);
    assertEquals("AvailableCapacity+1", zstok_checkAvailableCapacity(testWriter, availableWrite + 1), false);
}

void verifyBufferState2(bool tokenComplete, int availableWrite, int currentTokenKey, bool inNibble, int tokenLength) {
    verifyBufferState1(tokenComplete, availableWrite);
    assertEquals("isTokenComplete", zstok_isTokenComplete(testWriter), false);

    if (!zstok_isTokenComplete(testWriter)) {
        assertEquals("CurrentWriteTokenKey", zstok_getCurrentWriteTokenKey(testWriter), currentTokenKey);
        assertEquals("CurrentWriteTokenLength", zstok_getCurrentWriteTokenLength(testWriter), tokenLength);
        assertEquals("InNibble", zstok_isInNibble(testWriter), inNibble);
    }
}

void insertNumericToken(char key, int count, ...) {
    va_list args;
    va_start(args, count);

    zstok_startToken(testWriter, key, true);
    for (int i = 0; i < count; i++) {
        zstok_continueTokenByte(testWriter, va_arg(args, int));
    }
    va_end(args);
}

void insertTokenNibbles_priv(char key, bool isNumeric, int count, va_list args) {
    zstok_startToken(testWriter, key, isNumeric);
    for (int i = 0; i < count; i++) {
        zstok_continueTokenNibble(testWriter, va_arg(args, int) & 0xF); // Mask for 4-bit nibble
    }
}

void insertNumericTokenNibbles(char key, int count, ...) {
    va_list args;
    va_start(args, count);
    insertTokenNibbles_priv(key, true, count, args);
    va_end(args);
}

void insertNonNumericTokenNibbles(char key, int count, ...) {
    va_list args;
    va_start(args, count);
    insertTokenNibbles_priv(key, false, count, args);
    va_end(args);
}

void insertByteToken(int count) {
    zstok_startToken(testWriter, '+', false);

    for (int i = 0; i < count; i++) {
        zstok_continueTokenByte(testWriter, 0xa0 + i);
    }

    uint8_t exp[] = { '+', count, 0xa0, 0xa1 };
    assertStartsWith("insertByteToken initial check", testWriter, exp, sizeof exp);
    verifyBufferState2(false, TEST_BUF_LEN - 3 - count, '+', false, count);
}

void shouldInitialize(void) {
    setup();
    insertNumericTokenNibbles('A', 1, 5);
    assertEquals("init tokenbuffer inNibble", testRingBuffer.inNibble, true);
    assertEquals("init tokenbuffer numeric", testRingBuffer.numeric, true);

    zstok_initTokenBuffer(&testRingBuffer, testBufferSpace, TEST_BUF_LEN);

    assertTrue("init tokenbuffer buffer", testRingBuffer.data == testBufferSpace);
    assertEquals("init tokenbuffer bufLen", testRingBuffer.bufLen, 16);
    assertEquals("init tokenbuffer readStart", testRingBuffer.readStart, 0);
    assertEquals("init tokenbuffer writeStart", testRingBuffer.writeStart, 0);
    assertEquals("init tokenbuffer writeLastLen", testRingBuffer.writeLastLen, 0);
    assertEquals("init tokenbuffer writeCursor", testRingBuffer.writeCursor, 0);
    assertEquals("init tokenbuffer inNibble", testRingBuffer.inNibble, false);
    assertEquals("init tokenbuffer numeric", testRingBuffer.numeric, false);
    verifyBufferState1(true, TEST_BUF_LEN - 1);
}

int main(void) {
    shouldInitialize();
}
