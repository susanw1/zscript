/*
 * ZcodeSendDebugCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEADDRESSCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEADDRESSCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_VALUE_0011 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeAddressRouter;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeAddressCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x11;
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZP::AddressRouter::route(slot);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEADDRESSCOMMAND_HPP_ */
