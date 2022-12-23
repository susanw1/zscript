/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMAREGISTERS_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMAREGISTERS_HPP_

#include <llIncludes.hpp>

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

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMAREGISTERS_HPP_ */
