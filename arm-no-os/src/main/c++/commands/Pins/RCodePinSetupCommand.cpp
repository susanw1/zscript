/*
 * RCodePinSetupCommand.cpp
 *
 *  Created on: 8 Jan 2021
 *      Author: robert
 */

#include "RCodePinSetupCommand.hpp"

void RCodePinSetupCommand::execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
        RCodeOutStream<RCodeParameters> *out) {
    slot->setComplete(true);
    out->writeField('P', (uint8_t) RCodePeripheralParameters::pinNumber);
    out->writeField('N', (uint8_t) 0);
    out->writeField('M', (uint8_t) 1);
    if (slot->getBigField()->getLength() > (RCodePeripheralParameters::pinNumber + 1) / 2) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Cannot give setup data for pins which do not exist");
        return;
    }
    if (slot->getBigField()->getLength() != 0 && slot->getBigField()->getLength() != (RCodePeripheralParameters::pinNumber + 1) / 2) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Cannot give setup data for a subset of pins");
        return;
    }
    RCodePinInterruptSource::clearInterrupts();
    for (int i = 0; i < slot->getBigField()->getLength(); ++i) {
        uint8_t data1 = slot->getBigField()->getData()[i] & 0xF;
        if (data1 & 0x8) {
            if (!RCodePinInterruptSource::setInterrupt(RCodePinSystem::getRcodePinName(i * 2), data1 & 0x1, data1 & 0x2)) {
                RCodePinInterruptSource::clearInterrupts();
                slot->fail("", CMD_FAIL);
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("Setup includes incompatible pin interrupts");
                return;
            }
        }
    }
    uint8_t pinData[RCodePeripheralParameters::pinNumber];
    for (int i = 0; i < RCodePeripheralParameters::pinNumber; i++) {
        pinData[i] = 0xFE;
        if (AtoDManager::canPerformAtoD(RCodePinSystem::getRcodePinName(RCodePeripheralParameters::pinNumber - i))) {
            pinData[i] |= 1;
        }
    }
    out->writeBigHexField(pinData, RCodePeripheralParameters::pinNumber);
    out->writeStatus(OK);
}
