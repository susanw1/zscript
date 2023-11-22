/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_SERVO_MODULE_GENERALSERVOMODULE_HPP_
#define SRC_MAIN_C___ARDUINO_SERVO_MODULE_GENERALSERVOMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <Servo.h>
#include "../arduino-core-module/persistence/PersistenceSystem.hpp"

//#include <zscript/modules/ZscriptModule.hpp>
//#include <zscript/execution/ZscriptCommandContext.hpp>
//

//#define MODULE_EXISTS_008 EXISTENCE_MARKER_UTIL
//#define MODULE_SWITCH_008 MODULE_SWITCH_UTIL_WITH_ASYNC(ZscriptServoModule<ZP>::execute)

//#include "commands/ServoSetupCommand.hpp"
//#include "commands/ServoWriteCommand.hpp"
//#include "commands/ServoReadCommand.hpp"
//#include "commands/ServoCapabilitiesCommand.hpp"

namespace Zscript {

namespace servo_module {

template<class ZP>
class ZscriptGeneralServo {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    AsyncActionNotifier<ZP> async;
    uint16_t speed;
    uint16_t target :12;
#endif
    uint16_t pos: 12;
    uint16_t minPos: 12;
    uint16_t maxPos: 12;
    uint8_t index: 8;

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    uint8_t lastMillis16 :8;
#endif
    Servo s;

    void writePos() {
        uint16_t space = maxPos - minPos;
        uint8_t posr4 = pos >> 4;
        uint8_t spacer4 = space >> 4;
        //Does the bottom nibbles independently - to avoid 16bit x 16bit multiply.
        uint16_t diff = (posr4 * spacer4 + (((pos & 0xF) * spacer4 + (space & 0xF) * posr4) >> 4)) >> 4;
        s.writeMicroseconds(diff + minPos);
    }

public:
    ZscriptGeneralServo() :
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
            speed(0x1000), target(0x800),
#endif
            pos(0x800), minPos(1000), maxPos(1500), s(), index(0xFF)

#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
    , lastMillis16(0)
#endif
    {
    }

    void setup(uint8_t index, uint8_t persistStart) {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint8_t servoPersistLength = 5;
#else
        uint8_t servoPersistLength = 3;
#endif
        this->index = index;
        uint8_t persistedData[servoPersistLength];
        if (PersistenceSystem<ZP>::readSection(persistStart, servoPersistLength, persistedData)) {
            minPos = (persistedData[0] << 4) | (persistedData[1] >> 4);
            maxPos = ((persistedData[1] << 8) & 0xFFF) | (persistedData[2]);
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
            speed = (persistedData[3] << 8) | (persistedData[4]);
#endif
        }
    }

    void persist(uint8_t persistStart) {
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        uint8_t servoPersistLength = 5;
#else
        uint8_t servoPersistLength = 3;
#endif
        uint8_t persistedData[servoPersistLength];
        persistedData[0] = minPos >> 4;
        persistedData[1] = (minPos << 4) | (maxPos >> 8);
        persistedData[2] = (maxPos & 0xFF);
#ifdef ZSCRIPT_SERVO_MODULE_SLOW_MOVE
        persistedData[3] = speed>>8;
        persistedData[4] = speed & 0xFF;
#endif
        PersistenceSystem<ZP>::writeSection(persistStart, servoPersistLength, persistedData);
    }

    uint8_t getPin() {
        return ZP::servoPins[index];
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
        if (index == 0xFF) {
            return;
        }
        s.attach(ZP::servoPins[index], 544 - 127 * 4, 2400 + 127 * 4);
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

}

#endif /* SRC_MAIN_C___ARDUINO_SERVO_MODULE_GENERALSERVOMODULE_HPP_ */
