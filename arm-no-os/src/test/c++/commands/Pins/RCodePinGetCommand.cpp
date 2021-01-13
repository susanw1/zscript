/*
 * RCodePinGetCommand.cpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#include "RCodePinGetCommand.hpp"

void RCodePinGetCommand::execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
        RCodeOutStream<RCodeParameters> *out) {
    slot->setComplete(true);
    slot->getFields()->copyFieldTo(out, 'P');
    if (!slot->getFields()->has('P')) {
        slot->fail("", MISSING_PARAM);
        out->writeStatus(MISSING_PARAM);
        out->writeBigStringField("Must specify valid pin");
    }
    uint8_t pinNum = 0;
    if (slot->getFields()->getByteCount('P') > 1) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Must specify valid pin");
        return;
    }
    pinNum = slot->getFields()->getByte('P', 0, 0);
    if (pinNum >= RCodePeripheralParameters::pinNumber) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Must specify valid pin");
        return;
    }
    GpioPin *pin = GpioManager::getPin(RCodePinSystem::getRcodePinName(pinNum));

    if (slot->getFields()->has('A')) {
        if (!AtoDManager::canPerformAtoD(RCodePinSystem::getRcodePinName(pinNum))) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Pin cannot perform analog read");
            return;
        }
        pin->setMode(Analog);
        out->writeField('V', (uint16_t)(AtoDManager::performAtoD(RCodePinSystem::getRcodePinName(pinNum)) << 4));
    } else {
        if (pin->read()) {
            out->writeField('V', (uint8_t) 1);
        } else {
            out->writeField('V', (uint8_t) 0);
        }
    }
    out->writeStatus(OK);
}
