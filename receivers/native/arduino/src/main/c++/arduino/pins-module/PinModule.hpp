/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif
#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

#endif

#include <zscript/modules/ZscriptCommand.hpp>
#include <zscript/modules/ZscriptModule.hpp>

#include "PinManager.hpp"

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

#include <zscript/execution/ZscriptNotificationContext.hpp>
#include <zscript/notifications/ZscriptNotificationSource.hpp>

#include "notifications/DigitalNotification.hpp"

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS

#include "notifications/AnalogNotification.hpp"

#endif
#endif

#include "commands/PwmPinSetupCommand.hpp"
#include "commands/PwmPinWriteCommand.hpp"

#ifdef DEVICE_SUPPORTS_ANALOG_WRITE
#include "commands/DtoAPinSetupCommand.hpp"
#include "commands/DtoAPinWriteCommand.hpp"
#endif

#include "commands/AtoDPinSetupCommand.hpp"
#include "commands/AtoDPinReadCommand.hpp"
#include "commands/DigitalPinSetupCommand.hpp"
#include "commands/DigitalPinWriteCommand.hpp"
#include "commands/DigitalPinReadCommand.hpp"
#include "commands/PinCapabilitiesCommand.hpp"

#define MODULE_EXISTS_003 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_003 MODULE_SWITCH_UTIL(PinModule<ZP>::execute)

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
#define MODULE_NOTIFICATION_EXISTS_003 EXISTENCE_MARKER_UTIL
#define MODULE_NOTIFICATION_SWITCH_003 NOTIFICATION_SWITCH_UTIL(PinModule<ZP>::notification)
#endif
namespace Zscript {
template<class ZP>
class PinModule {
#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
public:
    static GenericCore::ZscriptNotificationSource<ZP> notificationSource;

private:
    static volatile bool hasDigitalNotificationWaiting;

    static void onNotificationNeededDigital() {
        if (!notificationSource.setNotification(&GenericCore::LockSet<ZP>::Empty, 0x30)) {
            hasDigitalNotificationWaiting = true;
        }
    }

#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
    static volatile bool hasAnalogNotificationWaiting;

    static void onNotificationNeededAnalog() {
        if (!notificationSource.setNotification(&GenericCore::LockSet<ZP>::Empty, 0x31)) {
            hasAnalogNotificationWaiting = true;
        }
    }

#endif

#endif

public:
    static void setup() {
#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
        PinManager<ZP>::setOnNotificationDigital(&onNotificationNeededDigital);
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        PinManager<ZP>::setOnNotificationAnalog(&onNotificationNeededAnalog);
#endif
#endif
    }

    static void poll() {
#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
        PinManager<ZP>::pollPinStates();
        if (!notificationSource.hasNotification()) {
            if (hasDigitalNotificationWaiting) {
                hasDigitalNotificationWaiting = false;
                notificationSource.setNotification(&GenericCore::LockSet<ZP>::Empty, 0x30);
                hasDigitalNotificationWaiting |= DigitalNotification<ZP>::checkForWaitingNotification();
            }
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
            else if (hasAnalogNotificationWaiting) {
                hasAnalogNotificationWaiting = false;
                notificationSource.setNotification(&GenericCore::LockSet<ZP>::Empty, 0x31);
                hasAnalogNotificationWaiting |= AnalogNotification<ZP>::checkForWaitingNotification();
            }
#endif
        }
#endif
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
            case 0x0:
                PinCapabilitiesCommand<ZP>::execute(ctx);
                break;
            case 0x1:
                DigitalPinSetupCommand<ZP>::execute(ctx);
                break;
            case 0x2:
                DigitalPinWriteCommand<ZP>::execute(ctx);
                break;
            case 0x3:
                DigitalPinReadCommand<ZP>::execute(ctx);
                break;
            case 0x4:
                AtoDPinSetupCommand<ZP>::execute(ctx);
                break;
            case 0x5:
                AtoDPinReadCommand<ZP>::execute(ctx);
                break;
#ifdef DEVICE_SUPPORTS_ANALOG_WRITE
                case 0x6:
                    DtoAPinSetupCommand<ZP>::execute(ctx);
                    break;
                case 0x7:
                    DtoAPinWriteCommand<ZP>::execute(ctx);
                    break;
#endif
            case 0x8:
                PwmPinSetupCommand<ZP>::execute(ctx);
                break;
            case 0x9:
                PwmPinWriteCommand<ZP>::execute(ctx);
                break;
            default:
                ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
                break;
        }
    }

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

    static void notification(ZscriptNotificationContext<ZP> ctx, bool moveAlong) {
        if ((ctx.getID() & 0xF) == 0) {
            DigitalNotification<ZP>::notification(ctx);
        }
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
        else if ((ctx.getID() & 0xF) == 1) {
            AnalogNotification<ZP>::notification(ctx);
        }
#endif
        else {
            ctx.getOutStream().writeField('S', ResponseStatus::COMMAND_ERROR_CONTROL);
        }
    }

#endif
};

#ifdef ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS

template<class ZP>
GenericCore::ZscriptNotificationSource<ZP> PinModule<ZP>::notificationSource;
template<class ZP>
volatile bool PinModule<ZP>::hasDigitalNotificationWaiting = false;
#ifdef ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
template<class ZP>
volatile bool PinModule<ZP>::hasAnalogNotificationWaiting = false;
#endif
#endif
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_ */
