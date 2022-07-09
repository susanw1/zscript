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
class ZcodeAddressRouter;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeAddressCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x0f;
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        if (slot.getZcode()->getAddressRouter() != NULL) {
            slot.getZcode()->getAddressRouter()->route(slot, 0);
        } else {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingNotSetup);
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESENDDEBUGCOMMAND_HPP_ */
