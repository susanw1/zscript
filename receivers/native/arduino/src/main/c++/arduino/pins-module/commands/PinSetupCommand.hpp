/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <arduino/pins-module/pin-controller/PinController.hpp>

#define COMMAND_EXISTS_0031 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptPinSetupCommand {

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

        uint16_t mode;
        bool hasMode = ctx.getField('C', &mode);
        uint16_t internalMode = 0;
        ctx.getField('M', &internalMode);
        if (PinController<ZP>::pinInfo(pin, ctx) && hasMode) {
            PinController<ZP>::setupPin(pin, (PinControllerMode) mode, internalMode, ctx);
        }
    }

};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZSCRIPTPINSETUPCOMMAND_HPP_ */
