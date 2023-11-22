/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../ServoModule.hpp"

#define COMMAND_EXISTS_0083 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace servo_module {

template<class ZP>
class ServoReadCommand {
public:
    static constexpr uint8_t CODE = 0x03;

    static constexpr char ParamServoInterface__I = 'I';
    static constexpr char ParamWait__W = 'W';

    static constexpr char RespCurrentTarget__T = 'T';
    static constexpr char RespCurrentPosition__C = 'C';
    static constexpr char RespCurrentMicroseconds__U = 'U';
    static constexpr char RespMoving__M = 'M';
    static constexpr char RespEnabled__E = 'E';

    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos, bool moveAlong) {
        uint16_t interface;
        if (!ctx.getField(ParamServoInterface__I, &interface)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (interface >= ZP::servoCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        ZscriptGeneralServo<ZP> *target = servos + interface;
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        if (ctx.hasField(ParamWait__W) && target->isMoving()) {
            ctx.commandNotComplete();
            target->setAsyncCallback(ctx.getAsyncActionNotifier());
            return;
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCurrentTarget__T, target->getTargetProp());
        out.writeField(RespCurrentPosition__C, target->getCurrentProp());
        out.writeField(RespCurrentMicroseconds__U, target->getCurrentMicros());
        if (target->isMoving()) {
            out.writeField(RespMoving__M, 0);
        }
        if (target->isEnabled()) {
            out.writeField(RespEnabled__E, 0);
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOREADCOMMAND_HPP_ */
