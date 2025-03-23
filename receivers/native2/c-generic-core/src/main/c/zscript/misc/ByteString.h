/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_BYTESTRING_H
#define _ZS_BYTESTRING_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Defines a handy byte pointer and length to represent a string of bytes.
 *
 * Generally intended for copy by value.
 */
typedef struct ZS_ByteString {
    const uint8_t *const data;
    const uint16_t length;
} ZS_ByteString;

static ZS_ByteString zs_createByteString(const uint8_t *const p, const uint16_t len) {
    return (ZS_ByteString) { p, len };
}

static uint16_t zs_copyByteStringTo(const ZS_ByteString bytes, const uint16_t srcPos, uint8_t *dest, const uint16_t max) {
    if (bytes.length - srcPos >= srcPos) {
        int len = bytes.length - srcPos - srcPos;
        len = len < max ? len : max;
        memcpy(dest, bytes.data, len);
        return len;
    }
    return 0;
}

#endif