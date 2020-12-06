/*
 * RCodeExecutionCommand.cpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionCommand.hpp"
#ifdef NOTIFICATIONS

void RCodeExecutionCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    bool worked = true;
    int16_t target = -1;
    if (slot->getFields()->has('T')) {
        if (slot->getFields()->get('T', 0x00) >= space->getChannelNum()
                || slot->getFields()->countFieldSections('T') != 1) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Not a valid Execution Space Channel");
        } else {
            target = slot->getFields()->get('T', 0xFF);
        }
    }
    if (worked && slot->getFields()->has('A')) {
        bool fits = false;
        executionSpaceAddress_t targetPos = 0;
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
                targetPos <<= 8 + sizeof(fieldUnit);
                targetPos = slot->getFields()->get('A', i, 0);
            }
        }
        if (!fits || targetPos >= space->getLength()) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField(
                    "Target Address beyond end of Execution Space");
        } else {
            if (target == -1) {
                for (int i = 0; i < space->getChannelNum(); ++i) {
                    space->getChannels()[i]->move(targetPos);
                }
            } else {
                space->getChannels()[target]->move(targetPos);
            }
        }
    }
    if (worked && slot->getFields()->has('D')) {
        if (slot->getFields()->get('D', 0x00) > 0xFF
                || slot->getFields()->countFieldSections('D') != 1) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Delay larger than 1 byte");
        } else {
            space->setDelay((uint8_t) slot->getFields()->get('D', 0x00));
        }
    }
    if (worked && slot->getFields()->has('G')) {
        space->setRunning(true);
    } else {
        space->setRunning(false);
    }
    space->setFailed(false);
    if (worked) {
        out->writeStatus(OK);
    }
    slot->setComplete(true);
}
#endif
