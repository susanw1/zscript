/*
 * RCodePersistentFetchCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodePersistentFetchCommand.hpp"

void RCodePersistentFetchCommand::execute(RCodeCommandSlot<RCodeParameters> *slot,
        RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
    if (!slot->getFields()->has('A')) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Address must be specified");
    } else if (!slot->getFields()->has('L')) {
        slot->fail("", BAD_PARAM);
        out->writeStatus(BAD_PARAM);
        out->writeBigStringField("Read length must be specified");
    } else {
        uint8_t addrLen = slot->getFields()->getByteCount('A');
        uint8_t lengthLen = slot->getFields()->getByteCount('L');
        if (addrLen > 4 || lengthLen > 4) {
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Cannot read data beyond persistent store length");
        } else {
            uint32_t length = 0;
            for (int i = 0; i < lengthLen; ++i) {
                length <<= 8;
                length |= slot->getFields()->getByte('L', i, 0);
            }
            uint32_t addr = 0;
            for (int i = 0; i < addrLen; ++i) {
                addr <<= 8;
                addr |= slot->getFields()->getByte('A', i, 0);
            }
            if (addr + length >= RCodeParameters::persistentMemorySize) {
                slot->fail("", BAD_PARAM);
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
