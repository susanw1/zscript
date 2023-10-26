/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../ServoModule.hpp"

#define COMMAND_EXISTS_0082 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ServoWriteCommand {
public:
    static constexpr uint8_t CODE = 0x02;

    static constexpr char ParamServoInterface__I = 'I';
    static constexpr char ParamValue__V = 'V';
    static constexpr char ParamFast__F = 'F';
    static constexpr char ParamEnable__E = 'E';

    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos) {
        uint16_t interface;
        if (!ctx.getField(ParamServoInterface__I, &interface)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (interface >= ZP::servoCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        uint16_t value;
        if (!ctx.getField(ParamValue__V, &value)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        ZscriptGeneralServo<ZP> *target = servos + interface;

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        if (ctx.hasField('F')) {
#endif
        target->fastMove(value);
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        } else {
            target->slowMove(value);
        }
#endif
        uint16_t enable;
        if (ctx.getField(ParamEnable__E, &enable)) {
            if (enable) {
                target->enable();
            } else {
                target->disable();
            }
        }
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_ */
