/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/I2cModule.hpp>
#include <Wire.h>

#define COMMAND_EXISTS_005d EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class I2cChannel;

template<class ZP>
class I2cModule;

template<class ZP>
class ZscriptI2cChannelSetupCommand {
public:
    static constexpr uint8_t CODE = 0x0d;

    static constexpr char ReqChannel__C = 'C';
    static constexpr char ReqAddress__A = 'A';


    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t channelIndex;
        if (!ctx.getReqdFieldCheckLimit(ReqChannel__C, Zscript<ZP>::zscript.getChannelCount(), &channelIndex)) {
            return;
        }
        ZscriptChannel<ZP> *selectedChannel = Zscript<ZP>::zscript.getChannels()[channelIndex];
        if (selectedChannel->getAssociatedModule() != i2c_module::MODULE_FULL_ID) {
            ctx.status(ResponseStatus::VALUE_UNSUPPORTED);
            return;
        }

        uint16_t address;
        if (ctx.getField(ReqAddress__A, &address)) {
            if (address > 0x80) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            I2cModule<ZP>::channel.setAddress(address & 0x7F);
        }
    }
};

}
#endif //SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CCHANNELSETUPCOMMAND_HPP_
