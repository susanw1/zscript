// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENBUFFERFLAGS_H
#define _ZS_TOKENBUFFERFLAGS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Flags to indicate current status, acting as a communication channel between the TokenWriter and TokenReader.
 */
typedef struct {
    volatile bool markerWritten : 1;
    volatile bool seqMarkerWritten : 1;
    volatile bool readerBlocked : 1;
    volatile bool fatalBufferError : 1;
} ZStok_BufferFlags;

static ZStok_BufferFlags zstok_createBufferFlags(void);

static void zstok_setMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_getAndClearMarkerWritten(ZStok_BufferFlags *bf);

static void zstok_setSeqMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_getAndClearSeqMarkerWritten(ZStok_BufferFlags *bf);

static bool zstok_isReaderBlocked(const ZStok_BufferFlags *bf);

static void zstok_setReaderBlocked(ZStok_BufferFlags *bf);

static void zstok_clearReaderBlocked(ZStok_BufferFlags *bf);

static bool zstok_isFatalBufferError(const ZStok_BufferFlags *bf);

static void zstok_setFatalBufferError(ZStok_BufferFlags *bf);

#endif // _ZS_TOKENBUFFERFLAGS_H
