/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PWMPINWRITECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PWMPINWRITECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0039 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class PwmPinWriteCommand {
    static constexpr char ParamPin__P = 'P';
    static constexpr char ParamValue__V = 'V';

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
        uint16_t value;
        if (!ctx.getField(ParamValue__V, &value)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        if (digitalPinHasPWM(pin) && !PIN_SUPPORTS_ANALOG_WRITE(pin)) {
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

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PWMPINWRITECOMMAND_HPP_ */
