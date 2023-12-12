/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_NOTIFICATIONS_ZSCRIPTI2CNOTIFICATION_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_NOTIFICATIONS_ZSCRIPTI2CNOTIFICATION_HPP_

#if !defined(ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS)
    #error ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS should be enabled to ensure all notification components are included
#endif

#if !defined(ZSCRIPT_SUPPORT_NOTIFICATIONS)
    #error ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS requires ZSCRIPT_SUPPORT_NOTIFICATIONS to be enabled
#endif

#include <zscript/modules/ZscriptCommand.hpp>
#include <zscript/execution/ZscriptNotificationContext.hpp>
#include <Wire.h>

namespace Zscript {

namespace i2c_module {

template<class ZP>
class ZscriptI2cNotification {
    static const uint8_t SMBUS_ALERT_ADDR = 0xC;

public:
    static void notification(ZscriptNotificationContext<ZP> ctx, bool moveAlong, bool isAddressing) {
        (void) moveAlong;
        NotificationOutStream<ZP> out = ctx.getOutStream();
        uint8_t len = Wire.requestFrom(SMBUS_ALERT_ADDR, (uint8_t) 1);
        if (len != 2 && len != 1) {
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
            if (isAddressing) {
                out.writeField(Zchars::Z_ADDRESSING, 0x5);
                out.writeField(Zchars::Z_ADDRESSING_CONTINUE, 0x0);
                out.writeField(Zchars::Z_STATUS, ResponseStatus::ADDRESS_NOT_FOUND);
            } else {
#endif
            out.writeField('I', 0x0);
            out.writeAndThen();
            out.writeField('F', 0);
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
            }
#endif
            return;
        }
        uint8_t addr = ((uint8_t) Wire.read()) >> 1;
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        if (!isAddressing) {
#endif
        out.writeField('I', 0x0);
        out.writeAndThen();
        out.writeField('A', addr);
        return;
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        }
        out.writeField(Zchars::Z_ADDRESSING, 0x5);
        out.writeField(Zchars::Z_ADDRESSING_CONTINUE, 0x0);
        out.writeField(Zchars::Z_ADDRESSING_CONTINUE, addr);
        uint8_t dataBuffer[ZP::i2cAddressingReadBlockLength];
        delayMicroseconds(10);
        while (true) {
            uint8_t len = Wire.requestFrom((uint8_t) addr, ZP::i2cAddressingReadBlockLength);
            if (len != ZP::i2cAddressingReadBlockLength) {
                out.endSequence();
                out.writeField('!', 0x5);
                out.writeField('I', 0);
                out.writeField('A', (uint8_t) addr);
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
#endif
    }
};

}

}
#endif //SRC_MAIN_CPP_ARDUINO_I2C_MODULE_NOTIFICATIONS_ZSCRIPTI2CNOTIFICATION_HPP_