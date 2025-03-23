// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TESTS_TESTTOOLS_H
#define _ZS_TESTS_TESTTOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include "zscript/misc/SysErrors_defs.h"

void checkErrno(void) {
    if (zs_errno) {
        fprintf(stderr, "*********** ERRNO IS SET: %d\n", zs_errno);
        exit(1);
    }
}

void assertEquals(const char *msg, int p1, int p2) {
    if (p1 != p2) {
        fprintf(stderr, "*********** Unexpected mismatch: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertNotEquals(const char *msg, int p1, int p2) {
    if (p1 == p2) {
        fprintf(stderr, "*********** Unexpected match: p1 = %d, p2 = %d: %s\n", p1, p2, msg);
        exit(1);
    }
}

void assertTrue(const char *msg, bool expected) {
    if (!expected) {
        fprintf(stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}

void assertFalse(const char *msg, bool expected) {
    if (expected) {
        fprintf(stderr, "*********** Expected true: %s\n", msg);
        exit(1);
    }
}


bool checkContains(const char *msg, const uint8_t actual[], const uint8_t expected[], int len) {
    for (int i = 0; i < len; i++) {
        uint8_t c = expected[i];
        if (c != actual[i]) {
//            fprintf (stderr, "  Buffer: ");
//            for (zstok_bufsz_t t = 0; t < tbw.tokenBuffer->bufLen; t++) {
//                fprintf (stderr, "%02x ", datap[zstok_offset(tbw.tokenBuffer, 0, t)]);
//            }

            fprintf(stderr, "\n\nExpected: ");
            for (int t = 0; t < len; t++) {
                fprintf(stderr, "%02x ", expected[t]);
            }
            fprintf(stderr, "\n  Actual: ");
            for (int t = 0; t < len; t++) {
                fprintf(stderr, "%02x ", actual[t]);
            }
            fprintf(stderr, "\n          ");
            for (int t = 0; t < i; t++) {
                fprintf(stderr, "   ");
            }
            fprintf(stderr, "^^\n*********** Expected=%02x(%c), actual=%02x(%c); pos=%d: %s\n", c, (c < ' ' ? '?' : c), actual[i], actual[i], i, msg);
            return false;
        }
    }
    return true;
}

void assertContains(const char *msg, const uint8_t actual[], const uint8_t expected[], int len) {
    if (!checkContains(msg, actual, expected, len)) {
        exit(1);
    }
}

#endif //_ZS_TESTS_TESTTOOLS_H
