/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ZSCRIPT_ANALOGNOTIFICATION_HPP
#define ZSCRIPT_ANALOGNOTIFICATION_HPP

#include <zscript/modules/ZscriptCommand.hpp>
#include <zscript/execution/ZscriptNotificationContext.hpp>
#include "../PinManager.hpp"

namespace Zscript {
template<class ZP>
class AnalogNotification {
    static constexpr char PinNumber__P = 'P';
    static constexpr char NotificationType__T = 'T';
    static constexpr char Value__V = 'V';

public:
    static void notification(ZscriptNotificationContext<ZP> ctx) {
        NotificationOutStream<ZP> out = ctx.getOutStream();
        for (uint8_t index = 0; index < ANALOG_PIN_COUNT_FOUND; index++) {
            if (PinManager<ZP>::hasAnalogNotification(index)) {
                out.writeField(PinNumber__P, PinManager<ZP>::getAnalogPin(index));
                out.writeField(NotificationType__T, PinManager<ZP>::wasAnalogTooHigh(index) ? 0 : 1);
                out.writeField(Value__V, PinManager<ZP>::getLastAnalog(index));
                PinManager<ZP>::clearAnalogNotification(index);
                return;
            }
        }
        out.writeField('S', ResponseStatus::INTERNAL_ERROR);
    }

    static bool checkForWaitingNotification() {
        bool hasFound = false;
        for (uint8_t index = 0; index < ANALOG_PIN_COUNT_FOUND; index++) {
            if (PinManager<ZP>::hasAnalogNotification(index)) {
                if (hasFound) {
                    return true;
                } else {
                    hasFound = true;
                }
            }
        }
        return false;
    }
};
}

#endif //ZSCRIPT_ANALOGNOTIFICATION_HPP
