/*
 * RCodeI2cBus.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include <parsing/RCodeCommandSlot.hpp>

#include "../LowLevel/I2cLowLevel/I2c.hpp"

class RCodeI2cPhysicalBus;

class RCodeI2cBus {
private:
    RCodeI2cPhysicalBus *phyBus;
    uint8_t busNum;
    uint8_t busLock;
    public:
    RCodeI2cBus() :
            phyBus(NULL), busNum(0), busLock(0) {
    }
    void setup(RCodeI2cPhysicalBus *phyBus, uint8_t busNum, uint8_t busLock) {
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
    void asyncTransmit(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData);

    void asyncReceive(uint16_t addr, uint8_t rxLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData);

    void asyncTransmitReceive(uint16_t addr, const uint8_t *txBuffer, uint8_t txLen, uint8_t rxLen, RCodeCommandSlot<RCodeParameters> *callbackSlot,
            void (*callbackFunc)(I2cTerminationStatus, RCodeCommandSlot<RCodeParameters>*, uint8_t), uint8_t callbackData);

    void activateBus();

    uint8_t* getReadBuffer();
    void freeReadBuffer();
    uint8_t getCallbackData();
    I2cTerminationStatus getStatus();

};

#include "RCodeI2cPhysicalBus.hpp"
#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_ */
