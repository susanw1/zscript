/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifdef _ZS_TOKENBUFFER_DEFS_H
#error _ZS_TOKENBUFFER_DEFS_H already defined - file should only be included once
#endif
#define _ZS_TOKENBUFFER_DEFS_H

#include <stdio.h>
#include "tokenbuffer.h"

static void zs_initTokenBuffer(TokenBuffer *tb) {
    (void) tb;  // unused
    printf("hello initTokenBuffer\n");
}
