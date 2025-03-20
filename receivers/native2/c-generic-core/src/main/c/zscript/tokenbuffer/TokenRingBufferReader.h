// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERREADER_H
#define _ZS_TOKENBUFFERREADER_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/misc/SysErrors.h>

#include "TokenBufferFlags.h"
#include "TokenRingBuffer.h"


/**
 * Represents the view of a single readable token in the buffer. The first readable token is computed from the `readStart` position of the TokenBuffer,
 * and subsequent tokens are calculated using this token's length, like a linked-list. They are deliberately small and stateless, so they
 * should ALWAYS be passed by value, not reference.
 *
 * <p>At a higher level, it has the following rules:
 * <ul>
 * <li>you can only create a ReadToken from the valid start of the readable space, or from another token.</li>
 * <li>you can't go off the end of the readable space - the tokenBuffer pointer will be NULL for any such invalid token object.</li>
 * </ul>
 *
 * <p>Any operation which returns a ReadToken should fail when these rules are broken.
 *
 * <p>Token-based operations include allowing access to the numeric fields, and creating an iterator for accessing the token's data as a series of contiguous byte arrays (including
 * navigating extension tokens). A ReadToken is also used to flush preceding tokens, allowing the readStart position to be updated (flushToToken()).
 */
typedef struct {
    ZStok_TokenBuffer *const tokenBuffer; // NULL if token is empty; can't be const* because flush mutates readStart
    const zstok_bufsz_t index;
} ZStok_ReadToken;

/**
 * Tool for working through a token's data (including extensions) in byte-by-byte or uint8_t[N] contiguous chunks, for a specified max length N.
 * This is useful for streaming a token's data via eg a DMA system.
 */
typedef struct {
    ZStok_TokenBuffer *const tokenBuffer; // NULL if token is empty; can't be const* because flush mutates readStart
    const zstok_bufsz_t tokenIndex;
    const zstok_bufsz_t byteIndex;
    const zstok_bufsz_t segRemaining;
} ZStok_BlockIterator;


#endif // _ZS_TOKENBUFFERREADER_H
