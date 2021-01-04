/*
 * RCodeSendDebugCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeDebugOutput;

template<class RP>
class RCode;

template<class RP>
class RCodeSendDebugCommand: public RCodeCommand<RP> {
private:
    const uint8_t code = 0x1f;
    RCode<RP> *const rcode;
public:
    RCodeSendDebugCommand(RCode<RP> *const rcode) :
            rcode(rcode) {
    }

    void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeSendDebugCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class RP>
void RCodeSendDebugCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    rcode->getDebug().println((const char*) slot->getBigField()->getData(), slot->getBigField()->getLength());
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../RCode.hpp"
#include "../RCodeOutStream.hpp"
#include "../RCodeDebugOutput.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_ */
