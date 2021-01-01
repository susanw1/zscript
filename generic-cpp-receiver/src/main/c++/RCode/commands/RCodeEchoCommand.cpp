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
    } else {
        slot->fail("", (RCodeResponseStatus) slot->getFields()->get('S', 0));
        out->writeStatus((RCodeResponseStatus) slot->getFields()->get('S', 0));
    }
    slot->getFields()->copyTo(out);
    slot->getBigField()->copyTo(out);
    slot->setComplete(true);
}
