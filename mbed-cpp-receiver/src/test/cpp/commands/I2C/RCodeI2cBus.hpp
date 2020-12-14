/*
 * RCodeI2cBus.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

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
};

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CBUS_HPP_ */
