/*
 * RCodeSendDebugCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeSendDebugCommand.hpp"

void RCodeSendDebugCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    rcode->getDebug()->println((const char*) slot->getBigField()->getData(),
            slot->getBigField()->getLength());
    out->writeStatus(OK);
    slot->setComplete(true);
}

