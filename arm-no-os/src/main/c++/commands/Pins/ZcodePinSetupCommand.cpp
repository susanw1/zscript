/*
 * ZcodePinSetupCommand.cpp
 *
 *  Created on: 8 Jan 2021
 *      Author: robert
 */

#include "ZcodePinSetupCommand.hpp"

void ZcodePinSetupCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence,
        ZcodeOutStream<ZcodeParameters> *out) {
    slot->setComplete(true);
    out->writeField('P', (uint8_t) ZcodePeripheralParameters::pinNumber);
    out->writeField('N', (uint8_t) 0);
    out->writeField('M', (uint8_t) 1);
    if (slot->getBigField()->getLength() > (ZcodePeripheralParameters::pinNumber + 1) / 2) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Cannot give setup data for pins which do not exist");
        return;
    }
    if (slot->getBigField()->getLength() != 0 && slot->getBigField()->getLength() != (ZcodePeripheralParameters::pinNumber + 1) / 2) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Cannot give setup data for a subset of pins");
        return;
    }
    ZcodePinInterruptSource::clearInterrupts();
    for (int i = 0; i < slot->getBigField()->getLength(); ++i) {
        uint8_t data1 = slot->getBigField()->getData()[i] & 0xF;
        if (data1 & 0x8) {
            if (!ZcodePinInterruptSource::setInterrupt(ZcodePinSystem::getRcodePinName(i * 2), data1 & 0x1, data1 & 0x2)) {
                ZcodePinInterruptSource::clearInterrupts();
                slot->fail("", CMD_FAIL);
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("Setup includes incompatible pin interrupts");
                return;
            }
        }
    }
    uint8_t pinData[ZcodePeripheralParameters::pinNumber];
    for (int i = 0; i < ZcodePeripheralParameters::pinNumber; i++) {
        pinData[i] = 0xFE;
        if (AtoDManager::canPerformAtoD(ZcodePinSystem::getRcodePinName(ZcodePeripheralParameters::pinNumber - i))) {
            pinData[i] |= 1;
        }
    }
    out->writeBigHexField(pinData, ZcodePeripheralParameters::pinNumber);
    out->writeStatus(OK);
}
