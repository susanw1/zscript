/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0031 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class DigitalPinSetupCommand {
    static constexpr char ParamPin__P = 'P';
    static constexpr char ParamModeSelect__M = 'M';

    static constexpr uint16_t ParamModeSelect__Input = 0;
    static constexpr uint16_t ParamModeSelect__InputPullUp = 1;
    static constexpr uint16_t ParamModeSelect__InputPullDown = 2;
    static constexpr uint16_t ParamModeSelect__Output = 4;
    static constexpr uint16_t ParamModeSelect__OpenDrain = 8;
    static constexpr uint16_t ParamModeSelect__OpenDrainPullUp = 9;
    static constexpr uint16_t ParamModeSelect__OpenDrainPullDown = 10;
    static constexpr uint16_t ParamModeSelect__OpenCollector = 12;
    static constexpr uint16_t ParamModeSelect__OpenCollectorPullUp = 13;
    static constexpr uint16_t ParamModeSelect__OpenCollectorPullDown = 14;

    static constexpr char RespSupportedModes__M = 'M';

    static constexpr uint16_t RespSupportedModes__Input = 0x1;
    static constexpr uint16_t RespSupportedModes__InputPullUp = 0x2;
    static constexpr uint16_t RespSupportedModes__InputPullDown = 0x4;
    static constexpr uint16_t RespSupportedModes__Output = 0x10;
    static constexpr uint16_t RespSupportedModes__OpenDrain = 0x100;
    static constexpr uint16_t RespSupportedModes__OpenDrainPullUp = 0x200;
    static constexpr uint16_t RespSupportedModes__OpenDrainPullDown = 0x400;
    static constexpr uint16_t RespSupportedModes__OpenCollector = 0x1000;
    static constexpr uint16_t RespSupportedModes__OpenCollectorPullUp = 0x2000;
    static constexpr uint16_t RespSupportedModes__OpenCollectorPullDown = 0x4000;

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
        uint16_t mode;
        if (ctx.getField(ParamModeSelect__M, &mode)) {
            switch (mode) {
            case ParamModeSelect__Input:
                PinManager<ZP>::setState(pin, false, false, false);
                pinMode(pin, INPUT);
                break;
            case ParamModeSelect__InputPullUp:
                PinManager<ZP>::setState(pin, false, true, false);
                pinMode(pin, INPUT_PULLUP);
                break;
            case ParamModeSelect__Output:
                PinManager<ZP>::setState(pin, false, false, false);
                pinMode(pin, OUTPUT);
                break;
            case ParamModeSelect__OpenDrain:
                PinManager<ZP>::setState(pin, true, false, false);
                pinMode(pin, INPUT);
                digitalWrite(pin, LOW);
                break;
            case ParamModeSelect__OpenDrainPullUp:
                PinManager<ZP>::setState(pin, true, true, false);
                pinMode(pin, INPUT_PULLUP);
                digitalWrite(pin, LOW);
                break;
            case ParamModeSelect__OpenCollector:
                PinManager<ZP>::setState(pin, true, false, true);
                pinMode(pin, INPUT);
                digitalWrite(pin, HIGH);
                break;
            case ParamModeSelect__OpenCollectorPullUp:
                PinManager<ZP>::setState(pin, true, true, true);
                pinMode(pin, INPUT_PULLUP);
                digitalWrite(pin, HIGH);
                break;
            default:
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                break;
            }
        }

        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespSupportedModes__M, RespSupportedModes__Input | RespSupportedModes__InputPullUp |
                RespSupportedModes__Output | RespSupportedModes__OpenDrain | RespSupportedModes__OpenDrainPullUp |
                RespSupportedModes__OpenCollector | RespSupportedModes__OpenCollectorPullUp);
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_DIGITALPINSETUPCOMMAND_HPP_ */