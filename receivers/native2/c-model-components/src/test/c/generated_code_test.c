/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <zscript/model/Zchars.h>
#include <zscript/model/Zstatus.h>

//#include <zscript/model/modules/base/CoreModule.h>
//#include <zscript/model/modules/base/UartModule.h>


void assertEquals(const char *msg, const int expected, const int actual) {
    if (expected != actual) {
        fprintf(stderr, "Expecting %d (%c), but actually got %d (%c) - %s\n", expected, expected, actual, actual, msg);
        exit(1);
    }
}

void shouldFindZcharConstants() {
    assertEquals("ZS_Zchars_CMD", 'Z', ZS_Zchars_CMD);
    assertEquals("ZS_Zchars_OPENPAREN", '(', ZS_Zchars_OPENPAREN);
    assertEquals("ZS_Zchars_NULL_CHAR", '\0', ZS_Zchars_NULL_CHAR);
}

void shouldValidateZcharConstants() {
    assertEquals("alwaysIgnore(ZchaZS_Zstatus_Z_NULL_CHAR)", true, ZS_Zchars_alwaysIgnore(ZS_Zchars_NULL_CHAR));
    assertEquals("alwaysIgnore('a')", false, ZS_Zchars_alwaysIgnore('a'));
    assertEquals("alwaysIgnore(' ')", false, ZS_Zchars_alwaysIgnore(' '));

    assertEquals("shouldIgnore(' ')", true, ZS_Zchars_shouldIgnore(' '));
    assertEquals("shouldIgnore('\\t')", true, ZS_Zchars_shouldIgnore('\t'));
    assertEquals("shouldIgnore('a')", false, ZS_Zchars_shouldIgnore('a'));

    assertEquals("isSeparator('&')", true, ZS_Zchars_isSeparator('&'));
    assertEquals("isSeparator('a')", false, ZS_Zchars_isSeparator('a'));
    assertEquals("isSeparator('\\n')", true, ZS_Zchars_isSeparator('\n'));
}

void shouldValidateZscriptStatus() {
    assertEquals("ZS_Zstatus_SUCCESS", 0, ZS_Zstatus_SUCCESS);
    assertEquals("ZS_Zstatus_COMMAND_FAIL", 1, ZS_Zstatus_COMMAND_FAIL);
    assertEquals("ZS_Zstatus_BUFFER_OVR_ERROR", 0x10, ZS_Zstatus_BUFFER_OVR_ERROR);


    assertEquals("isSuccess(ZS_Zstatus_SUCCESS)", true, ZS_Zstatus_isSuccess(ZS_Zstatus_SUCCESS));
    assertEquals("isSuccess(ZS_Zstatus_COMMAND_FAIL)", false, ZS_Zstatus_isSuccess(ZS_Zstatus_COMMAND_FAIL));
    assertEquals("isSuccess(ZS_Zstatus_BUFFER_OVR_ERROR)", false, ZS_Zstatus_isSuccess(ZS_Zstatus_BUFFER_OVR_ERROR));

    assertEquals("isFailure(ZS_Zstatus_SUCCESS)", false, ZS_Zstatus_isFailure(ZS_Zstatus_SUCCESS));
    assertEquals("isFailure(ZS_Zstatus_COMMAND_FAIL)", true, ZS_Zstatus_isFailure(ZS_Zstatus_COMMAND_FAIL));
    assertEquals("isFailure(ZS_Zstatus_BUFFER_OVR_ERROR)", false, ZS_Zstatus_isFailure(ZS_Zstatus_BUFFER_OVR_ERROR));

    assertEquals("isError(ZS_Zstatus_SUCCESS)", false, ZS_Zstatus_isError(ZS_Zstatus_SUCCESS));
    assertEquals("isError(ZS_Zstatus_COMMAND_FAIL)", false, ZS_Zstatus_isError(ZS_Zstatus_COMMAND_FAIL));
    assertEquals("isError(ZS_Zstatus_BUFFER_OVR_ERROR)", true, ZS_Zstatus_isError(ZS_Zstatus_BUFFER_OVR_ERROR));
}

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    shouldFindZcharConstants();
    shouldValidateZcharConstants();
    shouldValidateZscriptStatus();
    printf("Test successful: (%s)\n", argv[0]);
    return 0;
}
