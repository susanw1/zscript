// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFER_H
#define _ZS_TOKENBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/misc/SysErrors.h>
#include "TokenBufferFlags.h"


// The max-length of the data associated with a single token - limited by the 1-byte field-length field. Longer data needs an extension token.
#define ZSTOK_MAX_TOKEN_DATA_LENGTH ((uint8_t) 255)
#define MAX_NUMERIC_TOKEN_DATA_LENGTH ((uint8_t) 2)

// Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
#define ZSTOK_TOKEN_EXTENSION ((uint8_t) 0x81)

#ifndef ZSTOK_DEFINED_BUFSZ_TYPE
#define ZSTOK_DEFINED_BUFSZ_TYPE 1

// represents the size of (or index into) a token buffer - may reasonably be uint16_t or uint8_t.
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
    /** Indicates that we are writing an extended token (in order to help limit odd-nibble numerics) */
    bool extended : 1;

} ZStok_TokenBuffer;


// General TokenBuffer functions
static void zstok_initTokenBuffer(ZStok_TokenBuffer *tb, uint8_t *bufSpace, zstok_bufsz_t bufLen);

static bool zstok_isMarker(uint8_t key);

static bool zstok_isSequenceEndMarker(uint8_t key);

static zstok_bufsz_t zstok_offset(const ZStok_TokenBuffer *tb, zstok_bufsz_t index, zstok_bufsz_t offset);


#endif // _ZS_TOKENBUFFER_H
