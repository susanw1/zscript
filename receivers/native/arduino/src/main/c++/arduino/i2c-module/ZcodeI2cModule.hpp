/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZCODEI2CMODULE_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZCODEI2CMODULE_HPP_

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include <zcode/modules/ZcodeModule.hpp>

#include "commands/ZcodeI2cSetupCommand.hpp"
#include "commands/ZcodeI2cSendCommand.hpp"
#include "commands/ZcodeI2cReceiveCommand.hpp"
#include "commands/ZcodeI2cSendReceiveCommand.hpp"
#include "commands/ZcodeI2cCapabilitiesCommand.hpp"

#define MODULE_EXISTS_005 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_005 MODULE_SWITCH_UTIL(ZcodeI2cModule<ZP>::execute)

template<class ZP>
class ZcodeI2cModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {
        switch (bottomBits) {
        case ZcodeI2cCapabilitiesCommand<ZP>::CODE:
            ZcodeI2cCapabilitiesCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSetupCommand<ZP>::CODE:
            ZcodeI2cSetupCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendCommand<ZP>::CODE:
            ZcodeI2cSendCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cReceiveCommand<ZP>::CODE:
            ZcodeI2cReceiveCommand<ZP>::execute(slot);
            break;
        case ZcodeI2cSendReceiveCommand<ZP>::CODE:
            ZcodeI2cSendReceiveCommand<ZP>::execute(slot);
            break;
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }
};

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_ZCODEI2CMODULE_HPP_ */
