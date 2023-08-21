/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#include <zscript/execution/ZscriptAddressingContext.hpp>
#endif
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
#include <zscript/notifications/ZscriptNotificationSource.hpp>
#endif

#include "commands/I2cSetupCommand.hpp"
#include "commands/GeneralI2cAction.hpp"
#include "commands/I2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(I2cModule<ZP>::execute)

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#define MODULE_ADDRESS_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_ADDRESS_SWITCH_005 ADDRESS_SWITCH_UTIL(I2cModule<ZP>::address)
#endif
#endif

namespace Zscript {
template<class ZP>
class I2cModule: public ZscriptModule<ZP> {
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
    static bool isAddressing;
    static bool giveNotifs;
#endif

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
    static void SmBusAlertRecieved() {
        if(giveNotifs){
            notifSrc.set(NULL, 0x50, isAddressing);
        }
    }
#endif

public:

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
    static GenericCore::ZscriptNotificationSource<ZP> notifSrc;
#endif

    static void setup() {
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        pinMode(ZP::i2cAlertPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(ZP::i2cAlertPin), &SmBusAlertRecieved, FALLING);
#endif
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZscriptI2cCapabilitiesCommand<ZP>::CODE:
            ZscriptI2cCapabilitiesCommand<ZP>::execute(ctx);
            break;
        case ZscriptI2cSetupCommand<ZP>::CODE:
            ZscriptI2cSetupCommand<ZP>::execute(ctx

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
                    , &isAddressing, &giveNotifs
#endif
                    );
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

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    static void address(ZscriptAddressingContext<ZP> ctx) {
        if (!isAddressing) {
            ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        GeneralI2cAction<ZP>::executeAddressing(ctx);
    }
#endif
#endif
};
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
template<class ZP>
bool I2cModule<ZP>::isAddressing = false;
template<class ZP>
bool I2cModule<ZP>::giveNotifs = false;
#endif
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_ */
