/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <Servo.h>

#define COMMAND_EXISTS_0080 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ServoCapabilitiesCommand {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char RespCommandsSet__C = 'C';
    static constexpr char RespServoPinCount__P = 'P';
    static constexpr char RespPulseRate__R = 'R';
    static constexpr char RespMinPulseTime__N = 'N';
    static constexpr char RespMaxPulseTime__M = 'M';
    static constexpr char RespCapabilityBitset__B = 'B';

    static constexpr uint16_t RespCapabilityBitset__AdjustableRange = 0x10;
    static constexpr uint16_t RespCapabilityBitset__AdjustableCenter = 0x08;
    static constexpr uint16_t RespCapabilityBitset__PersistableCalibration = 0x04;
    static constexpr uint16_t RespCapabilityBitset__SlowMove = 0x02;
    static constexpr uint16_t RespCapabilityBitset__PreciseSpeed = 0x01;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(008));
        out.writeField(RespServoPinCount__P, ZP::servoCount);
        out.writeField(RespPulseRate__R, 20);
        out.writeField(RespMinPulseTime__N, 544 - 128 * 4);
        out.writeField(RespMaxPulseTime__M, 2400 + 127 * 4);
        out.writeField(RespCapabilityBitset__B,
                RespCapabilityBitset__AdjustableRange

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
                | RespCapabilityBitset__SlowMove | RespCapabilityBitset__PreciseSpeed
#endif
                );
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_ */
