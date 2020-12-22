/*
 * Dma.cpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#include "../Dma.hpp"
void Dma::interrupt() {
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
uint16_t Dma::fetchRemainingTransferLength() {
    return channel.getChannelRegisters()->CNDTR;
}

void Dma::halt() {
    channel.getChannelRegisters()->CCR &= ~1;
}

void Dma::resume() {
    channel.getChannelRegisters()->CCR |= 1;
}

void Dma::setupGeneric(const uint8_t *peripheralOrSource, bool peripheralOrSourceIncrement, uint8_t peripheralOrSourceSize, const uint8_t *target,
        bool targetIncrement, uint8_t targetSize, bool memToMem, uint16_t transferLength, uint8_t dir, bool circular, DmaPriority priority,
        void (*callback)(Dma*, DmaTerminationStatus), bool interruptOnHalf) {
    this->callback = callback;
    channel.clearTransferError();
    channel.clearHalfTransferred();
    channel.clearTransferComplete();
    channel.clearInterrupt();

    channel.getChannelRegisters()->CNDTR = transferLength;
    channel.getChannelRegisters()->CPAR = (uint32_t) peripheralOrSource;
    channel.getChannelRegisters()->CMAR = (uint32_t) target;
    uint32_t config = 0x000b;
    if (memToMem) {
        config |= 0x4000;
    }
    config |= priority << 12;
    config |= targetSize << 10;
    config |= peripheralOrSourceSize << 8;
    if (targetIncrement) {
        config |= 0x80;
    }
    if (peripheralOrSourceIncrement) {
        config |= 0x40;
    }
    if (circular) {
        config |= 0x20;
    }
    if (dir) {
        config |= 0x10;
    }
    if (interruptOnHalf) {
        config |= 0x40;
    }
    channel.getChannelRegisters()->CCR = config;
}
