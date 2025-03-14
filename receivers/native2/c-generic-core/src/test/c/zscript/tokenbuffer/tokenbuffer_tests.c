/*
 * Zscript - Command System for Microcontrollers
 * Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include <zscript/model/Zchars.h>
#include <zscript/tokenbuffer/tokenbuffer_defs.h>

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

    zs_initTokenBuffer(NULL);
    printf("Morning, c = %c!\n", ZS_Zchars_CMD);
}
