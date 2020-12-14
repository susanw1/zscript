/*
 * RCodeFetchGiudCommand.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "../persistence/RCodeFetchGiudCommand.hpp"

void RCodeFetchGiudCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    out->writeStatus(OK);
    RCodeOutStream::writeFieldType(out, 'P', persist->getStuff());
    out->writeBigHexField(persist->getGuid(), 16);
    slot->setComplete(true);
}
