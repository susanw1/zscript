/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELINFOCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELINFOCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>

#define COMMAND_EXISTS_007c EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class UartChannel;

template<class ZP>
class UartModule;

namespace uart_module {
namespace cmd_channel_info {

using namespace uart_module;


template<class ZP>
class ZscriptUartChannelInfoCommand {
public:
    static constexpr uint8_t CODE = 0xc;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channel;
        if (ctx.getField(ReqChannel__C, &channel)) {
            if (channel != 0) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespChannelCount__N, 1);
        out.writeField(RespChannel__C, UartModule<ZP>::channel.getParser()->getChannelIndex());
        out.writeField(RespInterface__I, 0);
    }
};
}
}
}

#endif //SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELINFOCOMMAND_HPP_
