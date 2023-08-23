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
#include <zscript/execution/ZscriptNotificationContext.hpp>
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
#define MODULE_NOTIFICATION_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_NOTIFICATION_SWITCH_005 NOTIFICATION_SWITCH_UTIL(I2cModule<ZP>::notification)
#endif

namespace Zscript {
template<class ZP>
class I2cModule: public ZscriptModule<ZP> {
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
    static const uint8_t I2C_ADDRESSING_READ_BLOCK_LENGTH = 8;
    static const uint8_t SMBUS_ALERT_ADDR = 0xC;
    static bool isAddressing;
    static bool giveNotifs;
    #endif

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
    static void SmBusAlertRecieved() {
        if (giveNotifs) {
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
        pinMode(ZP::i2cAlertInPin, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(ZP::i2cAlertInPin), &SmBusAlertRecieved, FALLING);
        pinMode(ZP::i2cAlertInPin, INPUT_PULLUP);
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
    static void notification(ZscriptNotificationContext<ZP> ctx, bool moveAlong) {
        (void) moveAlong;
        NotificationOutStream<ZP> out = ctx.getOutStream();
        uint8_t len = Wire.requestFrom(SMBUS_ALERT_ADDR, (uint8_t)2);
        if (len != 2 && len != 1) {
            if (isAddressing) {
                out.writeField(Zchars::Z_ADDRESSING, 0x5);
                out.writeField(Zchars::Z_ADDRESSING_CONTINUE, 0x0);
                out.writeField(Zchars::Z_STATUS, ResponseStatus::ADDRESS_NOT_FOUND);
            } else {
                out.writeField('I', 0x0);
            }
            return;
        }
        uint8_t addr1 = (uint8_t) Wire.read();
        uint16_t addrFull;
        bool tenBit;
        if ((addr1 & 0x7C) == 0x78) {
            addrFull = (addr1 & 0x3) << 8 | (uint8_t) Wire.read();
            tenBit = true;
        } else {
            addrFull = addr1>>1;
            tenBit = false;
        }
        if (!isAddressing) {
            out.writeField('I', 0x0);
            out.writeField('A', addrFull);
            if (tenBit) {
                out.writeField('T', 0);
            }
            return;
        }
        out.writeField(Zchars::Z_ADDRESSING, 0x5);
        out.writeField(Zchars::Z_ADDRESSING_CONTINUE, 0x0);
        if (tenBit) {
            out.writeField('A', addrFull | 0x8000);
            out.writeField(Zchars::Z_STATUS, ResponseStatus::ADDRESS_NOT_FOUND);
            return;
        }
        out.writeField(Zchars::Z_ADDRESSING_CONTINUE, addrFull);
        uint8_t dataBuffer[I2C_ADDRESSING_READ_BLOCK_LENGTH];
        delayMicroseconds(10);
        while (true) {
            uint8_t len = Wire.requestFrom((uint8_t)addrFull, (uint8_t)I2C_ADDRESSING_READ_BLOCK_LENGTH);
            if (len != I2C_ADDRESSING_READ_BLOCK_LENGTH) {
                out.endSequence();
                out.writeField('!', 0x5);
                out.writeField('S', ResponseStatus::ADDRESS_NOT_FOUND);
                return;
            }
            uint8_t c = 0;
            uint8_t i = 0;
            while (i < len && (c = Wire.read()) != '\n') {
                dataBuffer[i++] = c;
            }
            out.writeBytes(dataBuffer, i);
            if (c == '\n') {
                break;
            }
        }
    }
#endif
};
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
template<class ZP>
bool I2cModule<ZP>::isAddressing = false;
template<class ZP>
bool I2cModule<ZP>::giveNotifs = false;
template<class ZP>
GenericCore::ZscriptNotificationSource<ZP> I2cModule<ZP>::notifSrc;
#endif
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_ */
