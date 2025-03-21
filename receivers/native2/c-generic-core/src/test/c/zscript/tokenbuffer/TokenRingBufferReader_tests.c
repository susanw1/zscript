// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <string.h>

#include <zscript/tokenbuffer/TokenRingBufferWriter_defs.h>
#include <zscript/tokenbuffer/TokenRingBufferReader_defs.h>

#include "../testing/TestTools.h"

static const uint8_t CMD_END_ANDTHEN      = 0xe1;
static const uint8_t NORMAL_SEQUENCE_END  = 0xf0;
static const uint8_t ERROR_BUFFER_OVERRUN = 0xf1;

// handy test instances used by most tests
#define TEST_BUF_LEN        1024
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

void checkToken(const char *msg, ZStok_ReadToken tok, uint8_t key, uint8_t len, uint16_t value) {
    assertEquals(msg, zstok_getReadTokenKey(tok), key);
    assertEquals(msg, zstok_getReadTokenDataLength(tok), len);
    assertEquals(msg, zstok_getReadTokenData16(tok), value);
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
}

void shouldReadSingleToken(void) {
    setup();
    assertFalse("shouldReadSingleToken:hasReadToken before", zstok_hasReadToken(&testRingBuffer));
    insertNumericTokenNibbles('A', 3, 6, 7, 8);
    assertFalse("shouldReadSingleToken:hasReadToken during", zstok_hasReadToken(&testRingBuffer));
    zstok_endToken(testWriter);
    assertTrue("shouldReadSingleToken:hasReadToken after", zstok_hasReadToken(&testRingBuffer));

    const ZStok_ReadToken tok = zstok_getFirstReadToken(&testRingBuffer);
    assertTrue("shouldReadSingleToken:zstok_getFirstReadToken", zstok_isValidReadToken(tok));

    assertEquals("shouldReadSingleToken:zstok_getReadTokenKey", zstok_getReadTokenKey(tok), 'A');
    assertEquals("shouldReadSingleToken:DataLength", zstok_getReadTokenDataLength(tok), 2);
    assertEquals("shouldReadSingleToken:Data16", zstok_getReadTokenData16(tok), 0x0678);
    const ZStok_ReadToken tok2 = zstok_getNextReadToken(tok);
    assertFalse("shouldReadSingleToken:zstok_getNextReadToken", zstok_isValidReadToken(tok2));
}

void shouldReadNoTokenAtStart(void) {
    setup();
    assertFalse("shouldReadSingleToken:hasReadToken before", zstok_hasReadToken(&testRingBuffer));
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    assertFalse("shouldReadSingleToken:empty token1", zstok_isValidReadToken(tok1));

    const ZStok_ReadToken tok2 = zstok_getNextReadToken(tok1);
    assertFalse("shouldReadSingleToken:empty token2", zstok_isValidReadToken(tok1));
}


void shouldReadSingleTokenAfterMarker(void) {
    setup();
    zstok_writeMarker(testWriter, NORMAL_SEQUENCE_END);
    insertNumericTokenNibbles('Z', 3, 0xf, 0xf, 0xf);
    zstok_endToken(testWriter);
    zstok_writeMarker(testWriter, NORMAL_SEQUENCE_END);
    assertTrue("shouldReadSingleTokenAfterMarker:hasReadToken after", zstok_hasReadToken(&testRingBuffer));

    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    assertTrue("shouldReadSingleTokenAfterMarker:isReadTokenSequenceEndMarker", zstok_isReadTokenSequenceEndMarker(tok1));
    checkToken("shouldReadSingleToken:token1", tok1, NORMAL_SEQUENCE_END, 0, 0);

    const ZStok_ReadToken tok2 = zstok_getNextReadToken(tok1);
    checkToken("shouldReadSingleTokenAfterMarker:token2", tok2, 'Z', 2, 0x0fff);

    const ZStok_ReadToken tok3 = zstok_getNextReadToken(tok2);
    checkToken("shouldReadSingleTokenAfterMarker:token3", tok3, NORMAL_SEQUENCE_END, 0, 0);

    const ZStok_ReadToken endTok = zstok_getNextReadToken(tok3);
    assertTrue("shouldReadSingleTokenAfterMarker:endTok-valid", !zstok_isValidReadToken(endTok));
    checkToken("shouldReadSingleTokenAfterMarker:endTok", endTok, 0, 0, 0);
}

void shouldReadNoTokenAfterFlush(void) {
    setup();
    insertNumericTokenNibbles('A', 3, 0xf, 0xf, 0xf);
    zstok_endToken(testWriter);
    insertNumericTokenNibbles('B', 4, 0xe, 0xe, 0xe, 0xe);
    zstok_endToken(testWriter);

    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    checkToken("shouldReadNoTokenAfterFlush:tok1", tok1, 'A', 2, 0xfff);
    const ZStok_ReadToken tok2 = zstok_getNextReadTokenAndFlush(tok1);
    checkToken("shouldReadNoTokenAfterFlush:tok2", tok2, 'B', 2, 0xeeee);
    const ZStok_ReadToken tok2a = zstok_getFirstReadToken(&testRingBuffer);
    assertEquals("shouldReadNoTokenAfterFlush:check new", tok2.index, tok2a.index);
}


int main(void) {
    shouldReadSingleToken();
    shouldReadNoTokenAtStart();
    shouldReadSingleTokenAfterMarker();
    shouldReadNoTokenAfterFlush();
}
