/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_0050 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptI2cCapabilitiesCommand {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char CMD_RESP_COMMANDS_C = 'C';
    static constexpr char CMD_RESP_NOTIFICATION_SUPPORTED_N = 'N';
    static constexpr char CMD_RESP_PORT_COUNT_P = 'P';
    static constexpr char CMD_RESP_MAX_FREQUENCIES_SUPPORTED_F = 'F';

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(CMD_RESP_COMMANDS_C, MODULE_CAPABILITIES(005));

        out.writeField(CMD_RESP_NOTIFICATION_SUPPORTED_N, 0);
        out.writeField('B', 32);

        out.writeField(CMD_RESP_PORT_COUNT_P, 1);
        out.writeField(CMD_RESP_MAX_FREQUENCIES_SUPPORTED_F, 3);
    }

};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCAPABILITIESCOMMAND_HPP_ */
