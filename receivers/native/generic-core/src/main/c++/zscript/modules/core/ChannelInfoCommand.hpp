/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELINFOCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELINFOCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"
#include <net/zscript/model/modules/base/CoreModule.hpp>

#define COMMAND_EXISTS_0008 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptChannel;

namespace GenericCore {
template<class ZP>
class ChannelInfoCommand: public core_module::ChannelInfo_CommandDefs {

public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        uint8_t current = ctx.getChannelIndex();
        uint16_t channelCount = Zscript<ZP>::zscript.getChannelCount();

        uint16_t channelIndex;
        if (!ctx.getFieldCheckLimit(ReqChannel__C, channelCount, current, &channelIndex)) {
            return;
        }
        out.writeField(RespChannelCount__N, channelCount);
        out.writeField(RespAssociatedModule__M, Zscript<ZP>::zscript.getChannels()[channelIndex]->getAssociatedModule());
        out.writeField(RespBufferLength__B, Zscript<ZP>::zscript.getChannels()[channelIndex]->getBufferLength());
        out.writeField(RespCurrentChannel__U, current);
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELINFOCOMMAND_HPP_ */
