/*
 * ZcodeI2cBus.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "ZcodeI2cBus.hpp"

void ZcodeI2cBus::asyncTransmit(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncTransmit(addr, tenBit, txBuffer, txLen, callbackSlot, callbackFunc, callbackData);
}

void ZcodeI2cBus::asyncReceive(uint16_t addr, bool tenBit, uint8_t rxLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncReceive(addr, tenBit, rxLen, callbackSlot, callbackFunc, callbackData);
}

void ZcodeI2cBus::asyncTransmitReceive(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    phyBus->asyncTransmitReceive(addr, tenBit, txBuffer, txLen, rxLen, callbackSlot, callbackFunc, callbackData);
}

void ZcodeI2cBus::activateBus() {
    phyBus->activateSwitchedBus(busNum);
}

uint8_t* ZcodeI2cBus::getReadBuffer() {
    return phyBus->getReadBuffer();
}

void ZcodeI2cBus::freeReadBuffer() {
    return phyBus->freeReadBuffer();
}

uint8_t ZcodeI2cBus::getCallbackData() {
    return phyBus->getCallbackData();
}

I2cTerminationStatus ZcodeI2cBus::getStatus() {
    return phyBus->getStatus();
}
