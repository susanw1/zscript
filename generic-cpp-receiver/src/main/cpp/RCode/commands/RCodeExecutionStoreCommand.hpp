/*
 * RCodeExecutionStoreCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#ifdef NOTIFICATIONS
#include "RCodeCommand.hpp"
#include "../executionspace/RCodeExecutionSpace.hpp"

class RCodeExecutionStoreCommand: public RCodeCommand {
private:
    const uint8_t code = 0x22;
    RCodeExecutionSpace *space;
public:
    RCodeExecutionStoreCommand(RCodeExecutionSpace *space) :
            space(space) {

    }
    virtual void finish(RCodeCommandSlot *rCodeCommandSlot,
            RCodeOutStream *out) const {
    }

    virtual void execute(RCodeCommandSlot *slot, RCodeCommandSequence *sequence,
            RCodeOutStream *out);

    virtual void setLocks(RCodeCommandSlot *slot, RCodeLockSet *locks) const {
        locks->addLock(RCodeLockValues::executionSpaceLock, true);
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == RCodeExecutionStoreCommand::code;
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
#endif
#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_ */
