/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_TOKENBUFFER_H
#define _ZS_TOKENBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/misc/SysErrors.h>
#include "TokenBufferFlags.h"


// The max-length of the data associated with a single token - limited by the 1-byte length field. Longer data needs an extension token.
#define ZSTOK_MAX_TOKEN_DATA_LENGTH ((uint8_t) 255)

// Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
#define ZSTOK_TOKEN_EXTENSION ((uint8_t) 0x81)

#ifndef ZSTOK_DEFINED_BUFSZ_TYPE
#define ZSTOK_DEFINED_BUFSZ_TYPE 1

// represents the size of an index into the token buffer - may reasonably be uint16_t or uint8_t.
typedef uint16_t zstok_bufsz_t;
#endif

/**
 * Array ring-buffer implementation of a TokenBuffer - the tokens making up incoming sequences are encoded and accessed here. Rules are:
 * <ol>
 * <li>There is a writable area, owned by a single TokenWriter, in the space <i>writeStart &lt;= i &lt; readStart</i> (modulo bufLen).</li>
 * <li>There is a readable area, owned by a TokenIterator, in the space <i>readStart &lt;= i &lt; writeStart</i> (modulo bufLen).</li>
 * <li>A token is written as &gt;=2 bytes at <i>writeStart</i>: <code>key | datalen | [data]</code> - so tokens can be iterated by adding (datalen+2) to an existing token start.</li>
 * <li>A marker is written as 1 byte at <i>writeStart</i>, indicating a dataless key - markers are identified as a key with top 3 bits set (eg from 0xe0-0xff).</li>
 * <li>Tokens may exceed datalen of 255 using additional new token with special key <i>ZSTOK_TOKEN_EXTENSION</i></li>
 * </ol>
 */
typedef struct {
    // fixed pointer to the token memory buffer, of size bufLen
    uint8_t       *data;
    // length of usable data
    zstok_bufsz_t bufLen;

    zstok_bufsz_t     readStart;
    zstok_bufsz_t     writeStart;
    ZStok_BufferFlags flags;

    // Additional writer state:
    /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
    zstok_bufsz_t writeLastLen;
    /** the current write index into data array */
    zstok_bufsz_t writeCursor;
    /** are we in the middle of writing a 2-nibble value? */
    bool inNibble : 1;
    /** have we started writing a nibble-based "numeric" field (even or odd number of nibbles), or a strictly hex-pair field? */
    bool numeric : 1;

} ZStok_TokenBuffer;


/**
 * Type used for performing writes to the token-buffer. Zs_TokenWriter objects are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference. They exist in order to keep "reader"-side operations distinct from
 * "writer"-side ones. Their state lives in fields on the buffer itself.
 */
typedef struct {
    ZStok_TokenBuffer *tokenBuffer;
} Zs_TokenWriter;

/**
 * Type used for performing reads from the token-buffer. ZStok_TokenReader objects are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference. They exist in order to keep "reader"-side operations distinct from
 * "writer"-side ones. Their state lives in fields on the buffer itself.
 */
typedef struct {
    ZStok_TokenBuffer *tokenBuffer;
} ZStok_TokenReader;


// General TokenBuffer functions
static void zstok_initTokenBuffer(ZStok_TokenBuffer *tb, uint8_t *bufSpace, zstok_bufsz_t bufLen);

static bool zstok_isMarker(uint8_t key);

static bool zstok_isSequenceEndMarker(uint8_t key);

static zstok_bufsz_t zstok_offset(const ZStok_TokenBuffer *tb, zstok_bufsz_t index, zstok_bufsz_t offset);

static bool zstok_isInReadableArea(const ZStok_TokenBuffer *tb, zstok_bufsz_t index);

static Zs_TokenWriter zstok_getTokenWriter(ZStok_TokenBuffer *tb);

// Token writing operations
static void zstok_startToken(Zs_TokenWriter tbw, uint8_t key, bool numeric);

static void zstok_continueTokenNibble(Zs_TokenWriter tbw, uint8_t nibble);

static void zstok_continueTokenByte(Zs_TokenWriter tbw, uint8_t b);

static void zstok_endToken(Zs_TokenWriter tbw);

static void zstok_writeMarker(Zs_TokenWriter tbw, uint8_t key);

static void zstok_fail(Zs_TokenWriter tbw, uint8_t errorCode);

// status checks
static bool zstok_isInNibble(Zs_TokenWriter tbw);

static uint8_t zstok_getCurrentWriteTokenKey(Zs_TokenWriter tbw);

static zstok_bufsz_t zstok_getCurrentWriteTokenLength(Zs_TokenWriter tbw);

static bool zstok_isTokenComplete(Zs_TokenWriter tbw);

static bool zstok_checkAvailableCapacity(Zs_TokenWriter tbw, zstok_bufsz_t size);


#endif // _ZS_TOKENBUFFER_H
