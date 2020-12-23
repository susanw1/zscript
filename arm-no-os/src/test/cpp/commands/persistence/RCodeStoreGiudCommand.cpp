/*
 * RCodeStoreGiudCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodeStoreGiudCommand.hpp"

void RCodeStoreGiudCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (slot->getBigField()->getLength() != 16) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("GUIDs must be 16 bytes long");
    } else {
        if (persist->writeGuid(slot->getBigField()->getData())) {
            out->writeStatus(OK);
        } else {
            out->writeStatus(CMD_FAIL);
        }
    }
    slot->setComplete(true);
}
