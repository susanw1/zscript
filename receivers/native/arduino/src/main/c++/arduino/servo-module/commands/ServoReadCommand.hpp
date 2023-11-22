/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/ServoModule.hpp>
#include "../GeneralServo.hpp"

#define COMMAND_EXISTS_0083 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace servo_module {

template<class ZP>
class ServoReadCommand: public Read_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos, bool moveAlong) {
        uint16_t interface;
        if (!ctx.getReqdFieldCheckLimit(ReqServoInterface__I, ZP::servoCount, &interface)) {
            return;
        }
        ZscriptGeneralServo<ZP> *target = servos + interface;
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        if (ctx.hasField(ReqWait__W) && target->isMoving()) {
            ctx.commandNotComplete();
            target->setAsyncCallback(ctx.getAsyncActionNotifier());
            return;
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespTarget__T, target->getTargetProp());
        out.writeField(RespCurrent__C, target->getCurrentProp());
        out.writeField(RespCurrentMicroseconds__U, target->getCurrentMicros());
        if (target->isMoving()) {
            out.writeFlagFieldSet(RespMoving__M);
        }
        if (target->isEnabled()) {
            out.writeFlagFieldSet(RespEnabled__E);
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_ */
