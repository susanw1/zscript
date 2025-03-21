// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERWRITER_DEFS_H
#define _ZS_TOKENBUFFERWRITER_DEFS_H


#include <zscript/misc/SysErrors_defs.h>
#include "TokenBufferFlags_defs.h"
#include "TokenRingBuffer_defs.h"

#include "TokenRingBufferWriter.h"

// Private functions not used outside this unit
static bool zstok_isNumeric_priv(ZStok_TokenWriter tbw);

static void zstok_moveCursor_priv(ZStok_TokenWriter tbw);

static void zstok_writeNewTokenStart_priv(ZStok_TokenWriter tbw, uint8_t key);

static void zstok_fatalError_priv(ZStok_TokenWriter tbw, ZS_SystemErrorType systemErrorCode);

static zstok_bufsz_t zstok_getAvailableWrite_priv(ZStok_TokenWriter tbw);

/**
 * Creates a copy of an initialized Zs_TokenWriter. Zs_TokenWriter objects are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference. They exist in order to keep "reader"-side operations distinct from
 * "writer"-side ones.
 */
static ZStok_TokenWriter zstok_getTokenWriter(ZStok_TokenBuffer *tb) {
    return (ZStok_TokenWriter) { tb };
}

/**
 * Moves the writeCursor along by just one position. Fatal Error if space runs out.
 */
static void zstok_moveCursor_priv(const ZStok_TokenWriter tbw) {
    const zstok_bufsz_t nextCursor = zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeCursor, 1);
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
static void zstok_startToken(ZStok_TokenWriter tbw, uint8_t key, bool numeric) {
    if (key < 0x20 || key >= 0x80) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_ARG_NOT_TOKENKEY);   // "invalid token [key=0x" + key + "]""
        return;
    }
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
static void zstok_continueTokenNibble(ZStok_TokenWriter tbw, uint8_t nibble) {
    if (nibble > 0xf) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_ARG_NIBBLE_RANGE);    // "Nibble value out of range"
        return;
    }
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "Digit with missing field key"
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
static void zstok_continueTokenByte(ZStok_TokenWriter tbw, uint8_t b) {
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
static void zstok_endToken(ZStok_TokenWriter tbw) {
    if (zstok_isInNibble(tbw)) {
        if (zstok_isNumeric_priv(tbw)) {
            // if odd nibble count, then shuffle nibbles through token's data to ensure "right-aligned", eg 4ad0 really means 04ad
            uint8_t       hold = 0;
            zstok_bufsz_t pos  = zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeStart, 1);
            do {
                pos         = zstok_offset(tbw.tokenBuffer, pos, 1);
                uint8_t tmp = tbw.tokenBuffer->data[pos] & 0xF;
                tbw.tokenBuffer->data[pos] = hold | ((tbw.tokenBuffer->data[pos] >> 4) & 0xF);
                hold = tmp << 4;
            } while (pos != tbw.tokenBuffer->writeCursor);
        }
        zstok_moveCursor_priv(tbw);
    }

    tbw.tokenBuffer->writeStart = tbw.tokenBuffer->writeCursor;
    tbw.tokenBuffer->inNibble   = false;
}

/**
 * Determines the amount of writable space available, after the current writeCursor and before hitting the readStart.
 *
 * @param tbw the writer to check
 * @return the amount of writable space available
 */
static zstok_bufsz_t zstok_getAvailableWrite_priv(ZStok_TokenWriter tbw) {
    return (tbw.tokenBuffer->writeCursor >= tbw.tokenBuffer->readStart ? tbw.tokenBuffer->bufLen : 0)
           + tbw.tokenBuffer->readStart - tbw.tokenBuffer->writeCursor - 1;
}

/**
 * Determines whether the buffer has the specified number of bytes available.
 *
 * @param size the size to check
 * @return true if there is space; false otherwise
 */
static bool zstok_checkAvailableCapacity(ZStok_TokenWriter tbw, zstok_bufsz_t size) {
    return zstok_getAvailableWrite_priv(tbw) >= size;
}


/**
 * Determines whether the writer is in the middle of writing a nibble (ie there have been an odd number of nibbles so far).
 *
 * @return true if there is a nibble being constructed; false otherwise
 */
static bool zstok_isInNibble(ZStok_TokenWriter tbw) {
    return tbw.tokenBuffer->inNibble;
}

/**
 * Determines whether the writer is currently writing a numeric token (vs a non-numeric one).
 *
 * @return true if there is a numeric token being constructed; false otherwise
 */
static bool zstok_isNumeric_priv(ZStok_TokenWriter tbw) {
    return tbw.tokenBuffer->numeric;
}

/**
 * Gets the key of the current token. Fatal Error if no token is being written!
 *
 * @return the key
 */
static uint8_t zstok_getCurrentWriteTokenKey(ZStok_TokenWriter tbw) {
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
static zstok_bufsz_t zstok_getCurrentWriteTokenLength(ZStok_TokenWriter tbw) {
    if (zstok_isTokenComplete(tbw)) {
        zstok_fatalError_priv(tbw, ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN);   // "no token being written"
        return 0;
    }
    return tbw.tokenBuffer->data[zstok_offset(tbw.tokenBuffer, tbw.tokenBuffer->writeStart, 1)];
}

/**
 * Determines whether the last token has been ended correctly.
 *
 * @return true if complete; false if there's a token still being formed
 */
static bool zstok_isTokenComplete(ZStok_TokenWriter tbw) {
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
static void zstok_writeMarker(ZStok_TokenWriter tbw, uint8_t key) {
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
static void zstok_writeNewTokenStart_priv(ZStok_TokenWriter tbw, uint8_t key) {
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
static void zstok_fail(ZStok_TokenWriter tbw, uint8_t errorCode) {
    if (!zstok_isTokenComplete(tbw)) {
        // reset current token back to writeStart
        tbw.tokenBuffer->writeCursor = tbw.tokenBuffer->writeStart;
        tbw.tokenBuffer->inNibble    = false;
    }
    zstok_writeMarker(tbw, errorCode);
}

/**
 * Indicates a fatal error state to be communicated through the flags. Setting the flag prevents further use, and setting the system errno
 * allows some diagnostic assistance.
 *
 * @param systemErrorCode       a ZS_SystemErrorType to signal an error
 */
static void zstok_fatalError_priv(ZStok_TokenWriter tbw, ZS_SystemErrorType systemErrorCode) {
    zs_setErrno(systemErrorCode);
    zstok_setFatalBufferError(&tbw.tokenBuffer->flags);
}

#endif // _ZS_TOKENBUFFERWRITER_DEFS_H
