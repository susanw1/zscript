/*
 * RCodeExecutionStateCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeCommand.hpp"
#include "../executionspace/RCodeExecutionSpace.hpp"

class RCodeExecutionStateCommand: public RCodeCommand {
private:
    const uint8_t code = 0x20;
    RCodeExecutionSpace *space;
public:
    RCodeExecutionStateCommand(RCodeExecutionSpace *space) :
            space(space) {

    }
    virtual void finish(RCodeCommandSlot *rCodeCommandSlot,
            RCodeOutStream *out) const {
    }

    virtual void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    virtual void setLocks(RCodeLockSet *locks) const {
        locks->addLock(RCodeLockValues::executionSpaceLock, false);
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeExecutionStateCommand::code;
    }

    virtual uint8_t getCodeLength() const {
        return 1;
    }

    virtual uint8_t const* getFullCode() const {
        return &code;
    }
};
#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_ */
