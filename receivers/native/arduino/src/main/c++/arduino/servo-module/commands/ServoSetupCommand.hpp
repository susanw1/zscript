/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/ServoModule.hpp>
#include "../GeneralServo.hpp"

#define COMMAND_EXISTS_0081 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace servo_module {

template<class ZP>
class ServoSetupCommand: public Setup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos, uint8_t persistenceStart) {
        uint16_t interface;
        if (!ctx.getReqdFieldCheckLimit(ReqServoInterface__I, ZP::servoCount, &interface)) {
            return;
        }
        ZscriptGeneralServo<ZP> *target = servos + interface;
        if (ctx.hasField(ReqResetCalibrations__R)) {
            target->resetCalibration();
        }
        uint16_t minTime;
        if (ctx.getField(ReqMinimumPulseTime__N, &minTime)) {
            target->setMinMicros(minTime);
        }
        uint16_t maxTime;
        if (ctx.getField(ReqMaximumPulseTime__M, &maxTime)) {
            target->setMaxMicros(maxTime);
        }
        if (ctx.hasField(ReqPersistCalibrations__P)) {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
            uint8_t servoPersistLength = 5;
#else
            uint8_t servoPersistLength = 3;
#endif
            target->persist(persistenceStart + interface * (servoPersistLength + 2));
        }
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint16_t speed;
        if (ctx.getField(ReqPinChangeSpeedPrecise__W, &speed)) {
            target->setTravelSpeed(speed);
        } else if (ctx.getField(ReqPinChangeSpeed__V, &speed)) {
            target->setTravelSpeed(speed * 48);
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespPin__P, target->getPin());
        out.writeField(RespMinimumPulseTime__N, target->getMinMicros());
        out.writeField(RespMaximumPulseTime__M, target->getMaxMicros());
        out.writeField(RespCenterPulseTime__C, target->getCenterMicros());
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        out.writeField(RespPinChangeSpeedPrecise__W, target->getCurrentSpeed());
#endif

    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_ */
