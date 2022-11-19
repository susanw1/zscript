/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINWRITECOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINWRITECOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arduino/pins-module/pin-controller/PinController.hpp>

#define COMMAND_EXISTS_0032 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodePinWriteCommand: public ZcodeCommand<ZP> {

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t pin;
        if (!slot.getFields()->get('P', &pin)) {
            slot.fail(BAD_PARAM, "No pin specified");
            return;
        }
        if (pin >= ZP::pinCount) {
            slot.fail(BAD_PARAM, "Invalid pin");
            return;
        }

        uint16_t value = 0;
        if (!slot.getFields()->get('V', &value)) {
            slot.fail(BAD_PARAM, "No value given");
            return;
        }
        if (PinController<ZP>::writePin(pin, value, slot)) {
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_COMMANDS_ZCODEPINWRITECOMMAND_HPP_ */
