/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>

#include <zscript/model/Zchars.h>
#include <zscript/tokenbuffer/TokenBufferFlags_defs.h>

void assertEquals(const char *msg, int expected, int actual) {
    if (expected != actual) {
        fprintf (stderr, "*********** Expected = %d, actual = %d: %s\n", expected, actual, msg);
        exit(1);
    }
}

void test_markerWritten() {
    ZStok_BufferFlags bufferFlags;
    zstok_initBufferFlags(&bufferFlags);

    assertEquals("Check marker not written", bufferFlags.markerWritten, false);
    zstok_setMarkerWritten(&bufferFlags);
    assertEquals("Check marker after writing", bufferFlags.markerWritten, true);
    assertEquals("Check getAndClear", zstok_getAndClearMarkerWritten(&bufferFlags), true);
    assertEquals("Check marker after getAndClear", bufferFlags.markerWritten, false);
}

void test_seqMarkerWritten() {
    ZStok_BufferFlags bufferFlags;
    zstok_initBufferFlags(&bufferFlags);

    assertEquals("Check seqMarker not written", bufferFlags.seqMarkerWritten, false);
    zstok_setSeqMarkerWritten(&bufferFlags);
    assertEquals("Check seqMarker after writing", bufferFlags.seqMarkerWritten, true);
    assertEquals("Check seq getAndClear", zstok_getAndClearSeqMarkerWritten(&bufferFlags), true);
    assertEquals("Check seqMarker after getAndClear", bufferFlags.seqMarkerWritten, false);
}

void test_readerBlocked() {
    ZStok_BufferFlags bufferFlags;
    zstok_initBufferFlags(&bufferFlags);
    assertEquals("Check readerBlocked", zstok_isReaderBlocked(&bufferFlags), true);

    zstok_setReaderBlocked(&bufferFlags);
    assertEquals("Check readerBlocked set", zstok_isReaderBlocked(&bufferFlags), true);
    zstok_clearReaderBlocked(&bufferFlags);
    assertEquals("Check fatalBufferError clear", zstok_isReaderBlocked(&bufferFlags), false);
}

void test_fatalBufferError() {
    ZStok_BufferFlags bufferFlags;
    zstok_initBufferFlags(&bufferFlags);
    assertEquals("Check fatalBufferError", zstok_isFatalBufferError(&bufferFlags), false);

    zstok_setFatalBufferError(&bufferFlags);
    assertEquals("Check fatalBufferError set", zstok_isFatalBufferError(&bufferFlags), true);
}

int main() {
    test_markerWritten();
    test_seqMarkerWritten();
    test_readerBlocked();
    test_fatalBufferError();
    return 0;
}
