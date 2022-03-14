/*
 * ZcodePinSetCommand.cpp
 *
 *  Created on: 12 Jan 2021
 *      Author: robert
 */

#include "ZcodePinSetCommand.hpp"

void ZcodePinSetCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence,
        ZcodeOutStream<ZcodeParameters> *out) {
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
    if (pinNum >= ZcodePeripheralParameters::pinNumber) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Must specify valid pin");
        return;
    }
    GpioPin *pin = GpioManager::getPin(ZcodePinSystem::getZcodePinName(pinNum));
    if (slot->getFields()->has('V')) {
        if (slot->getFields()->getByteCount('V') > 1 || slot->getFields()->getByte('V', 0, 0) > 1) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Invalid pin output value");
            return;
        }
        if (slot->getFields()->getByte('V', 0, 0) == 0) {
            pin->reset();
        } else {
            pin->set();
        }
    }
    if (slot->getFields()->has('M')) {
        if (slot->getFields()->getByteCount('M') > 1 || slot->getFields()->getByte('M', 0, 0) > 6) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Invalid pin mode");
            return;
        }
        uint8_t mode = slot->getFields()->getByte('M', 0, 0);
        switch (mode) {
        case 0:
            pin->setPullMode(NoPull);
            pin->setMode(Input);
            break;
        case 1:
            pin->setPullMode(NoPull);
            pin->setMode(Output);
            pin->setOutputMode(PushPull);
            break;
        case 2:
            pin->setPullMode(PullUp);
            pin->setMode(Input);
            break;
        case 3:
            pin->setPullMode(PullDown);
            pin->setMode(Input);
            break;
        case 4:
            pin->setPullMode(NoPull);
            pin->setMode(Output);
            pin->setOutputMode(OpenDrain);
            break;
        case 5:
            pin->setPullMode(PullUp);
            pin->setMode(Output);
            pin->setOutputMode(OpenDrain);
            break;
        case 6:
            pin->setPullMode(PullDown);
            pin->setMode(Output);
            pin->setOutputMode(OpenDrain);
            break;
        default:
            break;
        }
    }
    out->writeStatus(OK);
}
