/*
 * RCodePersistentStoreCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodePersistentStoreCommand.hpp"

void RCodePersistentStoreCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (!slot->getFields()->has('A')) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Address must be specified");
    } else {
        uint8_t addrLen = slot->getFields()->countFieldSections('A');
        if (addrLen > 4) {
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Cannot store data beyond persistent store length");
        } else {
            uint32_t addr = 0;
            for (int i = 0; i < addrLen; ++i) {
                addr <<= 8;
                addr |= slot->getFields()->get('A', i, 0);
            }
            if (slot->getBigField()->getLength() + addr
                    >= RCodeParameters::persistentMemorySize) {
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField(
                        "Cannot store data beyond persistent store length");
            } else {
                if (persist->writePersistent(addr,
                        slot->getBigField()->getData(),
                        slot->getBigField()->getLength()) == 0) {
                    out->writeStatus(OK);
                } else {
                    out->writeStatus(CMD_FAIL);
                }
            }
        }
    }
    slot->setComplete(true);
}
