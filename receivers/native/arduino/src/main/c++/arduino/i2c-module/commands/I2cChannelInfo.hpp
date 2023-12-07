/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SRC_ZSCRIPT_BASE_I2C_CHANNEL_INFO_COMMAND)
#error This file should not be included more than once
#endif
#define SRC_ZSCRIPT_BASE_I2C_CHANNEL_INFO_COMMAND

#define COMMAND_EXISTS_005c EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class ZscriptChannel;

namespace i2c_module {

template<class ZP>
class I2cModule;

template<class ZP>
class I2cChannel;

template<class ZP>
class ZscriptI2cChannelInfoCommand : public ChannelInfo_CommandDefs {
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

        CommandOutStream<ZP> out = ctx.getOutStream();
        // Just one I2C channel supported currently, always on port #0
        out.writeField(RespChannelCount__N, 1);
        out.writeField(RespAddress__A, I2cModule<ZP>::channel.getAddress());
    }
};

}

}
