/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SRC_ZSCRIPT_BASE_I2C_CAPABILITIES_COMMAND)
#error This file should not be included more than once
#endif
#define SRC_ZSCRIPT_BASE_I2C_CAPABILITIES_COMMAND


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
#ifdef ZSCRIPT_HAVE_I2C_GENERAL
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities_Values::lowSpeedSupported_field
                                                  | RespBitsetCapabilities_Values::smBusAddressResolution_field);
#else
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities_Values::lowSpeedSupported_field);
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
