/*
 * RCodeFetchGiudCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodeFetchGiudCommand.hpp"

void RCodeFetchGiudCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (persist->hasGuid()) {
        out->writeStatus(OK);
    } else {
        out->writeStatus(CMD_FAIL);
    }
    out->writeBigHexField(persist->getGuid(), 16);
    slot->setComplete(true);
}
