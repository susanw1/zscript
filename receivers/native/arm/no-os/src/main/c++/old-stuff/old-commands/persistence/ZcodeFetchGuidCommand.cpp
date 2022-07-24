/*
 * ZcodeFetchGuidCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../../old-commands/persistence/ZcodeFetchGuidCommand.hpp"

void ZcodeFetchGuidCommand::execute(ZcodeCommandSlot<ZcodeParameters> *slot, ZcodeCommandSequence<ZcodeParameters> *sequence, ZcodeOutStream<ZcodeParameters> *out) {
    if (persist->hasGuid()) {
        out->writeStatus(OK);
    } else {
        slot->fail("", CMD_FAIL);
        out->writeStatus(CMD_FAIL);
    }
    out->writeBigHexField(persist->getGuid(), 16);
    slot->setComplete(true);
}
