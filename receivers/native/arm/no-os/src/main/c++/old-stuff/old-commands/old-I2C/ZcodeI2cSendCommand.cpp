/*
 * ZcodeI2cSendCommand.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "../../old-commands/old-I2C/ZcodeI2cSendCommand.hpp"

void ZcodeI2cSendCommand::setAsFinished(I2cTerminationStatus status, ZcodeCommandSlot<ZcodeParameters> *slot, uint8_t retries) {
    if (status == Complete || retries == 0) {
        slot->setComplete(true);
    } else {
        slot->setNeedsMoveAlong(true);
    }
}

void ZcodeI2cSendCommand::moveAlong(ZcodeCommandSlot<ZcodeParameters> *slot) const {
    bool tenBit = slot->getFields()->has('N');
    uint16_t address = slot->getFields()->getByte('A', 0, 0);
    if (slot->getFields()->getByteCount('A') > 1) {
        address = (address << 8) | slot->getFields()->getByte('A', 1, 0);
    }

    ZcodeI2cBus *bus = ZcodeI2cSubsystem::getZcodeBus(slot->getFields()->get('B', 0));
    bus->asyncTransmit(address, tenBit, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot, &ZcodeI2cSendCommand::setAsFinished,
            bus->getCallbackData() - 1);
}

void ZcodeI2cSendCommand::finish(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeOutStream<ZcodeParameters> *out) const {
    uint8_t addrLen = slot->getFields()->getByteCount('A');
    if (addrLen == 0 || addrLen > 2) {
        return;
    }
    if (slot->getFields()->countFieldSections('B') > 1) {
        return;
    }
    if (slot->getFields()->countFieldSections('T') > 1) {
        return;
    }
    bool tenBit = slot->getFields()->has('N');
    uint16_t address = slot->getFields()->getByte('A', 0, 0);
    if (addrLen > 1) {
        address = (address << 8) | slot->getFields()->getByte('A', 1, 0);
    }
    if (address > 127 && !tenBit) {
        return;
    }
    if (address >= 1024) {
        return;
    }
    ZcodeI2cBus *bus = ZcodeI2cSubsystem::getZcodeBus(slot->getFields()->get('B', 0));
    if (slot->getFields()->get('B', 0) >= 4 * ZcodePeripheralParameters::i2cBusesPerPhyBus) {
        return;
    }
    uint8_t retries = slot->getFields()->get('T', 5);
    if (retries == 0) {
        retries = 1;
    }

    out->writeField('A', address);
    out->writeField('T', (uint8_t) (retries - bus->getCallbackData()));
    I2cTerminationStatus status = bus->getStatus();
    if (status == AddressNack) {
        out->writeField('I', (uint8_t) 2);
    } else if (status == DataNack) {
        out->writeField('I', (uint8_t) 3);
    } else if (status == Complete) {
        out->writeField('I', (uint8_t) 0);
    } else {
        out->writeField('I', (uint8_t) 4);
    }
    if (status != Complete && !slot->getFields()->has('C')) {
        slot->fail("", CMD_FAIL);
        out->writeStatus(CMD_FAIL);
    } else {
        out->writeStatus(OK);
    }
    bus->freeReadBuffer();
}

void ZcodeI2cSendCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence,
        ZcodeOutStream<ZcodeParameters> *out) {
    uint8_t addrLen = slot->getFields()->getByteCount('A');
    if (addrLen == 0 || addrLen > 2) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("I2C addresses must be 7 or 10 bit");
        slot->setComplete(true);
        return;
    }
    if (slot->getFields()->countFieldSections('B') > 1) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("I2C bus cannot be more than 1 byte");
        slot->setComplete(true);
        return;
    }
    if (slot->getFields()->countFieldSections('T') > 1) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("No more than 255 retries supported");
        slot->setComplete(true);
        return;
    }

    uint8_t bus = slot->getFields()->get('B', 0);
    if (bus >= 4 * ZcodePeripheralParameters::i2cBusesPerPhyBus) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeField('B', bus);
        out->writeBigStringField("Bus given not supported");
        slot->setComplete(true);
        return;
    }

    uint8_t retries = slot->getFields()->get('T', 5);
    if (retries == 0) {
        retries = 1;
    }

    bool tenBit = slot->getFields()->has('N');
    uint16_t address = slot->getFields()->getByte('A', 0, 0);
    if (addrLen > 1) {
        address = (address << 8) | slot->getFields()->getByte('A', 1, 0);
    }

    if (address > 127 && !tenBit) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeField('A', address);
        out->writeBigStringField("I2C addresses in 7 bit mode must be < 127");
        slot->setComplete(true);
        return;
    }
    if (address >= 1024) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeField('A', address);
        out->writeBigStringField("I2C addresses must be < 1024");
        slot->setComplete(true);
        return;
    }
    if (slot->getBigField()->getLength() == 0) {
        ZcodeI2cSubsystem::getZcodeBus(bus)->activateBus();
        ZcodeI2cSubsystem::getZcodeBus(bus)->asyncTransmit(address, tenBit, NULL, 0, slot, &ZcodeI2cSendCommand::setAsFinished, retries - 1);
    } else {
        ZcodeI2cSubsystem::getZcodeBus(bus)->activateBus();
        ZcodeI2cSubsystem::getZcodeBus(bus)->asyncTransmit(address, tenBit, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot,
                &ZcodeI2cSendCommand::setAsFinished, retries - 1);
    }
}

void ZcodeI2cSendCommand::setLocks(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeLockSet<ZcodeParameters> *locks) const {
    if (slot->getFields()->countFieldSections('B') <= 1) {
        locks->addLock(ZcodeI2cSubsystem::getZcodeBus(slot->getFields()->get('B', 0))->getBusLock(), true);
    }
}

