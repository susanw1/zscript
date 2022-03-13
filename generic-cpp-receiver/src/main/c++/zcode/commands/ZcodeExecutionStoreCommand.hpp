/*
 * ZcodeExecutionStoreCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTORECOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTORECOMMAND_HPP_
#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"
#include "../executionspace/ZcodeExecutionSpace.hpp"

template<class ZP>
class ZcodeExecutionStoreCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;
    typedef typename ZP::fieldUnit_t fieldUnit_t;
    private:
    const uint8_t code = 0x22;
    ZcodeExecutionSpace<ZP> *space;
    public:
    ZcodeExecutionStoreCommand(ZcodeExecutionSpace<ZP> *space) :
            space(space) {
    }

    virtual void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out);

    virtual void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
        locks->addLock(ZP::executionSpaceLock, true);
    }

    virtual uint8_t getCode() const {
        return code;
    }

    virtual bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeExecutionStoreCommand::code;
    }

    virtual uint8_t getCodeLength() const {
        return 1;
    }

    virtual uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class ZP>
void ZcodeExecutionStoreCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
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
    if (fits && address + slot->getBigField()->getLength() < ZP::executionLength) {
        space->write(slot->getBigField()->getData(), slot->getBigField()->getLength(), address, slot->getFields()->has('L'));
        out->writeStatus(OK);
    } else {
        out->writeStatus(BAD_PARAM);
        slot->fail("", BAD_PARAM);
        out->writeBigStringField("Write goes off end of execution space");
    }
    slot->setComplete(true);
}

#include "../ZcodeOutStream.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"
#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONSTORECOMMAND_HPP_ */
