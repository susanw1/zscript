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

template<class ZP>
class ZscriptUartChannelSetupCommand: public ChannelSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channelIndex;
        if (!ctx.getFieldCheckLimit(ReqChannel__C, Zscript<ZP>::zscript.getChannelCount(), ctx.getChannelIndex(), &channelIndex)) {
            return;
        }
        ZscriptChannel<ZP> *selectedChannel = Zscript<ZP>::zscript.getChannels()[channelIndex];
        if (selectedChannel->getAssociatedModule() != MODULE) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }

        if (ctx.hasField(ReqFrequencySelection__F)) {

        }
        constexpr uint8_t freqCount = sizeof(ZP::uartSupportedFreqs) / sizeof(ZP::uartSupportedFreqs[0]);
        uint16_t freqIndex;
        if (!ctx.getFieldCheckLimit(ReqFrequencySelection__F, freqCount, freqCount - 1, &freqIndex)) {
            return;
        }


    }

};

}
}

#endif //SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELSETUPCOMMAND_HPP_
