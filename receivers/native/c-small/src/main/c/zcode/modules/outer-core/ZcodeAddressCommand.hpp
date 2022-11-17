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

#define COMMAND_EXISTS_0011 EXISTENCE_MARKER_UTIL

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
        if (slot.getChannel() != slot.getZcode()->getNotificationManager()->getNotificationChannel() || !slot.getChannel()->isFromNotification()) {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingOnlyFromNotificationChannel);
            return;
        }
        if (slot.getBigField()->getData()[slot.getBigField()->getLength() - 1] != EOL_SYMBOL) {
            slot.fail(BAD_ADDRESSING, (string_t) ZP::Strings::failAddressingNoNewLine);
            return;
        }
        slot.quietEnd();
        ZP::AddressRouter::route(slot);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEADDRESSCOMMAND_HPP_ */
