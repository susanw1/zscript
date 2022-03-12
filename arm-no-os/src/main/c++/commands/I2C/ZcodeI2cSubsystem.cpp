/*
 * ZcodeI2cSubsystem.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "ZcodeI2cSubsystem.hpp"

ZcodeI2cPhysicalBus ZcodeI2cSubsystem::phyBus0 = ZcodeI2cPhysicalBus(I2cManager::getI2cById(0));
ZcodeI2cPhysicalBus ZcodeI2cSubsystem::phyBus1 = ZcodeI2cPhysicalBus(I2cManager::getI2cById(1));
ZcodeI2cPhysicalBus ZcodeI2cSubsystem::phyBus2 = ZcodeI2cPhysicalBus(I2cManager::getI2cById(2));
ZcodeI2cPhysicalBus ZcodeI2cSubsystem::phyBus3 = ZcodeI2cPhysicalBus(I2cManager::getI2cById(3));
ZcodeI2cBus ZcodeI2cSubsystem::busses[] = { ZcodeI2cBus(), ZcodeI2cBus(), ZcodeI2cBus(), ZcodeI2cBus(), ZcodeI2cBus(), ZcodeI2cBus(), ZcodeI2cBus(),
        ZcodeI2cBus() };
bool ZcodeI2cSubsystem::createNotifications = false;

void ZcodeI2cSubsystem::i2cSubsystemCallback(I2c *i2c, I2cTerminationStatus status) {
    if (ZcodeI2cSubsystem::phyBus0.getI2c() == i2c) {
        ZcodeI2cSubsystem::phyBus0.status = status;
        ZcodeI2cSubsystem::phyBus0.callbackFunc(status, ZcodeI2cSubsystem::phyBus0.callbackSlot, ZcodeI2cSubsystem::phyBus0.callbackData);
    } else if (ZcodeI2cSubsystem::phyBus1.getI2c() == i2c) {
        ZcodeI2cSubsystem::phyBus1.status = status;
        ZcodeI2cSubsystem::phyBus1.callbackFunc(status, ZcodeI2cSubsystem::phyBus1.callbackSlot, ZcodeI2cSubsystem::phyBus1.callbackData);
    } else if (ZcodeI2cSubsystem::phyBus2.getI2c() == i2c) {
        ZcodeI2cSubsystem::phyBus2.status = status;
        ZcodeI2cSubsystem::phyBus2.callbackFunc(status, ZcodeI2cSubsystem::phyBus2.callbackSlot, ZcodeI2cSubsystem::phyBus2.callbackData);
    } else {
        ZcodeI2cSubsystem::phyBus3.status = status;
        ZcodeI2cSubsystem::phyBus3.callbackFunc(status, ZcodeI2cSubsystem::phyBus3.callbackSlot, ZcodeI2cSubsystem::phyBus3.callbackData);
    }
}
