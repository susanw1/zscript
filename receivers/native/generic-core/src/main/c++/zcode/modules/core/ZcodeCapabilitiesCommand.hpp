/*
 * ZcodeCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../ZcodeModule.hpp"

#define COMMAND_EXISTS_0000 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCapabilitiesCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x00;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        out->writeField16('C', MODULE_CAPABILITIES(000));
        out->writeField16('M', COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(00));
        out->writeField32('N', (uint32_t) ZP::fieldNum);
    }

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_HPP_ */
