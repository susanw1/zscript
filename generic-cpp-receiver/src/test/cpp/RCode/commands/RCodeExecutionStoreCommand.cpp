/*
 * RCodeExecutionStoreCommand.cpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionStoreCommand.hpp"
void RCodeExecutionStoreCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    executionSpaceAddress_t address = 0;
    if (slot->getFields()->has('A')) {
        bool fits = false;
        int fieldSectionNum = slot->getFields()->countFieldSections('A');
        int effectiveSize = (fieldSectionNum - 1) * sizeof(fieldUnit);
        fieldUnit first = slot->getFields()->get('A', 0);
        while (first != 0) {
            first >>= 8;
            effectiveSize++;
        }
        if (effectiveSize < sizeof(executionSpaceAddress_t)) {
            fits = true;
        }
        if (fits) {
            for (int i = 0; i < fieldSectionNum; i++) {
                address <<= 8 + sizeof(fieldUnit);
                address = slot->getFields()->get('A', i, 0);
            }
        }
    }
    if (address + slot->getBigField()->getLength()
            < RCodeParameters::executionLength) {
        space->write(slot->getBigField()->getData(),
                slot->getBigField()->getLength(), address,
                slot->getFields()->has('L'));
        out->writeStatus(OK);
    } else {
        out->writeStatus(BAD_PARAM);
        slot->fail("", BAD_PARAM);
        out->writeBigStringField("Write goes off end of execution space");
    }
    slot->setComplete(true);
}
