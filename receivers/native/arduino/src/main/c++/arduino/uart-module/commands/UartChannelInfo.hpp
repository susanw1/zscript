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
#include "UartUtil.hpp"

#define COMMAND_EXISTS_007c EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class UartModule;

template<class ZP>
class ZscriptChannel;

template<class ZP>
class UartChannel;

namespace uart_module {

template<class ZP>
class ZscriptUartChannelInfoCommand : public ChannelInfo_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channelIndex;
        if (!ctx.getFieldCheckLimit(ReqChannel__C, Zscript<ZP>::zscript.getChannelCount(), ctx.getChannelIndex(), &channelIndex)) {
            return;
        }
        UartChannel<ZP> *selectedChannel = Zscript<ZP>::zscript.getChannels()[channelIndex];
        if (selectedChannel->getAssociatedModule() != MODULE_FULL_ID) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }

        constexpr uint8_t freqCount = sizeof(ZP::uartSupportedFreqs) / sizeof(ZP::uartSupportedFreqs[0]);
        uint16_t freqIndex;
        if (!ctx.getFieldCheckLimit(ReqFrequencySelection__F, freqCount, freqCount - 1, &freqIndex)) {
            return;
        }

        CommandOutStream<ZP> out = ctx.getOutStream();
        // Just one UART channel supported currently, always on Serial #0
        out.writeField(RespChannelCount__N, 1);
        out.writeField(RespInterface__I, 0);
        out.writeField(RespChannel__C, UartModule<ZP>::channel.getParser()->getChannelIndex());
        out.writeField(RespFrequenciesSupported__F, freqCount);
        UartUtil<ZP>::writeFrequencySelection(out, freqIndex);
        out.writeField(RespBitsetCapabilities__B, static_cast<uint16_t>(RespBitsetCapabilities_Values::parityOn_field)
                                                  | static_cast<uint16_t>(RespBitsetCapabilities_Values::doubleStop_field));
    }
};

}
}

#endif //SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELINFOCOMMAND_HPP_
