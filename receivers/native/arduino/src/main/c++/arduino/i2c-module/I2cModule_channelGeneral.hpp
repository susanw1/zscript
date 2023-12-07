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

// Channel
#include <arduino/i2c-module/commands/I2cChannelInfo.hpp>
#include <arduino/i2c-module/commands/I2cChannelSetup.hpp>

// All
#include <arduino/i2c-module/commands/GeneralI2cAction.hpp>
#include <arduino/i2c-module/commands/I2cSetupCommand.hpp>
#include <arduino/i2c-module/commands/I2cCapabilitiesCommand.hpp>

namespace Zscript {

namespace i2c_module {

template<class ZP>
class I2cModule : public ZscriptModule<ZP> {
public:
    static I2cChannel<ZP> channel;

    static void setup() {
        channel.setup();
        channel.setAddress(ZscriptParams::i2cChannelAddress);
    }

    static void poll() {
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
            case ZscriptI2cCapabilitiesCommand<ZP>::CODE:
                ZscriptI2cCapabilitiesCommand<ZP>::execute(ctx, MODULE_CAPABILITIES(005));
                break;
                case ZscriptI2cSetupCommand<ZP>::CODE:
                ZscriptI2cSetupCommand<ZP>::execute(ctx);
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
                case ZscriptI2cChannelInfoCommand<ZP>::CODE:
                ZscriptI2cChannelInfoCommand<ZP>::execute(ctx);
                break;
            case ZscriptI2cChannelSetupCommand<ZP>::CODE:
                ZscriptI2cChannelSetupCommand<ZP>::execute(ctx);
                break;
            default:
                ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
                break;
        }
    }
};

#if defined(ZSCRIPT_HAVE_I2C_CHANNEL)
template<class ZP>
I2cChannel<ZP> I2cModule<ZP>::channel;
#endif

}

}

