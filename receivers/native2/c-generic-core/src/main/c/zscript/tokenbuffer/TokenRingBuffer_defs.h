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

#include <zscript/misc/General_defs.h>

#include "TokenRingBuffer.h"
#include "TokenBufferFlags_defs.h"

// The max-length of the data associated with a single token - limited by the 1-byte length field. Longer data needs an extension token.
#define ZSTOK_MAX_TOKEN_DATA_LENGTH ((uint8_t) 255)

// Special token key to indicate that this segment is a continuation of the previous token due to its data size hitting maximum.
#define ZSTOK_TOKEN_EXTENSION ((uint8_t) 0x81)

#ifndef ZSTOK_DEFINED_BUFSZ_TYPE
    #define ZSTOK_DEFINED_BUFSZ_TYPE 1

    // represents the size of an index into the token buffer - may reasonably be uint16_t or uint8_t.
    typedef uint16_t    zstok_bufsz_t;
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
    uint8_t             *data;
    // length of usable data
    zstok_bufsz_t       bufLen;

    zstok_bufsz_t       readStart;
    zstok_bufsz_t       writeStart;

    ZStok_BufferFlags   flags;

    // Additional writer state:
    /** index of data-length field of most recent token segment (esp required for long multi-segment tokens) */
    zstok_bufsz_t       writeLastLen;
    /** the current write index into data array */
    zstok_bufsz_t       writeCursor;
    /** are we in the middle of writing a 2-nibble value? */
    bool            inNibble :1;
    /** are we writing a nibble-based "numeric" field (even or odd number of nibbles), or a strictly hex-pair field? */
    bool            numeric :1;
} ZStok_TokenBuffer;


/**
 * Type used for performing writes to the token-buffer. Zs_TokenWriter objects are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference. They exist in order to keep "reader"-side operations distinct from
 * "writer"-side ones. Their state lives in fields on the buffer itself.
 */
typedef struct {
    ZStok_TokenBuffer      *tokenBuffer;
} Zs_TokenWriter;


// General TokenBuffer functions
static void zstok_initTokenBuffer(ZStok_TokenBuffer *tb, uint8_t *bufSpace, const zstok_bufsz_t bufLen);

static bool zstok_isMarker(const uint8_t key);
static bool zstok_isSequenceEndMarker(const uint8_t key);
static zstok_bufsz_t zstok_offset(const ZStok_TokenBuffer *tb, zstok_bufsz_t index, zstok_bufsz_t offset);
static bool zstok_isInReadableArea(const ZStok_TokenBuffer *tb, const zstok_bufsz_t index);

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

// Private functions not used outside this unit
static bool zstok_isNumeric_priv(Zs_TokenWriter tbw);
static void zstok_moveCursor_priv(Zs_TokenWriter tbw);
static void zstok_writeNewTokenStart_priv(Zs_TokenWriter tbw, uint8_t key);
static uint16_t zstok_highestPowerOf2_priv(uint16_t n);
static void zstok_fatalError_priv(Zs_TokenWriter tbw, ZS_SystemErrorType systemErrorCode);

static bool zstok_checkAvailableCapacity(Zs_TokenWriter tbw, zstok_bufsz_t size);
static zstok_bufsz_t zstok_getAvailableWrite_priv(Zs_TokenWriter tbw);


/**
 * Initializes a TokenBuffer, overwriting all fields, using the supplied `bufSpace` as a ring-buffer. The amount of space used must be a power
 * of 2 (eg 64 bytes, or 512 etc) to facilitate masking for fast ring-buffer wrapping. If `bufLen` is not a power of 2, then it is rounded down
 * to nearest, and only that part of the space will be used.
 *
 * @param   tb          the tokenBuffer to initialize
 * @param   bufSpace    pointer to space to use for buffer
 * @param   bufLen      the length of bufSpace - should be power of 2, or will be rounded down!
 */
static void zstok_initTokenBuffer(ZStok_TokenBuffer *tb, uint8_t *bufferSpace, const zstok_bufsz_t bufLen) {
    tb->data = bufferSpace;
    tb->bufLen =  zstok_highestPowerOf2_priv(bufLen);
    tb->readStart = 0;
    tb->writeStart = 0;
    tb->writeLastLen = 0;
    tb->writeCursor = 0;
    tb->inNibble = false;
    tb->numeric = false;
    zstok_initBufferFlags(&tb->flags);
}

/**
 * Determines whether the supplied byte is a valid "marker" key (including sequence marker).
 *
 * @param key any byte
 * @return true if it's a marker; false otherwise
 */
static bool zstok_isMarker(const uint8_t key) {
    return (key & 0xe0) == 0xe0;
}


/**
 * Determines whether the supplied byte is a sequence-end "marker" key (newline, fatal error).
 *
 * @param       key any byte
 * @return true if it's a sequence-end marker; false otherwise
 */
static bool zstok_isSequenceEndMarker(const uint8_t key) {
    return (key & 0xf0) == 0xf0;
}


/**
 * For a given `index`, calculate the new index having added the `offset`, wrapping around the ring-buffer as required.
 *
 * @param       key any byte
 * @return true if it's a sequence-end marker; false otherwise
 */
static zstok_bufsz_t zstok_offset(const ZStok_TokenBuffer *tb, zstok_bufsz_t index, zstok_bufsz_t offset) {
    return (index + offset) & (tb->bufLen - 1);
}


/**
 * Utility for determining whether the specified index is in the current readable area, defined as readStart <= index < writeStart (but accounting for this being a
 * ringbuffer).
 *
 * Everything included in the readable area is committed and immutable.
 *
 * @param index the index to check
 * @return true if in readable area; false otherwise
 */
static bool zstok_isInReadableArea(const ZStok_TokenBuffer *tb, const zstok_bufsz_t index) {
    return (tb->writeStart >= tb->readStart && tb->readStart <= index && index < tb->writeStart)
            || (tb->writeStart < tb->readStart && (index < tb->writeStart || index >= tb->readStart));
}

/**
 * Creates a copy of an initialized Zs_TokenWriter. Zs_TokenWriter objects are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference. They exist in order to keep "reader"-side operations distinct from
 * "writer"-side ones.
 */
static Zs_TokenWriter zstok_getTokenWriter(ZStok_TokenBuffer *tb) {
    Zs_TokenWriter w;
    w.tokenBuffer = tb;
    return w;
}

static void zstok_moveCursor_priv(Zs_TokenWriter tbw) {
    zstok_bufsz_t nextCursor = zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeCursor, 1);
    if (nextCursor == tbw.tokenBuffer->readStart) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_FATAL_OVERFLOW);   // "Out of buffer - should have reserved more"
        return;
    }
    tbw.tokenBuffer->writeCursor = nextCursor;
}

/**
 * Starts a new token with the specified key, along with whether nibble pairs are to be aggregated into bytes numerically. If a token is already being written, then it is
 * finished off first. The key must not be a marker - use {@link #writeMarker(byte)}.
 *
 * @param key     the byte to use as the key
 * @param numeric true if numeric nibble aggregation should be used, false otherwise
 */
static void zstok_startToken(Zs_TokenWriter tbw, uint8_t key, bool numeric) {
    zstok_endToken(tbw);
    tbw.tokenBuffer->numeric = numeric;
    zstok_writeNewTokenStart_priv(tbw, key);
}

/**
 * Adds a new nibble to the current token. Fatal Error if not writing a token, or nibble is out of range.
 *
 * @param b the nibble to add, with value in range 0-0xf
 * @throws IllegalStateException    if no token has been started
 * @throws IllegalArgumentException if the nibble is out of range, or this is a numeric token and we've exceeded the maximum size
 */
static void zstok_continueTokenNibble(Zs_TokenWriter tbw, uint8_t nibble) {
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "Digit with missing field key"
        return;
    }
    if (nibble > 0xf) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_ARG_NIBBLE_RANGE);    // "Nibble value out of range"
        return;
    }

    if (zstok_isInNibble(tbw)) {
        tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] |= nibble;
        zstok_moveCursor_priv(tbw);
    } else {
        if (tbw.tokenBuffer->data[tbw.tokenBuffer->writeLastLen] == ZSTOK_MAX_TOKEN_DATA_LENGTH) {
            if (zstok_isNumeric_priv(tbw)) {
                zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NUMERIC_TOO_LONG);    // "Illegal numeric field longer than 255 bytes"
                return;
            }
            zstok_writeNewTokenStart_priv(tbw, ZSTOK_TOKEN_EXTENSION);
        }
        tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] = (uint8_t) (nibble << 4);
        tbw.tokenBuffer->data[tbw.tokenBuffer->writeLastLen]++;
    }
    tbw.tokenBuffer->inNibble = !zstok_isInNibble(tbw);
}

/**
 * Adds a new byte to the current token. Fatal Error if not writing a token, or state is partway through a nibble.
 *
 * @param b the byte to add
 */
static void zstok_continueTokenByte(Zs_TokenWriter tbw, uint8_t b) {
    if (zstok_isInNibble(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_IN_NIBBLE);      // "Incomplete nibble"
        return;
    }
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "Byte with missing field key"
        return;
    }

    if (tbw.tokenBuffer->data[tbw.tokenBuffer->writeLastLen] == ZSTOK_MAX_TOKEN_DATA_LENGTH) {
        if (zstok_isNumeric_priv(tbw)) {
                zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NUMERIC_TOO_LONG);    // "Illegal numeric field longer than 255 bytes"
                return;
        }
        zstok_writeNewTokenStart_priv(tbw, ZSTOK_TOKEN_EXTENSION);
    }
    tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] = b;
    zstok_moveCursor_priv(tbw);
    tbw.tokenBuffer->data[tbw.tokenBuffer->writeLastLen]++;
}

/**
 * Forces the current token to be finished (eg on close-quote), wrapping up any numeric nibbles and resetting the state flags.
 */
static void zstok_endToken(Zs_TokenWriter tbw) {
    if (zstok_isInNibble(tbw)) {
        if (zstok_isNumeric_priv(tbw)) {
            // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
            uint8_t hold = 0;
            zstok_bufsz_t  pos  = zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeStart, 1);
            do {
                pos = zstok_offset(tbw.tokenBuffer, pos, 1);
                uint8_t tmp = tbw.tokenBuffer->data[pos] & 0xF;
                tbw.tokenBuffer->data[pos] = hold | ((tbw.tokenBuffer->data[pos] >> 4) & 0xF);
                hold = tmp << 4;
            } while (pos != tbw.tokenBuffer->writeCursor);
        }
        zstok_moveCursor_priv(tbw);
    }

    tbw.tokenBuffer->writeStart = tbw.tokenBuffer->writeCursor;
    tbw.tokenBuffer->inNibble = false;
}

/**
 * Determines the amount of writable space available, after the current writeCursor and before hitting the readStart.
 *
 * @param tbw the writer to check
 * @return the amount of writable space available
 */
static zstok_bufsz_t zstok_getAvailableWrite_priv(Zs_TokenWriter tbw) {
    return (tbw.tokenBuffer->writeCursor >= tbw.tokenBuffer->readStart ? tbw.tokenBuffer->bufLen : 0)
            + tbw.tokenBuffer->readStart - tbw.tokenBuffer->writeCursor - 1;
}

/**
 * Determines whether the buffer has the specified number of bytes available.
 *
 * @param size the size to check
 * @return true if there is space; false otherwise
 */
static bool zstok_checkAvailableCapacity(Zs_TokenWriter tbw, zstok_bufsz_t size) {
    return zstok_getAvailableWrite_priv(tbw) >= size;
}


/**
 * Determines whether the writer is in the middle of writing a nibble (ie there have been an odd number of nibbles so far).
 *
 * @return true if there is a nibble being constructed; false otherwise
 */
static bool zstok_isInNibble(Zs_TokenWriter tbw) {
    return tbw.tokenBuffer->inNibble;
}

/**
 * Determines whether the writer is currently writing a numeric token (vs a non-numeric one).
 *
 * @return true if there is a numeric token being constructed; false otherwise
 */
static bool zstok_isNumeric_priv(Zs_TokenWriter tbw) {
    return tbw.tokenBuffer->numeric;
}

/**
 * Gets the key of the current token. Fatal Error if no token is being written!
 *
 * @return the key
 */
static uint8_t zstok_getCurrentWriteTokenKey(Zs_TokenWriter tbw) {
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "no token being written"
        return 0;
    }
    return tbw.tokenBuffer->data[tbw.tokenBuffer->writeStart];
}

/**
 * Gets the current length of the current token. Fatal Error if no token is being written.
 *
 * @return the length
 */
static zstok_bufsz_t zstok_getCurrentWriteTokenLength(Zs_TokenWriter tbw) {
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "no token being written"
        return 0;   // "no token being written"
    }
    return tbw.tokenBuffer->data[zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeStart, 1)];
}

/**
 * Determines whether the last token has been ended correctly.
 *
 * @return true if complete; false if there's a token still being formed
 */
static bool zstok_isTokenComplete(Zs_TokenWriter tbw) {
    return tbw.tokenBuffer->writeStart == tbw.tokenBuffer->writeCursor;
}

/**
 * Writes the specified Marker. If a token is already being written, then it is finished off first. Markers are used to indicate that a parsable sequence of tokens has been
 * written which may trigger some reading activity.
 *
 * Fatal Error if key is not a marker.
 *
 * @param key the marker's key, as per {@link TokenBuffer#isMarker(byte)}
 */
static void zstok_writeMarker(Zs_TokenWriter tbw, uint8_t key) {
    if (!zstok_isMarker(key)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_ARG_NOT_MARKER);   // "invalid marker [key=0x" + key + "]""
        return;
    }
    zstok_endToken(tbw);
    tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] = key;
    zstok_moveCursor_priv(tbw);
    zstok_endToken(tbw);

    if (zstok_isSequenceEndMarker(key)) {
        zstok_setSeqMarkerWritten(&tbw.tokenBuffer->flags);
    }
    zstok_setMarkerWritten(&tbw.tokenBuffer->flags);
}

/**
 * Utility to initialize a new token. Fatal Error if key is a marker.
 *
 * @param key the token's key - must not be a marker
 */
static void zstok_writeNewTokenStart_priv(Zs_TokenWriter tbw, uint8_t key) {
    if (zstok_isMarker(key)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_ARG_NOT_TOKENKEY);   // "invalid token [key=0x" + key + "]""
        return;
    }
    tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] = key;
    zstok_moveCursor_priv(tbw);
    tbw.tokenBuffer->data[tbw.tokenBuffer->writeCursor] = 0;
    tbw.tokenBuffer->writeLastLen = tbw.tokenBuffer->writeCursor;
    zstok_moveCursor_priv(tbw);
}

/**
 * Indicates a failure state to be communicated through the buffer to the Reader. This is the identical to writing a Marker, _except_ if there's a token already being
 * written and not complete, then it is wound back and its space reclaimed. The errorCode is expected to be a Sequence Marker, as per
 * {@link TokenBuffer#isSequenceEndMarker(byte)}.
 * <p/>
 * One particular errorCode is the Buffer OVERRUN condition.
 *
 * @param errorCode a marker key to signal an error
 */
static void zstok_fail(Zs_TokenWriter tbw, uint8_t errorCode) {
    if (!zstok_isTokenComplete(tbw)) {
        // reset current token back to writeStart
        tbw.tokenBuffer->writeCursor = tbw.tokenBuffer->writeStart;
        tbw.tokenBuffer->inNibble = false;
    }
    zstok_writeMarker(tbw, errorCode);
}

/**
 * Indicates a fatal error state to be communicated through the flags. Setting the flag prevents further use, and setting the system errno
 * allows some diagnostic assistance.
 *
 * @param systemErrorCode       a ZS_SystemErrorType to signal an error
 */
static void zstok_fatalError_priv(Zs_TokenWriter tbw, ZS_SystemErrorType systemErrorCode) {
    zs_setErrno(systemErrorCode);
    zstok_setFatalBufferError(&tbw.tokenBuffer->flags);
}


/**
 * Determines the highest power of two that's <= n. Eg 127->64, 128->128, 129->128, and 0->0.
 *
 * @param n the input number
 * @return largest power of two that's not larger that n
 */
static uint16_t zstok_highestPowerOf2_priv(uint16_t n) {
    if (n == 0) return 0; // Edge case for zero
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    return n - (n >> 1);
}


//class RingBufferTokenIterator;
//class RingBufferToken;
//class RawTokenBlockIterator;
//class TokenRingWriter
//class TokenRingReader {
