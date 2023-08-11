/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELSETUPCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"

#define COMMAND_EXISTS_0018 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptChannel;
namespace GenericCore {
template<class ZP>
class ChannelSetupCommand {

public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint8_t current = ctx.getChannelIndex();

        uint16_t target = ctx.getField('C', current);
        if (target >= Zscript<ZP>::zscript.getChannelCount()) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        Zscript<ZP>::zscript.getChannels()[target]->channelSetup(ctx);
        if (ctx.hasField('N')) {
            Zscript<ZP>::zscript.setNotificationChannelIndex((uint8_t) target);
        }
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_CHANNELSETUPCOMMAND_HPP_ */
