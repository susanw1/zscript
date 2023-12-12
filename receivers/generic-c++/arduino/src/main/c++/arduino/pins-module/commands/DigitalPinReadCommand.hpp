/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINREADCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINREADCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0043 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class DigitalPinReadCommand: public DigitalRead_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getReqdFieldCheckLimit(ReqPin__P, ZP::pinCount, &pin)) {
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespValue__V, digitalRead(pin) == HIGH ? high_value : low_value);
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINREADCOMMAND_HPP_ */
