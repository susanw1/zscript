/*
 * RCodeI2cSendCommand.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "RCodeI2cSendCommand.hpp"

void RCodeI2cSendCommand::setAsFinished(I2cTerminationStatus status, RCodeCommandSlot *slot, uint8_t retries) {
    if (status == Complete || retries == 0) {
        slot->setComplete(true);
    } else {
        slot->setNeedsMoveAlong(true);
    }
}
void RCodeI2cSendCommand::moveAlong(RCodeCommandSlot *slot) const {
    uint16_t address = slot->getFields()->get('A', 0);
    if (slot->getFields()->countFieldSections('A') > 1) {
        address = (address << 8) | slot->getFields()->get('A', 1, 0);
    }

    RCodeI2cBus *bus = RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0));
    bus->asyncTransmit(address, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot, &RCodeI2cSendCommand::setAsFinished,
            bus->getCallbackData() - 1);
}

void RCodeI2cSendCommand::finish(RCodeCommandSlot *slot, RCodeOutStream *out) const {
    uint8_t addrLen = slot->getFields()->countFieldSections('A');
    if (addrLen == 0 || addrLen > 2) {
        return;
    }
    if (slot->getFields()->countFieldSections('B') > 1) {
        return;
    }
    if (slot->getFields()->countFieldSections('T') > 1) {
        return;
    }
    uint16_t address = slot->getFields()->get('A', 0);
    if (addrLen > 1) {
        address = (address << 8) | slot->getFields()->get('A', 1, 0);
    }
    if (address >= 1024) {
        return;
    }
    RCodeI2cBus *bus = RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0));
    if (slot->getFields()->get('B', 0) >= 4 * RCodeI2cParameters::i2cBussesPerPhyBus) {
        return;
    }
    uint8_t retries = slot->getFields()->get('T', 5);
    if (retries == 0) {
        retries = 1;
    }

    RCodeOutStream::writeFieldType(out, 'A', address);
    out->writeField('T', retries - bus->getCallbackData());
    I2cTerminationStatus status = bus->getStatus();
    if (status == AddressNack) {
        out->writeField('I', 2);
    } else if (status == DataNack) {
        out->writeField('I', 3);
    } else if (status == Complete) {
        out->writeField('I', 0);
    } else {
        out->writeField('I', 4);
    }
    if (status != Complete && !slot->getFields()->has('C')) {
        out->writeStatus(CMD_FAIL);
    } else {
        out->writeStatus(OK);
    }
    bus->freeReadBuffer();
}

void RCodeI2cSendCommand::execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence, RCodeOutStream *out) {
    uint8_t addrLen = slot->getFields()->countFieldSections('A');
    if (addrLen == 0 || addrLen > 2) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("I2C addresses must be 7 or 10 bit");
        slot->setComplete(true);
        return;
    }
    if (slot->getFields()->countFieldSections('B') > 1) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("I2C bus cannot be more than 1 byte");
        slot->setComplete(true);
        return;
    }
    if (slot->getFields()->countFieldSections('T') > 1) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("No more than 255 retries supported");
        slot->setComplete(true);
        return;
    }
    uint8_t bus = slot->getFields()->get('B', 0);
    if (bus >= 4 * RCodeI2cParameters::i2cBussesPerPhyBus) {
        out->writeStatus(BAD_PARAM);
        RCodeOutStream::writeFieldType(out, 'B', bus);
        out->writeBigStringField("Bus given not supported");
        slot->setComplete(true);
        return;
    }
    uint8_t retries = slot->getFields()->get('T', 5);
    if (retries == 0) {
        retries = 1;
    }
    uint16_t address = slot->getFields()->get('A', 0);
    if (addrLen > 1) {
        address = (address << 8) | slot->getFields()->get('A', 1, 0);
    }
    if (address >= 1024) {
        out->writeStatus(BAD_PARAM);
        RCodeOutStream::writeFieldType(out, 'A', address);
        out->writeBigStringField("I2C addresses must be < 1024");
        slot->setComplete(true);
        return;
    }
    if (slot->getBigField()->getLength() == 0) {
        RCodeI2cSubsystem::getRCodeBus(bus)->activateBus();
    } else {
        RCodeI2cSubsystem::getRCodeBus(bus)->activateBus();
        RCodeI2cSubsystem::getRCodeBus(bus)->asyncTransmit(address, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot,
                &RCodeI2cSendCommand::setAsFinished, retries - 1);
    }
}

void RCodeI2cSendCommand::setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
    if (slot->getFields()->countFieldSections('B') <= 1) {
        locks->addLock(RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0))->getBusLock(), true);
    }
}

