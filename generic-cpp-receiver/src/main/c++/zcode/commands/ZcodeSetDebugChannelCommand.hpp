/*
 * ZcodeSetDebugChannelCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESETDEBUGCHANNELCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESETDEBUGCHANNELCOMMAND_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeCommand.hpp"

template<class RP>
class ZcodeOutStream;

template<class RP>
class ZcodeDebugOutput;

template<class RP>
class Zcode;

template<class RP>
class ZcodeSetDebugChannelCommand: public ZcodeCommand<RP> {
private:
    const uint8_t code = 0x09;
    Zcode<RP> *const zcode;
public:
    ZcodeSetDebugChannelCommand(Zcode<RP> *const zcode) :
            zcode(zcode) {
    }

    void execute(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence, ZcodeOutStream<RP> *out);

    void setLocks(ZcodeCommandSlot<RP> *slot, ZcodeLockSet<RP> *locks) const {
    }

    uint8_t getCode() const {
        return code;
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 1 && code[0] == ZcodeSetDebugChannelCommand::code;
    }

    uint8_t getCodeLength() const {
        return 1;
    }

    uint8_t const* getFullCode() const {
        return &code;
    }
};

template<class RP>
void ZcodeSetDebugChannelCommand<RP>::execute(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence, ZcodeOutStream<RP> *out) {
    zcode->getDebug().setDebugChannel(sequence->getChannel());
    out->writeStatus(OK);
    slot->setComplete(true);
}

#include "../Zcode.hpp"
#include "../ZcodeOutStream.hpp"
#include "../ZcodeDebugOutput.hpp"
#include "../parsing/ZcodeCommandSlot.hpp"

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESETDEBUGCHANNELCOMMAND_HPP_ */
