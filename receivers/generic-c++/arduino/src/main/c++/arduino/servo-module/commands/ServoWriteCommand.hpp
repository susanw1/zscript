/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/ServoModule.hpp>
#include "../GeneralServo.hpp"

#define COMMAND_EXISTS_0082 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace servo_module {

template<class ZP>
class ServoWriteCommand: public Write_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos) {
        uint16_t interface;
        if (!ctx.getReqdFieldCheckLimit(ReqServoInterface__I, ZP::servoCount, &interface)) {
            return;
        }
        uint16_t targetPosition;
        if (!ctx.getField(ReqTarget__T, &targetPosition)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        ZscriptGeneralServo<ZP> *target = servos + interface;

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        if (ctx.hasField(ReqFast__F)) {
#endif
            target->fastMove(targetPosition);
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        } else {
            target->slowMove(targetPosition);
        }
#endif
        uint16_t enable;
        if (ctx.getField(ReqEnable__E, &enable)) {
            if (enable) {
                target->enable();
            } else {
                target->disable();
            }
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOWRITECOMMAND_HPP_ */
