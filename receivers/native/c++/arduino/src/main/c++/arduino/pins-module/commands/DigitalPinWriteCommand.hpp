/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINWRITECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINWRITECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0032 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class DigitalPinWriteCommand {
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
        if (!PinManager<ZP>::isModeActivated(pin)) {
            digitalWrite(pin, value ? HIGH : LOW);
        } else {
            if ((value != 0) == PinManager<ZP>::isCollectorNDrain(pin)) {
                pinMode(pin, OUTPUT);
            } else {
                if (PinManager<ZP>::isPullup(pin)) {
                    pinMode(pin, INPUT_PULLUP);
                } else {
                    pinMode(pin, INPUT);
                }
            }
        }
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINWRITECOMMAND_HPP_ */
