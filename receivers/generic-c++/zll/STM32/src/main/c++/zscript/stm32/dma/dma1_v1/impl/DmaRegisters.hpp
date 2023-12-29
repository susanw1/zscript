/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */


struct DmaChannelRegisters {
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
    volatile uint32_t Reserved;
};

struct DmaRegisters {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    DmaChannelRegisters CHR1;
    DmaChannelRegisters CHR2;
    DmaChannelRegisters CHR3;
    DmaChannelRegisters CHR4;
    DmaChannelRegisters CHR5;
    DmaChannelRegisters CHR6;
    DmaChannelRegisters CHR7;
    DmaChannelRegisters CHR8;
};
