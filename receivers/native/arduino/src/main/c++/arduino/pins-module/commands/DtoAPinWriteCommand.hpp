/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DTOAWRITECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DTOAWRITECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0047 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class DtoAPinWriteCommand: public AnalogWrite_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getReqdFieldCheckLimit(ReqPin__P, ZP::pinCount, &pin)) {
            return;
        }
        uint16_t value;
        if (!ctx.getField(ReqValue__V, &value)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        if (PIN_SUPPORTS_ANALOG_WRITE(pin)) {
            analogWrite(pin, value >> 8);
        } else {
            if (value >= 0x8000) {
                digitalWrite(pin, HIGH);
            } else {
                digitalWrite(pin, LOW);
            }
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DTOAWRITECOMMAND_HPP_ */
