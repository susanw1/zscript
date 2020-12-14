/*
 * RCodeI2cSubsystem.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeI2cPhysicalBus.hpp"
#include "RCodeI2cBus.hpp"
#include <mbed.h>

class RCodeI2cSubsystem {
private:
    RCodeI2cPhysicalBus phyBus0;
    RCodeI2cPhysicalBus phyBus1;
    RCodeI2cPhysicalBus phyBus2;
    RCodeI2cPhysicalBus phyBus3;
    RCodeI2cBus busses[4 * RCodeI2cParameters::i2cBussesPerPhyBus];
    bool createNotifications = false;
public:
    RCodeI2cSubsystem() :
            phyBus0(RCodeI2cParameters::i2c0sda, RCodeI2cParameters::i2c0scl), phyBus1(
                    RCodeI2cParameters::i2c1sda, RCodeI2cParameters::i2c1scl), phyBus2(
                    RCodeI2cParameters::i2c2sda, RCodeI2cParameters::i2c2scl), phyBus3(
                    RCodeI2cParameters::i2c3sda, RCodeI2cParameters::i2c3scl) {
        for (int i = 0; i < RCodeI2cParameters::i2cBussesPerPhyBus; ++i) {
            busses[i * 4].setup(&phyBus0, i * 4,
                    RCodeLockValues::i2cPhyBus0Lock);
            busses[i * 4 + 1].setup(&phyBus1, i * 4 + 1,
                    RCodeLockValues::i2cPhyBus1Lock);
            busses[i * 4 + 2].setup(&phyBus2, i * 4 + 2,
                    RCodeLockValues::i2cPhyBus2Lock);
            busses[i * 4 + 3].setup(&phyBus3, i * 4 + 3,
                    RCodeLockValues::i2cPhyBus3Lock);
        }
    }
    RCodeI2cBus* getRCodeBus(uint8_t busNum) {
        return busses + busNum;
    }
    bool shouldCreateNotifications() {
        return createNotifications;
    }
    void setCreateNotifications(bool createNotifications) {
        this->createNotifications = createNotifications;
    }
    void setFrequency(int hz) {
        phyBus0.setFrequency(hz);
        phyBus1.setFrequency(hz);
        phyBus2.setFrequency(hz);
        phyBus3.setFrequency(hz);
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_ */
