/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_ZSCRIPTSYS_H
#define _ZS_ZSCRIPTSYS_H

#include <stdint.h>
#include <zscript/sys/core-version.h>
#include <zscript/tokenbuffer/tokenbuffer.h>

typedef struct {
    tokunit *buffer;
} ZscriptSystem;

void setupZscript();

#endif // _ZS_ZSCRIPTSYS_H
