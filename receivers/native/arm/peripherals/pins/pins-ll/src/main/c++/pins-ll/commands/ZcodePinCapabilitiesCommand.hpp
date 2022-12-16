/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0030 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodePinCapabilitiesCommand: public ZcodeCommand<ZP> {
public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        out->writeField8('C', MODULE_CAPABILITIES(003));
        out->writeField8('P', ZP::LL::HW::pinCount);
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_COMMANDS_ZCODEPINCAPABILITIESCOMMAND_HPP_ */
