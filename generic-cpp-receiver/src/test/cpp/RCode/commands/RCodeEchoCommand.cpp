/*
 * RCodeEchoCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeEchoCommand.hpp"

void RCodeEchoCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (!slot->getFields()->has('S')) {
        out->writeStatus(OK);
    }
    slot->getFields()->copyTo(out);
    slot->getBigField()->copyTo(out);
    slot->setComplete(true);
}
