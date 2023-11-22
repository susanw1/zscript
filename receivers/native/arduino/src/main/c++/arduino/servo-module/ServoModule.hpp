/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_SERVOMODULE_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_SERVOMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <Servo.h>
#include "../arduino-core-module/persistence/PersistenceSystem.hpp"

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>


#define MODULE_EXISTS_008 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_008 MODULE_SWITCH_UTIL_WITH_ASYNC(servo_module::ZscriptServoModule<ZP>::execute)

#include "GeneralServo.hpp"
#include "commands/ServoSetupCommand.hpp"
#include "commands/ServoWriteCommand.hpp"
#include "commands/ServoReadCommand.hpp"
#include "commands/ServoCapabilitiesCommand.hpp"

namespace Zscript {

namespace servo_module {

template<class ZP>
class ZscriptServoModule : public ZscriptModule<ZP> {
    static ZscriptGeneralServo<ZP> servos[ZP::servoCount];
    static uint8_t persistenceStart;

public:
    static void setup() {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint8_t servoPersistLength = 5;
#else
        uint8_t servoPersistLength = 3;
#endif
        persistenceStart = PersistenceSystem<ZP>::reserveSection(servoPersistLength);
        for (uint8_t i = 1; i < ZP::servoCount; ++i) {
            PersistenceSystem<ZP>::reserveSection(servoPersistLength);
        }
        for (uint8_t i = 0; i < ZP::servoCount; ++i) {
            servos[i].setup(ZP::servoPins[i], persistenceStart + i * (servoPersistLength + 2));
        }
        moveAlongServos();
    }

    // Should be called regularly (if slow move is enabled).
    // Will forget motion that happened during any period when it wasn't called for >=4s.
    static void moveAlongServos() {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint8_t timeMillis16 = (millis() / 16) & 0xFF;
        for (uint8_t i = 0; i < ZP::servoCount; ++i) {
            servos[i].updatePos(timeMillis16);
        }
#endif
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits, bool moveAlong) {
        switch (bottomBits) {
            case ServoCapabilitiesCommand<ZP>::CODE:
                ServoCapabilitiesCommand<ZP>::execute(ctx);
                break;
            case ServoSetupCommand<ZP>::CODE:
                ServoSetupCommand<ZP>::execute(ctx, servos, persistenceStart);
                break;
            case ServoWriteCommand<ZP>::CODE:
                ServoWriteCommand<ZP>::execute(ctx, servos);
                break;
            case ServoReadCommand<ZP>::CODE:
                ServoReadCommand<ZP>::execute(ctx, servos, moveAlong);
                break;
            default:
                ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
                break;
        }
    }
};

template<class ZP>
ZscriptGeneralServo<ZP> ZscriptServoModule<ZP>::servos[ZP::servoCount];

template<class ZP>
uint8_t ZscriptServoModule<ZP>::persistenceStart = 0xFF;

}

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_SERVOMODULE_HPP_ */
