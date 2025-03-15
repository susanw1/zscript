/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifdef _ZS_TOKENBUFFER_DEFS_H
#error _ZS_TOKENBUFFER_DEFS_H already defined - file should only be included once
#endif
#define _ZS_TOKENBUFFER_DEFS_H

#include <stdio.h>
#include "TokenRingBuffer.h"
#include "TokenBufferFlags_defs.h"

typedef uint16_t    zstok_tokenBufferSize_t;

/**
 * Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
 */
const uint8_t ZStok_TOKEN_EXTENSION = 0x81;


/**
 * Array based implementation of a TokenBuffer - the tokens making up incoming command or response sequences are encoded and accessed here. Rules are:
 * <ol>
 * <li>There is a writable area, owned by a TokenWriter, in the space <i>writeStart &lt;= i &lt; readStart</i>.</li>
 * <li>There is a readable area, owned by a TokenIterator, in the space <i>readStart &lt;= i &lt; writeStart</i>.</li>
 * <li>A token is written as &gt;=2 bytes at <i>writeStart</i>: <code>key | datalen | [data]</code> - so tokens can be iterated by adding (datalen+2) to an existing token start.</li>
 * <li>A marker is written as 1 byte at <i>writeStart</i>, indicating a dataless key - markers are identified as a key with top 3 bits set (eg from 0xe0-0xff).</li>
 * <li>Tokens may exceed datalen of 255 using additional new token with special key <i>TOKEN_EXTENSION</i></li>
 * </ol>
 */
typedef struct zstok__TokenBuffer {
    // fixed pointer to the token memory buffer, of size bufLen
    uint8_t                 *buffer;
    zstok_tokenBufferSize_t    bufLen;

    zstok_tokenBufferSize_t    readStart;
    zstok_tokenBufferSize_t    writeStart;

    ZStok_BufferFlags flags;

    // Additional writer state:
    /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
    zstok_tokenBufferSize_t    writeLastLen;
    /** the current write index into data array */
    zstok_tokenBufferSize_t    writeCursor;

    bool inNibble :1;
    bool numeric :1;
} ZStok_TokenBuffer;


typedef struct {
    ZStok_TokenBuffer      *tokenBuffer;
} Zs_TokenWriter;


static void zstok_initTokenBuffer(ZStok_TokenBuffer *tb, uint8_t *bufSpace, zstok_tokenBufferSize_t bufLen) {

    tb->buffer = bufSpace;
    tb->bufLen = bufLen;
    tb->readStart = 0;
    tb->writeStart = 0;
    tb->writeLastLen = 0;
    tb->writeCursor = 0;
    zstok_initBufferFlags(&tb->flags);
}

/**
 * Determines whether the supplied byte is a valid "marker" key (including sequence marker).
 *
 * @param key any byte
 * @return true if it's a marker; false otherwise
 */
static bool zstok_isMarker(uint8_t key) {
    return (key & 0xe0) == 0xe0;
}

/**
 * Determines whether the supplied byte is a sequence-end "marker" key (newline, fatal error).
 *
 * @param key any byte
 * @return true if it's a sequence-end marker; false otherwise
 */
static bool zstok_isSequenceEndMarker(uint8_t key) {
    return (key & 0xf0) == 0xf0;
}




//class TokenRingBuffer;
//class RingBufferTokenIterator;
//class RingBufferToken;
//class RawTokenBlockIterator;
//class TokenRingWriter
//class TokenRingReader {
