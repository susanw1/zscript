/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_OPTINT_H
#define _ZS_OPTINT_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Defines a handy OptionalInt type.
 *
 * Generally intended for copy-by-value.
 */
typedef struct ZS_OptUint16 {
    const uint16_t value;
    const bool     isPresent;
} ZS_OptUint16;

static ZS_OptUint16 zs_createOptUint16(const uint16_t value) {
    return (ZS_OptUint16) { value, true };
}

static ZS_OptUint16 zs_createEmptyOptUint16() {
    return (ZS_OptUint16) { 0, false };
}

static uint16_t zs_getValueOrDefault(const ZS_OptUint16 optInt, const uint16_t def) {
    return optInt.isPresent ? optInt.value : def;
}

#endif //_ZS_OPTINT_H
