/*
 * RCodeI2cSendCommand.cpp
 *
 *  Created on: 13 Dec 2020
 *      Author: robert
 */

#include "RCodeI2cSendCommand.hpp"

void RCodeI2cSendCommand::setAsFinished(I2cTerminationStatus status, RCodeCommandSlot<RCodeParameters> *slot, uint8_t retries) {
    if (status == Complete || retries == 0) {
        slot->setComplete(true);
    } else {
        slot->setNeedsMoveAlong(true);
    }
}
void RCodeI2cSendCommand::moveAlong(RCodeCommandSlot<RCodeParameters> *slot) const {
    bool tenBit = slot->getFields()->has('N');
    uint16_t address = slot->getFields()->getByte('A', 0, 0);
    if (slot->getFields()->getByteCount('A') > 1) {
        address = (address << 8) | slot->getFields()->getByte('A', 1, 0);
    }

    RCodeI2cBus *bus = RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0));
    bus->asyncTransmit(address, tenBit, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot, &RCodeI2cSendCommand::setAsFinished,
            bus->getCallbackData() - 1);
}

void RCodeI2cSendCommand::finish(RCodeCommandSlot<RCodeParameters> *slot, RCodeOutStream<RCodeParameters> *out) const {
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
    RCodeI2cBus *bus = RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0));
    if (slot->getFields()->get('B', 0) >= 4 * RCodePeripheralParameters::i2cBussesPerPhyBus) {
        return;
    }
    uint8_t retries = slot->getFields()->get('T', 5);
    if (retries == 0) {
        retries = 1;
    }

    out->writeField('A', address);
    out->writeField('T', (uint8_t)(retries - bus->getCallbackData()));
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

void RCodeI2cSendCommand::execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
        RCodeOutStream<RCodeParameters> *out) {
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
    if (bus >= 4 * RCodePeripheralParameters::i2cBussesPerPhyBus) {
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
        RCodeI2cSubsystem::getRCodeBus(bus)->activateBus();
        RCodeI2cSubsystem::getRCodeBus(bus)->asyncTransmit(address, tenBit, NULL, 0, slot, &RCodeI2cSendCommand::setAsFinished, retries - 1);
    } else {
        RCodeI2cSubsystem::getRCodeBus(bus)->activateBus();
        RCodeI2cSubsystem::getRCodeBus(bus)->asyncTransmit(address, tenBit, slot->getBigField()->getData(), slot->getBigField()->getLength(), slot,
                &RCodeI2cSendCommand::setAsFinished, retries - 1);
    }
}

void RCodeI2cSendCommand::setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    if (slot->getFields()->countFieldSections('B') <= 1) {
        locks->addLock(RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0))->getBusLock(), true);
    }
}

