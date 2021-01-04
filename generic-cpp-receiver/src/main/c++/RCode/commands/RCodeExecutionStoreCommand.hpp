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
    if (slot->getFields()->has('A')) {
        // FIXME: this should go into FieldMap
        bool fits = false;
        uint8_t fieldSectionNum = slot->getFields()->countFieldSections('A');
        uint16_t effectiveSize = (uint16_t) ((fieldSectionNum - 1) * sizeof(fieldUnit_t));
        fieldUnit_t first = slot->getFields()->get('A', 0);
        while (first != 0) {
            first = (fieldUnit_t) (first >> 8);
            effectiveSize++;
        }
        if (effectiveSize < sizeof(executionSpaceAddress_t)) {
            fits = true;
        }
        if (fits) {
            for (int i = 0; i < fieldSectionNum; i++) {
                address = (executionSpaceAddress_t) ((address << (8 * sizeof(fieldUnit_t))) | slot->getFields()->get('A', i, 0));
            }
        }
        // FIXME: needs to fail if !fits
    }
    if (address + slot->getBigField()->getLength() < RP::executionLength) {
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
