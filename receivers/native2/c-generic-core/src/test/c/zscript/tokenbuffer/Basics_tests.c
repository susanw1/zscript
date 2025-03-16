/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include <zscript/model/Zchars.h>
#include <zscript/model/Zstatus.h>

// Simple test to ensure that cross-module includes are working, and to output version info under ctest -V

int main() {
    #if __STDC_VERSION__ == 201112L
        printf("C Standard: C11\n");
    #elif __STDC_VERSION__ == 199901L
        printf("C Standard: C99\n");
    #elif __STDC_VERSION__ == 201710L
        printf("C Standard: C17\n");
    #else
        printf("C Standard: Unknown\n");
    #endif

    printf("Checking Zchars, c = %c\n", ZS_Zchars_CMD);
    printf("Checking Zstatus, S = %x\n", ZS_Zstatus_SUCCESS);
}
