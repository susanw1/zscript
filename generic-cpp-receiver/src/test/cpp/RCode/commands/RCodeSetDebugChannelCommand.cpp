/*
 * RCodeSetDebugChannelCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeSetDebugChannelCommand.hpp"

void RCodeSetDebugChannelCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    rcode->getDebug()->setDebugChannel(sequence->getChannel());
    out->writeStatus(OK);
    slot->setComplete(true);
}

