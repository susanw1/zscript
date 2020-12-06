/*
 * RCodeCapabilitiesCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeCapabilitiesCommand.hpp"

void RCodeCapabilitiesCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (RCodeParameters::bigBigFieldLength > 0) {
        RCodeOutStream::writeFieldType(out, 'B',
                RCodeParameters::bigBigFieldLength);
    } else {
        RCodeOutStream::writeFieldType(out, 'B',
                RCodeParameters::bigFieldLength);
    }
    uint8_t capabilities = 0x02;
    if (RCodeParameters::slotNum > 1) {
        capabilities |= 0x04;
    }
    RCodeOutStream::writeFieldType(out, 'N', RCodeParameters::fieldNum);
    RCodeOutStream::writeFieldType(out, 'M', 1);
    out->writeField('U', sizeof(fieldUnit));
    RCodeSupportedCommandArray supported =
            rcode->getCommandFinder()->getSupportedCommands();
    out->writeBigHexField(supported.cmds, supported.cmdNum);
    slot->setComplete(true);
}

