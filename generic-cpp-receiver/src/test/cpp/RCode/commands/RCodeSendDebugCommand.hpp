/*
 * RCodeSendDebugCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_

#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeCommand.hpp"

class RCodeOutStream;
class RCodeDebugOutput;
class RCode;

class RCodeSendDebugCommand: public RCodeCommand {
private:
    const uint8_t code = 0xff;
    RCode *const rcode;
public:
    RCodeSendDebugCommand(RCode *const rcode) :
            rcode(rcode) {

    }
    bool continueLocking(RCodeCommandChannel *c) const {
        return true;
    }

    void finish(RCodeCommandSlot *rCodeCommandSlot, RCodeOutStream *out) const {
    }

    void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    void setLocks(RCodeLockSet *locks) const {
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

#include "../RCode.hpp"
#include "../RCodeOutStream.hpp"
#include "../RCodeDebugOutput.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODESENDDEBUGCOMMAND_HPP_ */
