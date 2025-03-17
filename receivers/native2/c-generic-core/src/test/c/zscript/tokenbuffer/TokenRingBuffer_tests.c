/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <zscript/tokenbuffer/TokenRingBuffer_defs.h>

// handy test instances used by most tests
#define TEST_BUF_LEN        16
static const uint8_t ERROR_BUFFER_OVERRUN = 0xf1;

uint8_t testBufferSpace[TEST_BUF_LEN];
ZStok_TokenBuffer testRingBuffer;
Zs_TokenWriter testWriter;

void setup() {
    zs_errno = 0; // Clear error state
    memset(testBufferSpace, 0, TEST_BUF_LEN);
    zstok_initTokenBuffer(&testRingBuffer, testBufferSpace, TEST_BUF_LEN);
    testWriter = zstok_getTokenWriter(&testRingBuffer);
}

void assertEquals(const char *msg, int p1, int p2) {
    if (p1 != p2) {
        fprintf (stderr, "*********** Unexpected mismatch: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertNotEquals(const char *msg, int p1, int p2) {
    if (p1 == p2) {
        fprintf (stderr, "*********** Unexpected match: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertTrue(const char *msg, int expected) {
    if (!expected) {
        fprintf (stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}

void assertFalse(const char *msg, int expected) {
    if (expected) {
        fprintf (stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}

void assertContainsAt(const char *msg, Zs_TokenWriter tbw, zstok_bufsz_t start, uint8_t contents[], zstok_bufsz_t len) {
    uint8_t *datap = tbw.tokenBuffer->data;
    for (zstok_bufsz_t i = 0; i < len; i++) {
        uint8_t c = datap[start + i];
        if (c != contents[i]) {
            fprintf (stderr, "Expected: ");
            for (zstok_bufsz_t t = 0; t < len; t++) {
                fprintf (stderr, "%02x ", contents[t]);
            }
            fprintf (stderr, "\n  Actual: ");
            for (zstok_bufsz_t t = 0; t < len; t++) {
                fprintf (stderr, "%02x ", datap[start + t]);
            }
            fprintf (stderr, "\n          ");
            for (zstok_bufsz_t t = 0; t < i; t++) {
                fprintf (stderr, "   ");
            }
            fprintf (stderr, "^^\n*********** Expected=%02x(%c), actual=%02x(%c); pos=%d: %s\n", contents[i], contents[i], c, (c < ' '? '?' : c), i, msg);
            exit(1);
        }
    }
}

void assertStartsWith(const char *msg, Zs_TokenWriter tbw, uint8_t contents[], zstok_bufsz_t len) {
    assertContainsAt(msg, tbw, 0, contents, len);
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

void shouldInitialize() {
    setup();
    assertEquals("init tokenbuffer buffer", testRingBuffer.data == NULL, false);
    assertEquals("init tokenbuffer bufLen", testRingBuffer.bufLen, 16);
    assertEquals("init tokenbuffer readStart", testRingBuffer.readStart, 0);
    assertEquals("init tokenbuffer writeStart", testRingBuffer.writeStart, 0);
    assertEquals("init tokenbuffer writeLastLen", testRingBuffer.writeLastLen, 0);
    assertEquals("init tokenbuffer writeCursor", testRingBuffer.writeCursor, 0);
    assertEquals("init tokenbuffer inNibble", testRingBuffer.inNibble, false);
    assertEquals("init tokenbuffer numeric", testRingBuffer.numeric, false);
    verifyBufferState1(true, TEST_BUF_LEN - 1);
}

void shouldTokenizeNumericFieldWithNoValue() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    uint8_t exp1[] = { 'A', 0, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithNoValue", testWriter, exp1, sizeof exp1);
    verifyBufferState2(false, 13, 'A', false, 0);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 13);
}

void shouldTokenizeNumericFieldWithNibbleValue() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 5);

    verifyBufferState2(false, 13, 'A', true, 1);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 1, 5, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithNibbleValue", testWriter, exp2, sizeof exp2);
    verifyBufferState1(true, 12);
}

void shouldTokenizeNumericFieldWithByteValue() {
    setup();
    insertNumericToken('A', 1, 234); // 1 is the number of vararg bytes following, not actually the datalen!

    uint8_t exp3[] = { 'A', 1, 234, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithByteValue", testWriter, exp3, sizeof exp3);
    verifyBufferState2(false, 12, 'A', false, 1);
}

void shouldTokenizeNumericFieldWith2ByteValue() {
    setup();
    insertNumericToken('A', 2, 234, 123);

    uint8_t exp4[] = { 'A', 2, 234, 123, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith2ByteValue", testWriter, exp4, sizeof exp4);
    verifyBufferState2(false, 11, 'A', false, 2);
}

void shouldFailToAcceptByteIfNoTokenStarted() {
    setup();
    zstok_continueTokenByte(testWriter, 0xA2);
    assertEquals("shouldFailToAcceptByteIfNoTokenStarted:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 15);
}

void shouldFailToAcceptNibbleIfNoTokenStarted() {
    setup();
    zstok_continueTokenNibble(testWriter, 3);
    assertEquals("shouldFailToAcceptNibbleIfNoTokenStarted:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 15);
}

void shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 3);
    zstok_continueTokenByte(testWriter, 123);
    assertEquals("shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue", ZS_SystemErrorType_TOKBUF_STATE_IN_NIBBLE, zs_errno);
    verifyBufferState2(false, 13, 'A', true, 1);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 12);
}

void shouldTokenizeNumericFieldWith2NibbleValue() {
    setup();
    insertNumericTokenNibbles('A', 2, 5, 0xd);
    assertEquals("zs_errno", 0, zs_errno);
    uint8_t exp1[] = { 'A', 1, 0x5d, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith2NibbleValue", testWriter, exp1, sizeof exp1);
}

void shouldHandleFailingMidNibble() {
    setup();
    insertNumericTokenNibbles('C', 1, 6);
    insertNumericTokenNibbles('A', 1, 5);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp2[] = { 'C', 1, 6, ERROR_BUFFER_OVERRUN};
    assertStartsWith("shouldHandleFailingMidNibble", testWriter, exp2, sizeof exp2);
}

void shouldHandleFailingMidToken() {
    setup();
    insertNumericTokenNibbles('C', 1, 6);
    insertNumericTokenNibbles('A', 2, 5, 2);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp1[] = { 'C', 1, 6, ERROR_BUFFER_OVERRUN };
    assertStartsWith("shouldHandleFailingMidToken", testWriter, exp1, sizeof exp1);
}

void shouldTokenizeNumericFieldWith3NibbleValue() {
    setup();
    insertNumericTokenNibbles('A', 3, 5, 0xd, 0xa);

    verifyBufferState2(false, 12, 'A', true, 2);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 2, 0x5, 0xda, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith3NibbleValue", testWriter, exp2, sizeof exp2);
}

void shouldTokenizeNumericFieldWith4NibbleValue() {
    setup();
    insertNumericTokenNibbles('A', 4, 5, 0xd, 0xa, 0x3);

    verifyBufferState2(false, 11, 'A', false, 2);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { 'A', 2, 0x5d, 0xa3, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith4NibbleValue", testWriter, exp3, sizeof exp3);
}

void shouldTokenizeNumericFieldWith5NibbleValue() {
    setup();
    insertNumericTokenNibbles('A', 5, 5, 0xd, 0xa, 0x3, 0xe);

    verifyBufferState2(false, 11, 'A', true, 3);
    zstok_endToken(testWriter);

    uint8_t exp4[] = { 'A', 3, 0x5, 0xda, 0x3e, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith5NibbleValue", testWriter, exp4, sizeof exp4);
}

void shouldTokenize2NumericFields() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_endToken(testWriter);
    zstok_startToken(testWriter, 'B', true);
    verifyBufferState2(false, 11, 'B', false, 0);
    zstok_endToken(testWriter);

    uint8_t exp1[] = { 'A', 0, 'B', 0, 0 };
    assertStartsWith("shouldTokenize2NumericFields", testWriter, exp1, sizeof exp1);
    verifyBufferState1(true, 11);
}

void shouldTokenize2NumericFieldsWithValues() {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 5);
    zstok_endToken(testWriter);

    zstok_startToken(testWriter, 'B', true);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 1, 5, 'B', 0, 0 };
    assertStartsWith("shouldTokenize2NumericFieldsWithValues", testWriter, exp2, sizeof exp2);
    verifyBufferState1(true, 10);
}

void shouldTokenizeNonNumericField() {
    setup();
    zstok_startToken(testWriter, '+', false);
    verifyBufferState2(false, 13, '+', false, 0);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { '+', 0, 0 };
    assertStartsWith("shouldTokenizeNonNumericField", testWriter, exp3, sizeof exp3);
    verifyBufferState1(true, 13);
}

void shouldTokenize2NonNumericFields() {
    setup();
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp4[] = { '+', 0, '+', 0, 0 };
    assertStartsWith("shouldTokenize2NonNumericFields", testWriter, exp4, sizeof exp4);
}

void shouldTokenize2NonNumericFieldsWithValues() {
    setup();
    insertNonNumericTokenNibbles('+', 2, 5, 0xa);
    zstok_endToken(testWriter);
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp5[] = { '+', 1, 0x5a, '+', 0, 0 };
    assertStartsWith("shouldTokenize2NonNumericFieldsWithValues", testWriter, exp5, sizeof exp5);
}

void shouldTokenizeMixedFieldsWithOddNibbleValues() {
    setup();
    insertNumericTokenNibbles('A', 3, 5, 0xa, 0xb);
    verifyBufferState2(false, 12, 'A', true, 2);
    zstok_endToken(testWriter);

    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp1[] = { 'A', 2, 0x5, 0xab, '+', 0, 0 };
    assertStartsWith("shouldTokenizeMixedFieldsWithOddNibbleValues", testWriter, exp1, sizeof exp1);
    verifyBufferState1(true, 9);
}

void shouldTokenizeMixedFieldsWithValues() {
    setup();
    insertNumericTokenNibbles('A', 3, 5, 0xa, 0xb);
    zstok_endToken(testWriter);

    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 2, 0x5, 0xab, '+', 0, 0 };
    assertStartsWith("shouldTokenizeMixedFieldsWithValues", testWriter, exp2, sizeof exp2);
}

void shouldTokenizeNonNumericFieldWithOddNibbles() {
    setup();
    insertNonNumericTokenNibbles('+', 3, 0xa, 0xb, 0xc);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { '+', 2, 0xab, 0xc0 };
    assertStartsWith("shouldTokenizeNonNumericFieldWithOddNibbles", testWriter, exp3, sizeof exp3);
    verifyBufferState1(true, 11);
}

void shouldWriteBufferOverflowOnTokenKey() {
    setup();
    insertByteToken(5);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 8);

    zstok_startToken(testWriter, 'A', true);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp1[] = { '+', 5, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, ERROR_BUFFER_OVERRUN, 0x0 };
    assertStartsWith("shouldWriteBufferOverflowOnTokenKey", testWriter, exp1, sizeof exp1);
}

void shouldWriteBufferOverflowOnTokenData() {
    setup();
    insertByteToken(3);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 10);

    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenByte(testWriter, 0x32);
    zstok_continueTokenByte(testWriter, 0x33);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp2[] = { '+', 3, 0xa0, 0xa1, 0xa2, ERROR_BUFFER_OVERRUN, 2, 0x32, 0x33 };
    assertStartsWith("shouldWriteBufferOverflowOnTokenData", testWriter, exp2, sizeof exp2);
}

void shouldTokenizeContinuedBigField() {
    uint8_t bigBufferSpace[300];
    ZStok_TokenBuffer bigRingBuffer;
    Zs_TokenWriter bigWriter;
    zstok_initTokenBuffer(&bigRingBuffer, bigBufferSpace, sizeof bigBufferSpace);
    bigWriter = zstok_getTokenWriter(&bigRingBuffer);

    zstok_startToken(bigWriter, '+', false);
    for (int i = 0; i < 258; i++) {
        zstok_continueTokenNibble(bigWriter, 4);
        zstok_continueTokenNibble(bigWriter, i & 0xf);
    }
    zstok_endToken(bigWriter);

    uint8_t exp1[] = { '+', 255, 64, 65, };
    uint8_t exp2[] = { 77, 78, 129, 3, 79, 64, 65, 0 };

    assertStartsWith("shouldTokenizeContinuedBigField", bigWriter, exp1, sizeof exp1);
//    assertContainsAt("shouldTokenizeContinuedBigField", bigWriter, 250, exp2, sizeof exp2);
}

int main() {
    shouldInitialize();
    shouldTokenizeNumericFieldWithNoValue();
    shouldTokenizeNumericFieldWithNibbleValue();
    shouldTokenizeNumericFieldWithByteValue();
    shouldTokenizeNumericFieldWith2ByteValue();
    shouldFailToAcceptByteIfNoTokenStarted();
    shouldFailToAcceptNibbleIfNoTokenStarted();
    shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue();
    shouldTokenizeNumericFieldWith2NibbleValue();
    shouldHandleFailingMidNibble();
    shouldHandleFailingMidToken();
    shouldTokenizeNumericFieldWith3NibbleValue();
    shouldTokenizeNumericFieldWith4NibbleValue();
    shouldTokenizeNumericFieldWith5NibbleValue();
    shouldTokenize2NumericFields();
    shouldTokenize2NumericFieldsWithValues();
    shouldTokenizeNonNumericField();
    shouldTokenize2NonNumericFields();
    shouldTokenize2NonNumericFieldsWithValues();
    shouldTokenizeMixedFieldsWithOddNibbleValues();
    shouldTokenizeMixedFieldsWithValues();
    shouldTokenizeNonNumericFieldWithOddNibbles();
    shouldWriteBufferOverflowOnTokenKey();
    shouldWriteBufferOverflowOnTokenData();
    shouldTokenizeContinuedBigField();
}
