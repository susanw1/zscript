/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>

#define COMMAND_EXISTS_0030 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptPinCapabilitiesCommand {
public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField('C', MODULE_CAPABILITIES(003));
        out.writeField('P', ZP::pinCount);
    }

};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINCAPABILITIESCOMMAND_HPP_ */
