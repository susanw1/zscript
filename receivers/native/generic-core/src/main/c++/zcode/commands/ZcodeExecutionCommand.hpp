/*
 * ZcodeExecutionCommand.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONCOMMAND_HPP_
#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"
#include "../executionspace/ZcodeExecutionSpace.hpp"

template<class ZP>
class ZcodeExecutionCommand: public ZcodeCommand<ZP> {
    private:
        typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;
        typedef typename ZP::fieldUnit_t fieldUnit_t;

        const uint8_t code = 0x21;
        ZcodeExecutionSpace<ZP> *space;

    public:
        ZcodeExecutionCommand(ZcodeExecutionSpace<ZP> *space) :
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
            return length == 1 && code[0] == ZcodeExecutionCommand::code;
        }

        virtual uint8_t getCodeLength() const {
            return 1;
        }

        virtual uint8_t const* getFullCode() const {
            return &code;
        }
};

template<class ZP>
void ZcodeExecutionCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
    bool worked = true;
    int16_t target = -1;
    if (slot->getFields()->has('T')) {
        if (slot->getFields()->get('T', 0x00) >= space->getChannelNum() || slot->getFields()->countFieldSections('T') != 1) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Not a valid Execution Space Channel");
        } else {
            target = slot->getFields()->get('T', 0xFF);
        }
    }
    if (worked && slot->getFields()->has('A')) {
        bool fits = false;
        executionSpaceAddress_t address = 0;
        uint8_t effectiveSize = slot->getFields()->getByteCount('A');
        if (effectiveSize < sizeof(executionSpaceAddress_t)) {
            fits = true;
            for (uint8_t i = 0; i < effectiveSize; i++) {
                address = (executionSpaceAddress_t) ((address << 8) | slot->getFields()->getByte('A', i, 0));
            }
        }
        if (!fits || address >= space->getLength()) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Target Address beyond end of Execution Space");
        } else {
            if (target == -1) {
                for (int i = 0; i < space->getChannelNum(); ++i) {
                    space->getChannels()[i]->move(address);
                }
            } else {
                space->getChannels()[target]->move(address);
            }
        }
    }
    if (worked && slot->getFields()->has('D')) {
        if (slot->getFields()->get('D', 0x00) > 0xFF || slot->getFields()->countFieldSections('D') != 1) {
            worked = false;
            slot->fail("", BAD_PARAM);
            out->writeStatus(BAD_PARAM);
            out->writeBigStringField("Delay larger than 1 byte");
        } else {
            space->setDelay((uint8_t) slot->getFields()->get('D', 0x00));
        }
    }
    if (worked && slot->getFields()->has('G')) {
        space->setRunning(true);
    } else {
        space->setRunning(false);
    }
    space->setFailed(false);
    if (worked) {
        out->writeStatus(OK);
    }
    slot->setComplete(true);
}

#include "../executionspace/ZcodeExecutionSpaceChannel.hpp"

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXECUTIONCOMMAND_HPP_ */
