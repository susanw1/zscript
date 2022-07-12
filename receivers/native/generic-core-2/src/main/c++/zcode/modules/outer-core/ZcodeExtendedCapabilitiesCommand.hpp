/*
 * ZcodeExtendedCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../ZcodeModule.hpp"

#define COMMAND_VALUE_0010 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeExtendedCapabilitiesCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x10;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('C', MODULE_CAPABILITIES(001));
        out->writeStatus(OK);
    }

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_ */
