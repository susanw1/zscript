/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELINFOCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELINFOCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_005c EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class I2cChannel;
template<class ZP>
class I2cModule;

template<class ZP>
class ZscriptI2cChannelInfoCommand {
public:
    static constexpr uint8_t CODE = 0x0c;

    static constexpr char ParamChannel__C = 'C';

    static constexpr char RespChannelCount__N = 'N';
    static constexpr char RespChannelGlobal__C = 'C';
    static constexpr char RespAddress__A = 'A';
    static constexpr char RespInterface__I = 'I';


    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channel;
        if (ctx.getField(ParamChannel__C, &channel)) {
            if (channel != 0) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespChannelCount__N, 1);
        out.writeField(RespChannelGlobal__C, I2cModule<ZP>::channel.getParser()->getChannelIndex());
        out.writeField(RespAddress__A, I2cModule<ZP>::channel.getAddress());
        out.writeField(RespInterface__I, 0);
    }

};
}

#endif //SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELINFOCOMMAND_HPP_
