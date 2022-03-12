/*
 * ZcodeI2cSubsystem.hpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_
#define SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "ZcodeI2cPhysicalBus.hpp"
#include "ZcodeI2cBus.hpp"
#include "../LowLevel/I2cLowLevel/I2cManager.hpp"

void i2cSubsystemCallback(I2c *i2c, I2cTerminationStatus status);

class ZcodeI2cSubsystem {
private:
    static ZcodeI2cPhysicalBus phyBus0;
    static ZcodeI2cPhysicalBus phyBus1;
    static ZcodeI2cPhysicalBus phyBus2;
    static ZcodeI2cPhysicalBus phyBus3;
    static ZcodeI2cBus busses[4 * ZcodePeripheralParameters::i2cBussesPerPhyBus];
    static bool createNotifications;

public:
    static void init() {
        createNotifications = false;
        for (int i = 0; i < ZcodePeripheralParameters::i2cBussesPerPhyBus; ++i) {
            busses[i * 4].setup(&phyBus0, i * 4, ZcodeParameters::i2cPhyBus0Lock);
            busses[i * 4 + 1].setup(&phyBus1, i * 4 + 1, ZcodeParameters::i2cPhyBus1Lock);
            busses[i * 4 + 2].setup(&phyBus2, i * 4 + 2, ZcodeParameters::i2cPhyBus2Lock);
            busses[i * 4 + 3].setup(&phyBus3, i * 4 + 3, ZcodeParameters::i2cPhyBus3Lock);
        }
    }
    static void i2cSubsystemCallback(I2c *i2c, I2cTerminationStatus status);

    static ZcodeI2cBus* getZcodeBus(uint8_t busNum) {
        return busses + busNum;
    }
    static bool shouldCreateNotifications() {
        return createNotifications;
    }
    static void setCreateNotifications(bool createNotifications) {
        ZcodeI2cSubsystem::createNotifications = createNotifications;
    }
    static void setFrequency(I2cFrequency freq) {
        phyBus0.setFrequency(freq);
        phyBus1.setFrequency(freq);
        phyBus2.setFrequency(freq);
        phyBus3.setFrequency(freq);
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_I2C_RCODEI2CSUBSYSTEM_HPP_ */
