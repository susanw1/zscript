/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/ServoModule.hpp>
#include <Servo.h>
#include "../GeneralServo.hpp"

#define COMMAND_EXISTS_0080 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace servo_module {

template<class ZP>
class ServoCapabilitiesCommand: public Capabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, uint16_t commandsSet) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, commandsSet);
        out.writeField(RespServoInterfaceCount__I, ZP::servoCount);
        out.writeField(RespPulseRate__R, 20);
        out.writeField(RespMinimumPulseTime__N, 544 - 127 * 4);
        out.writeField(RespMaximumPulseTime__M, 2400 + 127 * 4);
        out.writeField(RespCapabilityBitset__B, RespCapabilityBitset_Values::adjustableRange_field
                | RespCapabilityBitset_Values::persistableCalibration_field
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
                | RespCapabilityBitset_Values::slowMove_field
                | RespCapabilityBitset_Values::preciseSpeed_field
#endif
        );
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_COMMANDS_SERVOCAPABILITIESCOMMAND_HPP_ */
