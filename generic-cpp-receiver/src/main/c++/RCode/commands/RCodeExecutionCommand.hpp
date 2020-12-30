/*
 * RCodeExecutionCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONCOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONCOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#ifdef NOTIFICATIONS
#include "RCodeCommand.hpp"
#include "../executionspace/RCodeExecutionSpace.hpp"

class RCodeExecutionCommand: public RCodeCommand {
private:
    const uint8_t code = 0x21;
    RCodeExecutionSpace *space;
public:
    RCodeExecutionCommand(RCodeExecutionSpace *space) :
            space(space) {

    }

    virtual void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    virtual void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
        locks->addLock(RCodeLockValues::executionSpaceLock, false);
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeExecutionCommand::code;
    }

    virtual uint8_t getCodeLength() const {
        return 1;
    }

    virtual uint8_t const* getFullCode() const {
        return &code;
    }
};
#include "../executionspace/RCodeExecutionSpaceChannel.hpp"

#endif
#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONCOMMAND_HPP_ */
