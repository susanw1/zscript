/*
 * ZcodeI2cPhysicalBus.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "ZcodeI2cPhysicalBus.hpp"

void ZcodeI2cPhysicalBus::asyncTransmit(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;
    i2c->asyncTransmit(addr, tenBit, txBuffer, txLen, &ZcodeI2cSubsystem::i2cSubsystemCallback);
}

void ZcodeI2cPhysicalBus::asyncReceive(uint16_t addr, bool tenBit, uint8_t rxLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;

#ifndef USE_I2C_STATIC_READ_BUFFERS
    if (rxLen > 0) {
        readBuffer = (uint8_t*) malloc(rxLen);
    }
#endif

    i2c->asyncReceive(addr, tenBit, readBuffer, rxLen, &ZcodeI2cSubsystem::i2cSubsystemCallback);
}

void ZcodeI2cPhysicalBus::asyncTransmitReceive(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen,
        ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;
#ifndef USE_I2C_STATIC_READ_BUFFERS
    if (rxLen > 0) {
        readBuffer = (uint8_t*) malloc(rxLen);
    }
#endif
    i2c->asyncTransmitReceive(addr, tenBit, txBuffer, txLen, readBuffer, rxLen, &ZcodeI2cSubsystem::i2cSubsystemCallback);
}
