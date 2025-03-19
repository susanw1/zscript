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

/**
 * Signals that some Marker (possibly a sequence marker) has just been written to the TokenBuffer.
 *
 * <p>This allows the Writer to tell the Reader that it can perform some processing.
 */
static void zstok_setMarkerWritten(ZStok_BufferFlags *bf) {
    bf->markerWritten = true;
}

/**
 * Indicates if any Marker (sequence or otherwise) has been written (flag is auto-cleared).
 *
 * <p>This allows the Reader to tell that it can proceed with token processing.
 *
 * @return true if set, false otherwise
 */
static bool zstok_getAndClearMarkerWritten(ZStok_BufferFlags *bf) {
    if (bf->markerWritten) {
        bf->markerWritten = false;
        return true;
    }
    return false;
}

/**
 * Signals that some sequence marker has just been written to the TokenBuffer.
 *
 * <p>This allows the Writer to tell the Reader that there's a completed sequence ready to process (or an error or overrun). It's possible that this flag is already set, but the
 * Reader should process as much as it can anyway, so lost notifications don't matter.
 */
static void zstok_setSeqMarkerWritten(ZStok_BufferFlags *bf) {
    bf->seqMarkerWritten = true;
}

/**
 * Indicates whether a *sequence* Marker has been written (flag is auto-cleared).
 *
 * <p>This allows the Reader to know that there's a completed sequence awaiting processing (if it didn't know already). If the marker is indicating an error, then this might mean
 * the current sequence should be aborted.
 *
 * @return true if set, false otherwise
 */
static bool zstok_getAndClearSeqMarkerWritten(ZStok_BufferFlags *bf) {
    if (bf->seqMarkerWritten) {
        bf->seqMarkerWritten = false;
        return true;
    }
    return false;
}

/**
 * Indicates whether the Reader is blocked (ie waiting for a Marker to say it can proceed with processing).
 *
 * <p>This allows a Writer (ie the Tokenizer) to decide whether there's any point rejecting an offered byte if there's no capacity, given that the Reader will never clear any
 * space. This might happen if a single command exceeds the size of the buffer. Instead, it will add an OVERRUN marker which will cause the Reader to proceed into an error
 * state.
 *
 * @return true if the reader is blocked, false otherwise
 */
static bool zstok_isReaderBlocked(const ZStok_BufferFlags *bf) {
    return bf->readerBlocked;
}

/**
 * Used by a Reader to signal that it is now blocked - it has processed everything it can and is awaiting more tokens.
 *
 * <p>This signals to the Tokenizer that the Reader needs feeding, and if the buffer is running out of space then it's not the Reader's fault.
 */
static void zstok_setReaderBlocked(ZStok_BufferFlags *bf) {
    bf->readerBlocked = true;
}

/**
 * Used by a Reader to signal that it is not (or no longer) blocked, and can proceed with processing tokens if given the chance. This is effectively an acknowledgement that a
 * Marker has been identified.
 *
 * <p>If the Tokenizer has been struggling with buffer space, this allows it to start recovering.
 */
static void zstok_clearReaderBlocked(ZStok_BufferFlags *bf) {
    bf->readerBlocked = false;
}

/**
 * Indicates whether the Buffer has detected a usage error or been put in an unstable state. This should be checked and raised as an error condition.
 *
 * @return true if the buffer is in error state, false otherwise
 */
static bool zstok_isFatalBufferError(const ZStok_BufferFlags *bf) {
    return bf->fatalBufferError;
}

/**
 * Signals that the buffer is in an error state. Check \zs_errno for specifics.
 */
static void zstok_setFatalBufferError(ZStok_BufferFlags *bf) {
    bf->fatalBufferError = true;
}
