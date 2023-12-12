/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMACPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMACPP_HPP_

#include <dma-ll/Dma.hpp>

template<class LL>
void Dma<LL>::interrupt() {
    if (channel.hasTransferError()) {
        callback(this, DmaError);
        channel.clearTransferError();
        channel.clearInterrupt();
    } else if (channel.hasTransferComplete()) {
        callback(this, DataTransferComplete);
        channel.clearTransferComplete();
        channel.clearInterrupt();
    } else if (channel.hasHalfTransferred()) {
        callback(this, DataTransferHalfway);
        channel.clearHalfTransferred();
        channel.clearInterrupt();
    }
}

template<class LL>
uint16_t Dma<LL>::fetchRemainingTransferLength() {
    return channel.getChannelRegisters()->CNDTR;
}

template<class LL>
void Dma<LL>::halt() {
    channel.getChannelRegisters()->CCR &= ~1;
    channel.setMux(DMAMUX_NO_MUX);
}

template<class LL>
void Dma<LL>::setupGeneric(const uint8_t *peripheralOrSource, bool peripheralOrSourceIncrement, uint8_t peripheralOrSourceSize, DmaMuxRequest request,
        const uint8_t *target, bool targetIncrement, uint8_t targetSize, bool memToMem, uint16_t transferLength, uint8_t dir, bool circular,
        DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {

    const uint32_t enableDma = 0x01;
    const uint32_t transferCompleteInterruptEnable = 0x02;
    const uint32_t transferErrorInterruptEnable = 0x08;
    const uint32_t dmaMem2Mem = 0x4000;
    const uint32_t targetIncrementBit = 0x80;
    const uint32_t sourceIncrementBit = 0x40;
    const uint32_t circularBit = 0x20;
    const uint32_t dirBit = 0x10;
    const uint32_t interruptOnHalfBit = 0x4;

    this->callback = callback;

    channel.clearTransferError();
    channel.clearHalfTransferred();
    channel.clearTransferComplete();
    channel.clearInterrupt();

    channel.getChannelRegisters()->CNDTR = transferLength;
    channel.getChannelRegisters()->CPAR = (uint32_t) peripheralOrSource;
    channel.getChannelRegisters()->CMAR = (uint32_t) target;
    uint32_t config = transferErrorInterruptEnable | transferCompleteInterruptEnable;
    if (memToMem) {
        config |= dmaMem2Mem;
    }
    config |= priority << 12;
    config |= targetSize << 10;
    config |= peripheralOrSourceSize << 8;
    if (targetIncrement) {
        config |= targetIncrementBit;
    }
    if (peripheralOrSourceIncrement) {
        config |= sourceIncrementBit;
    }
    if (circular) {
        config |= circularBit;
    }
    if (dir) {
        config |= dirBit;
    }
    if (interruptOnHalf) {
        config |= interruptOnHalfBit;
    }
    this->request = request;
    channel.getChannelRegisters()->CCR = config;
    channel.setMux(request);
    channel.getChannelRegisters()->CCR |= enableDma;
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SYSTEM_DMA_SPECIFIC_DMACPP_HPP_ */
