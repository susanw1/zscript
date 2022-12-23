/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMA_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMA_HPP_

#include <llIncludes.hpp>
#include "specific/DmaChannelInternal.hpp"

enum DmaTerminationStatus {
    DataTransferComplete, DataTransferHalfway, DmaError, SetupError
};

enum DmaPriority {
    Low, Medium, High, VeryHigh
};

// interrupt on half is pretty specific, but could be nice in certain cases - probably the largest leak in this abstraction.
//  Could replace it with a bit field for other callback reasons? Looking at other arms though, it turns out its not uncommon, so hmmm.
template<class LL>
class DmaManager;

template<class LL>
class Dma {
private:
    DmaChannelInternal channel;
    volatile bool lockBool = false;

    void (*volatile callback)(Dma*, DmaTerminationStatus);
    void interrupt();

    friend DmaManager<LL> ;

    void setDma(DmaChannelInternal channel) {
        this->channel = channel;
    }
public:
    void peripheralRead(const uint8_t *peripheral, bool incrementPeripheral, uint8_t *memory, bool incrementMemory,
            uint16_t transferLength, bool circular, DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, memory, incrementMemory, 0, false, transferLength, 0, circular, priority, callback,
                interruptOnHalf);
    }

    void peripheralWrite(const uint8_t *memory, bool incrementMemory, uint8_t *peripheral, bool incrementPeripheral,
            uint16_t transferLength, bool circular, DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, memory, incrementMemory, 0, false, transferLength, 1, circular, priority, callback,
                interruptOnHalf);
    }
    void peripheralRead(DmaMuxRequest rq, const uint8_t *peripheral, bool incrementPeripheral, uint8_t *memory, bool incrementMemory,
            uint16_t transferLength, bool circular, DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, memory, incrementMemory, 0, false, transferLength, 0, circular, priority, callback,
                interruptOnHalf);
    }

    void peripheralWrite(DmaMuxRequest rq, const uint8_t *memory, bool incrementMemory, uint8_t *peripheral, bool incrementPeripheral,
            uint16_t transferLength, bool circular, DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, memory, incrementMemory, 0, false, transferLength, 1, circular, priority, callback,
                interruptOnHalf);
    }

    void memCopy(const uint8_t *source, bool increment, uint8_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(source, increment, 0, target, true, 0, true, transferLength, 0, false, priority, callback, interruptOnHalf);
    }

    void memCopy(const uint16_t *source, bool increment, uint16_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric((uint8_t*) source, increment, 1, (uint8_t*) target, true, 1, true, transferLength, 0, false, priority, callback,
                interruptOnHalf);
    }

    void memCopy(const uint32_t *source, bool increment, uint32_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric((uint8_t*) source, increment, 2, (uint8_t*) target, true, 2, true, transferLength, 0, false, priority, callback,
                interruptOnHalf);
    }

    uint16_t fetchRemainingTransferLength();

    void halt();

    void setupGeneric(const uint8_t *peripheralOrSource, bool peripheralOrSourceIncrement, uint8_t peripheralOrSourceSize, DmaMuxRequest request,
            const uint8_t *target, bool targetIncrement, uint8_t targetSize, bool memToMem, uint16_t transferLength, uint8_t dir, bool circular,
            DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf);

    bool lock() {
        if (!lockBool) {
            lockBool = true;
            return true;
        }
        return false;
    }

    bool isLocked() {
        return lockBool;
    }

    void unlock() {
        lockBool = false;
    }
};

#include "specific/Dmacpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SYSTEM_DMA_DMA_HPP_ */
