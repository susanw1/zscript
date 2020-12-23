/*
 * RCodeI2cSubsystem.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "RCodeI2cSubsystem.hpp"

RCodeI2cPhysicalBus RCodeI2cSubsystem::phyBus0 = RCodeI2cPhysicalBus(I2cManager::getI2cById(0));
RCodeI2cPhysicalBus RCodeI2cSubsystem::phyBus1 = RCodeI2cPhysicalBus(I2cManager::getI2cById(1));
RCodeI2cPhysicalBus RCodeI2cSubsystem::phyBus2 = RCodeI2cPhysicalBus(I2cManager::getI2cById(2));
RCodeI2cPhysicalBus RCodeI2cSubsystem::phyBus3 = RCodeI2cPhysicalBus(I2cManager::getI2cById(3));
RCodeI2cBus RCodeI2cSubsystem::busses[] = { RCodeI2cBus(), RCodeI2cBus(), RCodeI2cBus(), RCodeI2cBus(), RCodeI2cBus(), RCodeI2cBus(), RCodeI2cBus(),
        RCodeI2cBus() };
bool RCodeI2cSubsystem::createNotifications = false;

void RCodeI2cSubsystem::i2cSubsystemCallback(I2c *i2c, I2cTerminationStatus status) {
    if (RCodeI2cSubsystem::phyBus0.getI2c() == i2c) {
        RCodeI2cSubsystem::phyBus0.status = status;
        RCodeI2cSubsystem::phyBus0.callbackFunc(status, RCodeI2cSubsystem::phyBus0.callbackSlot, RCodeI2cSubsystem::phyBus0.callbackData);
    } else if (RCodeI2cSubsystem::phyBus1.getI2c() == i2c) {
        RCodeI2cSubsystem::phyBus1.status = status;
        RCodeI2cSubsystem::phyBus1.callbackFunc(status, RCodeI2cSubsystem::phyBus1.callbackSlot, RCodeI2cSubsystem::phyBus1.callbackData);
    } else if (RCodeI2cSubsystem::phyBus2.getI2c() == i2c) {
        RCodeI2cSubsystem::phyBus2.status = status;
        RCodeI2cSubsystem::phyBus2.callbackFunc(status, RCodeI2cSubsystem::phyBus2.callbackSlot, RCodeI2cSubsystem::phyBus2.callbackData);
    } else {
        RCodeI2cSubsystem::phyBus3.status = status;
        RCodeI2cSubsystem::phyBus3.callbackFunc(status, RCodeI2cSubsystem::phyBus3.callbackSlot, RCodeI2cSubsystem::phyBus3.callbackData);
    }
}
