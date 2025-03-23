// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERREADER_DEFS_H
#define _ZS_TOKENBUFFERREADER_DEFS_H

#include <stdlib.h>

#include <zscript/misc/SysErrors_defs.h>
#include <zscript/misc/ByteString.h>
#include "TokenBufferFlags_defs.h"
#include "TokenRingBuffer_defs.h"

#include "TokenRingBufferReader.h"


// Private functions not used outside this unit:
static bool zstok_isInReadableArea_priv(const ZStok_TokenBuffer *tb, zstok_bufsz_t index);

static ZStok_ReadToken zstok_createEmptyReadToken_priv(void);

static zstok_bufsz_t zstok_getSegmentDataSizeNonMarker_priv(ZStok_ReadToken token);

static zstok_bufsz_t zstok_getSegmentDataSize_priv(ZStok_ReadToken token);

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
inline static ZStok_ReadToken zstok_createEmptyReadToken_priv(void) {
    return (ZStok_ReadToken) { .tokenBuffer = NULL, .index = 0 };
}

inline static bool zstok_isEmptyReadToken(const ZStok_ReadToken token) {
    return token.tokenBuffer == NULL;
}

inline static bool zstok_isValidReadToken(const ZStok_ReadToken token) {
    return token.tokenBuffer != NULL;
}

/**
 * Get the key associated with this ReadToken. Must not be called on an empty token (returns 0).
 *
 * @return the key associated with this ReadToken
 */
inline static uint8_t zstok_getReadTokenKey(const ZStok_ReadToken token) {
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
inline static bool zstok_isReadTokenMarker(const ZStok_ReadToken token) {
    return zstok_isMarker(zstok_getReadTokenKey(token));
}

/**
 * Handy method equivalent to using {@link TokenBuffer#isSequenceEndMarker(byte)} on this ReadToken. Must not be called on an empty token (returns false).
 *
 * @return true if this is a sequence-marker token; false otherwise
 */
inline static bool zstok_isReadTokenSequenceEndMarker(const ZStok_ReadToken token) {
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

    zstok_bufsz_t totalSz  = 0;
    zstok_bufsz_t segStart = token.index;

    const ZStok_TokenBuffer *buf = token.tokenBuffer;
    do {
        zstok_bufsz_t segSz = buf->data[zstok_offset(buf, segStart, 1)];
        totalSz += segSz;
        segStart            = zstok_offset(buf, segStart, segSz + 2);
    } while (segStart != buf->writeStart && buf->data[segStart] == ZSTOK_TOKEN_EXTENSION);

    return totalSz;
}

/**
 * Determines the size of the data portion of just this token segment ASSUMING it is not a marker! (No checks are made!)
 *
 * @return this Token's data size - 0 to 255
 */
inline static zstok_bufsz_t zstok_getSegmentDataSizeNonMarker_priv(const ZStok_ReadToken token) {
    return token.tokenBuffer->data[zstok_offset(token.tokenBuffer, token.index, 1)];
}

/**
 * Determines the size of the data portion of just this token (not including any extensions).
 *
 * @return this Token's data size - 0 to 255, and 0 if it's a Marker
 */
inline static zstok_bufsz_t zstok_getSegmentDataSize_priv(const ZStok_ReadToken token) {
    return zstok_isReadTokenMarker(token)
           ? 0
           : zstok_getSegmentDataSizeNonMarker_priv(token);
}

/**
 * Exposes the data as a single number (uint16).
 *
 * @return the value of the data, as a 2 byte number
 */
static uint16_t zstok_getReadTokenData16(const ZStok_ReadToken token) {
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
 * Clears any data read thus far from the buffer up *and including* this token, returning the "next" token - which is also the new "first" token. This method allows a Parser to
 * say "I've finished with everything I've seen so far", also allowing the writer side to use the newly freed space in the ring-buffer.
 * <p/>
 * Caution: Note that calling this method affects the state of the underlying buffer, unlike other "read"-side operations. In particular, it could cause any retained ReadTokens
 * (including the supplied one) to find themselves dangling, pointing into "writer"-space which is subject to change without notice by the Writer. Call with care!
 * <p/>
 * Generally, avoid retaining existing ReadTokens or related objects after calling this.
 *
 * @param token the token to be "flushed" (along with any predecessors). If empty, simply returned with no action.
 * @return the next token (which may be empty if we've reached end-of-tokens)
 */
static ZStok_ReadToken zstok_getNextReadTokenAndFlush(const ZStok_ReadToken token) {
    if (zstok_isEmptyReadToken(token)) {
        return token;
    }
    const ZStok_ReadToken next = zstok_getNextReadToken(token);
    token.tokenBuffer->readStart = (next.tokenBuffer != NULL)
                                   ? next.index
                                   : token.tokenBuffer->writeStart;
    return next;
}


/**
 * Exposes the token's data as an Iterator, which can deliver contiguous chunks of bytes.
 *
 * <p>Note: if the token is empty, calling {@refitem zstok_hasNextIteratorBlock()} on the returned iterator will safely return false.
 *
 * <p>Usage pattern:
 *
 *     ZStok_BlockIterator it = zstok_getReadTokenDataIterator(token);
 *     while(zstok_hasNextIteratorBlock(&it)) {
 *         ZS_ByteString str = zstok_nextContiguousIteratorBlock(&it, 128);
 *         uint8_t *p = str.data;
 *         uint16_t n = str.length;
 *     }
 *
 * @return a new block iterator pointing at the beginning of this token's data
 */
static ZStok_BlockIterator zstok_getReadTokenDataIterator(const ZStok_ReadToken token) {
    zstok_bufsz_t ind = 0;
    zstok_bufsz_t rem = 0;

    if (zstok_isValidReadToken(token)) {
        ind = zstok_offset(token.tokenBuffer, token.index, 2);
        rem = zstok_getSegmentDataSize_priv(token);
    }
    return (ZStok_BlockIterator) {
            .token = token,
            .byteIndex = ind,
            .segRemaining = rem
    };
}

/**
 * Determines whether there are more blocks of bytes to be returned from this iterator's token data.
 *
 * @param blockIterator the iterator to manage progress
 * @return true if there are more blocks to return, false if there is no more
 */
static bool zstok_hasNextIteratorBlock(ZStok_BlockIterator *blockIterator) {
    // Copes with variably-sized extensions (including empty!)

    // note: must not actually access blockIterator->token.tokenBuffer (or buf) until we know it's not null (see zstok_isEmptyReadToken call below)
    const ZStok_TokenBuffer *buf = blockIterator->token.tokenBuffer;
    while (blockIterator->segRemaining == 0) {
        if (zstok_isEmptyReadToken(blockIterator->token) || (blockIterator->byteIndex == buf->writeStart) || (buf->data[blockIterator->byteIndex] != ZSTOK_TOKEN_EXTENSION)) {
            return false;
        }
        blockIterator->segRemaining = buf->data[zstok_offset(buf, blockIterator->byteIndex, 1)];
        blockIterator->byteIndex    = zstok_offset(buf, blockIterator->byteIndex, 2);
    }
    return true;
}

/**
 * Retrieves a pointer to the next block of contiguous bytes in the currently iterated token. It is not required that {@refitem zstok_hasNextIteratorBlock()} be called, though
 * it would be unusual not to do so. It is guaranteed that if we are not at the end of the data (and the supplied `maxLength` > 0!), then length returned is always > 0 - empty
 * data never happens until the end of data. Generally, use {@refitem zstok_hasNextIteratorBlock()} to determine whether we're at the end.
 *
 * <p>Note that the ByteString points to, but does not copy, the bytes. The pointer points straight into the token-buffer. Changing that buffer is undefined! - it should be considered immutable.
 * Once the token is flushed, the iterated data is undefined (its space becomes "writable" as far as the TokenBuffer cares).
 *
 * @param blockIterator the iterator to manage iteration progress
 * @param maxLength max number of bytes to return (use 255 for max possible). May be 0, though won't advance the iterator.
 * @return struct containing pointer to the bytes, including its length (or data,length == NULL,0  value if iteration is ended)
 */
static ZS_ByteString zstok_nextContiguousIteratorBlock(ZStok_BlockIterator *blockIterator, uint8_t maxLength) {
    if (!zstok_hasNextIteratorBlock(blockIterator)) {
        return (ZS_ByteString) { .data = NULL, .length = 0 };
    }

    const ZStok_TokenBuffer *buf = blockIterator->token.tokenBuffer;

    // find least of a) requested maxLength, b) unreturned data left in this segment, c) bytes before ring buffer wraps
    zstok_bufsz_t       bytesLen = maxLength < blockIterator->segRemaining ? maxLength : blockIterator->segRemaining;
    const zstok_bufsz_t span     = buf->bufLen - blockIterator->byteIndex;
    bytesLen = bytesLen < span ? bytesLen : span;

    const uint8_t *bytes = &buf->data[blockIterator->byteIndex];

    blockIterator->byteIndex = zstok_offset(buf, blockIterator->byteIndex, bytesLen);
    blockIterator->segRemaining -= bytesLen;

    return (ZS_ByteString) {
            .data = bytes,
            .length = bytesLen
    };
}

/**
 * Retrieves a pointer to the next block of contiguous bytes in the currently iterated token. It is not required that {@refitem zstok_hasNextIteratorBlock()} be called
 *
 * @param blockIterator the iterator to manage iteration progress
 * @return struct containing pointer to the bytes, with its length (or contains a NULL data value if iteration is ended)
 */
inline static uint8_t zstok_nextIteratorByte(ZStok_BlockIterator *blockIterator) {
    ZS_ByteString bs = zstok_nextContiguousIteratorBlock(blockIterator, 1);
    return (bs.length == 0) ? 0 : *(bs.data);
}

/**
 * Calls the supplied action function on each data block in the supplied token in their order of appearance. The iteration
 * will continue as long as the action function returns true or until no more blocks exist. Returning `false` from the action
 * function allows early termination.
 *
 * @param token the token whose data is to be iterated, may be empty (for zero iterations)
 * @param maxLength max number of bytes to return in any one read (use 255 for max possible)
 * @param misc a pointer to any helper object, which is passed to the action object. May be NULL.
 * @param action a function to be called
 * @return true if action function always returned true, false otherwise
 */
static bool zstok_forEachDataBlock(const ZStok_ReadToken token, uint8_t maxLength, void *misc, bool (*action)(const ZS_ByteString s, void *misc)) {
    ZStok_BlockIterator it        = zstok_getReadTokenDataIterator(token);
    bool                keepGoing = true;
    while (keepGoing && zstok_hasNextIteratorBlock(&it)) {
        keepGoing &= action(zstok_nextContiguousIteratorBlock(&it, maxLength), misc);
    }
    return keepGoing;
}

#endif // _ZS_TOKENBUFFERREADER_DEFS_H
