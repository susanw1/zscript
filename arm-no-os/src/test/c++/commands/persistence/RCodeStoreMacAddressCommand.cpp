/*
 * RCodeStoreMacAddressCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodeStoreMacAddressCommand.hpp"

void RCodeStoreMacAddressCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (slot->getBigField()->getLength() != 6) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("MAC addresses must be 16 bytes long");
    } else {
        if (persist->writeMac(slot->getBigField()->getData())) {
            out->writeStatus(OK);
        } else {
            out->writeStatus(CMD_FAIL);
        }
    }
    slot->setComplete(true);
}
