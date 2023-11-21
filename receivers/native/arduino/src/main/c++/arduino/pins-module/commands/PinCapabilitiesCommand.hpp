/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>

#define COMMAND_EXISTS_0030 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class PinCapabilitiesCommand {
    static constexpr char RespCommandsSet__C = 'C';
    static constexpr char RespPinCount__P = 'P';
    static constexpr char RespAvailableControllersBitset__B = 'B';

    static constexpr uint16_t RespAvailableControllersBitset__Digital = 0x1;
    static constexpr uint16_t RespAvailableControllersBitset__AtoD = 0x2;
    static constexpr uint16_t RespAvailableControllersBitset__DtoA = 0x4;
    static constexpr uint16_t RespAvailableControllersBitset__PWM = 0x8;

public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(004));
        out.writeField(RespPinCount__P, ZP::pinCount);
        out.writeField(RespAvailableControllersBitset__B, RespAvailableControllersBitset__Digital | RespAvailableControllersBitset__AtoD |

                                                          #ifdef DEVICE_SUPPORTS_ANALOG_WRITE
                                                          RespAvailableControllersBitset__DtoA |
                                                          #endif

                                                          RespAvailableControllersBitset__PWM);
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_ */
