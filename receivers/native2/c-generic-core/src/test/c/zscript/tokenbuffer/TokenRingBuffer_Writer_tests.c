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

uint8_t           testBufferSpace[TEST_BUF_LEN];
ZStok_TokenBuffer testRingBuffer;
Zs_TokenWriter    testWriter;

void setup(void) {
    zs_errno = 0; // Clear error state
    memset(testBufferSpace, 0, TEST_BUF_LEN);
    zstok_initTokenBuffer(&testRingBuffer, testBufferSpace, TEST_BUF_LEN);
    testWriter = zstok_getTokenWriter(&testRingBuffer);
}

void checkErrno(void) {
    if (zs_errno) {
        fprintf(stderr, "*********** ERRNO IS SET: %d\n", zs_errno);
        exit(1);
    }
}

void assertEquals(const char *msg, int p1, int p2) {
    if (p1 != p2) {
        fprintf(stderr, "*********** Unexpected mismatch: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertNotEquals(const char *msg, int p1, int p2) {
    if (p1 == p2) {
        fprintf(stderr, "*********** Unexpected match: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertTrue(const char *msg, bool expected) {
    if (!expected) {
        fprintf(stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}

void assertFalse(const char *msg, bool expected) {
    if (expected) {
        fprintf(stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}

void assertContainsAt(const char *msg, Zs_TokenWriter tbw, zstok_bufsz_t start, uint8_t contents[], zstok_bufsz_t len) {
    checkErrno();
    uint8_t *datap = tbw.tokenBuffer->data;

    for (zstok_bufsz_t i = 0; i < len; i++) {
        uint8_t c = datap[start + i];
        if (c != contents[i]) {
//            fprintf (stderr, "  Buffer: ");
//            for (zstok_bufsz_t t = 0; t < tbw.tokenBuffer->bufLen; t++) {
//                fprintf (stderr, "%02x ", datap[zstok_offset(tbw.tokenBuffer, 0, t)]);
//            }

            fprintf(stderr, "\n\nExpected: ");
            for (zstok_bufsz_t t = 0; t < len; t++) {
                fprintf(stderr, "%02x ", contents[t]);
            }
            fprintf(stderr, "\n  Actual: ");
            for (zstok_bufsz_t t = 0; t < len; t++) {
                fprintf(stderr, "%02x ", datap[zstok_offset(tbw.tokenBuffer, start, t)]);
            }
            fprintf(stderr, "\n          ");
            for (zstok_bufsz_t t = 0; t < i; t++) {
                fprintf(stderr, "   ");
            }
            fprintf(stderr, "^^\n*********** Expected=%02x(%c), actual=%02x(%c); pos=%d: %s\n", contents[i], contents[i], c, (c < ' ' ? '?' : c), i, msg);
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

void shouldTokenizeNumericFieldWithNoValue(void) {
    setup();
    zstok_startToken(testWriter, 'A', true);
    uint8_t exp1[] = { 'A', 0, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithNoValue", testWriter, exp1, sizeof exp1);
    verifyBufferState2(false, 13, 'A', false, 0);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 13);
}

void shouldTokenizeNumericFieldWithNibbleValue(void) {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 5);

    verifyBufferState2(false, 13, 'A', true, 1);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 1, 5, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithNibbleValue", testWriter, exp2, sizeof exp2);
    verifyBufferState1(true, 12);
}

void shouldTokenizeNumericFieldWithByteValue(void) {
    setup();
    insertNumericToken('A', 1, 234); // 1 is the number of vararg bytes following, not actually the datalen!

    uint8_t exp3[] = { 'A', 1, 234, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWithByteValue", testWriter, exp3, sizeof exp3);
    verifyBufferState2(false, 12, 'A', false, 1);
}

void shouldTokenizeNumericFieldWith2ByteValue(void) {
    setup();
    insertNumericToken('A', 2, 234, 123);

    uint8_t exp4[] = { 'A', 2, 234, 123, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith2ByteValue", testWriter, exp4, sizeof exp4);
    verifyBufferState2(false, 11, 'A', false, 2);
}

void shouldFailToAcceptByteIfNoTokenStarted(void) {
    setup();
    zstok_continueTokenByte(testWriter, 0xA2);
    assertEquals("shouldFailToAcceptByteIfNoTokenStarted:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 15);
}

void shouldFailToAcceptNibbleIfNoTokenStarted(void) {
    setup();
    zstok_continueTokenNibble(testWriter, 3);
    assertEquals("shouldFailToAcceptNibbleIfNoTokenStarted:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 15);
}

void shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue(void) {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 3);
    zstok_continueTokenByte(testWriter, 123);
    assertEquals("shouldFailToTokenizeNumericFieldWithOddNibbleAndByteValue", ZS_SystemErrorType_TOKBUF_STATE_IN_NIBBLE, zs_errno);
    verifyBufferState2(false, 13, 'A', true, 1);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 12);
}

void shouldTokenizeNumericFieldWith2NibbleValue(void) {
    setup();
    insertNumericTokenNibbles('A', 2, 5, 0xd);
    assertEquals("zs_errno", 0, zs_errno);
    uint8_t exp1[] = { 'A', 1, 0x5d, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith2NibbleValue", testWriter, exp1, sizeof exp1);
}

void shouldHandleFailingMidNibble(void) {
    setup();
    insertNumericTokenNibbles('C', 1, 6);
    insertNumericTokenNibbles('A', 1, 5);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp2[] = { 'C', 1, 6, ERROR_BUFFER_OVERRUN };
    assertStartsWith("shouldHandleFailingMidNibble", testWriter, exp2, sizeof exp2);
}

void shouldHandleFailingMidToken(void) {
    setup();
    insertNumericTokenNibbles('C', 1, 6);
    insertNumericTokenNibbles('A', 2, 5, 2);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp1[] = { 'C', 1, 6, ERROR_BUFFER_OVERRUN };
    assertStartsWith("shouldHandleFailingMidToken", testWriter, exp1, sizeof exp1);
}

void shouldTokenizeNumericFieldWith3NibbleValue(void) {
    setup();
    insertNumericTokenNibbles('A', 3, 5, 0xd, 0xa);

    verifyBufferState2(false, 12, 'A', true, 2);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 2, 0x5, 0xda, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith3NibbleValue", testWriter, exp2, sizeof exp2);
}

void shouldTokenizeNumericFieldWith4NibbleValue(void) {
    setup();
    insertNumericTokenNibbles('A', 4, 5, 0xd, 0xa, 0x3);

    verifyBufferState2(false, 11, 'A', false, 2);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { 'A', 2, 0x5d, 0xa3, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith4NibbleValue", testWriter, exp3, sizeof exp3);
}

void shouldTokenizeNumericFieldWith5NibbleValue(void) {
    setup();
    insertNumericTokenNibbles('A', 5, 5, 0xd, 0xa, 0x3, 0xe);

    verifyBufferState2(false, 11, 'A', true, 3);
    zstok_endToken(testWriter);

    uint8_t exp4[] = { 'A', 3, 0x5, 0xda, 0x3e, 0 };
    assertStartsWith("shouldTokenizeNumericFieldWith5NibbleValue", testWriter, exp4, sizeof exp4);
}

void shouldTokenize2NumericFields(void) {
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

void shouldTokenize2NumericFieldsWithValues(void) {
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

void shouldTokenizeNonNumericField(void) {
    setup();
    zstok_startToken(testWriter, '+', false);
    verifyBufferState2(false, 13, '+', false, 0);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { '+', 0, 0 };
    assertStartsWith("shouldTokenizeNonNumericField", testWriter, exp3, sizeof exp3);
    verifyBufferState1(true, 13);
}

void shouldTokenize2NonNumericFields(void) {
    setup();
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp4[] = { '+', 0, '+', 0, 0 };
    assertStartsWith("shouldTokenize2NonNumericFields", testWriter, exp4, sizeof exp4);
}

void shouldTokenize2NonNumericFieldsWithValues(void) {
    setup();
    insertNonNumericTokenNibbles('+', 2, 5, 0xa);
    zstok_endToken(testWriter);
    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp5[] = { '+', 1, 0x5a, '+', 0, 0 };
    assertStartsWith("shouldTokenize2NonNumericFieldsWithValues", testWriter, exp5, sizeof exp5);
}

void shouldTokenizeMixedFieldsWithOddNibbleValues(void) {
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

void shouldTokenizeMixedFieldsWithValues(void) {
    setup();
    insertNumericTokenNibbles('A', 3, 5, 0xa, 0xb);
    zstok_endToken(testWriter);

    zstok_startToken(testWriter, '+', false);
    zstok_endToken(testWriter);

    uint8_t exp2[] = { 'A', 2, 0x5, 0xab, '+', 0, 0 };
    assertStartsWith("shouldTokenizeMixedFieldsWithValues", testWriter, exp2, sizeof exp2);
}

void shouldTokenizeNonNumericFieldWithOddNibbles(void) {
    setup();
    insertNonNumericTokenNibbles('+', 3, 0xa, 0xb, 0xc);
    zstok_endToken(testWriter);

    uint8_t exp3[] = { '+', 2, 0xab, 0xc0 };
    assertStartsWith("shouldTokenizeNonNumericFieldWithOddNibbles", testWriter, exp3, sizeof exp3);
    verifyBufferState1(true, 11);
}

void shouldWriteBufferOverflowOnTokenKey(void) {
    setup();
    insertByteToken(5);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 8);

    zstok_startToken(testWriter, 'A', true);
    zstok_fail(testWriter, ERROR_BUFFER_OVERRUN);

    uint8_t exp1[] = { '+', 5, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, ERROR_BUFFER_OVERRUN, 0x0 };
    assertStartsWith("shouldWriteBufferOverflowOnTokenKey", testWriter, exp1, sizeof exp1);
}

void shouldWriteBufferOverflowOnTokenData(void) {
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

// handy test that checks continuation tokens in both text and nibble modes
void shouldTokenizeContinuedBigField_util(bool useTextMode) {
    const int BIG_BUF_LEN = 512;
    uint8_t   bigBufferSpace[BIG_BUF_LEN];
    memset(bigBufferSpace, 0, BIG_BUF_LEN);

    ZStok_TokenBuffer bigRingBuffer;
    zstok_initTokenBuffer(&bigRingBuffer, bigBufferSpace, sizeof bigBufferSpace);
    Zs_TokenWriter bigWriter = zstok_getTokenWriter(&bigRingBuffer);

    zstok_startToken(bigWriter, '+', false);
    for (zstok_bufsz_t i = 0; i < 258; i++) {
        if (useTextMode) {
            zstok_continueTokenByte(bigWriter, 0x40 + (i & 0xf));
        } else {
            zstok_continueTokenNibble(bigWriter, 4);
            zstok_continueTokenNibble(bigWriter, i & 0xf);
        }
    }
    zstok_endToken(bigWriter);

    // verify that we didn't run out of space
    checkErrno();
    uint8_t exp1[] = { '+', 255, 64, 65, };
    assertStartsWith("shouldTokenizeContinuedBigField - start", bigWriter, exp1, sizeof exp1);

    uint8_t exp2[] = { 0x4c, 0x4d, 0x4e, 0x81, 0x3, 0x4f, 0x40, 0x41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    assertContainsAt("shouldTokenizeContinuedBigField - cont", bigWriter, 254, exp2, sizeof exp2);
}

void shouldTokenizeContinuedTextBigField(void) {
    shouldTokenizeContinuedBigField_util(true);
}

void shouldTokenizeContinuedHexBigField(void) {
    shouldTokenizeContinuedBigField_util(false);
}

void shouldFailOnHugeNumeric(void) {
    const int BIG_BUF_LEN = 512;
    uint8_t   bigBufferSpace[BIG_BUF_LEN];
    memset(bigBufferSpace, 0, BIG_BUF_LEN);

    ZStok_TokenBuffer bigRingBuffer;
    zstok_initTokenBuffer(&bigRingBuffer, bigBufferSpace, sizeof bigBufferSpace);
    Zs_TokenWriter bigWriter = zstok_getTokenWriter(&bigRingBuffer);

    zstok_startToken(bigWriter, 'A', true);
    for (zstok_bufsz_t i = 0; i < 255; i++) {
        zstok_continueTokenByte(bigWriter, 0x40 + (i & 0xf));
    }
    checkErrno();
    zstok_continueTokenByte(bigWriter, 'x');
    assertEquals("shouldFailOnHugeNumeric:errno", ZS_SystemErrorType_TOKBUF_STATE_NUMERIC_TOO_LONG, zs_errno);
}


void shouldFailToAcceptNibbleOutOfRange(void) {
    setup();
    zstok_startToken(testWriter, 'A', true);
    zstok_continueTokenNibble(testWriter, 17);
    assertEquals("shouldFailToAcceptNibbleOutOfRange:errno", ZS_SystemErrorType_TOKBUF_ARG_NIBBLE_RANGE, zs_errno);
    verifyBufferState1(false, 13);
    zstok_endToken(testWriter);
    verifyBufferState1(true, 13);
}

void shouldFailToGetTokenKeyWhenNoTokenInPlay(void) {
    setup();
    zstok_getCurrentWriteTokenKey(testWriter);
    assertEquals("shouldFailToGetTokenKeyWhenNoTokenInPlay:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
}

void shouldFailToGetTokenLengthWhenNoTokenInPlay(void) {
    setup();
    zstok_getCurrentWriteTokenLength(testWriter);
    assertEquals("shouldFailToGetTokenLengthWhenNoTokenInPlay:errno", ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN, zs_errno);
}

void shouldFailToWriteNonMarkerAsMarker(void) {
    setup();
    zstok_writeMarker(testWriter, 'A');
    assertEquals("shouldFailToWriteNonMarkerAsMarker:errno", ZS_SystemErrorType_TOKBUF_ARG_NOT_MARKER, zs_errno);
}

void shouldFailToWriteMarkerAsToken(void) {
    setup();
    zstok_startToken(testWriter, ERROR_BUFFER_OVERRUN, true);
    assertEquals("shouldFailToWriteMarkerAsToken:errno", ZS_SystemErrorType_TOKBUF_ARG_NOT_TOKENKEY, zs_errno);
}

void shouldFailOnOverrun(void) {
    setup();
    zstok_startToken(testWriter, 'A', true);
    for (zstok_bufsz_t i = 0; i < 13; i++) {
        zstok_continueTokenByte(testWriter, 0x40 + (i & 0xf));
    }
    checkErrno();
    zstok_continueTokenByte(testWriter, 'x');
    assertEquals("shouldFailOnOverrun:errno", ZS_SystemErrorType_TOKBUF_FATAL_OVERFLOW, zs_errno);
}


int main(void) {
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
    shouldTokenizeContinuedHexBigField();
    shouldTokenizeContinuedTextBigField();
    shouldFailOnHugeNumeric();
    shouldFailToAcceptNibbleOutOfRange();
    shouldFailToGetTokenKeyWhenNoTokenInPlay();
    shouldFailToGetTokenLengthWhenNoTokenInPlay();
    shouldFailToWriteNonMarkerAsMarker();
    shouldFailToWriteMarkerAsToken();
    shouldFailOnOverrun();
}
