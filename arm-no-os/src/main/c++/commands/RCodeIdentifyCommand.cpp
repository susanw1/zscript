/*
 * RCodeEchoCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeIdentifyCommand.hpp"

void RCodeIdentifyCommand::execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence,
        RCodeOutStream<RCodeParameters> *out) {
    out->writeStatus(OK);
    out->writeField('V', (uint8_t) 0);
    out->continueField((uint8_t) 0);
    out->continueField((uint8_t) 1);
    out->writeBigStringField("rcode/arm/no-os/initial-demo");
    slot->setComplete(true);
}
