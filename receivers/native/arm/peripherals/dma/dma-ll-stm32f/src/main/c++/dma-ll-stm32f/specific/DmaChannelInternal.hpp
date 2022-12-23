/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_SPECIFIC_DMACHANNELINTERNAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_SPECIFIC_DMACHANNELINTERNAL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "DmaRegisters.hpp"

class DmaChannelInternal {
private:
    DmaRegisters *registers;
    DmaChannelRegisters *channelRegs;
    uint8_t channelOffset;

public:
    DmaChannelInternal() :
            registers(NULL), channelRegs(NULL), channelOffset(0) {
    }

    DmaChannelInternal(DmaRegisters *registers, DmaChannelRegisters *channelRegs, uint8_t channelOffset) :
            registers(registers), channelRegs(channelRegs), channelOffset(channelOffset) {
    }

    void operator=(const DmaChannelInternal &i) {
        registers = i.registers;
        channelRegs = i.channelRegs;
        channelOffset = i.channelOffset;
    }

    bool hasTransferError() {
        return (8 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearTransferError() {
        registers->IFCR |= 8 << (channelOffset * 4);
    }

    bool hasHalfTransferred() {
        return (4 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearHalfTransferred() {
        registers->IFCR |= 4 << (channelOffset * 4);
    }

    bool hasTransferComplete() {
        return (2 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearTransferComplete() {
        registers->IFCR |= 2 << (channelOffset * 4);
    }

    bool hasInterrupt() {
        return (1 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearInterrupt() {
        registers->IFCR |= 1 << (channelOffset * 4);
    }

    void clearAll() {
        registers->IFCR |= 0xf << (channelOffset * 4);
    }

    DmaChannelRegisters* getChannelRegisters() {
        return channelRegs;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_SPECIFIC_DMACHANNELINTERNAL_HPP_ */
