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

#define COMMAND_EXISTS_0042 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class DigitalPinWriteCommand: public DigitalWrite_CommandDefs {
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
        if (!PinManager<ZP>::isModeActivated(pin)) {
            digitalWrite(pin, value != low_Value ? HIGH : LOW);
        } else {
            if ((value != low_Value) == PinManager<ZP>::isCollectorNDrain(pin)) {
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

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINWRITECOMMAND_HPP_ */
