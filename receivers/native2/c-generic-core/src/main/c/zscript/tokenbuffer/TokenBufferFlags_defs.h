/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifdef _ZS_TOKENBUFFERFLAGS_DEFS_H
#error _ZS_TOKENBUFFERFLAGS_DEFS_H already defined - file should only be included once
#endif
#define _ZS_TOKENBUFFERFLAGS_DEFS_H

#include "TokenBufferFlags.h"

static void zstok_initBufferFlags(ZStok_BufferFlags *bf) {
    bf->markerWritten    = false;
    bf->seqMarkerWritten = false;
    bf->readerBlocked    = true;
    bf->fatalBufferError = false;
}

static void zstok_setMarkerWritten(ZStok_BufferFlags *bf) {
    bf->markerWritten = true;
}

static bool zstok_getAndClearMarkerWritten(ZStok_BufferFlags *bf) {
    if (bf->markerWritten) {
        bf->markerWritten = false;
        return true;
    }
    return false;
}

static void zstok_setSeqMarkerWritten(ZStok_BufferFlags *bf) {
    bf->seqMarkerWritten = true;
}

static bool zstok_getAndClearSeqMarkerWritten(ZStok_BufferFlags *bf) {
    if (bf->seqMarkerWritten) {
        bf->seqMarkerWritten = false;
        return true;
    }
    return false;
}

static bool zstok_isReaderBlocked(const ZStok_BufferFlags *bf) {
    return bf->readerBlocked;
}

static void zstok_setReaderBlocked(ZStok_BufferFlags *bf) {
    bf->readerBlocked = true;
}

static void zstok_clearReaderBlocked(ZStok_BufferFlags *bf) {
    bf->readerBlocked = false;
}

static bool zstok_isFatalBufferError(const ZStok_BufferFlags *bf) {
    return bf->fatalBufferError;
}

static void zstok_setFatalBufferError(ZStok_BufferFlags *bf) {
    bf->fatalBufferError = true;
}
