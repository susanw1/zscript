/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODREADCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODREADCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0035 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class AtoDPinReadCommand: public AnalogRead_CommandDefs {
public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getField(ReqPin__P, &pin)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (pin >= ZP::pinCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        if (PIN_SUPPORTS_ANALOG_READ(pin)) {
            out.writeField(RespValue__V, analogRead(pin) << 6);
        } else {
            out.writeField(RespValue__V, digitalRead(pin) == HIGH ? 0xffff : 0);
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODREADCOMMAND_HPP_ */
