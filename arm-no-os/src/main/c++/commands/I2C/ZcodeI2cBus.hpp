/*
 * ZcodeI2cBus.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include <parsing/ZcodeCommandSlot.hpp>

#include "../LowLevel/I2cLowLevel/I2c.hpp"

class ZcodeI2cPhysicalBus;

class ZcodeI2cBus {
private:
    ZcodeI2cPhysicalBus *phyBus;
    uint8_t busNum;
    uint8_t busLock;
    public:
    ZcodeI2cBus() :
            phyBus(NULL), busNum(0), busLock(0) {
    }
    void setup(ZcodeI2cPhysicalBus *phyBus, uint8_t busNum, uint8_t busLock) {
        this->phyBus = phyBus;
        this->busNum = busNum;
        this->busLock = busLock;
    }
    uint8_t getBusNum() {
        return busNum;
    }
    uint8_t getBusLock() {
        return busLock;
    }
    void asyncTransmit(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData);

    void asyncReceive(uint16_t addr, bool tenBit, uint8_t rxLen, ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData);

    void asyncTransmitReceive(uint16_t addr, bool tenBit, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen,
            ZcodeCommandSlot<ZcodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, ZcodeCommandSlot<ZcodeParameters>*, uint8_t), uint8_t callbackData);

    void activateBus();

    uint8_t* getReadBuffer();
    void freeReadBuffer();
    uint8_t getCallbackData();
    I2cTerminationStatus getStatus();

};

#include "ZcodeI2cPhysicalBus.hpp"
#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_ */
