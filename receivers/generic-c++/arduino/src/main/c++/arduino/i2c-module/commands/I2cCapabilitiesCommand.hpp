/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/I2cModule.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_0050 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace i2c_module {

template<class ZP>
class ZscriptI2cCapabilitiesCommand : public Capabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, uint16_t commandsSet) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, commandsSet);
        out.writeField(RespInterfaceCount__I, 1);
        out.writeField(RespFrequenciesSupported__F, 3);
#ifdef ZSCRIPT_HAVE_I2C_MODULE
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities_Values::lowSpeedSupported_field
                                                  | RespBitsetCapabilities_Values::smBusAddressResolution_field);
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

}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_ */
