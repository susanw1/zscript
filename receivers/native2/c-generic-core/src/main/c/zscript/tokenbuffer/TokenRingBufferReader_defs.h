// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERREADER_DEFS_H
#define _ZS_TOKENBUFFERREADER_DEFS_H

#include <stdlib.h>

#include <zscript/misc/SysErrors_defs.h>
#include "TokenBufferFlags_defs.h"
#include "TokenRingBuffer_defs.h"

#include "TokenRingBufferReader.h"

// TokenBuffer operations for constructing/using ReadTokens:

static ZStok_ReadToken zstok_getFirstReadToken(ZStok_TokenBuffer *tb);

static bool zstok_hasReadToken(const ZStok_TokenBuffer *tb);

// ReadToken operations:
static uint8_t zstok_getReadTokenKey(ZStok_ReadToken token);

uint16_t zstok_getReadTokenData16(ZStok_ReadToken token);

static zstok_bufsz_t zstok_getReadTokenDataLength(ZStok_ReadToken token);


static bool zstok_isReadTokenMarker(ZStok_ReadToken token);

static bool zstok_isReadTokenSequenceEndMarker(ZStok_ReadToken token);

static ZStok_ReadToken zstok_getNextReadToken(ZStok_ReadToken token);

static ZStok_ReadToken zstok_getNextReadTokenAndFlush(ZStok_ReadToken token);

static bool zstok_isValidReadToken(ZStok_ReadToken token);

static bool zstok_isEmptyReadToken(ZStok_ReadToken token);

// Private functions not used outside this unit:
static bool zstok_isInReadableArea_priv(const ZStok_TokenBuffer *tb, zstok_bufsz_t index);

static ZStok_ReadToken zstok_createEmptyReadToken_priv(void);

zstok_bufsz_t zstok_getSegmentDataSizeNonMarker_priv(ZStok_ReadToken token);

zstok_bufsz_t zstok_getSegmentDataSize_priv(ZStok_ReadToken token);

/**
 * Utility for determining whether the specified index is in the current readable area, defined as readStart <= index < writeStart (but accounting for this being a
 * ringbuffer).
 *
 * Everything that's included in the readable area is committed and immutable.
 *
 * @param index the index to check
 * @return true if in readable area; false otherwise
 */
static bool zstok_isInReadableArea_priv(const ZStok_TokenBuffer *tb, const zstok_bufsz_t index) {
    return (tb->writeStart >= tb->readStart && tb->readStart <= index && index < tb->writeStart)
           || (tb->writeStart < tb->readStart && (index < tb->writeStart || index >= tb->readStart));
}

/**
 * Checks whether any tokens can be read.
 *
 * @return true iff a token is available for reading; false otherwise.
 */
static bool zstok_hasReadToken(const ZStok_TokenBuffer *tb) {
    return tb->readStart != tb->writeStart;
}

/**
 * Creates a ReadToken representing the current first readable token in the buffer. If no readable token exists, then the returned token will be NULL/zero.
 *
 * @return a ReadToken representing the current first readable token
 */
static ZStok_ReadToken zstok_getFirstReadToken(ZStok_TokenBuffer *tb) {
    if (!zstok_hasReadToken(tb)) {
        return zstok_createEmptyReadToken_priv();
    }
    return (ZStok_ReadToken) { .tokenBuffer = tb, .index = tb->readStart };
}

/**
 * Creates an empty ReadToken, whose fields are NULL/zero. This signifies an "end-of-tokens" condition, when we've read up to the writeStart position.
 *
 * @return an empty ReadToken
 */
static ZStok_ReadToken zstok_createEmptyReadToken_priv(void) {
    return (ZStok_ReadToken) { .tokenBuffer = NULL, .index = 0 };
}

static bool zstok_isEmptyReadToken(const ZStok_ReadToken token) {
    return token.tokenBuffer == NULL;
}

static bool zstok_isValidReadToken(const ZStok_ReadToken token) {
    return token.tokenBuffer != NULL;
}

/**
 * Get the key associated with this ReadToken. Must not be called on an empty token (returns 0).
 *
 * @return the key associated with this ReadToken
 */
static uint8_t zstok_getReadTokenKey(const ZStok_ReadToken token) {
    if (token.tokenBuffer == NULL) {
        return 0;
    }
    return token.tokenBuffer->data[token.index];
}

/**
 * Handy method equivalent to using {@link TokenBuffer#isMarker(byte)} on this ReadToken. Must not be called on an empty token (returns false).
 *
 * @return true if this is a marker token (including sequence marker); false otherwise
 */
static bool zstok_isReadTokenMarker(const ZStok_ReadToken token) {
    return zstok_isMarker(zstok_getReadTokenKey(token));
}

/**
 * Handy method equivalent to using {@link TokenBuffer#isSequenceEndMarker(byte)} on this ReadToken. Must not be called on an empty token (returns false).
 *
 * @return true if this is a sequence-marker token; false otherwise
 */
static bool zstok_isReadTokenSequenceEndMarker(const ZStok_ReadToken token) {
    return zstok_isSequenceEndMarker(zstok_getReadTokenKey(token));
}

/**
 * Determines the amount of data associated with this token, including any (immediately) following extension segment tokens. If the Token is a marker, then returns
 * zero.
 *
 * @return the "real" size of this token, including following extension segments, or zero if token is a marker.
 */
static zstok_bufsz_t zstok_getReadTokenDataLength(const ZStok_ReadToken token) {
    if (zstok_isEmptyReadToken(token) || zstok_isReadTokenMarker(token)) {
        return 0;
    }

    zstok_bufsz_t totalSz = 0;
    zstok_bufsz_t index   = token.index;

    const ZStok_TokenBuffer *buf = token.tokenBuffer;
    do {
        zstok_bufsz_t segSz = buf->data[zstok_offset(buf, index, 1)];
        totalSz += segSz;
        index               = zstok_offset(buf, index, segSz + 2);
    } while (index != buf->writeStart && buf->data[index] == ZSTOK_TOKEN_EXTENSION);

    return totalSz;
}

/**
 * Determines the size of the data portion of just this token segment ASSUMING it is not a marker! (No checks are made!)
 *
 * @return this Token's data size - 0 to 255
 */
zstok_bufsz_t zstok_getSegmentDataSizeNonMarker_priv(const ZStok_ReadToken token) {
    return token.tokenBuffer->data[zstok_offset(token.tokenBuffer, token.index, 1)];
}

/**
 * Determines the size of the data portion of just this token (not including any extensions).
 *
 * @return this Token's data size - 0 to 255, and 0 if it's a Marker
 */
zstok_bufsz_t zstok_getSegmentDataSize_priv(const ZStok_ReadToken token) {
    return zstok_isReadTokenMarker(token)
           ? 0
           : zstok_getSegmentDataSizeNonMarker_priv(token);
}

/**
 * Exposes the data as a single number (uint16).
 *
 * @return the value of the data, as a 2 byte number
 */
uint16_t zstok_getReadTokenData16(const ZStok_ReadToken token) {
    if (zstok_isEmptyReadToken(token) || zstok_isReadTokenMarker(token)) {
        return 0;
    }
    uint16_t           value = 0;
    for (zstok_bufsz_t i     = 0, n = zstok_getSegmentDataSizeNonMarker_priv(token); i < n; i++) {
        // Check before we left shift. Avoids overflowing data type.
        if (value > 0xFF) {
            // raise error!
        }
        value <<= 8;
        value += token.tokenBuffer->data[zstok_offset(token.tokenBuffer, token.index, i + 2)];
    }
    return value;
}

/**
 * Creates a ReadToken representing the next readable token in the buffer after this one, skipping extension tokens. If there is no such token (ie we have reached writeStart), then the returned token will be NULL/zero.
 *
 * @return a ReadToken representing the following token if any, or an empty token
 */
static ZStok_ReadToken zstok_getNextReadToken(const ZStok_ReadToken token) {
    if (zstok_isEmptyReadToken(token)) {
        return token;
    }

    const ZStok_TokenBuffer *buf           = token.tokenBuffer;
    zstok_bufsz_t           nextStartIndex = token.index;

    if (zstok_isReadTokenMarker(token)) {
        nextStartIndex = zstok_offset(buf, nextStartIndex, 1);
    } else {
        do {
            int tokenDataLength = buf->data[zstok_offset(buf, nextStartIndex, 1)];
            nextStartIndex = zstok_offset(buf, nextStartIndex, tokenDataLength + 2);
        } while (buf->data[nextStartIndex] == ZSTOK_TOKEN_EXTENSION);
    }
    return nextStartIndex == buf->writeStart
           ? zstok_createEmptyReadToken_priv()
           : (ZStok_ReadToken) { token.tokenBuffer, nextStartIndex };
}

/**
 * Clears any data read thus far from the buffer up and including this token, returning the "next" token - which is also the new "first" token. This method allows a Parser to
 * say "I've finished with everything I've seen so far", also allowing the writer side to use the newly freed space in the case of a ring-buffer.
 * <p/>
 * Caution: Note that calling this method affects the state of the underlying buffer, unlike other "read"-side operations. In particular, it could cause any retained ReadTokens
 * (including the supplied one) to find themselves dangling, pointing into "writer"-space which is subject to change without notice by the Writer. Call with care!
 * <p/>
 * Generally, avoid retaining existing ReadTokens or related objects after calling this.
 * @param token the token to be flushed (along with any predecessor)
 * @return the next token (which may be empty if we've reached end-of-tokens
 */
static ZStok_ReadToken zstok_getNextReadTokenAndFlush(const ZStok_ReadToken token) {
    const ZStok_ReadToken next = zstok_getNextReadToken(token);
    token.tokenBuffer->readStart = (next.tokenBuffer != NULL)
                                   ? next.index
                                   : next.tokenBuffer->writeStart;
    return next;
}


#endif // _ZS_TOKENBUFFERREADER_DEFS_H
