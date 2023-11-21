/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_UART_MODULE_ZSCRIPTUARTMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_UART_MODULE_ZSCRIPTUARTMODULE_HPP_

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>

#ifdef ZSCRIPT_HAVE_UART_CHANNEL
#include "commands/UartChannelInfo.hpp"
#include "commands/UartChannelSetup.hpp"
#endif

#ifdef ZSCRIPT_HAVE_UART_MODULE
#include <net/zscript/model/modules/base/UartModule.hpp>
#endif

#include "commands/UartCapabilitiesCommand.hpp"

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#define MODULE_EXISTS_007 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_007 MODULE_SWITCH_UTIL(uart_module::UartModule<ZP>::execute)

namespace Zscript {

namespace uart_module {

template<class ZP>
class UartModule : public ZscriptModule<ZP> {
public:
#ifdef ZSCRIPT_HAVE_UART_CHANNEL
    static UartChannel<ZP> channel;
#endif


    static void setup() {
#ifdef ZSCRIPT_HAVE_UART_CHANNEL
        channel.setup();
#else
        Serial.begin(ZP::uartSupportedFreqs[0]);
#endif
    }

    static void poll() {
    }

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
            case UartCapabilitiesCommand<ZP>::CODE:
                UartCapabilitiesCommand<ZP>::execute(ctx);
                break;
#ifdef ZSCRIPT_HAVE_UART_CHANNEL
            case UartChannelInfoCommand<ZP>::CODE:
                UartChannelInfoCommand<ZP>::execute(ctx);
                break;
            case UartChannelSetupCommand<ZP>::CODE:
                UartChannelSetupCommand<ZP>::execute(ctx);
                break;
#endif
            default:
                ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
                break;
        }
    }
};

#ifdef ZSCRIPT_HAVE_UART_CHANNEL
template<class ZP>
UartChannel<ZP> UartModule<ZP>::channel;
#endif

}

}

#endif /* SRC_MAIN_CPP_ARDUINO_UART_MODULE_ZSCRIPTUARTMODULE_HPP_ */
