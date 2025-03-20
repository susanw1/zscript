// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERWRITER_H
#define _ZS_TOKENBUFFERWRITER_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/misc/SysErrors.h>

#include "TokenBufferFlags.h"
#include "TokenRingBuffer.h"

/**
 * Type used for performing writes to the token-buffer. Zs_TokenWriter objects are deliberately small, immutable and
 * essentially stateless, so they should ALWAYS be passed by value, not reference. They exist in order to keep "writer"-side
 * operations distinct from "reader"-side ones. Their state lives in fields on the buffer itself.
 */
typedef struct {
    ZStok_TokenBuffer *tokenBuffer;
} ZStok_TokenWriter;

// Fetches a TokenWriter for the specified TokenBuffer.
static ZStok_TokenWriter zstok_getTokenWriter(ZStok_TokenBuffer *tb);

// Token writing operations
static void zstok_startToken(ZStok_TokenWriter tbw, uint8_t key, bool numeric);

static void zstok_continueTokenNibble(ZStok_TokenWriter tbw, uint8_t nibble);

static void zstok_continueTokenByte(ZStok_TokenWriter tbw, uint8_t b);

static void zstok_endToken(ZStok_TokenWriter tbw);

static void zstok_writeMarker(ZStok_TokenWriter tbw, uint8_t key);

static void zstok_fail(ZStok_TokenWriter tbw, uint8_t errorCode);

// status checks
static bool zstok_isInNibble(ZStok_TokenWriter tbw);

static uint8_t zstok_getCurrentWriteTokenKey(ZStok_TokenWriter tbw);

static zstok_bufsz_t zstok_getCurrentWriteTokenLength(ZStok_TokenWriter tbw);

static bool zstok_isTokenComplete(ZStok_TokenWriter tbw);

static bool zstok_checkAvailableCapacity(ZStok_TokenWriter tbw, zstok_bufsz_t size);


#endif // _ZS_TOKENBUFFERWRITER_H
