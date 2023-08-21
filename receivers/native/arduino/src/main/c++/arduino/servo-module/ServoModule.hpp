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

namespace Zscript {
template<class ZP>
class ZscriptGeneralServo {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    AsyncActionNotifier<ZP> async;
    uint16_t speed;
    uint16_t target :12;
    #endif
    uint16_t pos :12;
    uint16_t minPos :12;
    uint16_t maxPos :12;
    uint8_t pin :8;

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    uint8_t lastMillis16 :8;
    #endif
    Servo s;

    void writePos() {
        uint16_t space = maxPos - minPos;
        uint8_t posr4 = pos >> 4;
        uint8_t spacer4 = space >> 4;
        //Does the bottom nibbles independently - to avoid 16bitx16bit multiply.
        uint16_t diff = (posr4 * spacer4 + (((pos & 0xF) * spacer4 + (space & 0xF) * posr4) >> 4)) >> 4;
        s.writeMicroseconds(diff + minPos);
    }

public:
    ZscriptGeneralServo() :
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
                    speed(0x1000), target(0x800),
                    #endif
                    pos(0x800), minPos(1000), maxPos(1500), s(), pin(0xFF)

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
                            , lastMillis16(0)
    #endif
    {
    }
    void setup(uint8_t pin) {
        this->pin = pin;
    }
    uint8_t getPin() {
        return pin;
    }

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    void updatePos(uint8_t timeMillis16) {
        if (target == pos) {
            lastMillis16 = timeMillis16;
            return;
        }
        if (timeMillis16 - lastMillis16 == 0) {
            return;
        }
        // find the number of units of 16, 64, and 2048 ms have passed.
        // note that the `>>` must be done before the `-` so as to trigger every nth time.
        // the '&' is necessary to cope with wrap around of the underlying uint8_t.
        uint8_t unitsPassed = timeMillis16 - lastMillis16;
        uint8_t unitsPassed64 = ((timeMillis16 >> 2) - (lastMillis16 >> 2)) & 0x3F;
        uint8_t unitsPassed2048 = ((timeMillis16 >> 6) - (lastMillis16 >> 6)) & 0x3;

        // 8 bit * 8 bit = 16 bit: the part of the speed that could overflow... This is applied more rapidly for smooth motion.
        uint16_t offset = (speed >> 8) * unitsPassed;

        // is ~10 bit, cannot overflow. Applies smaller factors to motion more rarely to avoid losing precision.
        uint16_t lowerOffset = ((speed >> 4) & 0xF) * unitsPassed64 + (speed & 0xF) * (unitsPassed2048);

        // Checks if the motion will overflow - lowerOffset is on RHS to avoid overflows.
        // If this comes as true, then the motion is >= full sweep range, so must reach destination.
        if (offset >= (uint16_t) 0x4000 - (lowerOffset >> 2)) {
            pos = target;
        } else {
            // This reassembles the offset now we know it can't overflow.
            offset <<= 2;
            offset += lowerOffset;
            if (pos > target) {
                if (pos - target < offset) {
                    pos = target;
                } else {
                    pos -= offset;
                }
            } else {
                if (target - pos < offset) {
                    pos = target;
                } else {
                    pos += offset;
                }
            }
        }
        if (pos == target) {
            async.notifyNeedsAction();
            async = AsyncActionNotifier<ZP>();
        }
        lastMillis16 = timeMillis16;
        writePos();
    }
    void setAsyncCallback(AsyncActionNotifier<ZP> callback) {
        async = callback;
    }
#endif

    uint16_t getCurrentProp() {
        return pos << 4;
    }
    uint16_t getTargetProp() {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        return target << 4;
#else
        return pos << 4;
#endif
    }
    void fastMove(uint16_t newTarget) {
        pos = newTarget >> 4;
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        target = newTarget >> 4;
#endif
        writePos();
    }
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    void slowMove(uint16_t newTarget) {
        target = newTarget >> 4;
        lastMillis16 = (millis() / 16) & 0xFF;
        writePos();
    }
    void setTravelSpeed(uint16_t speed) {
        this->speed = speed;
    }
    uint16_t getCurrentSpeed() {
        return speed;
    }
#endif

    void setMinMicros(uint16_t min) {
        minPos = min & 0xFFF;
    }
    void setMaxMicros(uint16_t max) {
        maxPos = max & 0xFFF;
    }

    uint16_t getMinMicros() {
        return minPos;
    }
    uint16_t getMaxMicros() {
        return maxPos;
    }
    uint16_t getCentreMicros() {
        return (maxPos + minPos) / 2;
    }
    uint16_t getCurrentMicros() {
        return s.readMicroseconds();
    }

    void enable() {
        if (pin == 0xFF) {
            return;
        }
        s.attach(pin, 544 - 127 * 4, 2400 + 127 * 4);
        writePos();
    }
    void disable() {
        if (!s.attached()) {
            return;
        }
        s.detach();
    }
    bool isEnabled() {
        return s.attached();
    }

    bool isMoving() {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        return pos != target;
#else
        return false;
#endif
    }

    void resetCalibration() {
        minPos = 1000;
        maxPos = 1500;
    }

};
}

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>

#define MODULE_EXISTS_008 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_008 MODULE_SWITCH_UTIL_WITH_ASYNC(ZscriptServoModule<ZP>::execute)

#include "commands/ServoSetupCommand.hpp"
#include "commands/ServoWriteCommand.hpp"
#include "commands/ServoReadCommand.hpp"
#include "commands/ServoCapabilitiesCommand.hpp"

namespace Zscript {
template<class ZP>
class ZscriptServoModule: public ZscriptModule<ZP> {
    static ZscriptGeneralServo<ZP> servos[ZP::servoCount];

public:

    static void setup() {
        for (uint8_t i = 0; i < ZP::servoCount; ++i) {
            servos[i].setup(ZP::servoPins[i]);
        }
        moveAlongServos();
    }

    // Should be called regularly (if slow move is enabled).
    // Will forget motion that happened during any space when it wasn't called for >=4s.
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
            ServoSetupCommand<ZP>::execute(ctx, servos);
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
}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_SERVOMODULE_HPP_ */
