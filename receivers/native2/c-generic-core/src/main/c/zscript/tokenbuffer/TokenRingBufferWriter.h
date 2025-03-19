/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_TOKENBUFFERWRITER_H
#define _ZS_TOKENBUFFERWRITER_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/misc/SysErrors.h>

#include "TokenBufferFlags.h"
#include "TokenRingBuffer.h"


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


#endif // _ZS_TOKENBUFFERWRITER_H
