/*
 * ZcodeSendDebugCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESENDDEBUGCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESENDDEBUGCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeSendDebugCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x0f;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        slot.getZcode()->getDebug().println((const char*) slot.getBigField()->getData(), slot.getBigField()->getLength());
        out->writeStatus(OK);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESENDDEBUGCOMMAND_HPP_ */
