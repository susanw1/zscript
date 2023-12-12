/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../PinManager.hpp"

#define COMMAND_EXISTS_0044 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class AtoDPinSetupCommand: public AToDSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getReqdFieldCheckLimit(ReqPin__P, ZP::pinCount, &pin)) {
            return;
        }
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        uint8_t index = PinManager<ZP>::getAnalogIndex(pin);
        uint16_t enableNotifications;
        if (ctx.getField(ReqEnableNotifications__N, &enableNotifications)) {
            if (enableNotifications != 0) {
                PinManager<ZP>::enableAnalogNotification(index);
            } else {
                PinManager<ZP>::disableAnalogNotification(index);
            }
        }
        uint16_t notificationLower;
        if (ctx.getField(ReqLowerLimitNotify__L, &notificationLower)) {
            PinManager<ZP>::setAnalogNotificationLower(index, notificationLower);
        }
        uint16_t notificationUpper;
        if (ctx.getField(ReqUpperLimitNotify__U, &notificationUpper)) {
            PinManager<ZP>::setAnalogNotificationUpper(index, notificationUpper);
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        out.writeField(RespSupportedNotifications__N, 0);
#endif
        if (PIN_SUPPORTS_ANALOG_READ(pin)) {
            out.writeField(RespBitCount__B, 10);
        } else {
            out.writeField(RespBitCount__B, 1);
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_ */
