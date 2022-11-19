/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_OUTER_CORE_ZCODEADDRESSCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_OUTER_CORE_ZCODEADDRESSCOMMAND_HPP_

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

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_OUTER_CORE_ZCODEADDRESSCOMMAND_HPP_ */
