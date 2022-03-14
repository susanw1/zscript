/*
 * ZcodeExecutionStateCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTATECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTATECOMMAND_HPP_
#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"
#include "../executionspace/ZcodeExecutionSpace.hpp"

template<class ZP>
class ZcodeExecutionStateCommand: public ZcodeCommand<ZP> {
private:
    const uint8_t code = 0x20;
    ZcodeExecutionSpace<ZP> *space;
    public:
    ZcodeExecutionStateCommand(ZcodeExecutionSpace<ZP> *space) :
            space(space) {
    }

    virtual void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out);

    virtual void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
        locks->addLock(ZP::executionSpaceLock, false);
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeExecutionStateCommand::code;
    }

    virtual uint8_t getCodeLength() const {
        return 1;
    }

    virtual uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class ZP>
void ZcodeExecutionStateCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
    out->writeField('D', space->getDelay());
    if (space->hasFailed()) {
        out->writeField('F', (uint8_t) 0);
    }
    out->writeField('G', (uint8_t) space->isRunning());
    out->writeField('L', space->getLength());
    out->writeField('M', ZP::executionLength);
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../ZcodeOutStream.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTATECOMMAND_HPP_ */
