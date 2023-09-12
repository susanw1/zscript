/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0034 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class AtoDPinSetupCommand {
    static constexpr char ParamPin__P = 'P';

    static constexpr char RespBitCount__B = 'B';

public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getField(ParamPin__P, &pin)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (pin >= ZP::pinCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        if (PIN_SUPPORTS_ANALOG_READ(pin)) {
            out.writeField(RespBitCount__B, 10);
        } else {
            out.writeField(RespBitCount__B, 1);
        }
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_ */
