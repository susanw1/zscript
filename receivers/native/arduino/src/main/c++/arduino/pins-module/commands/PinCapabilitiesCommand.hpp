/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>

#define COMMAND_EXISTS_0040 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace pins_module {

template<class ZP>
class PinCapabilitiesCommand : public Capabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, uint16_t commandsSet) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, commandsSet);
        out.writeField(RespPinCount__P, ZP::pinCount);
        out.writeField(RespAvailableControllers__B, RespAvailableControllers_Values::digital_field
                                                    | RespAvailableControllers_Values::atoD_field
                                                    #ifdef DEVICE_SUPPORTS_ANALOG_WRITE
                                                    | RespAvailableControllers_Values::dtoA_field
                                                    #endif
                                                    | RespAvailableControllers_Values::pwm_field);
    }
};

}

}

#endif /* SRC_MAIN_C___ARDUINO_PINS_MODULE_COMMANDS_PINCAPABILITIESCOMMAND_HPP_ */
