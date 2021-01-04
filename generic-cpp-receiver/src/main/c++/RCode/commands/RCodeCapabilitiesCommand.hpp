/*
 * RCodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCode;

template<class RP>
class RCodeCapabilitiesCommand: public RCodeCommand<RP> {
    typedef typename RP::fieldUnit_t fieldUnit_t;
private:
    RCode<RP> *rcode;
    const uint8_t code = 0x02;
public:
    RCodeCapabilitiesCommand(RCode<RP> *rcode) :
            rcode(rcode) {
    }

    void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeCapabilitiesCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"
#include "../parsing/RCodeCommandFinder.hpp"

template<class RP>
void RCodeCapabilitiesCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    out->writeStatus(OK);
    if (RP::bigBigFieldLength > 0) {
        RCodeOutStream<RP>::writeFieldType(out, 'B', (uint32_t) RP::bigBigFieldLength);
    } else {
        RCodeOutStream<RP>::writeFieldType(out, 'B', (uint32_t) RP::bigFieldLength);
    }
    uint8_t capabilities = 0x01;
    if (RP::slotNum > 1) {
        capabilities |= 0x02;
    }
    if (RP::findInterruptSourceAddress) {
        capabilities |= 0x04;
    }
    if (RP::isUsingInterruptVector) {
        capabilities |= 0x08;
    }
    out->writeField('C', capabilities);
    if (RP::persistentMemorySize != 0) {
        RCodeOutStream<RP>::writeFieldType(out, 'P', RP::persistentMemorySize);
    }
    RCodeOutStream<RP>::writeFieldType(out, 'N', (uint32_t) RP::fieldNum);
    RCodeOutStream<RP>::writeFieldType(out, 'M', (uint8_t) 1);
    out->writeField('U', sizeof(fieldUnit_t));
    RCodeSupportedCommandArray<RP> supported = rcode->getCommandFinder()->getSupportedCommands();
    out->writeBigHexField(supported.cmds, supported.cmdNum);
    slot->setComplete(true);
}

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODECAPABILITIESCOMMAND_HPP_ */
