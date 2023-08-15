/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <zscript/modules/ZscriptCommand.hpp>
#include <zscript/modules/ZscriptModule.hpp>

#include "commands/PinSetupCommand.hpp"
#include "commands/PinWriteCommand.hpp"
#include "commands/PinReadCommand.hpp"
#include "commands/PinCapabilitiesCommand.hpp"

#define MODULE_EXISTS_003 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_003 MODULE_SWITCH_UTIL(ZscriptPinModule<ZP>::execute)

namespace Zscript {
template<class ZP>
class ZscriptPinModule {

public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
        case 0x0:
            ZscriptPinCapabilitiesCommand<ZP>::execute(ctx);
            break;
        case 0x1:
            ZscriptPinSetupCommand<ZP>::execute(ctx);
            break;
        case 0x2:
            ZscriptPinWriteCommand<ZP>::execute(ctx);
            break;
        case 0x3:
            ZscriptPinReadCommand<ZP>::execute(ctx);
            break;
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_ */
