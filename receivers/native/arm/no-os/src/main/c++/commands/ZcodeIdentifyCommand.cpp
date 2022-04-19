/*
 * ZcodeEchoCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeIdentifyCommand.hpp"

void ZcodeIdentifyCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
    out->writeStatus(OK);
    out->writeField('V', (uint8_t) 0);
    out->continueField((uint8_t) 0);
    out->continueField((uint8_t) 1);
    out->writeBigStringField("zcode/arm/no-os/initial-demo");
    slot->setComplete(true);
}
