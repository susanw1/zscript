/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_GENERAL_DEFS_H
#define _ZS_GENERAL_DEFS_H

#include "General.h"

static void zs_setErrno(const ZS_SystemErrorType newValue) {
    if (zs_errno == ZS_SystemErrorType_NONE) {
        zs_errno = newValue;
    }
}

#endif // _ZS_GENERAL_DEFS_H
