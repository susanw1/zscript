/*
 * RCodeI2cPhysicalBus.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "RCodeI2cPhysicalBus.hpp"

void RCodeI2cPhysicalBus::asyncTransmit(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;
    i2c->asyncTransmit(addr, txBuffer, txLen, &RCodeI2cSubsystem::i2cSubsystemCallback);
}
void RCodeI2cPhysicalBus::asyncReceive(uint16_t addr, uint8_t rxLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;
#ifndef USE_I2C_STATIC_READ_BUFFERS
    if (rxLen > 0) {
        readBuffer = (uint8_t*) malloc(rxLen);
    }
#endif
    i2c->asyncReceive(addr, readBuffer, rxLen, &RCodeI2cSubsystem::i2cSubsystemCallback);
}
void RCodeI2cPhysicalBus::asyncTransmitReceive(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen,
        RCodeCommandSlot<RCodeParameters> *callbackSlot,
        void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData) {
    this->callbackSlot = callbackSlot;
    this->callbackFunc = callbackFunc;
    this->callbackData = callbackData;
#ifndef USE_I2C_STATIC_READ_BUFFERS
    if (rxLen > 0) {
        readBuffer = (uint8_t*) malloc(rxLen);
    }
#endif
    i2c->asyncTransmitReceive(addr, txBuffer, txLen, readBuffer, rxLen, &RCodeI2cSubsystem::i2cSubsystemCallback);
}
