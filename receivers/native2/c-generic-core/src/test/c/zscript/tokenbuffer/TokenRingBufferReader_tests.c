// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <string.h>

#include <zscript/tokenbuffer/TokenRingBufferWriter_defs.h>
#include <zscript/tokenbuffer/TokenRingBufferReader_defs.h>

#include "../testing/TestTools.h"

static const uint8_t NORMAL_SEQUENCE_END = 0xf0;

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


void assertBufferContainsAt(const char *msg, ZStok_TokenWriter tbw, zstok_bufsz_t start, const uint8_t contents[], zstok_bufsz_t len) {
    checkErrno();
    if (start + len > tbw.tokenBuffer->bufLen) {
        fprintf(stderr, "start(%d) + len(%d) goes off the end(%d)!\n", start, len, tbw.tokenBuffer->bufLen);
        exit(1);
    }
    assertContains(msg, tbw.tokenBuffer->data + start, contents, len);
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

void insertByteToken(int count, uint8_t firstByte) {
    zstok_startToken(testWriter, '+', false);

    for (int i = 0; i < count; i++) {
        zstok_continueTokenByte(testWriter, firstByte + (i % 50));
    }
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
    assertFalse("shouldReadSingleToken:empty token2", zstok_isValidReadToken(tok2));
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

void shouldSafelyFlushWithNoTokens(void) {
    setup();
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    assertFalse("shouldSafelyFlushWithNoTokens:tok1-valid", zstok_isValidReadToken(tok1));
    const ZStok_ReadToken tok2 = zstok_getNextReadTokenAndFlush(tok1);
    assertFalse("shouldSafelyFlushWithNoTokens:tok2-valid", zstok_isValidReadToken(tok2));
}

/**
 * Test adds:
 * * a 260 byte token (so, extended once: 0-256,257:263), but flushed
 * * a 750 byte token (extended 3 times: 264:520,521:777,778:1019)
 * * a 258 byte token (extended once and wrapped, 1020:252,253:257, only after flushing first token)
 * * a final marker (258)
 */
void setupExtendedTokenScenario_priv(void) {
    // assumes buffer is 1024 bytes long
    setup();
    insertByteToken(260, 'A');
    zstok_endToken(testWriter);
    uint8_t exp1[] = { '+', 0xff, 'A', 'B' };
    assertBufferContainsAt("shouldReadExtendedTokens:insertByteToken check#1", testWriter, 0, exp1, sizeof exp1);

    insertByteToken(750, 'B');
    zstok_endToken(testWriter);
    uint8_t exp2[] = { 'p', 'q', 'r', 's', 0 };
    assertBufferContainsAt("shouldReadExtendedTokens:insertByteToken check#2", testWriter, 1016, exp2, sizeof exp2);

    ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    assertEquals("shouldReadExtendedTokens:tok1", zstok_getReadTokenDataLength(tok1), 260);
    assertEquals("shouldReadExtendedTokens:tok1-seg", zstok_getSegmentDataSize_priv(tok1), 255);

    ZStok_ReadToken tok2 = zstok_getNextReadTokenAndFlush(tok1);
    assertEquals("shouldReadExtendedTokens:tok2", zstok_getReadTokenDataLength(tok2), 750);

    const ZStok_ReadToken endTok = zstok_getNextReadToken(tok2);
    assertTrue("shouldReadExtendedTokens:endTok-valid", !zstok_isValidReadToken(endTok));

    insertByteToken(258, 'C');
    zstok_endToken(testWriter);
    uint8_t exp3a[] = { 'r', 's', '+', 0xff, 'C', 'D' };
    assertBufferContainsAt("shouldReadExtendedTokens:insertByteToken check#3a", testWriter, 1018, exp3a, sizeof exp3a);
    uint8_t exp3b[] = { 'E', 'F', 'G', 'H' };
    assertBufferContainsAt("shouldReadExtendedTokens:insertByteToken check#3b", testWriter, 0, exp3b, sizeof exp3b);

    const ZStok_ReadToken tok3 = zstok_getNextReadToken(tok2);
    assertTrue("shouldReadExtendedTokens:tok3-valid", zstok_isValidReadToken(tok3));
    assertEquals("shouldReadExtendedTokens:tok3", zstok_getReadTokenDataLength(tok3), 258);

    zstok_writeMarker(testWriter, NORMAL_SEQUENCE_END);

    // Includes tok3's extension, the marker (0xf0), and the detritus from tok1
    const uint8_t exp4[] = { 'E', 'F', 'G', 0x81, 3, 'H', 'I', 'J', 0xf0, 'F', 'G', 'H' };
    assertBufferContainsAt("shouldReadExtendedTokens:insertByteToken check#4", testWriter, 250, exp4, sizeof exp4);
}

void shouldReadExtendedTokens(void) {
    setupExtendedTokenScenario_priv();
}

void shouldFlushCorrectlyWithWrappedToken(void) {
    setupExtendedTokenScenario_priv();
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    assertEquals("shouldFlushCorrectlyWithWrappedToken:tok1", zstok_getReadTokenDataLength(tok1), 750);
    const ZStok_ReadToken tok2 = zstok_getNextReadTokenAndFlush(tok1);
    assertEquals("shouldFlushCorrectlyWithWrappedToken:tok2", zstok_getReadTokenDataLength(tok2), 258);
    const ZStok_ReadToken tok3 = zstok_getNextReadTokenAndFlush(tok2);
    assertEquals("shouldFlushCorrectlyWithWrappedToken:tok3 marker", zstok_getReadTokenDataLength(tok3), 0);
    assertTrue("shouldFlushCorrectlyWithWrappedToken:tok3-valid", zstok_isValidReadToken(tok3));
    const ZStok_ReadToken endTok = zstok_getNextReadTokenAndFlush(tok3);
    assertFalse("shouldFlushCorrectlyWithWrappedToken:endTok-empty", zstok_isValidReadToken(endTok));
    // and just to make sure, do it once more!
    const ZStok_ReadToken endTok2 = zstok_getNextReadTokenAndFlush(endTok);
    assertFalse("shouldFlushCorrectlyWithWrappedToken:endTok2-empty", zstok_isValidReadToken(endTok2));
}


void shouldIterateNoTokens(void) {
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    ZStok_BlockIterator   it   = zstok_getReadTokenDataIterator(tok1);
    assertFalse("shouldIterateNoTokens:hasNext", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block = zstok_nextContiguousIteratorBlock(&it, 100);
    assertTrue("shouldIterateNoTokens:next", block.data == NULL);
    assertEquals("shouldIterateNoTokens:next", block.length, 0);
    // This would be silly, but let's verify doing it again to ensure it's safe
    ZS_ByteString block2 = zstok_nextContiguousIteratorBlock(&it, 100);
    assertTrue("shouldIterateNoTokens:next", block2.data == NULL);
    assertEquals("shouldIterateNoTokens:next", block2.length, 0);
}

void shouldIterateSingleToken(void) {
    insertNonNumericTokenNibbles('+', 6, 1, 2, 3, 4, 5, 6);
    zstok_endToken(testWriter);

    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    ZStok_BlockIterator   it   = zstok_getReadTokenDataIterator(tok1);
    assertTrue("shouldIterateSingleToken:hasNext1", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block = zstok_nextContiguousIteratorBlock(&it, 100);
    assertTrue("shouldIterateSingleToken:next1", *block.data == 0x12);
    assertEquals("shouldIterateSingleToken:next1 len", block.length, 3);

    ZS_ByteString block2 = zstok_nextContiguousIteratorBlock(&it, 100);
    assertTrue("shouldIterateSingleToken:next2", block2.data == NULL);
    assertEquals("shouldIterateSingleToken:next2 len", block2.length, 0);
}

void shouldIterateSingleTokenWithMaxLength(void) {
    insertNonNumericTokenNibbles('+', 6, 1, 2, 3, 4, 5, 6);
    zstok_endToken(testWriter);

    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    ZStok_BlockIterator   it   = zstok_getReadTokenDataIterator(tok1);
    assertTrue("shouldIterateSingleToken:hasNext1", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block = zstok_nextContiguousIteratorBlock(&it, 2);
    assertEquals("shouldIterateSingleToken:next1", *block.data, 0x12);
    assertEquals("shouldIterateSingleToken:next1 len", block.length, 2);

    assertTrue("shouldIterateSingleToken:hasNext2", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block2 = zstok_nextContiguousIteratorBlock(&it, 2);
    assertEquals("shouldIterateSingleToken:next2", *block2.data, 0x56);
    assertEquals("shouldIterateSingleToken:next2 len", block2.length, 1);
}


void shouldIterateExtendedTokens(void) {
    setupExtendedTokenScenario_priv();

    // Dissect first scenario 750-byte token at 264:520,521:777,778:1019
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    ZStok_BlockIterator   it   = zstok_getReadTokenDataIterator(tok1);

    // 264:520: Read in 2 x chunks of 240
    assertTrue("shouldIterateExtendedTokens:hasNext1a", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block1a = zstok_nextContiguousIteratorBlock(&it, 240);
    assertEquals("shouldIterateExtendedTokens:next1a", *block1a.data, 'B');
    assertEquals("shouldIterateExtendedTokens:next1a len", block1a.length, 240);

    assertTrue("shouldIterateExtendedTokens:hasNext1b", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block1b = zstok_nextContiguousIteratorBlock(&it, 240);
    assertEquals("shouldIterateExtendedTokens:next1b", *block1b.data, 'j');
    assertEquals("shouldIterateExtendedTokens:next1b len", block1b.length, 15);

    // 521:777
    assertTrue("shouldIterateExtendedTokens:hasNext2", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block2 = zstok_nextContiguousIteratorBlock(&it, 255);
    assertEquals("shouldIterateExtendedTokens:next2", *block2.data, 'G');
    assertEquals("shouldIterateExtendedTokens:next2 len", block2.length, 255);

    // 778:1019
    assertTrue("shouldIterateExtendedTokens:hasNext2", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block3 = zstok_nextContiguousIteratorBlock(&it, 255);
    assertEquals("shouldIterateExtendedTokens:next3 len", block3.length, 240);

    ZS_ByteString empty = zstok_nextContiguousIteratorBlock(&it, 255);
    assertEquals("shouldIterateExtendedTokens:next4 len", empty.length, 0);
}

void shouldIterateWrappedTokens(void) {
    setupExtendedTokenScenario_priv();

    // Dissect second scenario 258-byte token at 1020:252,253:257 - reading in chunks of 128
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    const ZStok_ReadToken tok2 = zstok_getNextReadToken(tok1);

    ZStok_BlockIterator it = zstok_getReadTokenDataIterator(tok2);

    // 1020:1023 - first part up to end of buffer
    assertTrue("shouldIterateWrappedTokens:hasNext1", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block1 = zstok_nextContiguousIteratorBlock(&it, 128);
    const uint8_t exp1[] = { 'C', 'D' };
    assertContains("shouldIterateWrappedTokens:next1", block1.data, exp1, sizeof exp1);
    assertEquals("shouldIterateWrappedTokens:next1 len", block1.length, 2);

    // 0:252: truncated to 0:127 by maxLength
    assertTrue("shouldIterateWrappedTokens:hasNext2", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block2 = zstok_nextContiguousIteratorBlock(&it, 128);
    assertEquals("shouldIterateWrappedTokens:next2", *block2.data, 'E');
    assertEquals("shouldIterateWrappedTokens:next2 len", block2.length, 128);

    // 0:252 pt2: read 128:252
    assertTrue("shouldIterateWrappedTokens:hasNext3", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block3 = zstok_nextContiguousIteratorBlock(&it, 128);
    assertEquals("shouldIterateWrappedTokens:next3", *block3.data, 'a');
    assertEquals("shouldIterateWrappedTokens:next3 len", block3.length, 125);

    // last little bit: 253:257
    assertTrue("shouldIterateWrappedTokens:hasNext4", zstok_hasNextIteratorBlock(&it));
    ZS_ByteString block4 = zstok_nextContiguousIteratorBlock(&it, 128);
    assertEquals("shouldIterateWrappedTokens:next4", *block4.data, 'H');
    assertEquals("shouldIterateWrappedTokens:next4 len", block4.length, 3);

    assertFalse("shouldIterateWrappedTokens:hasNext5", zstok_hasNextIteratorBlock(&it));
}

/* *********************************************************
 * ForEach block tests
 ***********************************************************/

bool myLengthAccumAction(ZS_ByteString bytes, void *p) {
    *((int *) p) += bytes.length;
    return true;
}

void shouldIterateWithForEach(void) {
    setupExtendedTokenScenario_priv();

    const ZStok_ReadToken tok1     = zstok_getFirstReadToken(&testRingBuffer);
    int                   totalLen = 0;
    const bool            res      = zstok_forEachDataBlock(tok1, 255, &totalLen, myLengthAccumAction);
    assertTrue("shouldIterateWithForEach action result", res);
    assertEquals("shouldIterateWithForEach total", totalLen, 750);
}

void shouldIterateEmptyBufferWithForEach(void) {
    setup();
    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);
    const bool            res  = zstok_forEachDataBlock(tok1, 255, NULL, myLengthAccumAction);
    assertTrue("shouldIterateWithForEach action result", res);
}

struct foreach_testdata {
    int totalLen, count, maxCount;
};

// add up all the block lengths until we hit one that's longer than a max value, just as a demo
bool myEarlyTerminateAccumAction(ZS_ByteString bytes, void *p) {
    struct foreach_testdata *dp = (struct foreach_testdata *) p;
    if (dp->count < dp->maxCount) {
        dp->totalLen += bytes.length;
        dp->count++;
        return true;
    }
    return false;
}

void shouldTerminateForEachEarly(void) {
    setupExtendedTokenScenario_priv();

    const ZStok_ReadToken tok1 = zstok_getFirstReadToken(&testRingBuffer);

    // maxCount=2 should get the first two blocks, then stop
    struct foreach_testdata testData = { .totalLen= 0, .count=0, .maxCount=2 };
    const bool              res      = zstok_forEachDataBlock(tok1, 255, &testData, myEarlyTerminateAccumAction);
    assertFalse("shouldTerminateForEachEarly action result", res);
    assertEquals("shouldTerminateForEachEarly count", testData.count, 2);
    assertEquals("shouldTerminateForEachEarly total", testData.totalLen, 510);
}


int main(void) {
    shouldReadSingleToken();
    shouldReadNoTokenAtStart();
    shouldReadSingleTokenAfterMarker();
    shouldReadNoTokenAfterFlush();
    shouldSafelyFlushWithNoTokens();
    shouldReadExtendedTokens();
    shouldFlushCorrectlyWithWrappedToken();
    shouldIterateNoTokens();
    shouldIterateSingleToken();
    shouldIterateSingleTokenWithMaxLength();
    shouldIterateExtendedTokens();
    shouldIterateWrappedTokens();
    shouldIterateWithForEach();
    shouldIterateEmptyBufferWithForEach();
    shouldTerminateForEachEarly();
}
