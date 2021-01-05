/*
 * RCodeExecutionStateCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"
#include "../executionspace/RCodeExecutionSpace.hpp"

template<class RP>
class RCodeExecutionStateCommand: public RCodeCommand<RP> {
private:
    const uint8_t code = 0x20;
    RCodeExecutionSpace<RP> *space;
    public:
    RCodeExecutionStateCommand(RCodeExecutionSpace<RP> *space) :
            space(space) {
    }

    virtual void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    virtual void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
        locks->addLock(RP::executionSpaceLock, false);
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

template<class RP>
void RCodeExecutionStateCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    out->writeField('D', space->getDelay());
    if (space->hasFailed()) {
        out->writeField('F', (uint8_t) 0);
    }
    out->writeField('G', (uint8_t) space->isRunning());
    out->writeField('L', space->getLength());
    out->writeField('M', RP::executionLength);
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTATECOMMAND_HPP_ */
