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

#include "PinManager.hpp"

#include "commands/PwmPinSetupCommand.hpp"
#include "commands/PwmPinWriteCommand.hpp"
#ifdef DEVICE_SUPPORTS_ANALOG_WRITE
#include "commands/DtoAPinSetupCommand.hpp"
#include "commands/DtoAPinWriteCommand.hpp"
#endif
#include "commands/AtoDPinSetupCommand.hpp"
#include "commands/AtoDPinReadCommand.hpp"
#include "commands/DigitalPinSetupCommand.hpp"
#include "commands/DigitalPinWriteCommand.hpp"
#include "commands/DigitalPinReadCommand.hpp"
#include "commands/PinCapabilitiesCommand.hpp"

#define MODULE_EXISTS_003 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_003 MODULE_SWITCH_UTIL(PinModule<ZP>::execute)

namespace Zscript {
template<class ZP>
class PinModule {
public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
        case 0x0:
            PinCapabilitiesCommand<ZP>::execute(ctx);
            break;
        case 0x1:
            DigitalPinSetupCommand<ZP>::execute(ctx);
            break;
        case 0x2:
            DigitalPinWriteCommand<ZP>::execute(ctx);
            break;
        case 0x3:
            DigitalPinReadCommand<ZP>::execute(ctx);
            break;
        case 0x4:
            AtoDPinSetupCommand<ZP>::execute(ctx);
            break;
        case 0x5:
            AtoDPinReadCommand<ZP>::execute(ctx);
            break;
#ifdef DEVICE_SUPPORTS_ANALOG_WRITE
        case 0x6:
            DtoAPinSetupCommand<ZP>::execute(ctx);
            break;
        case 0x7:
            DtoAPinWriteCommand<ZP>::execute(ctx);
            break;
#endif
        case 0x8:
            PwmPinSetupCommand<ZP>::execute(ctx);
            break;
        case 0x9:
            PwmPinWriteCommand<ZP>::execute(ctx);
            break;
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_PINS_MODULE_ZSCRIPTPINMODULE_HPP_ */
