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
        CommandOutStream<ZP> out = ctx.getOutStream();
        uint16_t target = 0;
        if (ctx.getField('C', &target)) {
            if (target >= Zscript < ZP > ::zscript.getChannelCount()) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            if (!ctx.isActivated()) {
                ctx.status(ResponseStatus::NOT_ACTIVATED);
                return;
            }
            if (!Zscript < ZP > ::zscript.getChannels()[target]->reset()) {
                ctx.status(ResponseStatus::COMMAND_FAIL);
                return;
            }
        } else {
            void (*resetFunc)(void) = 0;
            resetFunc();
        }
    }

};
}

#define ZSCRIPT_RESET_COMMAND

#endif /* SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZSCRIPTRESETCOMMAND_HPP_ */
