/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>

#define COMMAND_EXISTS_007d EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class UartChannel;

template<class ZP>
class UartModule;

namespace uart_module {
namespace cmd_channel_setup {

using namespace uart_module;

template<class ZP>
class ZscriptUartChannelSetupCommand {
public:
    static constexpr uint8_t CODE = 0xd;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channel;
        if (ctx.getField(ReqChannel__C, &channel)) {
            if (channel != 0) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
        }
        uint16_t address;
//        if (ctx.getField(ReqAddress__A, &address)) {
//            if (channel > 0x80) {
//                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
//                return;
//            }
//            UartModule<ZP>::channel.setAddress(address & 0x7F);
//        }

    }

};
}
}
}

#endif //SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELSETUPCOMMAND_HPP_
