/*
 * RCodeCapabilitiesCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeCapabilitiesCommand.hpp"

void RCodeCapabilitiesCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    out->writeStatus(OK);
    if (RCodeParameters::bigBigFieldLength > 0) {
        RCodeOutStream::writeFieldType(out, 'B',
                (uint32_t) RCodeParameters::bigBigFieldLength);
    } else {
        RCodeOutStream::writeFieldType(out, 'B',
                (uint32_t) RCodeParameters::bigFieldLength);
    }
    uint8_t capabilities = 0x01;
    if (RCodeParameters::slotNum > 1) {
        capabilities |= 0x02;
    }
    if (RCodeParameters::findInterruptSourceAddress) {
        capabilities |= 0x04;
    }
    if (RCodeParameters::isUsingInterruptVector) {
        capabilities |= 0x08;
    }
    out->writeField('C', capabilities);
    if (RCodeParameters::persistentMemorySize != 0) {
        RCodeOutStream::writeFieldType(out, 'P',
                RCodeParameters::persistentMemorySize);
    }
    RCodeOutStream::writeFieldType(out, 'N',
            (uint32_t) RCodeParameters::fieldNum);
    RCodeOutStream::writeFieldType(out, 'M', (uint8_t) 1);
    out->writeField('U', sizeof(fieldUnit));
    RCodeSupportedCommandArray supported =
            rcode->getCommandFinder()->getSupportedCommands();
    out->writeBigHexField(supported.cmds, supported.cmdNum);
    slot->setComplete(true);
}

