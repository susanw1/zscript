// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFER_DEFS_H
#define _ZS_TOKENBUFFER_DEFS_H


#include <zscript/misc/SysErrors_defs.h>
#include "TokenBufferFlags_defs.h"

#include "TokenRingBuffer.h"

// Private functions not used outside this unit
static uint16_t zstok_highestPowerOf2_priv(uint16_t n);

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
    tb->data         = bufferSpace;
    tb->bufLen       = zstok_highestPowerOf2_priv(bufLen);
    tb->readStart    = 0;
    tb->writeStart   = 0;
    tb->writeLastLen = 0;
    tb->writeCursor  = 0;
    tb->inNibble     = false;
    tb->numeric      = false;
    tb->flags        = zstok_createBufferFlags();
}

/**
 * Determines whether the supplied byte is a valid "marker" key (including sequence marker).
 *
 * @param key any byte
 * @return true if it's a marker; false otherwise
 */
inline static bool zstok_isMarker(const uint8_t key) {
    return (key & 0xe0) == 0xe0;
}

/**
 * Determines whether the supplied byte is a sequence-end "marker" key (newline, fatal error).
 *
 * @param       key any byte
 * @return true if it's a sequence-end marker; false otherwise
 */
inline static bool zstok_isSequenceEndMarker(const uint8_t key) {
    return (key & 0xf0) == 0xf0;
}

/**
 * For a given `index`, calculate the new index having added the `offset`, wrapping around the ring-buffer as required.
 *
 * @param tb the tokenBuffer whose length we are wrapping
 * @param index the index to be offset
 * @param offset the offset to add
 * @return the index+offset, wrapped
 */
inline static zstok_bufsz_t zstok_offset(const ZStok_TokenBuffer *tb, zstok_bufsz_t index, zstok_bufsz_t offset) {
    // assumes bufLen is a power of 2, so that bufLen-1 is a valid bitmask
    return (index + offset) & (tb->bufLen - 1);
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


#endif // _ZS_TOKENBUFFER_DEFS_H
