/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../ServoModule.hpp"

#define COMMAND_EXISTS_0081 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ServoSetupCommand {
public:
    static constexpr uint8_t CODE = 0x01;

    static constexpr char ParamServoInterface__I = 'I';
    static constexpr char ParamMinPulseTime__N = 'N';
    static constexpr char ParamMaxPulseTime__M = 'M';
    static constexpr char ParamTravelSpeed__V = 'V';
    static constexpr char ParamTravelSpeedPrecise__W = 'W';
    static constexpr char ParamResetCalibrations__R = 'R';
    static constexpr char ParamPersist__P = 'P';

    static constexpr char RespPin__P = 'P';
    static constexpr char RespMinPulseTime__N = 'N';
    static constexpr char RespMaxPulseTime__M = 'M';
    static constexpr char RespCentrePulseTime__C = 'C';
    static constexpr char RespTravelSpeedPrecise__W = 'W';

    static void execute(ZscriptCommandContext<ZP> ctx, ZscriptGeneralServo<ZP> *servos, uint8_t persistenceStart) {
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
        if (ctx.hasField(ParamResetCalibrations__R)) {
            target->resetCalibration();
        }
        uint16_t minTime;
        if (ctx.getField(ParamMinPulseTime__N, &minTime)) {
            target->setMinMicros(minTime);
        }
        uint16_t maxTime;
        if (ctx.getField(ParamMaxPulseTime__M, &maxTime)) {
            target->setMaxMicros(maxTime);
        }
        if (ctx.hasField(ParamPersist__P)) {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
            uint8_t servoPersistLength = 5;
#else
            uint8_t servoPersistLength = 3;
#endif
            target->persist(persistenceStart + interface * (servoPersistLength + 2));
        }
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint16_t speed;
        if (ctx.getField(ParamTravelSpeedPrecise__W, &speed)) {
            target->setTravelSpeed(speed);
        } else if (ctx.getField(ParamTravelSpeed__V, &speed)) {
            target->setTravelSpeed(speed * 48);
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespPin__P, target->getPin());
        out.writeField(RespMinPulseTime__N, target->getMinMicros());
        out.writeField(RespMaxPulseTime__M, target->getMaxMicros());
        out.writeField(RespCentrePulseTime__C, target->getCentreMicros());
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        out.writeField(RespTravelSpeedPrecise__W, target->getCurrentSpeed());
#endif

    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOSETUPCOMMAND_HPP_ */
