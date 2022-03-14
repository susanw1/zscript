/*
 * ZcodePersistentStoreCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/ZcodePersistentStoreCommand.hpp"

void ZcodePersistentStoreCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot,
        ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
    if (!slot->getFields()->has('A')) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Address must be specified");
    } else {
        uint8_t addrLen = slot->getFields()->getByteCount('A');
        if (addrLen > 4) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Cannot store data beyond persistent store length");
        } else {
            uint32_t addr = 0;
            for (int i = 0; i < addrLen; ++i) {
                addr <<= 8;
                addr |= slot->getFields()->getByte('A', i, 0);
            }
            if (slot->getBigField()->getLength() + addr
                    >= ZcodeParameters::persistentMemorySize) {
                slot->fail("", BAD_PARAM);
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField(
                        "Cannot store data beyond persistent store length");
            } else {
                persist->writePersistent(addr,
                        slot->getBigField()->getData(),
                        slot->getBigField()->getLength());
                out->writeStatus(OK);
            }
        }
    }
    slot->setComplete(true);
}