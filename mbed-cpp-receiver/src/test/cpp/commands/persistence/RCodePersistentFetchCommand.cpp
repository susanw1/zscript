/*
 * RCodePersistentFetchCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodePersistentFetchCommand.hpp"

void RCodePersistentFetchCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (!slot->getFields()->has('A')) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Address must be specified");
    } else if (!slot->getFields()->has('L')) {
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Read length must be specified");
    } else {
        uint8_t addrLen = slot->getFields()->countFieldSections('A');
        uint8_t lengthLen = slot->getFields()->countFieldSections('L');
        if (addrLen > 4) {
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Cannot read data beyond persistent store length");
        } else if (lengthLen > 4) {
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Cannot read data beyond persistent store length");
        } else {
            uint32_t length = 0;
            for (int i = 0; i < lengthLen; ++i) {
                length <<= 8;
                length |= slot->getFields()->get('L', i, 0);
            }
            uint32_t addr = 0;
            for (int i = 0; i < addrLen; ++i) {
                addr <<= 8;
                addr |= slot->getFields()->get('A', i, 0);
            }
            if (addr + length >= RCodeParameters::persistentMemorySize) {
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField(
                        "Cannot read data beyond persistent store length");
            } else {
                out->writeStatus(OK);
                out->writeBigHexField(persist->getPersistentMemory(), length);
            }
        }
    }
    slot->setComplete(true);
}
