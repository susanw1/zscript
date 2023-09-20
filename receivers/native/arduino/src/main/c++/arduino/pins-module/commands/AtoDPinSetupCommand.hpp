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

#define COMMAND_EXISTS_0034 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class AtoDPinSetupCommand {
    static constexpr char ParamPin__P = 'P';
    static constexpr char ParamEnableNotifications__N = 'N';
    static constexpr char ParamLowerLimit__L = 'L';
    static constexpr char ParamUpperLimit__U = 'U';

    static constexpr char RespBitCount__B = 'B';
    static constexpr char RespSupportsNotifications__N = 'N';

public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t pin;
        if (!ctx.getField(ParamPin__P, &pin)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (pin >= ZP::pinCount) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        uint8_t index = PinManager<ZP>::getAnalogIndex(pin);
        uint16_t enableNotifications;
        if (ctx.getField(ParamEnableNotifications__N, &enableNotifications)) {
            if (enableNotifications != 0) {
                PinManager<ZP>::enableAnalogNotification(index);
            } else {
                PinManager<ZP>::disableAnalogNotification(index);
            }
        }
        uint16_t notificationLower;
        if (ctx.getField(ParamLowerLimit__L, &notificationLower)) {
            PinManager<ZP>::setAnalogNotificationLower(index, notificationLower);
        }
        uint16_t notificationUpper;
        if (ctx.getField(ParamUpperLimit__U, &notificationUpper)) {
            PinManager<ZP>::setAnalogNotificationUpper(index, notificationUpper);
        }
#endif
        CommandOutStream<ZP> out = ctx.getOutStream();
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        out.writeField(RespSupportsNotifications__N, 0);
#endif
        if (PIN_SUPPORTS_ANALOG_READ(pin)) {
            out.writeField(RespBitCount__B, 10);
        } else {
            out.writeField(RespBitCount__B, 1);
        }
    }

};
}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_ATODSETUPCOMMAND_HPP_ */
