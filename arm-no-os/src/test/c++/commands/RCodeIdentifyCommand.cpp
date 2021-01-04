/*
 * RCodeEchoCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeIdentifyCommand.hpp"

void RCodeIdentifyCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    out->writeStatus(OK);
    out->writeField('V', 0);
    out->continueField(0);
    out->continueField(1);
    out->writeBigStringField("rcode/arm/no-os/initial-demo");
    slot->setComplete(true);
}
