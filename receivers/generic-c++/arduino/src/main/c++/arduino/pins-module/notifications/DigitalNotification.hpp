/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ZSCRIPT_DIGITALNOTIFICATION_HPP
#define ZSCRIPT_DIGITALNOTIFICATION_HPP

#include <zscript/modules/ZscriptCommand.hpp>
#include <zscript/execution/ZscriptNotificationContext.hpp>
#include "../PinManager.hpp"

namespace Zscript {

namespace pins_module {

template<class ZP>
class DigitalNotification {
    static constexpr char PinNumber__P = 'P';
    static constexpr char NotificationType__T = 'T';
    static constexpr char Value__V = 'V';

public:
    static void notification(ZscriptNotificationContext<ZP> ctx) {
        NotificationOutStream<ZP> out = ctx.getOutStream();
        for (uint8_t pin = 0; pin < ZP::pinCount; pin++) {
            if (PinManager<ZP>::hasNotification(pin)) {
                out.writeField(PinNumber__P, pin);
                out.writeField(NotificationType__T, (uint8_t) PinManager<ZP>::getNotificationType(pin));
                out.writeField(Value__V, digitalRead(pin) == HIGH ? 1 : 0);
                PinManager<ZP>::clearNotification(pin);
                return;
            }
        }
        out.writeField('S', ResponseStatus::INTERNAL_ERROR);
    }

    static bool checkForWaitingNotification() {
        bool hasFound = false;
        for (uint8_t pin = 0; pin < ZP::pinCount; pin++) {
            if (PinManager<ZP>::hasNotification(pin)) {
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

}

#endif //ZSCRIPT_DIGITALNOTIFICATION_HPP
