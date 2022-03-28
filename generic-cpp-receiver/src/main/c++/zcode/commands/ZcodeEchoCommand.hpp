/*
 * ZcodeEchoCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"

template<class ZP>
class ZcodeEchoCommand: public ZcodeCommand<ZP> {
    private:
        const uint8_t code = 0x01;

    public:
        virtual void execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out);

        virtual void setLocks(ZcodeCommandSlot<ZP> *slot, ZcodeLockSet<ZP> *locks) const {
        }

        virtual uint8_t getCode() const {
            return code;
        }

        virtual bool matchesCode(uint8_t code[], uint8_t length) const {
            return length == 1 && code[0] == ZcodeEchoCommand::code;
        }

        virtual uint8_t getCodeLength() const {
            return 1;
        }

        virtual uint8_t const* getFullCode() const {
            return &code;
        }
};

template<class ZP>
void ZcodeEchoCommand<ZP>::execute(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence, ZcodeOutStream<ZP> *out) {
    if (!slot->getFields()->has(Zchars::STATUS_RESP_PARAM)) {
        out->writeStatus(OK);
    } else {
        slot->fail("", (ZcodeResponseStatus) slot->getFields()->get(Zchars::STATUS_RESP_PARAM, 0));
        out->writeStatus((ZcodeResponseStatus) slot->getFields()->get(Zchars::STATUS_RESP_PARAM, 0));
    }
    slot->getFields()->copyTo(out);
    slot->getBigField()->copyTo(out);
    slot->setComplete(true);
}

#include "../ZcodeOutStream.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_ */
