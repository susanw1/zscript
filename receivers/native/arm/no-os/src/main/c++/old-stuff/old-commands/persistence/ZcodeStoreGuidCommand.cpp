/*
 * ZcodeStoreGuidCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../../old-commands/persistence/ZcodeStoreGuidCommand.hpp"

void ZcodeStoreGuidCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
    if (slot->getBigField()->getLength() != 16) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("GUIDs must be 16 bytes long");
    } else {
        persist->writeGuid(slot->getBigField()->getData());
        out->writeStatus(OK);
    }
    slot->setComplete(true);
}
