/*
 * ZcodeEchoCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "ZcodeIdentifyCommand.hpp"

void ZcodeIdentifyCommand::execute(ZcodeCommandSlot *slot,
        ZcodeCommandSequence *sequence, ZcodeOutStream *out) {
    out->writeStatus(OK);
    out->writeField('V', 0);
    out->continueField(0);
    out->continueField(1);
    out->writeBigStringField("C++/MBED/initial-demo");
    slot->setComplete(true);
}
