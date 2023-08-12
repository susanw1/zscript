/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINREADCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINREADCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <arduino/pins-module/pin-controller/PinController.hpp>

#define COMMAND_EXISTS_0033 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptPinReadCommand {

public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getField('P', &pin)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (pin >= ZP::pinCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }

        PinController<ZP>::readPin(pin, ctx);
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINREADCOMMAND_HPP_ */
