/*
 * RCodeStoreMacAddressCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodeStoreMacAddressCommand.hpp"

void RCodeStoreMacAddressCommand::execute(RCodeCommandSlot<RCodeParameters> *slot,
        RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
    if (slot->getBigField()->getLength() != 6) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("MAC addresses must be 6 bytes long");
    } else {
        persist->writeMac(slot->getBigField()->getData());
        out->writeStatus(OK);
    }
    slot->setComplete(true);
}
