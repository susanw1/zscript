/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_TOKENBUFFERFLAGS_H
#define _ZS_TOKENBUFFERFLAGS_H

#include <stdint.h>
#include <stdbool.h>


typedef struct zstok_BufferFlags ZStok_BufferFlags;


static void zstok_initBufferFlags(ZStok_BufferFlags *bf);

static void zstok_setMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_getAndClearMarkerWritten(ZStok_BufferFlags *bf);

static void zstok_setSeqMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_getAndClearSeqMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_isReaderBlocked(const ZStok_BufferFlags *bf);

static void zstok_setReaderBlocked(ZStok_BufferFlags *bf);

static void zstok_clearReaderBlocked(ZStok_BufferFlags *bf);

#endif // _ZS_TOKENBUFFERFLAGS_H
