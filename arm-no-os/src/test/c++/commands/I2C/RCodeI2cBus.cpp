/*
 * RCodeI2cBus.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "RCodeI2cBus.hpp"

void RCodeI2cBus::asyncTransmit(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncTransmit(addr, txBuffer, txLen, callbackSlot, callbackFunc, callbackData);
}

void RCodeI2cBus::asyncReceive(uint16_t addr, uint8_t rxLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncReceive(addr, rxLen, callbackSlot, callbackFunc, callbackData);
}

void RCodeI2cBus::asyncTransmitReceive(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncTransmitReceive(addr, txBuffer, txLen, rxLen, callbackSlot, callbackFunc, callbackData);
}

void RCodeI2cBus::activateBus() {
    phyBus->activateSwitchedBus(busNum);
}

uint8_t* RCodeI2cBus::getReadBuffer() {
    return phyBus->getReadBuffer();
}
void RCodeI2cBus::freeReadBuffer() {
    return phyBus->freeReadBuffer();
}
uint8_t RCodeI2cBus::getCallbackData() {
    return phyBus->getCallbackData();
}
I2cTerminationStatus RCodeI2cBus::getStatus() {
    return phyBus->getStatus();
}
