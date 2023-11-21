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

namespace uart_module {

template<class ZP>
class UartModule;

template<class ZP>
class UartChannel;

template<class ZP>
class ZscriptUartOutStream;

template<class ZP>
class UartChannelInfoCommand : public ChannelInfo_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channelIndex;
        if (!ctx.getReqdFieldCheckLimit(ReqChannel__C, Zscript<ZP>::zscript.getChannelCount(), &channelIndex)) {
            return;
        }
        UartChannel<ZP> *selectedChannel = (UartChannel<ZP> *) Zscript<ZP>::zscript.getChannels()[channelIndex];
        if (selectedChannel->getAssociatedModule() != MODULE_FULL_ID) {
            ctx.status(ResponseStatus::VALUE_UNSUPPORTED);
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
        out.writeField(RespFrequenciesSupported__F, freqCount);
        UartUtil<ZP>::writeFrequencySelection(out, freqIndex);

        ZscriptUartOutStream<ZP> *channelOut = selectedChannel->getUartOutStream();
        uint8_t parity = channelOut->getConfigParity(); // 0 = off, 2 = even, 3 = odd
        uint8_t stopBits = channelOut->getConfigStopBits();

        out.writeField(RespBitsetCapabilities__B, (parity & 0x2 ? RespBitsetCapabilities_Values::parityOn_field : 0)
                                                  | (parity & 0x1 ? RespBitsetCapabilities_Values::parityOdd_field : 0)
                                                  | (stopBits ? RespBitsetCapabilities_Values::doubleStop_field : 0)
        );
    }
};

}
}

#endif //SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCHANNELINFOCOMMAND_HPP_
