/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SRC_ZSCRIPT_BASE_I2C_MODULE)
    #error This file should not be included more than once
#endif
#define SRC_ZSCRIPT_BASE_I2C_MODULE

#ifdef ZSCRIPT_HPP_INCLUDED
    #error Must be included before Zscript.hpp
#endif

#include <Wire.h>

#include <zscript/modules/ZscriptModule.hpp>
#include <net/zscript/model/modules/base/I2cModule.hpp>

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(i2c_module::I2cModule<ZP>::execute)

// Notifications
#include <arduino/i2c-module/notifications/I2cNotification.hpp>

#define MODULE_NOTIFICATION_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_NOTIFICATION_SWITCH_005 NOTIFICATION_SWITCH_UTIL(i2c_module::I2cModule<ZP>::notification)

// Addressing
#include <zscript/execution/ZscriptAddressingContext.hpp>

#define MODULE_ADDRESS_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_ADDRESS_SWITCH_005 ADDRESS_SWITCH_UTIL(i2c_module::I2cModule<ZP>::address)

// All
#include <arduino/i2c-module/commands/GeneralI2cAction.hpp>
#include <arduino/i2c-module/commands/I2cSetupCommand.hpp>
#include <arduino/i2c-module/commands/I2cCapabilitiesCommand.hpp>

namespace Zscript {

namespace i2c_module {

template<class ZP>
class I2cModule : public ZscriptModule<ZP> {
    static bool isAddressing;
    static bool giveNotifs;

    static void smBusAlertReceived() {
        if (giveNotifs) {
            notifSrc.set(NULL, 0x50, isAddressing);
        }
    }

public:
    static GenericCore::ZscriptNotificationSource<ZP> notifSrc;

    static void setup() {
        pinMode(ZP::i2cAlertInPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(ZP::i2cAlertInPin), &smBusAlertReceived, FALLING);
        pinMode(ZP::i2cAlertInPin, INPUT_PULLUP);

        Wire.begin();
    }

    static void poll() {
        if (digitalRead(ZP::i2cAlertInPin) == LOW) {
            smBusAlertReceived();
        }
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
            case ZscriptI2cCapabilitiesCommand<ZP>::CODE:
                ZscriptI2cCapabilitiesCommand<ZP>::execute(ctx, MODULE_CAPABILITIES(005));
                break;
                case ZscriptI2cSetupCommand<ZP>::CODE:
                ZscriptI2cSetupCommand<ZP>::execute(ctx, &isAddressing, &giveNotifs);
                break;
            case GeneralI2cAction<ZP>::SEND_CODE:
                GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::SEND);
                break;
            case GeneralI2cAction<ZP>::RECEIVE_CODE:
                GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::RECEIVE);
                break;
            case GeneralI2cAction<ZP>::SEND_RECEIVE_CODE:
                GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::SEND_RECEIVE);
                break;
            default:
                ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
                break;
        }
    }

    static void address(ZscriptAddressingContext<ZP> ctx) {
        if (!isAddressing) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        GeneralI2cAction<ZP>::executeAddressing(ctx);
    }

    static void notification(ZscriptNotificationContext<ZP> ctx, bool moveAlong) {
        ZscriptI2cNotification<ZP>::notification(ctx, moveAlong, isAddressing);
    }
};

template<class ZP>
bool I2cModule<ZP>::isAddressing = false;

template<class ZP>
bool I2cModule<ZP>::giveNotifs = false;

template<class ZP>
GenericCore::ZscriptNotificationSource<ZP> I2cModule<ZP>::notifSrc;

}

}

