/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_0050 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptI2cCapabilitiesCommand {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char RespCommands__C = 'C';
    static constexpr char RespNotificationsSupported__N = 'N';
    static constexpr char RespAddressingSupported__A = 'A';
    static constexpr char RespInterfaceCount__I = 'I';
    static constexpr char RespFrequenciesSupported__F = 'F';
    static constexpr char RespBitsetCapabilities__B = 'B';

    static constexpr uint16_t RespBitsetCapabilities__LowSpeedSupported = 0x1;
    static constexpr uint16_t RespBitsetCapabilities__BusFreeSupported = 0x2;
    static constexpr uint16_t RespBitsetCapabilities__SmBusAddressResolution = 0x4;
    static constexpr uint16_t RespBitsetCapabilities__TenBit = 0x8;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommands__C, MODULE_CAPABILITIES(005));
        out.writeField(RespInterfaceCount__I, 1);
        out.writeField(RespFrequenciesSupported__F, 3);
#ifdef ZSCRIPT_HAVE_I2C_MODULE
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities__LowSpeedSupported | RespBitsetCapabilities__SmBusAddressResolution);
#else
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities__LowSpeedSupported);
#endif
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        out.writeField(RespNotificationsSupported__N, 0);
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        out.writeField(RespAddressingSupported__A, 0);
#endif
#endif
    }

};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_ */
