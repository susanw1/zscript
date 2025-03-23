/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#ifndef _ZS_SYSERRORS_H
#define _ZS_SYSERRORS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum ZS_SystemErrorType {
    ZS_SystemErrorType_NONE = 0,

    ZS_SystemErrorType_TOKBUF_FATAL_OVERFLOW         = 1,
    ZS_SystemErrorType_TOKBUF_ARG_NIBBLE_RANGE       = 2,
    ZS_SystemErrorType_TOKBUF_ARG_NOT_MARKER         = 3,
    ZS_SystemErrorType_TOKBUF_ARG_NOT_TOKENKEY       = 4,
    ZS_SystemErrorType_TOKBUF_STATE_NOT_IN_TOKEN     = 5,
    ZS_SystemErrorType_TOKBUF_STATE_NUMERIC_TOO_LONG = 6,
    ZS_SystemErrorType_TOKBUF_STATE_IN_NIBBLE        = 7,
} ZS_SystemErrorType;

static volatile ZS_SystemErrorType zs_errno = ZS_SystemErrorType_NONE;

static void zs_setErrno(const ZS_SystemErrorType newValue);

#endif // _ZS_SYSERRORS_H
