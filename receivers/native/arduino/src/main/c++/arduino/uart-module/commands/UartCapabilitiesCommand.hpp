/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/components/ZscriptStatus.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>
#include "UartUtil.hpp"

#define COMMAND_EXISTS_0070 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace uart_module {

template<class ZP>
class UartCapabilitiesCommand: public Capabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();

        constexpr uint8_t freqCount = sizeof(ZP::uartSupportedFreqs) / sizeof(ZP::uartSupportedFreqs[0]);

        uint16_t freqIndex;
        if (!ctx.getFieldCheckLimit(ReqFrequencySelection__F, freqCount, freqCount - 1, &freqIndex)) {
            return;
        }

        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(007));
        UartUtil<ZP>::writeFrequencySelection(out, freqIndex);

        // implements just a single Serial port at this time. Should be expanded to more...
        out.writeField(RespInterfaceCount__I, ZP::uartCount);
        out.writeField(RespFrequenciesSupported__F, freqCount);
        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities_Values::parity_field | RespBitsetCapabilities_Values::doubleStop_field);
        out.writeField(RespRxBufferSize__R, ZP::uartRxBufferSize);
        out.writeField(RespTxBufferSize__T, ZP::uartTxBufferSize);
#ifdef ZSCRIPT_UART_SUPPORT_NOTIFICATIONS
        out.writeField(RespNotificationsSupported__N, 0);
#endif
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        out.writeField(RespAddressingSupported__A, 0);
#endif
    }
};

}
}

#endif /* SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_ */
