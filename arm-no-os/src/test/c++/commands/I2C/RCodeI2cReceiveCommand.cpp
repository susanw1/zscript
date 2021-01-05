/*
 * RCodeI2cReceiveCommand.cpp
 *
 *  Created on: 2 Jan 2021
 *      Author: robert
 */

#include "RCodeI2cReceiveCommand.hpp"

void RCodeI2cReceiveCommand::setAsFinished(I2cTerminationStatus status, RCodeCommandSlot<RCodeParameters> *slot, uint8_t retries) {
    if (status == Complete || retries == 0) {
        slot->setComplete(true);
    } else {
        slot->setNeedsMoveAlong(true);
    }
}
void RCodeI2cReceiveCommand::moveAlong(RCodeCommandSlot<RCodeParameters> *slot) const {
    bool tenBit = slot->getFields()->has('N');
    uint16_t address = slot->getFields()->getByte('A', 0, 0);
    if (slot->getFields()->getByteCount('A') > 1) {
        address = (address << 8) | slot->getFields()->getByte('A', 1, 0);
    }
    uint16_t length = slot->getFields()->get('L', 0);
    RCodeI2cBus *bus = RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0));
    bus->asyncReceive(address, tenBit, length, slot, &RCodeI2cReceiveCommand::setAsFinished, bus->getCallbackData() - 1);
}

void RCodeI2cReceiveCommand::finish(RCodeCommandSlot<RCodeParameters> *slot, RCodeOutStream<RCodeParameters> *out) const {
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
    if (slot->getFields()->get('B', 0) >= 4 * RCodeI2cParameters::i2cBussesPerPhyBus) {
        return;
    }
    uint16_t length = slot->getFields()->get('L', 0);
    if (length == 0) {
        return;
    }
    out->writeBigHexField(bus->getReadBuffer(), length);
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
        out->writeStatus(CMD_FAIL);
    } else {
        out->writeStatus(OK);
    }
    bus->freeReadBuffer();
}

void RCodeI2cReceiveCommand::execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
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
    if (bus >= 4 * RCodeI2cParameters::i2cBussesPerPhyBus) {
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
    uint16_t length = slot->getFields()->get('L', 0);
    if (length == 0) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Cannot perform 0 byte I2C read");
        slot->setComplete(true);
        return;
    }
    RCodeI2cSubsystem::getRCodeBus(bus)->activateBus();
    RCodeI2cSubsystem::getRCodeBus(bus)->asyncReceive(address, tenBit, length, slot,
            &RCodeI2cReceiveCommand::setAsFinished, retries - 1);
}

void RCodeI2cReceiveCommand::setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    if (slot->getFields()->countFieldSections('B') <= 1) {
        locks->addLock(RCodeI2cSubsystem::getRCodeBus(slot->getFields()->get('B', 0))->getBusLock(), true);
    }
}
