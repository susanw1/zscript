/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SRC_ZSCRIPT_BASE_I2C_CHANNEL_SETUP_COMMAND)
#error This file should not be included more than once
#endif
#define SRC_ZSCRIPT_BASE_I2C_CHANNEL_SETUP_COMMAND

#define COMMAND_EXISTS_005d EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace i2c_module {

template<class ZP>
class I2cChannel;

template<class ZP>
class I2cModule;

template<class ZP>
class ZscriptI2cChannelSetupCommand : public ChannelSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channelIndex;
        if (!ctx.getReqdFieldCheckLimit(ReqChannel__C, Zscript<ZP>::zscript.getChannelCount(), &channelIndex)) {
            return;
        }
        ZscriptChannel<ZP> *selectedChannel = Zscript<ZP>::zscript.getChannels()[channelIndex];
        if (selectedChannel->getAssociatedModule() != MODULE_FULL_ID) {
            ctx.status(ResponseStatus::VALUE_UNSUPPORTED);
            return;
        }

        uint16_t address;
        if (ctx.getField(ReqAddress__A, &address)) {
            if (address > 0x80) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            I2cModule<ZP>::channel.setAddress(address & 0x7F);
        }
    }
};

}

}
