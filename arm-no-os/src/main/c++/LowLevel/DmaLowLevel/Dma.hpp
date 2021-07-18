/*
 * Dma.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMA_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMA_HPP_
#include "specific/DmaChannelInternal.hpp"
#include "../GeneralLLSetup.hpp"

enum DmaTerminationStatus {
    DataTransferComplete, DataTransferHalfway, DmaError, SetupError
};
enum DmaPriority {
    Low, Medium, High, VeryHigh
};

//interrupt on half is pretty specific, but could be nice in certain cases - probably the largest leak in this abstraction.
//  Could replace it with a bit field for other callback reasons?
class DmaManager;
class DmaInterruptManager;

class Dma {
private:
    DmaChannelInternal channel;
    DmaMuxRequest request = DMAMUX_NO_MUX;
    volatile bool lockBool = false;

    void (*volatile callback)(Dma*, DmaTerminationStatus);
    void interrupt();

    friend DmaManager;
    friend DmaInterruptManager;
    void setDma(DmaChannelInternal channel) {
        this->channel = channel;
    }
public:
    void peripheralRead(DmaMuxRequest request, const uint8_t *peripheral, bool incrementPeripheral, uint8_t *memory, bool incrementMemory,
            uint16_t transferLength, bool circular,
            DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, request, memory, incrementMemory, 0, false, transferLength, 0, circular, priority, callback,
                interruptOnHalf);
    }

    void peripheralWrite(DmaMuxRequest request, const uint8_t *memory, bool incrementMemory, uint8_t *peripheral, bool incrementPeripheral,
            uint16_t transferLength, bool circular,
            DmaPriority priority, void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(peripheral, incrementPeripheral, 0, request, memory, incrementMemory, 0, false, transferLength, 1, circular, priority, callback,
                interruptOnHalf);
    }

    void memCopy(const uint8_t *source, bool increment, uint8_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric(source, increment, 0, DMAMUX_NO_MUX, target, true, 0, true, transferLength, 0, false, priority, callback, interruptOnHalf);
    }

    void memCopy(const uint16_t *source, bool increment, uint16_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric((uint8_t*) source, increment, 1, DMAMUX_NO_MUX, (uint8_t*) target, true, 1, true, transferLength, 0, false, priority, callback,
                interruptOnHalf);
    }

    void memCopy(const uint32_t *source, bool increment, uint32_t *target, uint16_t transferLength, DmaPriority priority,
            void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
        setupGeneric((uint8_t*) source, increment, 2, DMAMUX_NO_MUX, (uint8_t*) target, true, 2, true, transferLength, 0, false, priority, callback,
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

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_DMALOWLEVEL_DMA_HPP_ */
