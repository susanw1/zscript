/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/UartModule.hpp>

#define COMMAND_EXISTS_0070 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace uart_module {
namespace cmd_capabilities {

using namespace uart_module;

template<class ZP>
class ZscriptUartCapabilitiesCommand {
public:
    static constexpr uint8_t CODE = 0x0;

    static constexpr uint8_t ReqFrequencySelection__F = 'F';

    static constexpr uint8_t RespCommandsSet__C = 'C';
    static constexpr uint8_t RespNotificationsSupported__N = 'N';
    /** Flag (Optional): whether addressing via the serial interface is available */
    static constexpr uint8_t RespAddressingSupported__A = 'A';
    /** Number (Required): how many interfaces are available */
    static constexpr uint8_t RespInterfaceCount__I = 'I';
    /** Number (Optional): how many frequencies are supported as a &#39;menu&#39; of frequencies - if not present, no preset frequencies are supported */
    static constexpr uint8_t RespFrequenciesSupported__F = 'F';
    /** Bitset (Required): a set of more specific capabilities of the module */
    static constexpr uint8_t RespBitsetCapabilities__B = 'B';
    /** Number (Required): how many bytes of buffer have been assigned for received data */
    static constexpr uint8_t RespRxBufferSize__R = 'R';
    /** Number (Required): how many bytes of buffer have been assigned for transmitted data */
    static constexpr uint8_t RespTxBufferSize__T = 'T';
    /** Bytes (Required): the baud rate option requested in Hz (or maximum rate supported if not given) */
    static constexpr uint8_t RespBaudRate__Bytes = '+';


//    static constexpr char RespCommands__C = 'C';
//    static constexpr char RespNotificationsSupported__N = 'N';
//    static constexpr char RespAddressingSupported__A = 'A';
//    static constexpr char RespInterfaceCount__I = 'I';
//    static constexpr char RespFrequenciesSupported__F = 'F';
//    static constexpr char RespBitsetCapabilities__B = 'B';
//
    static void execute(ZscriptCommandContext<ZP> ctx) {
        // implements just a single Serial port at this time. Should be expanded to more...
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(007));
        out.writeField(RespInterfaceCount__I, 1);
        out.writeField(RespFrequenciesSupported__F, 1);
#ifdef ZSCRIPT_HAVE_UART_MODULE
        //        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities__LowSpeedSupported | RespBitsetCapabilities__SmBusAddressResolution);
#else
        //        out.writeField(RespBitsetCapabilities__B, RespBitsetCapabilities__LowSpeedSupported);
#endif
#ifdef ZSCRIPT_UART_SUPPORT_NOTIFICATIONS
        out.writeField(RespNotificationsSupported__N, 0);
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        out.writeField(RespAddressingSupported__A, 0);
#endif
#endif
    }

};
}
}
}

#endif /* SRC_MAIN_CPP_ARDUINO_UART_MODULE_COMMANDS_ZSCRIPTUARTCAPABILITIESCOMMAND_HPP_ */
