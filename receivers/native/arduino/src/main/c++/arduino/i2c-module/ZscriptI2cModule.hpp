/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include <zscript/modules/ZscriptModule.hpp>
#include <zscript/execution/ZscriptCommandContext.hpp>

#include "commands/ZscriptI2cSetupCommand.hpp"
#include "commands/ZscriptI2cSendCommand.hpp"
#include "commands/ZscriptI2cReceiveCommand.hpp"
#include "commands/ZscriptI2cSendReceiveCommand.hpp"
#include "commands/ZscriptI2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(ZscriptI2cModule<ZP>::execute)

template<class ZP>
class ZscriptI2cModule: public ZscriptModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZscriptExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZscriptI2cCapabilitiesCommand<ZP>::CODE:
            ZscriptI2cCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZscriptI2cSetupCommand<ZP>::CODE:
            ZscriptI2cSetupCommand<ZP>::execute(slot);
            break;
        case ZscriptI2cSendCommand<ZP>::CODE:
            ZscriptI2cSendCommand<ZP>::execute(slot);
            break;
        case ZscriptI2cReceiveCommand<ZP>::CODE:
            ZscriptI2cReceiveCommand<ZP>::execute(slot);
            break;
        case ZscriptI2cSendReceiveCommand<ZP>::CODE:
            ZscriptI2cSendReceiveCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZSCRIPTI2CMODULE_HPP_ */
