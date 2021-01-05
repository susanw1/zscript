/*
 * RCodeExecutionStoreCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_
#define SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeCommand.hpp"
#include "../executionspace/RCodeExecutionSpace.hpp"

template<class RP>
class RCodeExecutionStoreCommand: public RCodeCommand<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
    typedef typename RP::fieldUnit_t fieldUnit_t;
    private:
    const uint8_t code = 0x22;
    RCodeExecutionSpace<RP> *space;
    public:
    RCodeExecutionStoreCommand(RCodeExecutionSpace<RP> *space) :
            space(space) {
    }

    virtual void execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out);

    virtual void setLocks(RCodeCommandSlot<RP> *slot, RCodeLockSet<RP> *locks) const {
        locks->addLock(RP::executionSpaceLock, true);
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

template<class RP>
void RCodeExecutionStoreCommand<RP>::execute(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence, RCodeOutStream<RP> *out) {
    executionSpaceAddress_t address = 0;
    bool fits = true;
    if (slot->getFields()->has('A')) {
        uint8_t effectiveSize = slot->getFields()->getByteCount('A');
        if (effectiveSize < sizeof(executionSpaceAddress_t)) {
            for (uint8_t i = 0; i < effectiveSize; i++) {
                address = (executionSpaceAddress_t) ((address << 8) | slot->getFields()->getByte('A', i, 0));
            }
        } else {
            fits = false;
        }
    }
    if (fits && address + slot->getBigField()->getLength() < RP::executionLength) {
        space->write(slot->getBigField()->getData(), slot->getBigField()->getLength(), address, slot->getFields()->has('L'));
        out->writeStatus(OK);
    } else {
        out->writeStatus(BAD_PARAM);
        slot->fail("", BAD_PARAM);
        out->writeBigStringField("Write goes off end of execution space");
    }
    slot->setComplete(true);
}

#include "../RCodeOutStream.hpp"
#include "../parsing/RCodeCommandSlot.hpp"
#endif /* SRC_TEST_CPP_RCODE_COMMANDS_RCODEEXECUTIONSTORECOMMAND_HPP_ */
