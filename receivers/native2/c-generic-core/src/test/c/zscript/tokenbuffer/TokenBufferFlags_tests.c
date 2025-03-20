/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <zscript/model/Zchars.h>
#include <zscript/tokenbuffer/TokenBufferFlags_defs.h>

#include "../testing/TestTools.h"

void test_markerWritten(void) {
    ZStok_BufferFlags bufferFlags = zstok_createBufferFlags();

    assertEquals("Check marker not written", bufferFlags.markerWritten, false);
    zstok_setMarkerWritten(&bufferFlags);
    assertEquals("Check getAndClear", zstok_getAndClearMarkerWritten(&bufferFlags), true);
    assertEquals("Check getAndClear 2nd", zstok_getAndClearMarkerWritten(&bufferFlags), false);
}

void test_seqMarkerWritten(void) {
    ZStok_BufferFlags bufferFlags = zstok_createBufferFlags();

    assertEquals("Check seqMarker not written", bufferFlags.seqMarkerWritten, false);
    zstok_setSeqMarkerWritten(&bufferFlags);
    assertEquals("Check seq getAndClear after writing", zstok_getAndClearSeqMarkerWritten(&bufferFlags), true);
    assertEquals("Check seq getAndClear 2nd", zstok_getAndClearSeqMarkerWritten(&bufferFlags), false);
}

void test_readerBlocked(void) {
    ZStok_BufferFlags bufferFlags = zstok_createBufferFlags();
    assertEquals("Check readerBlocked", zstok_isReaderBlocked(&bufferFlags), true);

    zstok_setReaderBlocked(&bufferFlags);
    assertEquals("Check readerBlocked set", zstok_isReaderBlocked(&bufferFlags), true);
    zstok_clearReaderBlocked(&bufferFlags);
    assertEquals("Check fatalBufferError clear", zstok_isReaderBlocked(&bufferFlags), false);
}

void test_fatalBufferError(void) {
    ZStok_BufferFlags bufferFlags = zstok_createBufferFlags();
    assertEquals("Check fatalBufferError", zstok_isFatalBufferError(&bufferFlags), false);

    zstok_setFatalBufferError(&bufferFlags);
    assertEquals("Check fatalBufferError set", zstok_isFatalBufferError(&bufferFlags), true);
}

int main(void) {
    test_markerWritten();
    test_seqMarkerWritten();
    test_readerBlocked();
    test_fatalBufferError();
    return 0;
}
