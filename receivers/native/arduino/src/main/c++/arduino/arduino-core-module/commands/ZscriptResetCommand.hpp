/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZSCRIPTRESETCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZSCRIPTRESETCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>

#define COMMAND_EXISTS_0003 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptResetCommand {

public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        void (*resetFunc)(void) = 0;
        resetFunc();
    }

};
}

#define ZSCRIPT_RESET_COMMAND

#endif /* SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZSCRIPTRESETCOMMAND_HPP_ */
